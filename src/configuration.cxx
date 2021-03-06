/*
Created:        20 August    2016
Last Updated:   16 October   2017

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University

-----

Configuration class
  -- Read config file and use functions
     to return configurations later

*/
#include "Analysis/CyMiniAna/interface/configuration.h"


configuration::configuration(const std::string &configFile) : 
  m_configFile(configFile),
  m_isMC(false),
  m_isGridFile(false),
  m_useTruth(false),
  m_useJets(false),
  m_useLeptons(false),
  m_useLargeRJets(false),
  m_useNeutrinos(false),
  m_useDNN(false),
  m_useWprime(false),
  m_input_selection("SetMe"),
  m_treename("SetMe"),
  m_filename("SetMe"),
  m_primaryDataset("SetMe"),
  m_NTotalEvents(0),
  m_verboseLevel("SetMe"),
  m_nEventsToProcess(0),
  m_firstEvent(0),
  m_outputFilePath("SetMe"),
  m_customDirectory("SetMe"),
  m_makeTTree(false),
  m_makeHistograms(false),
  m_cma_absPath("SetMe"),
  m_metadataFile("SetMe"),
  m_DNNinference(false),
  m_DNNtraining(false),
  m_dnnFile("SetMe"),
  m_dnnKey("SetMe"),
  m_doRecoEventLoop(false),
  m_doTruthEventLoop(false),
  m_matchTruthToReco(true),
  m_jet_btag_wkpt("SetMe"),
  m_calcWeightSystematics(false),
  m_listOfWeightSystematicsFile("SetMe"),
  m_listOfWeightVectorSystematicsFile("SetMe"),
  m_neutrinoReco(false){
    m_selections.clear();
    m_cutsfiles.clear();

    m_XSection.clear();
    m_KFactor.clear();
    m_AMI.clear();
    m_map_config.clear();

    m_isQCD   = false;
    m_isTtbar = false;
    m_isWjets = false;
    m_isZjets = false;
    m_isSingleTop    = false;
    m_primaryDataset = "";
    m_NTotalEvents   = 0;
  }

configuration::~configuration() {}

configuration &configuration::operator=(const configuration &rhs) { return *this; }

void configuration::initialize() {
    /* Initialize the configurations */
    std::vector<std::string> configurations; 
    cma::read_file( m_configFile, configurations ); // read config file into vector

    // fill map with values from configuration file
    for (const auto& config : configurations){
        // split config items by space
        std::istringstream cfg(config);
        std::istream_iterator<std::string> start(cfg), stop;
        std::vector<std::string> tokens(start, stop);

        m_map_config.insert( std::pair<std::string,std::string>(tokens.at(0),tokens.at(1)) );
    }

    // Protection against default settings missing in custom configuration
    // -- map of defaultConfigs defined in header (can't use 'verbose' tools, not defined yet!)
    for (const auto& defaultConfig : m_defaultConfigs){
        if ( m_map_config.find(defaultConfig.first) == m_map_config.end() ){ // item isn't in config file
            std::cout << " WARNING :: CONFIG : Configuration " << defaultConfig.first << 
                         " undefined, setting to default '" << defaultConfig.second << "'." << std::endl;
            m_map_config[defaultConfig.first] = defaultConfig.second;
        }
    }


    // Set the verbosity level (the amount of output to the console)
    std::map<std::string,unsigned int> verboseMap = cma::verboseMap(); // load mapping of string to integer
    m_verboseLevel = getConfigOption("verboseLevel");
    if (verboseMap.find(m_verboseLevel)==verboseMap.end()){
        m_verboseLevel = "INFO";
        cma::setVerboseLevel(m_verboseLevel);

        cma::WARNING( "CONFIG : Verbose level selected, "+m_verboseLevel+", is not supported " );
        cma::WARNING( "CONFIG : Please select one of the following: " );
        for (const auto& dm : verboseMap)
            cma::WARNING( "CONFIG :          "+dm.first);
        cma::WARNING( "CONFIG : Continuing; setting verbose level to "+m_verboseLevel);
    }
    else{
        cma::setVerboseLevel(m_verboseLevel);
    }


    // Get the absolute path to CyMiniAna for loading
    char* cma_path = getenv("CYMINIANADIR");
    if (cma_path==NULL){
        cma::WARNING("CONFIG : environment variable " );
        cma::WARNING("CONFIG :    'CYMINIANADIR' " );
        cma::WARNING("CONFIG : is not set.  Using PWD to set path." );
        cma::WARNING("CONFIG : This may cause problems submitting batch jobs." );
        cma_path = getenv("PWD");
    }
    m_cma_absPath = cma_path;
    cma::DEBUG("CONFIG : path set to: "+m_cma_absPath );

    // Assign values
    m_nEventsToProcess = std::stoi(getConfigOption("NEvents"));
    m_firstEvent       = std::stoi(getConfigOption("firstEvent"));
    m_input_selection  = getConfigOption("input_selection"); // "grid", "pre", etc.
    cma::split( m_map_config.at("selection"), ',', m_selections );  // different event selections
    cma::split( m_map_config.at("cutsfile"), ',', m_cutsfiles );  // different event selections


    // check that b-tag and top-tag WPs are recognized as one of supported values
    check_btag_WP(getConfigOption("jet_btag_wkpt"));

    m_jet_btag_wkpt    = getConfigOption("jet_btag_wkpt");
    m_outputFilePath   = getConfigOption("output_path");
    m_customDirectory  = getConfigOption("customDirectory");
    m_useTruth         = cma::str2bool( getConfigOption("useTruth") );
    m_useJets          = cma::str2bool( getConfigOption("useJets") );
    m_useLeptons       = cma::str2bool( getConfigOption("useLeptons") );
    m_useLargeRJets    = cma::str2bool( getConfigOption("useLargeRJets") );
    m_useNeutrinos     = cma::str2bool( getConfigOption("useNeutrinos") );
    m_neutrinoReco     = cma::str2bool( getConfigOption("neutrinoReco") );
    m_useDNN           = cma::str2bool( getConfigOption("useDNN") );
    m_useWprime        = cma::str2bool( getConfigOption("useWprime") );
    m_makeTTree        = cma::str2bool( getConfigOption("makeTTree") );
    m_makeHistograms   = cma::str2bool( getConfigOption("makeHistograms") );
    m_makeEfficiencies = cma::str2bool( getConfigOption("makeEfficiencies") );
    m_dnnFile          = getConfigOption("dnnFile");
    m_dnnKey           = getConfigOption("dnnKey");
    m_DNNtraining      = cma::str2bool( getConfigOption("DNNtraining") );
    m_DNNinference     = cma::str2bool( getConfigOption("DNNinference") );
    m_doRecoEventLoop  = cma::str2bool( getConfigOption("doRecoEventLoop") );
    m_metadataFile     = getConfigOption("metadataFile");
    m_calcWeightSystematics             = cma::str2bool( getConfigOption("calcWeightSystematics") );
    m_listOfWeightSystematicsFile       = getConfigOption("weightSystematicsFile");
    m_listOfWeightVectorSystematicsFile = getConfigOption("weightVectorSystematicsFile");

    cma::read_file( getConfigOption("inputfile"), m_filesToProcess );
    cma::read_file( getConfigOption("treenames"), m_treeNames );

    m_isGridFile = (m_input_selection.compare("grid")==0) ? true : false;
    m_isExtendedSample = cma::str2bool( getConfigOption("isExtendedSample") );

    m_XSection.clear();
    m_KFactor.clear();
    m_AMI.clear();
    m_NEvents.clear();
    m_mapOfSamples.clear();
    cma::getSampleWeights( m_metadataFile, m_mapOfSamples );

    // systematics that are weights in the nominal tree
    m_listOfWeightSystematics.resize(0);
    cma::read_file(m_listOfWeightSystematicsFile,m_listOfWeightSystematics);

    std::vector<std::string> weightVectorSystematics; // vector btagging SF
    cma::read_file(m_listOfWeightVectorSystematicsFile,weightVectorSystematics);

    for (const auto& weightVectorSystematic : weightVectorSystematics){
        // split config items by space
        std::istringstream cfg(weightVectorSystematic);
        std::istream_iterator<std::string> start(cfg), stop;
        std::vector<std::string> tokens(start, stop);

        m_mapOfWeightVectorSystematics.insert( std::pair<std::string,unsigned int>( tokens.at(0),std::stoi(tokens.at(1)) ) );
    }

    return;
}


void configuration::print(){
    // -- Print the configuration
    std::cout << " ** CyMiniAna ** " << std::endl;
    std::cout << " --------------- " << std::endl;
    std::cout << " CONFIGURATION :: Printing configuration " << std::endl;
    std::cout << " " << std::endl;
    for (const auto& config : m_map_config){
        std::cout << " " << config.first << "\t\t\t" << config.second << std::endl;
    }
    std::cout << " --------------- " << std::endl;

    return;
}


std::string configuration::getConfigOption( std::string item ){
    /* Check that the item exists in the map & return it; otherwise throw exception  */
    std::string value("");

    try{
        value = m_map_config.at(item);
    }
    catch(const std::exception&){
        cma::ERROR("CONFIG : Option "+item+" does not exist in configuration.");
        cma::ERROR("CONFIG : This does not exist in the default configuration either.");
        cma::ERROR("CONFIG : Returing an empty string.");
    }

    return value;
}

void configuration::setTreename(std::string treeName){
    m_treename = treeName;
    return;
}

void configuration::setFilename(std::string fileName){
    m_filename = fileName;
    return;
}

bool configuration::isNominalTree(){
    return isNominalTree( m_treename );
}

bool configuration::isNominalTree( const std::string &tree_name ){
    /* Check if tree is a nominal one */
    bool isNominal = (tree_name.compare("tree/eventVars")==0 || tree_name.compare("eventVars")==0);
    return isNominal;
}

bool configuration::checkPrimaryDataset(const std::vector<std::string>& files){
    /* Check if filename is in list of files 
       6 April 2018: It is possible the ntuples have incorrect metadata information.
                     Keeping m_mapOfSamples/m_mapOfPrimaryDatasets in case something needs to be updated/fixed
       18 May 2018:  No longer using this function
    */
    return false;
}


void configuration::readMetadata(TFile& file,const std::string& metadataTreeName){
    /* Read metadata TTree */
    m_sample.clear();
    m_primaryDataset = "";
    m_NTotalEvents   = 1;

    if (metadataTreeName.size()<1) return;  // no metadata tree to read

    TTreeReader metadata(metadataTreeName.c_str(), &file);

    TTreeReaderValue<std::string> primaryDataset(metadata, "primaryDataset");
    TTreeReaderValue<float> xsection(metadata, "xsection");
    TTreeReaderValue<float> kfactor(metadata,  "kfactor");
    TTreeReaderValue<float> sumOfWeights(metadata,   "sumOfWeights");
    TTreeReaderValue<unsigned int> NEvents(metadata, "NEvents");

    metadata.Next();

    std::string pd  = *primaryDataset;
    std::size_t pos = pd.find_first_of("/");
    std::size_t extpos = pd.find("-ext");

    if (m_isExtendedSample && extpos==std::string::npos){
        cma::WARNING("CONFIGURATION : 'isExtendedSample' set to true but '-ext' not found in primary dataset name");
        if ( m_mapOfSamples.find(pd+"-ext")==m_mapOfSamples.end() ){
            cma::WARNING("CONFIGURATION : The extended sample name, '"+pd+"-ext' is not available, using the default name -- please check configuration!");
        }
        else{
            cma::WARNING("CONFIGURATION : Using the extended sample name, '"+pd+"-ext'");
            pd += "-ext";
        }
    }

    bool pd_in_map = m_mapOfSamples.find(pd)!=m_mapOfSamples.end();    // check if the primary dataset exists in our map
    Sample this_sample;
    if (pd_in_map)
        this_sample = m_mapOfSamples.at(pd);

    if (pos==0){
        // bad name for metadata -- need to use map to get metadata
        // given something like '/ttbar/run2/.../', want 'ttbar'
        m_recalculateMetadata = true;
        std::size_t found = pd.find_first_of("/",pos+1);
        pd = pd.substr(pos+1,found-1);

        if (!pd_in_map) return;
        m_sample = this_sample;

        m_primaryDataset = m_sample.primaryDataset;
        m_NTotalEvents   = m_sample.NEvents;
    }
    else{
        m_recalculateMetadata = false;    // first assume the information in the root file (metadata tree) is good to use

        m_sample.sampleType = "data";     // not storing sample type in metadata tree, need metadata file (default: data)
        m_sample.primaryDataset = pd;
        m_sample.XSection = *xsection;
        m_sample.KFactor  = *kfactor;
        m_sample.NEvents  = *NEvents;
        m_sample.sumOfWeights = *sumOfWeights;

        if (pd_in_map){
            m_sample.sampleType = this_sample.sampleType;  // reset to value in the metadatafile (data not in metadata file)

            // check if the metadata in the file can be trusted (compare with the text file)
            float xsec_diff = (this_sample.XSection - *xsection) / this_sample.XSection;
            float sumw_diff = (this_sample.sumOfWeights - *sumOfWeights) / this_sample.sumOfWeights;
            int nevents_diff = this_sample.NEvents - *NEvents;

            if (xsec_diff>1e-3 || sumw_diff>1e-3 || nevents_diff!=0){
                // obtain values from map, not root file (something may have been updated/corrected!)
                m_recalculateMetadata = true;
                m_sample = this_sample;
            }
        }

        m_primaryDataset = m_sample.primaryDataset;
        m_NTotalEvents   = m_sample.NEvents;
    }

    cma::DEBUG("CONFIGURATION : Primary dataset = "+m_primaryDataset);

    return;
}


void configuration::inspectFile( TFile& file, const std::string& metadataTreeName ){
    /* Compare filenames to determine file type */
    m_isQCD   = false;
    m_isTtbar = false;
    m_isWjets = false;
    m_isZjets = false;
    m_isSingleTop = false;
    m_isDiboson   = false;
    m_NTotalEvents = 0;
    m_primaryDataset = "";
    m_recalculateMetadata = false;

    readMetadata(file,metadataTreeName);               // access metadata; recalculate if necessary

    m_isSignal = m_sample.sampleType=="signal";        // check if file is a signal sample
    m_isQCD    = m_sample.sampleType=="qcd";           // check if file is QCD
    m_isTtbar  = m_sample.sampleType=="ttbar";         // check if file is ttbar
    m_isWjets  = m_sample.sampleType=="wjets";         // check if file is wjets
    m_isZjets  = m_sample.sampleType=="zjets";         // check if file is wjets
    m_isDiboson = m_sample.sampleType=="diboson";      // check if file is diboson
    m_isSingleTop = m_sample.sampleType=="singletop";  // check if file is single top

    // check if 'isMC' by comparing primarydataset with datanames (quicker) -- should work for 2016 and 2017 names
    //   > 'string::find() returns 0' is the equivalent of python 'str.startswith()'
    bool isData = m_sample.primaryDataset.find("SingleElectron")==0 || m_sample.primaryDataset.find("SingleMuon")==0 || m_sample.primaryDataset.find("JetHT")==0;
    m_isMC = !isData;
        // (m_isQCD || m_isTtbar || m_isWjets || m_isZjets || m_isSingleTop || m_isDiboson || m_isSignal);

    // get the metadata
    cma::DEBUG("CONFIGURATION : Found primary dataset = "+m_primaryDataset);
    if (m_primaryDataset.size()>0) m_NTotalEvents = m_sample.NEvents;
    else{
        cma::WARNING("CONFIGURATION : Primary dataset name not found, checking the map");
        cma::WARNING("CONFIGURATION : - isMC = "+std::to_string(m_isMC));
        for (const auto& s : m_mapOfSamples){
            Sample samp = s.second;
            std::size_t found = m_filename.find(samp.primaryDataset);
            if (found!=std::string::npos){ 
                m_primaryDataset = samp.primaryDataset;
                m_NTotalEvents   = samp.NEvents;
                break;
            } // end if the filename contains this primary dataset
        } // end loop over map of samples (to access metadata info)
    } // end else

    // Protection against accessing truth information that may not exist
    if (!m_isMC && m_useTruth){
        cma::WARNING("CONFIGURATION : 'useTruth=true' but 'isMC=false'");
        cma::WARNING("CONFIGURATION : Setting 'useTruth' to false");
        m_useTruth = false;
    }

    return;
}


bool configuration::isMC( TFile& file ){
    /* Check the sum of weights tree DSIDs (to determine Data || MC) */
    inspectFile( file );
    return m_isMC;
}



// weights + systematics
double configuration::XSectionMap( std::string mcChannelNumber ){
    /* XSection values */
    double XSectionValue(0.0);

    if (m_mapOfSamples.find(mcChannelNumber)==m_mapOfSamples.end()){
        XSectionValue = 1.;
        cma::WARNING("CONFIG : Request for XSection value that does not exist "+mcChannelNumber);
        cma::WARNING("CONFIG : -- returning 1.0");
    }
    else{
        XSectionValue = m_mapOfSamples.at( mcChannelNumber ).XSection;
    }

    return XSectionValue;
}

double configuration::KFactorMap( std::string mcChannelNumber ){
    /* KFactor values */
    double KFactorValue(0.0);

    if (m_mapOfSamples.find(mcChannelNumber)==m_mapOfSamples.end()){
        KFactorValue = 1.;
       	cma::WARNING("CONFIG : Request for KFactor value that does not exist "+mcChannelNumber);
        cma::WARNING("CONFIG : -- returning 1.0");
    }
    else{
        KFactorValue = m_mapOfSamples.at( mcChannelNumber ).KFactor;
    }

    return KFactorValue;
}

double configuration::sumWeightsMap( std::string mcChannelNumber ){
    /* Sum of Weights values */
    double AMIValue(0.0);

    if (m_mapOfSamples.find(mcChannelNumber)==m_mapOfSamples.end()){
        AMIValue = 1.;
        cma::WARNING("CONFIG : Request for SumOfWeights value that does not exist "+mcChannelNumber);
        cma::WARNING("CONFIG : -- returning 1.0");
    }
    else{
        AMIValue = m_mapOfSamples.at( mcChannelNumber ).sumOfWeights;
    }

    return AMIValue;
}


void configuration::check_btag_WP(const std::string &wkpt){
    /* Check the b-tagging working point */
    if(! std::any_of(m_btag_WPs.begin(), m_btag_WPs.end(), [&](const std::string& s){return (s.compare(wkpt) == 0);} ) ) {
        cma::ERROR("CONFIG : Unknown b-tagging WP: "+wkpt+". Aborting!");
        cma::ERROR("CONFIG : Available b-tagging WPs: "+cma::vectorToStr(m_btag_WPs));
        exit(EXIT_FAILURE);
    }

    return;
}

void configuration::setMatchTruthToReco(bool truthToReco){
    m_matchTruthToReco = truthToReco;
    return;
}

// THE END
