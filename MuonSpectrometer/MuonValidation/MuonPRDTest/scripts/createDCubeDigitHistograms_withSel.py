# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse
import math
from DCubeHistograms import MyHistoFiller

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeSimHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.digi.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.digi.dcube.root', type=str)
    parser.add_argument('--doCSC', help='turn off CSC if using Run4 input ROOT file', default=False, action='store_true')
    parser.add_argument('--CSCsel', help='Choose eta_sector selections for CSC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--TGCsel', help='Choose eta_sector selections for TGC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)

    Options = parser.parse_args()

    ROOT.gROOT.SetBatch(True)

    if not os.path.exists(Options.inputFile):
        print ('ERROR: File %s does not exist'%Options.inputFile)
        sys.exit(1)

    inputFile = ROOT.TFile(Options.inputFile, "READ")
    if not inputFile:
        print ('ERROR: Failed to open file %s'%Options.inputFile)
        sys.exit(1)
    inputTree = inputFile.Get("NSWValTree")
    if not inputTree:
        print ('ERROR: NSWValTree does not exist in file %s'%Options.inputFile)
        sys.exit(1)

    nEntries = inputTree.GetEntries()
    if nEntries==0:
        print ('ERROR: NSWValTree of file %s has 0 entries'%Options.inputFile)
        sys.exit(1)

    outputFile = ROOT.TFile(Options.outputFile, "RECREATE")
    if not outputFile:
        print ('ERROR: Failed to open file %s'%Options.outputFile)
        sys.exit(1)

    outputFile.cd()
    outputFile.mkdir("digitization/")
    ODir = outputFile.GetDirectory("digitization/")
    ODir.cd()
    
    CSCselections = Options.CSCsel.split("_")
    CSC_eta = CSCselections[0]
    if CSCselections[1] != "None":
        CSC_sector = int (CSCselections[1])
    else:
        CSC_sector = CSCselections[1]
        
    TGCselections = Options.TGCsel.split("_")
    TGC_eta = TGCselections[0]
    if TGCselections[1] != "None":
        TGC_sector = int (TGCselections[1])
    else:
        TGC_sector = TGCselections[1]
        
    #Filling
    for i in range(inputTree.GetEntries()):
        inputTree.GetEntry(i)
        cschists = []
        tgchists = []

        # CSCs
        if Options.doCSC == True:
            if CSC_eta == "positive":
                csc_eta_sel = lambda t: t.Digits_CSC_stationEta[ncscDigit] >= 0
            elif CSC_eta == "negative":
                csc_eta_sel = lambda t: t.Digits_CSC_stationEta[ncscDigit] < 0
            else:
                csc_eta_sel = lambda t: t.Digits_CSC_stationEta[ncscDigit] < 9

            if CSC_sector == "None":
                csc_sector_sel = lambda s: s.Digits_CSC_stationPhi[ncscDigit] < 10
            else:
                csc_sector_sel = lambda s: s.Digits_CSC_stationPhi[ncscDigit] == CSC_sector

            for ncscDigit in range(0,len(inputTree.Digits_CSC_truth_localPosX)):
                cschists += [MyHistoFiller( chamber_name = "CSC_Digit", eta_sel = csc_eta_sel, sector_sel = csc_sector_sel )]
                cschists[ncscDigit].fill(inputTree, ncscDigit)
                
        # TGCs
        if TGC_eta == "positive":
            tgc_eta_sel = lambda t: t.Digits_TGC_stationEta[ntgcDigit] >= 0
        elif TGC_eta == "negative":
            tgc_eta_sel = lambda t: t.Digits_TGC_stationEta[ntgcDigit] < 0
        else:
            tgc_eta_sel = lambda t: t.Digits_TGC_stationEta[ntgcDigit] < 9

        if TGC_sector == "None":
            tgc_sector_sel = lambda s: s.Digits_TGC_stationPhi[ntgcDigit] < 51
        else:
            tgc_sector_sel = lambda s: s.Digits_TGC_stationPhi[ntgcDigit] == TGC_sector

        for ntgcDigit in range(0,len(inputTree.Digits_TGC_localPosX)):
            tgchists += [MyHistoFiller( chamber_name = "TGC_Digit", eta_sel = tgc_eta_sel, sector_sel = tgc_sector_sel )]
            tgchists[ntgcDigit].fill(inputTree, ntgcDigit)

    #Writing
    if Options.doCSC == True:
        cschist = MyHistoFiller( chamber_name = "CSC_Digit", eta_sel = None, sector_sel = None )
        cschist.write(ODir)
    
    tgchist = MyHistoFiller( chamber_name = "TGC_Digit", eta_sel = None, sector_sel = None )
    tgchist.write(ODir)
    