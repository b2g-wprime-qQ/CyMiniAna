"""
Created:         3 September 2016
Last Updated:    3 September 2016

Dan Marley
daniel.edison.marley@cernSPAMNOT.ch
University of Michigan, Ann Arbor, MI 48109
-----
Steering script for making simple histograms.
Primarily want to do this from root histograms (faster to make those in c++)

This can be modified or extended by whomever.

To run:
python python/runHistogram.py --files <files.txt> --hists <histogramNames.txt> -o <output_path>
"""
import sys
import ROOT
from argparse import ArgumentParser

from Analysis.CyMiniAna.hepPlotter.hepPlotter import HepPlotter
import Analysis.CyMiniAna.hepPlotter.hepPlotterTools as hpt
import Analysis.CyMiniAna.hepPlotter.hepPlotterLabels as hpl


parser = ArgumentParser(description="Histogram Plotter")

parser.add_argument('-f','--files', action='store',default=None,
                    dest='listOfFiles',
                    help='Name of file that contains root files to plot')
parser.add_argument('--hists', action='store',default=None,
                    dest='listOfHists',
                    help='Name of file that contains histograms to plot')
parser.add_argument('-o','--outpath', action='store',default=None,
                    dest='outpath',
                    help='Directory for storing output plots')
results = parser.parse_args()

listOfFiles = results.listOfFiles
listOfHists = results.listOfHists
outpath     = results.outpath

files      = open(listOfFiles,"r").readlines()
histograms = open(listOfHists,"r").readlines()

betterColors = hpt.betterColors()['linecolors']

x_labels = {} # labels based on histogram plotted (e.g., 'Jet pT')
labels   = {} # labels based on filetype being plotted (e.g., 'ttbar')

numberOfHists = 0
# Access data -- assumes you are plotting histograms from multiple sources in one figure
for hi,histogram in enumerate(histograms):

    histogram = histogram.strip('\n')
    print "  :: Plotting "+histogram+"\n"

    ## setup histogram
    hist = HepPlotter("histogram",1)

    hist.ratio_plot  = False       # plot a ratio of things [Data/MC]
    hist.ratio_type  = "ratio"     # "ratio"
    hist.stacked     = True        # stack plots
    hist.rebin       = 1
    hist.logplot     = False       # plot on log scale
    hist.x_label     = x_labels[histogram]
    hist.y_label     = "Events"
    hist.y_ratio_label = ""
    hist.lumi          = 'XY.Z'   # in /fb
    hist.format        = 'png'       # file format for saving image
    hist.saveAs        = outpath+"_hist_"+histogram # save figure with name
    hist.ATLASlabel       = 'top left'  # 'top left', 'top right'; hack code for something else
    hist.ATLASlabelStatus = 'Internal'  # ('Simulation')+'Internal' || 'Preliminary' 

    hist.initialize()

    ## Add the data from each file
    for fi,file in enumerate(files):
        file = file.rstrip("\n")
        f = ROOT.TFile.Open(file)
        filename = file.split("/")[-1].split(".")[0]

        print "     > Opening data from ",file

        h_hist       = getattr(f,histogram)       # retrieve the histogram

        hist.Add(h_hist,name=filename+"_"+histogram,linecolor=betterColors[numberOfHists],
                 draw='step',label=labels[filename])
        numberOfHists+=1

    plot = hist.execute()
    hist.savefig()
    print "  :: Saved plot to "+hist.saveAs+"\n"


## THE END
