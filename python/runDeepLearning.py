"""
Created:        12 November  2016
Last Updated:   11 April     2018

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
Texas A&M University
-----

Script for running the deep learning implementation

To run:
$ python python/runDeepLearning.py config/mlconfig.txt
-- the second argument is the text file with configurations for the NN/setup
"""
import os
import sys
import json
from collections import Counter
from time import strftime,localtime

import util
from config import Config
from deepLearning.classification import Classification
from deepLearning.regression import Regression

print
print " ------------------------------ "
print " *  Deep Learning with Keras  * "
print " ------------------------------ "
print


date   = strftime("%d%b", localtime())
cmaDir = os.path.expandvars('$CYMINIANADIR')
vb     = util.VERBOSE()

## Set configuration options ##
if len(sys.argv)<2:
    vb.HELP()
    sys.exit(1)

config = Config(sys.argv[1])
vb.level = config.verbose_level
vb.initialize()

if not config.runTraining and not config.runInference:
    vb.ERROR("RUN :  No configuration set ")
    vb.ERROR("RUN :  Please set the arguments 'runTraining' or 'runInference' to define workflow ")
    vb.ERROR("RUN :  Exiting.")
    sys.exit(1)


NN_parameters = ['epochs','batch_size','loss','optimizer','metrics','activations',
                 'nHiddenLayers','nNodes','input_dim','kfold_splits']

## Setup features -- keep track of features used with each NN
try:
    featureKeys = json.load(open('config/features.json'))
except IOError:
    featureKeys = {0:config.features}

featureKey = -1
for key in featureKeys.keys():
    if Counter(featureKeys[key])==Counter(config.features):
        featureKey = int(key)
        break
if featureKey<0:
    featureKey = max([int(i) for i in featureKeys.keys()])+1
    featureKeys[str(featureKey)] = config.features
    vb.INFO("RUN :  New features for NN ")
    with open('config/features.json','w') as outfile:
        json.dump(featureKeys,outfile)

## Set output directory
output_dir  = "nHiddenLayers{0}_".format(config.nHiddenLayers)
output_dir += "nNodes{0}_".format('-'.join(config.nNodes))
output_dir += "epoch{0}_".format(config.epochs)
output_dir += "batch{0}_".format(config.batch_size)
output_dir += "kfold{0}_".format(config.kfold_splits)
output_dir += "activation-{0}_".format(config.activation.replace(',','-'))
output_dir += "featureKey{0}".format(featureKey)
hep_data_name = config.hep_data.split('/')[-1].split('.')[0]


## Setup Deep Learning class
if config.method=='regression':
    dnn = Regression()
else:
    dnn = Classification()

dnn.target_names  = ["pz"]
dnn.target_values = None
dnn.hep_data   = config.hep_data
dnn.model_name = config.dnn_data
dnn.verbose_level = config.verbose_level
dnn.treename   = config.treename
dnn.useLWTNN   = True
dnn.dnn_name   = "dnn"
dnn.output_dim = config.output_dim
dnn.dnn_method = config.method
dnn.loss       = config.loss
dnn.init       = config.init
dnn.nNodes     = config.nNodes
dnn.dropout    = None
dnn.metrics    = config.metrics
dnn.features   = config.features
dnn.epochs     = config.epochs
dnn.optimizer  = config.optimizer
dnn.input_dim  = len(config.features)
dnn.batch_size = config.batch_size
dnn.activations   = config.activation.split(',')
dnn.kfold_splits  = config.kfold_splits
dnn.nHiddenLayers = config.nHiddenLayers
#dnn.earlystopping = {'monitor':'loss','min_delta':0.0001,'patience':5,'mode':'auto'}


## inference/training
output = "{0}/{1}/{2}".format( config.output_path,output_dir,hep_data_name)
if config.runTraining:
    output += "/training/"
else:
    output += "/inference/"
dnn.output_dir = output

if not os.path.isdir(output):
    vb.INFO("RUN : '{0}' does not exist ".format(output))
    vb.INFO("RUN :       Creating the directory. ")
    os.system( 'mkdir -p {0}'.format(output) )
else:
    vb.INFO("RUN :  Saving output to {0}".format(output))

## load hep data (physics data -- .json file). Always need this for testing/training
dnn.features = config.features

## Setup
dnn.initialize()


if config.runTraining:
    vb.INFO("RUN :  > Build the NN")

    # set properties of the NN
    dnn.runTraining()    # add extra attributes to plot as list

    ## -- Save information on the NN to a text file to reference later
    outputFile = open(dnn.output_dir+'/ABOUT.txt','w')
    outputFile.write(" * NN Setup * \n")
    outputFile.write("\n > NN parameters: \n")
    for NN_parameter in NN_parameters:
        outputFile.write( "   >> {0}: {1}\n".format(NN_parameter,getattr(dnn,NN_parameter)))
    outputFile.write( "\n > NN Features: \n" )
    for feature in dnn.features:
        outputFile.write("   >> "+feature+"\n" )
    outputFile.close()


if config.runInference:
    vb.INFO("RUN :  > Load NN model from disk")
    dnn.runInference()


## END ##
