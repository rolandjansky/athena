#!/usr/bin/env python

import os
import re
import ROOT as R
from subprocess import Popen, PIPE
from array import array
    
R.gROOT.LoadMacro(os.getcwd() + "/plotting/AtlasStyle/AtlasStyle.C")
R.gROOT.LoadMacro(os.getcwd() + "/plotting/AtlasStyle/AtlasLabels.C")
R.gROOT.LoadMacro(os.getcwd() + "/plotting/AtlasStyle/AtlasUtils.C")

def get_grl(year):
    CVMFS = "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists"
    if year == "15":
        grl = CVMFS + "/data15_13TeV/20190708/data15_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns.xml" 
    elif year == "16":
        grl = CVMFS + "/data16_13TeV/20190708/data16_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_WITH_IGNORES.xml" 
    elif year == "17":
        grl = CVMFS + "/data17_13TeV/20190708/data17_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml" 
    elif year == "18":
        grl = CVMFS + "/data18_13TeV/20190708/data18_13TeV.periodAllYear_DetStatus-v105-pro22-13_Unknown_PHYS_StandardGRL_All_Good_25ns_Triggerno17e33prim.xml" 

    pipe = Popen(["grep", "RunList", grl], stdout=PIPE, stderr=PIPE)
    runs = re.sub("[^0-9,]", "", str(pipe.communicate()[0])).split(",")

    return runs


def setAtlasStyle():
    R.SetAtlasStyle()

def drawAtlasLabel(x, y, text):
    R.ATLASLabel(x, y, text, 1)

def drawText(x, y, text, size=False):
    R.myText(x, y, 1, text, size)


def make_bands(vec_in, stdev, yval):
    vec_y = array('d', [yval] * (len(vec_in) + 2))
    vec_x = array('d', sorted(vec_in))
    err_y = array('d', [stdev] * (len(vec_in) + 2))

    vec_x.insert(0, 0)
    vec_x.append(1595252102)
    
    line = R.TGraphErrors(len(vec_x), vec_x, vec_y, R.nullptr, err_y)
    line.SetFillColorAlpha(8, 0.35)
    line.SetFillStyle(4050)

    return line
