# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# this script can be used to create DCube histograms from the output ntuples of NSWPRDValAlg

import os, sys, ROOT, argparse
import math
from DCubeHistograms import MyHistoFiller

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='createDCubeSimHistograms', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputFile', help='choose input ROOT file', default='NSWPRDValAlg.sim.ntuple.root', type=str)
    parser.add_argument('-o', '--outputFile', help='choose output ROOT file', default='NSWPRDValAlg.dcube.root', type=str)
    parser.add_argument('--doCSC', help='turn off CSC if using Run4 input ROOT file', default=False, action='store_true')
    parser.add_argument('--doMM', help='turn off MM if using Run2 input ROOT file', default=False, action='store_true')
    parser.add_argument('--doSTGC', help='turn off STGC if using Run2 input ROOT file', default=False, action='store_true')
    parser.add_argument('--RPCsel', help='Choose eta_sector selections for RPC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--MDTsel', help='Choose eta_sector selections for MDT, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--CSCsel', help='Choose eta_sector selections for CSC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--TGCsel', help='Choose eta_sector selections for TGC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--MMsel', help='Choose eta_sector selections for MM, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
    parser.add_argument('--sTGCsel', help='Choose eta_sector selections for sTGC, e.g. positive_1 for positive eta and sector 1, None_None for no selection', default='None_None', type=str)
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
    outputFile.mkdir("simulation/")
    ODir = outputFile.GetDirectory("simulation/")
    ODir.cd()
       
    RPCselections = Options.RPCsel.split("_")
    RPC_eta = RPCselections[0]
    if RPCselections[1] != "None":
        RPC_sector = int (RPCselections[1])
    else:
        RPC_sector = RPCselections[1]
    
    MDTselections = Options.MDTsel.split("_")
    MDT_eta = MDTselections[0]
    if MDTselections[1] != "None":
        MDT_sector = int (MDTselections[1])
    else:
        MDT_sector = MDTselections[1]

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
    
    MMselections = Options.MMsel.split("_")
    MM_eta = MMselections[0]
    if MMselections[1] != "None":
        MM_sector = int (MMselections[1])
    else:
        MM_sector = MMselections[1]
    
    sTGCselections = Options.sTGCsel.split("_")
    sTGC_eta = sTGCselections[0]
    if sTGCselections[1] != "None":
        sTGC_sector = int (sTGCselections[1])
    else:
        sTGC_sector = sTGCselections[1]

    # Filling
    for i in range(inputTree.GetEntries()):
        inputTree.GetEntry(i)
        truthhists = []
        rpchists = []
        mdthists = []
        cschists = []
        tgchists = []
        mmhists = []
        stgchists = []
    
        # truth information
        for ntruth in range(0,len(inputTree.MuEntry_Particle_Pt)):
            truthhists += [MyHistoFiller( chamber_name = "TruthInfo", eta_sel = None, sector_sel = None )]
            truthhists[ntruth].fill(inputTree,ntruth)
            
        #RPCs
        if RPC_eta == "positive":
            rpc_eta_sel = lambda t: t.RPC_stationEta[nrpcHit] >= 0
        elif RPC_eta == "negative":
            rpc_eta_sel = lambda t: t.RPC_stationEta[nrpcHit] < 0
        else:
            rpc_eta_sel = lambda t: t.RPC_stationEta[nrpcHit] < 9

        if RPC_sector == "None":
            rpc_sector_sel = lambda s: s.RPC_stationPhi[nrpcHit] < 10
        else:
            rpc_sector_sel = lambda s: s.RPC_stationPhi[nrpcHit] == RPC_sector

        for nrpcHit in range(0,len(inputTree.RPC_hitLocalPositionX)):
            rpchists += [MyHistoFiller( chamber_name = "RPC", eta_sel = rpc_eta_sel, sector_sel = rpc_sector_sel )]
            rpchists[nrpcHit].fill(inputTree, nrpcHit)

        # MDTs
        if MDT_eta == "positive":
            mdt_eta_sel = lambda t: t.MDT_stationEta[nmdtHit] >= 0
        elif MDT_eta == "negative":
            mdt_eta_sel = lambda t: t.MDT_stationEta[nmdtHit] < 0
        else:
            mdt_eta_sel = lambda t: t.MDT_stationEta[nmdtHit] < 9

        if MDT_sector == "None":
            mdt_sector_sel = lambda s: s.MDT_stationPhi[nmdtHit] < 10
        else:
            mdt_sector_sel = lambda s: s.MDT_stationPhi[nmdtHit] == MDT_sector

        for nmdtHit in range(0,len(inputTree.MDT_hitLocalPositionX)):
            mdthists += [MyHistoFiller( chamber_name = "MDT", eta_sel = mdt_eta_sel, sector_sel = mdt_sector_sel )]
            mdthists[nmdtHit].fill(inputTree, nmdtHit)
    
        # CSCs
        if Options.doCSC == True:
            if CSC_eta == "positive":
                csc_eta_sel = lambda t: t.CSC_stationEta[ncscHit] >= 0
            elif CSC_eta == "negative":
                csc_eta_sel = lambda t: t.CSC_stationEta[ncscHit] < 0
            else:
                csc_eta_sel = lambda t: t.CSC_stationEta[ncscHit] < 9

            if CSC_sector == "None":
                csc_sector_sel = lambda s: s.CSC_stationPhi[ncscHit] < 10
            else:
                csc_sector_sel = lambda s: s.CSC_stationPhi[ncscHit] == CSC_sector

            for ncscHit in range(0,len(inputTree.CSC_hitGlobalPositionX)):
                cschists += [MyHistoFiller( chamber_name = "CSC", eta_sel = csc_eta_sel, sector_sel = csc_sector_sel )]
                cschists[ncscHit].fill(inputTree, ncscHit)
        
        # TGCs
        if TGC_eta == "positive":
            tgc_eta_sel = lambda t: t.TGC_stationEta[ntgcHit] >= 0
        elif TGC_eta == "negative":
            tgc_eta_sel = lambda t: t.TGC_stationEta[ntgcHit] < 0
        else:
            tgc_eta_sel = lambda t: t.TGC_stationEta[ntgcHit] < 9

        if TGC_sector == "None":
            tgc_sector_sel = lambda s: s.TGC_stationPhi[ntgcHit] < 51
        else:
            tgc_sector_sel = lambda s: s.TGC_stationPhi[ntgcHit] == TGC_sector

        for ntgcHit in range(0,len(inputTree.TGC_hitLocalPositionX)):
            tgchists += [MyHistoFiller( chamber_name = "TGC", eta_sel = tgc_eta_sel, sector_sel = tgc_sector_sel )]
            tgchists[ntgcHit].fill(inputTree, ntgcHit)

        # MMs
        if Options.doMM == True:
            if MM_eta == "positive":
                mm_eta_sel = lambda t: t.Hits_MM_off_stationEta[nmmHit] >= 0
            elif MM_eta == "negative":
                mm_eta_sel = lambda t: t.Hits_MM_off_stationEta[nmmHit] < 0
            else:
                mm_eta_sel = lambda t: t.Hits_MM_off_stationEta[nmmHit] < 9

            if MM_sector == "None":
                mm_sector_sel = lambda s: s.Hits_MM_off_stationPhi[nmmHit] < 10
            else:
                mm_sector_sel = lambda s: s.Hits_MM_off_stationPhi[nmmHit] == MM_sector

            for nmmHit in range(0,len(inputTree.Hits_MM_hitGlobalPositionX)):
                mmhists += [MyHistoFiller( chamber_name = "MM", eta_sel = mm_eta_sel, sector_sel = mm_sector_sel )]
                mmhists[nmmHit].fill(inputTree, nmmHit)

        # sTGCs
        if Options.doSTGC == True:
            if sTGC_eta == "positive":
                stgc_eta_sel = lambda t: t.Hits_sTGC_off_stationEta[nstgcHit] >= 0
            elif sTGC_eta == "negative":
                stgc_eta_sel = lambda t: t.Hits_sTGC_off_stationEta[nstgcHit] < 0
            else:
                stgc_eta_sel = lambda t: t.Hits_sTGC_off_stationEta[nstgcHit] < 9

            if sTGC_sector == "None":
                stgc_sector_sel = lambda s: s.Hits_sTGC_off_stationPhi[nstgcHit] < 10
            else:
                stgc_sector_sel = lambda s: s.Hits_sTGC_off_stationPhi[nstgcHit] == sTGC_sector

            for nstgcHit in range(0,len(inputTree.Hits_sTGC_hitGlobalPositionX)):
                stgchists += [MyHistoFiller( chamber_name = "sTGC", eta_sel = stgc_eta_sel, sector_sel = stgc_sector_sel )]
                stgchists[nstgcHit].fill(inputTree, nstgcHit) 


    # Writing  
    truthhist = MyHistoFiller( chamber_name = "TruthInfo", eta_sel = None, sector_sel = None )
    truthhist.write(ODir)
          
    rpchist = MyHistoFiller( chamber_name = "RPC", eta_sel = None, sector_sel = None )
    rpchist.write(ODir)

    mdthist = MyHistoFiller( chamber_name = "MDT", eta_sel = None, sector_sel = None )
    mdthist.write(ODir)

    if Options.doCSC == True:
        cschist = MyHistoFiller( chamber_name = "CSC", eta_sel = None, sector_sel = None )
        cschist.write(ODir)

    tgchist = MyHistoFiller( chamber_name = "TGC", eta_sel = None, sector_sel = None )
    tgchist.write(ODir)

    if Options.doMM == True:
        mmhist = MyHistoFiller( chamber_name = "MM", eta_sel = None, sector_sel = None )
        mmhist.write(ODir)

    if Options.doSTGC == True:
        stgchist = MyHistoFiller( chamber_name = "sTGC", eta_sel = None, sector_sel = None )
        stgchist.write(ODir)
