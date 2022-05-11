#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import argparse
import os
import sys
import ROOT

if __name__== '__main__':

    parser = argparse.ArgumentParser(description='Create a default config file for a specific mu profile file')
    parser.add_argument('--outfile', type=str,
                        help='Output file for PURW tool (default: purw.root)',
                        default='purw.root')
    parser.add_argument('--lcRunNum',type=int,default=0,help="if specified, create a matching dummy lumicalc file")
    parser.add_argument('profileFile', type=str,help="name of the config file - will automatically obtain with get_files")



    args = parser.parse_args()

    if not os.path.exists(args.profileFile):
        os.system("get_files {}".format(args.profileFile))
        if not os.path.exists(args.profileFile):
            print("Could not obtain profile file:",args.profileFile)
            sys.exit(1)

    RunDMC_testing_configuration = True
    def include(s) : return
    exec(open(args.profileFile).read())

    if "JobMaker" not in dir():
        print("Could not find mu profile in",args.profileFile)
        sys.exit(1)

    tool = ROOT.CP.TPileupReweighting("tool")
    tool.UsePeriodConfig("Run2")
    # decide binning based on if first mu is half int or not (override Run2 default)
    if (JobMaker[0]["mu"] * 2) % 2 == 1:
        tool.SetUniformBinning(100,0,100) # half-integer mu values so use this binning
    else:
        tool.SetUniformBinning(100,-0.5,99.5)

    for d in JobMaker:
        # d is a dict, want run, evts, and mu
        tool.Fill(d['run'],0,d['evts'],d['mu'])

    tool.WriteToFile(args.outfile)

    if args.lcRunNum > 0:
        # create a dummy lumicalc file too
        f = ROOT.TFile(f"dummy.lumicalc.{args.lcRunNum}.root","RECREATE")
        t = ROOT.TTree("LumiMetaData","LumiMetaData")
        from array import array
        RunNbr = array('I',[args.lcRunNum]) # unsigned int
        AvergeInteractionPerXing = array('f',[0.])
        IntLumi = array('f',[0.])
        LBStart = array('I',[0])
        t.Branch("RunNbr",RunNbr,"RunNbr/i")
        t.Branch("AvergeInteractionPerXing",AvergeInteractionPerXing,"AvergeInteractionPerXing/F")
        t.Branch("IntLumi",IntLumi,"IntLumi/F")
        t.Branch("LBStart",LBStart,"LBStart/i")
        for d in JobMaker:
            IntLumi[0] = d['evts']
            AvergeInteractionPerXing[0] = d['mu']
            t.Fill()
        t.Write()
        f.Close()

        print("Testing the files",args.outfile,f.GetName())
        ROOT.AAH() # makes possible to use ToolHandles
        testTool = ROOT.ToolHandle['CP::IPileupReweightingTool']("CP::PileupReweightingTool/tool")
        ROOT.AAH.setProperty(testTool,"ConfigFiles",f"['{args.outfile}']")
        ROOT.AAH.setProperty(testTool,"LumiCalcFiles",f"['{f.GetName()}']")
        # must also 'reset' the following properties that have become equal to run2-analysis specific defaults
        ROOT.AAH.setProperty(testTool,"PeriodAssignments","[]")
        ROOT.AAH.setProperty(testTool,"DataScaleFactor","1")
        testTool.retrieve()

        for d in JobMaker:
            weight = testTool.expert().GetCombinedWeight(d['run'],45,d['mu'])
            if abs(weight - 1.)>1e-9:
                print("Unexpected pileup weight",weight,"for mu=",d['mu'])
                sys.exit(-1)
            rrn = testTool.expert().GetRandomRunNumber(d['run'],d['mu'])
            if rrn != args.lcRunNum:
                print("Unexpected random run number",rrn)
                sys.exit(-1)

        print("Test passed")


