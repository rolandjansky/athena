#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

__author__ = "Tim Brooks <brooks@cern.ch>"
__doc__    = """
Python script used to run on SUSY D3PDs. Currently runs baseline and R&D di-lepton analysis.
Usage: 
chmod u+x susycutflow_2lep.py
./susycutflow_2lep.py --help
./susycutflow_2lep.py

Tommaso Lari, 21-apr-2011:
  Output is now written on out.log (unless changed with --out flag)
  Updated trigger strategy. Defaults are ok to run on MC; set --isdataeg
  when running on electron stream and --isdatamu when running on muon stream

based on susycutflow_1lep.py by:
Renaud Bruneliere <Renaud.Bruneliere@cern.ch>
"""

from time import time
from datetime import datetime
from array import array
from math import *
import sys
import subprocess
import copy
import ROOT

def parseCmdLine(args):
    """ Parse input command line to optdict.
    To get the whole list of options type : susycutflow_2lep.py -h"""
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("--out", dest="outfile", help="output",
                      default='')
    parser.add_option("--infile", dest="infile", help="Input file",
                      default='/castor/cern.ch/atlas/atlascerngroupdisk/phys-susy/D4PD/p60x/user.bruneli.mc10_7TeV.106484.SU4_herwigpp_susy.merge.NTUP_SUSY.e598_s933_s946_r2302_r2300_p601_v3/user.bruneli.006134._00001.susy.root') 
    parser.add_option("--isdata", dest="isdata", help="Data or Monte Carlo ?",
                      action='store_true', default=False) 
    parser.add_option("--trgwgt", dest="trgwgt", help="Apply a weight to emulate trigger in MC ?",
                      action='store_true', default=False) 
    parser.add_option("--isdataeg", dest="isdataeg",
                      help="egamma data stream",
                      action='store_true', default=False)
    parser.add_option("--isdatamu", dest="isdatamu",
                      help="muons data stream",
                      action='store_true', default=False)
    parser.add_option("-v", dest="verbose", help="Print detailed information",
                      action='store_true', default=False)
    (config, args) = parser.parse_args(args)
    if config.isdataeg or config.isdatamu: config.isdata = True
    return config

def getinfiles(infile):
    """ Get input files from castor directory.
    Remember to never run directly on castor"""
    ret = subprocess.call('xrdcp root://castoratlas/%s $TMPDIR/' % infile,shell=True)
    if ret != 0: print("getinfiles: could not copy",infile,"\nEXIT")
    newfile = '$TMPDIR/%s' % infile.split('/')[len(infile.split('/'))-1]
    return newfile

def main():
    print("susycutflow_2lep.py: starting", datetime.now() )
    config = parseCmdLine(sys.argv[1:])
    verbose = config.verbose

    ## redefine output
    if config.outfile:
      out_file = open(config.outfile, 'w');
      sys.stdout = out_file  

    ## Read input file
    infiles = config.infile
    t = ROOT.TChain("susy")
    for infile in infiles.split(','):
      print("Loading file " + infile)
      if '/castor/cern.ch' in config.infile: infile = getinfiles(config.infile)
      t.Add(infile)

    ## Load SUSYTools
    ROOT.gSystem.Load("../StandAlone/libSUSYTools.so")
    have_grl = False
    if ROOT.gSystem.Load("../../../../DataQuality/GoodRunsLists/StandAlone/libGoodRunsLists.so") == 0:
        have_grl = True
        grl_reader = ROOT.Root.TGoodRunsListReader('data10_7TeV.pro05.merged_LBSUMM_Susy.xml')
        # grl_reader.AddXMLFile(".xml");
        grl_reader.Interpret()
        goodrunslist = grl_reader.GetMergedGRLCollection()
        goodrunslist.Summary() # Print out a little summary of runs
        #goodrunslist.Summary(True) # Print out a list of all good runs and lbns
    have_pileuptool = False
    if ROOT.gSystem.Load("../../../AnalysisCommon/PileupReweighting/StandAlone/libPileupReweighting.so") == 0:
      have_pileuptool = True
      elPileUpTool = ROOT.Root.TPileupReweighting("Electrons")
      tool_failure = elPileUpTool.initialize("../data/susy_1Electron_avgintperbx.root", "avgintperbx", "../data/mu_mc10b.root", "mu_mc10b");
      #tool_failure = elPileUpTool.initialize("../data/ilumicalc_histograms_EF_e20_medium_178044-183602.root", "avgintperbx", "../data/mu_mc10b.root", "mu_mc10b");
      if tool_failure: print("El PileUpTool failed to initialise with code: %d" %tool_failure)
      muPileUpTool = ROOT.Root.TPileupReweighting("Muons")
      tool_failure = muPileUpTool.initialize("../data/susy_1Muon_avgintperbx.root", "avgintperbx", "../data/mu_mc10b.root", "mu_mc10b");
      #tool_failure = muPileUpTool.initialize("../data/ilumicalc_histograms_EF_mu18_178044-183602.root", "avgintperbx", "../data/mu_mc10b.root", "mu_mc10b");
      if tool_failure: print("MU PileUpTool failed to initialise with code: %d" %tool_failure)
      print("Pileup weights:");
      for i in range(25):
        print("weight(mu = %4.1f) = %13.10f(EL) %13.10f(MU)" %(float(i), elPileUpTool.getPileupWeight(i), muPileUpTool.getPileupWeight(i) ) )
    susyObjDef = ROOT.SUSYObjDef()
    susyObjDef.initialize()

    ## Prepare class for muon trigger rescaling in MC
    trigWeighter = None
    if not config.isdata:
        reweight_file = ROOT.TFile("../data/EF_mu18.root")
        trig_numerator_hist   = reweight_file.Get("passes") # Get the numerator histogram
        trig_denominator_hist = reweight_file.Get("trials") # Get the denominator histogram
        trigWeighter = ROOT.APReweightND(trig_denominator_hist,trig_numerator_hist,True) # Instantiate the tool
        reweight_file.Close()

    ## Loop over events (not at all optimized for speed, more for clarity)
    nEvts = t.GetEntries()
    cuts = ['No cut',
            'GRL',
            'LArError',
            'Electron LArHole veto',
            'Jet LArHole veto',
            'Trigger',
            'Jet/MET Cleaning',
            'Primary vertex cut',
            #'Electron crack removal',
            'Cosmic muon veto',
            'Dilepton requirement',
            'Lepton signs & M_ll > 12GeV',
            'Tight iso electrons',
            'MET > 80 GeV',
            'MET > 250 GeV',
            'Njet(80,40,40) > 3',
            '3jet + MET > 220 GeV',
            'Njet(100,70,70,70) > 4',
            '4jet + MET > 100 GeV',
            'MET > 100 GeV',
            '2jets (pt > 50,50)',
            '2jet MET > 80 GeV']
    OSeecuts = {'No cut': 0,
            'GRL': 0,
            'LArError': 0,
            'Electron LArHole veto': 0,
            'Jet LArHole veto': 0,
            'Trigger': 0,
            'Jet/MET Cleaning': 0,
            'Primary vertex cut': 0,
            #'Electron crack removal': 0,
            'Cosmic muon veto': 0,
            'Dilepton requirement': 0,
            'Lepton signs & M_ll > 12GeV': 0,
            'Tight iso electrons': 0,
            'MET > 80 GeV': 0,
            'MET > 250 GeV': 0,
            'Njet(80,40,40) > 3': 0,
            '3jet + MET > 220 GeV': 0,
            'Njet(100,70,70,70) > 4': 0,
            '4jet + MET > 100 GeV': 0,
            'MET > 100 GeV': 0,
            '2jets (pt > 50,50)': 0,
            '2jet MET > 80 GeV': 0,
            }
    OSeecuts = copy.deepcopy(OSeecuts)
    OSemcuts = copy.deepcopy(OSeecuts)
    OSmecuts = copy.deepcopy(OSeecuts)
    OSmmcuts = copy.deepcopy(OSeecuts)
    SSeecuts = copy.deepcopy(OSeecuts)
    SSemcuts = copy.deepcopy(OSeecuts)
    SSmecuts = copy.deepcopy(OSeecuts)
    SSmmcuts = copy.deepcopy(OSeecuts)
    setTime = time()
    for iEvt in range(nEvts):
        if time() - setTime > 15:
          print("Reading event",iEvt,"/",nEvts)
          setTime = time()
        t.GetEntry(iEvt)

        ## Do not forget to clear arrays once per event
        susyObjDef.Reset()

        ## Get electrons
        ## WARNING: with 2011 data there is a new flag el_OQ to check object quality
        el_idx = {'signal':[],'baseline':[],'crack':[],'met':ROOT.std.vector(int)()}
        for iEl in range(t.el_n):
            # Apply baseline SUSY electron selection 
            # On Monte Carlo, if you need rejecting electrons in dead regions of LAr, then set t.RunNumber = 180614
            # For further information, please check https://twiki.cern.ch/twiki/bin/view/AtlasProtected/LArCleaningAndObjectQuality#Dealing_with_LAr_hardware_proble
            myRunNb = 180614 # Reject electrons in LAr hole by default in MC
            if susyObjDef.IsElectron(iEl,myRunNb,
                                     t.el_cl_eta.at(iEl),t.el_cl_phi.at(iEl),t.el_cl_E.at(iEl),
                                     t.el_tracketa.at(iEl),t.el_trackphi.at(iEl),
                                     t.el_author.at(iEl),t.el_isEM.at(iEl),t.el_OQ.at(iEl),
                                     t.el_nPixHits.at(iEl),t.el_nSCTHits.at(iEl),
                                     config.isdata,20000,2.47,False):
                el_idx['met'].push_back(iEl)
                el_idx['baseline'].append(iEl)
                # Check whether a baseline electron is within crack region
                if susyObjDef.IsInCrack(t.el_cl_eta.at(iEl)):
                    el_idx['crack'].append(iEl)
                # Apply additionnal cuts for signal electrons 
                # Electron pt cut is 20 GeV, with a leading electron above 25GeV for e20_medium trigger
                # Electron ptcone20/pt must be < 0.1
                if susyObjDef.IsSignalElectron(iEl,t.el_isEM.at(iEl),t.el_ptcone20.at(iEl),20000.,0.1):
                    el_idx['signal'].append(iEl)

        ## Get muons
        mu_idx = {'signal':[],'baseline':[],'cosmic':[],'met':ROOT.std.vector(int)()}
        for iMu in range(t.mu_staco_n):
            # Baseline muon selection does not include any isolation cut => directly used in Etmiss computation
            if susyObjDef.IsMuon(iMu,t.mu_staco_pt.at(iMu),t.mu_staco_eta.at(iMu),t.mu_staco_phi.at(iMu),
                                 t.mu_staco_E.at(iMu),t.mu_staco_me_qoverp_exPV.at(iMu),
                                 t.mu_staco_id_qoverp_exPV.at(iMu),t.mu_staco_me_theta_exPV.at(iMu),
                                 t.mu_staco_id_theta_exPV.at(iMu),t.mu_staco_id_theta.at(iMu),
                                 t.mu_staco_isCombinedMuon.at(iMu),t.mu_staco_isLowPtReconstructedMuon.at(iMu),
                                 t.mu_staco_loose.at(iMu),
                                 t.mu_staco_expectBLayerHit.at(iMu),t.mu_staco_nBLHits.at(iMu),
                                 t.mu_staco_nPixHits.at(iMu),t.mu_staco_nPixelDeadSensors.at(iMu),
                                 t.mu_staco_nPixHoles.at(iMu),t.mu_staco_nSCTHits.at(iMu),
                                 t.mu_staco_nSCTDeadSensors.at(iMu),t.mu_staco_nSCTHoles.at(iMu),
                                 t.mu_staco_nTRTHits.at(iMu),t.mu_staco_nTRTOutliers.at(iMu),
                                 config.isdata,10000.,2.4):
                mu_idx['met'].push_back(iMu)
                mu_idx['baseline'].append(iMu)
                # Additionnal cut for signal muons: check whether it also pass isolation cut and if pT>10 GeV
                # Add leading muon cut of 20 GeV later
                if susyObjDef.IsSignalMuon(iMu,t.mu_staco_ptcone20.at(iMu),10000.): 
                    mu_idx['signal'].append(iMu)
                # Check whether muon could be a cosmic candidate
                if susyObjDef.IsCosmicMuon(t.mu_staco_z0_exPV.at(iMu),t.mu_staco_d0_exPV.at(iMu), 1.0, 0.2):
                    mu_idx['cosmic'].append(iMu)

        ## Get jets
        ## It is assumed that no sorting of jets as function of jet pT is necessary with AntiKt4TopoNewEM jets
        jet_idx = {'good':[],'bad':[],'LArHoleVeto':[],'goodpt':[]}
        for iJet in range(t.jet_AntiKt4TopoNewEM_n):
            ## LAr hole veto applies to any jet (before overlap removal)
            ## Recommend to apply jet veto only to N leading jets in an N jet channel
            if susyObjDef.IsLArHoleVeto(t.jet_AntiKt4TopoNewEM_pt.at(iJet),t.jet_AntiKt4TopoNewEM_eta.at(iJet),
                                                     t.jet_AntiKt4TopoNewEM_phi.at(iJet),
                                                     t.jet_AntiKt4TopoNewEM_BCH_CORR_JET.at(iJet), 
                                                     t.jet_AntiKt4TopoNewEM_BCH_CORR_CELL.at(iJet), 
                                                     config.isdata,40000.):
                ## Skip event
                jet_idx['LArHoleVeto'].append(iJet)

            ## Do not apply any eta cut for jet cleaning
            ## Remember jet cleaning is applied only to data
            isgoodjet = False
            if susyObjDef.IsJet(iJet,t.jet_AntiKt4TopoNewEM_pt.at(iJet),t.jet_AntiKt4TopoNewEM_eta.at(iJet),
                                t.jet_AntiKt4TopoNewEM_phi.at(iJet),t.jet_AntiKt4TopoNewEM_E.at(iJet),
                                t.jet_AntiKt4TopoNewEM_emscale_eta.at(iJet),
                                t.jet_AntiKt4TopoNewEM_emfrac.at(iJet),t.jet_AntiKt4TopoNewEM_hecf.at(iJet),
                                t.jet_AntiKt4TopoNewEM_LArQuality.at(iJet),t.jet_AntiKt4TopoNewEM_HECQuality.at(iJet),
                                t.jet_AntiKt4TopoNewEM_Timing.at(iJet),t.jet_AntiKt4TopoNewEM_sumPtTrk.at(iJet),
                                t.jet_AntiKt4TopoNewEM_fracSamplingMax.at(iJet),t.jet_AntiKt4TopoNewEM_NegativeE.at(iJet),
                                config.isdata,20000.,10.):
                isgoodjet = True

            ## Consider only jets with pT > 20 GeV for following
            if t.jet_AntiKt4TopoNewEM_pt.at(iJet) <= 20000.: continue

            ## Check overlap removal with electrons, skip jets overlapping with electrons
            isoverlap = False
            for iEl in el_idx['baseline']:
                if susyObjDef.GetElecTLV(iEl).DeltaR(susyObjDef.GetJetTLV(iJet)) > 0.2: continue
                isoverlap = True
                break
            if isoverlap: continue

            ## Flag remaining jets
            if not isgoodjet:
                jet_idx['bad'].append(iJet)
            elif isgoodjet and fabs(t.jet_AntiKt4TopoNewEM_eta.at(iJet)) < 2.8:
                jet_idx['goodpt'].append(t.jet_AntiKt4TopoNewEM_pt.at(iJet))
                jet_idx['good'].append(iJet)
              
        ## Apply descendent pT sorting of jets
        ptandidx = zip(jet_idx['goodpt'],jet_idx['good'])
        ptandidx.sort(reverse=True)
        del jet_idx['good'][:],jet_idx['goodpt'][:]
        jet_idx['good']   = [ x[1] for x in ptandidx ]
        jet_idx['goodpt'] = [ x[0] for x in ptandidx ]

        ## Compute corrected Etmiss (make use of mu_idx['met'] which contains list of muons to be included in Etmiss)
        ## First compute Etx and Ety up to abs(eta)<4.5 (jet/etmiss wg recommendations)
        etx_RefFinal = t.MET_Simplified20_RefFinal_etx_CentralReg + t.MET_Simplified20_RefFinal_etx_EndcapRegion + t.MET_Simplified20_RefFinal_etx_ForwardReg
        ety_RefFinal = t.MET_Simplified20_RefFinal_ety_CentralReg + t.MET_Simplified20_RefFinal_ety_EndcapRegion + t.MET_Simplified20_RefFinal_ety_ForwardReg
        etx_RefJet = t.MET_Simplified20_RefJet_etx_CentralReg + t.MET_Simplified20_RefJet_etx_EndcapRegion + t.MET_Simplified20_RefJet_etx_ForwardReg
        ety_RefJet = t.MET_Simplified20_RefJet_ety_CentralReg + t.MET_Simplified20_RefJet_ety_EndcapRegion + t.MET_Simplified20_RefJet_ety_ForwardReg
        met = susyObjDef.GetMET(etx_RefFinal,ety_RefFinal,etx_RefJet,ety_RefJet,
                                t.MET_Simplified20_Muon_Total_Staco_etx,t.MET_Simplified20_Muon_Total_Staco_ety,
                                mu_idx['met'],el_idx['met'],False)

        ## Apply lepton veto
        for iJet in jet_idx['good']:
            mylist = copy.deepcopy(el_idx['baseline'])
            for iEl in mylist:
                if susyObjDef.GetElecTLV(iEl).DeltaR(susyObjDef.GetJetTLV(iJet)) > 0.4: continue
                el_idx['baseline'].remove(iEl)
                if iEl in el_idx['signal']: el_idx['signal'].remove(iEl)
            mylist = copy.deepcopy(mu_idx['baseline'])
            for iMu in mylist:
                if susyObjDef.GetMuonTLV(iMu).DeltaR(susyObjDef.GetJetTLV(iJet)) > 0.4: continue
                mu_idx['baseline'].remove(iMu)
                if iMu in mu_idx['cosmic']: mu_idx['cosmic'].remove(iMu) # Check cosmic cut only if sufficiently away from a jet
                if iMu in mu_idx['signal']: mu_idx['signal'].remove(iMu)

        ## Make cuts
        wgt = {'el': 1., 'mu': 1.} # el,mu channels respectively
        if not config.isdata:
          if have_pileuptool:
            wgt['el'] *= elPileUpTool.getPileupWeight(t.lbn)
            wgt['mu'] *= muPileUpTool.getPileupWeight(t.lbn)
        passel = True
        passmu = True
        OSeecuts['No cut'] += wgt['el'] 
        OSemcuts['No cut'] += wgt['el']
        OSmecuts['No cut'] += wgt['mu']
        OSmmcuts['No cut'] += wgt['mu']
        SSeecuts['No cut'] += wgt['el'] 
        SSemcuts['No cut'] += wgt['el'] 
        SSmecuts['No cut'] += wgt['mu'] 
        SSmmcuts['No cut'] += wgt['mu']
        # Apply GRL selection if we loaded the reader OK
        if not have_grl or not config.isdata or goodrunslist.HasRunLumiBlock(t.RunNumber, t.lbn):
          OSeecuts['GRL'] += wgt['el']
          OSemcuts['GRL'] += wgt['el']
          OSmecuts['GRL'] += wgt['mu']
          OSmmcuts['GRL'] += wgt['mu']
          SSeecuts['GRL'] += wgt['el']
          SSemcuts['GRL'] += wgt['el']
          SSmecuts['GRL'] += wgt['mu']
          SSmmcuts['GRL'] += wgt['mu']
        else:
          passel = passmu = False
        if passel and (t.larError == 0 or not config.isdata):
          OSeecuts['LArError'] += wgt['el']
          OSemcuts['LArError'] += wgt['el']
          OSmecuts['LArError'] += wgt['mu']
          OSmmcuts['LArError'] += wgt['mu']
          SSeecuts['LArError'] += wgt['el']
          SSemcuts['LArError'] += wgt['el']
          SSmecuts['LArError'] += wgt['mu']
          SSmmcuts['LArError'] += wgt['mu']
        else:
          passel = passmu = False
        if passel:
          for el in el_idx['baseline']:
            el_tlv = susyObjDef.GetElecTLV(el)
            if susyObjDef.IsLArHole(t.el_cl_eta[el], t.el_cl_phi[el]):
              passel = passmu = False
        if passel:
          OSeecuts['Electron LArHole veto'] += wgt['el']
          OSemcuts['Electron LArHole veto'] += wgt['el']
          OSmecuts['Electron LArHole veto'] += wgt['mu']
          OSmmcuts['Electron LArHole veto'] += wgt['mu']
          SSeecuts['Electron LArHole veto'] += wgt['el']
          SSemcuts['Electron LArHole veto'] += wgt['el']
          SSmecuts['Electron LArHole veto'] += wgt['mu']
          SSmmcuts['Electron LArHole veto'] += wgt['mu']
        if passel and not len(jet_idx['LArHoleVeto']):
          OSeecuts['Jet LArHole veto'] += wgt['el']
          OSemcuts['Jet LArHole veto'] += wgt['el']
          OSmecuts['Jet LArHole veto'] += wgt['mu']
          OSmmcuts['Jet LArHole veto'] += wgt['mu']
          SSeecuts['Jet LArHole veto'] += wgt['el']
          SSemcuts['Jet LArHole veto'] += wgt['el']
          SSmecuts['Jet LArHole veto'] += wgt['mu']
          SSmmcuts['Jet LArHole veto'] += wgt['mu']
        else:
          passel = passmu = False
        # Trigger
        if passel:
          if config.isdata:
            if config.isdataeg or config.isdatamu:
              trig_el = t.EF_e20_medium and config.isdataeg
              trig_mu = t.EF_mu18 and config.isdatamu
            else:
              trig_el = t.EF_e20_medium
              trig_mu = t.EF_mu18
          else:
            trig_el = t.EF_e20_medium
            if config.trgwgt:
              for iMu in range(t.mu_staco_n):
                if susyObjDef.MuonHasTriggerMatch(iMu, t.mu_staco_n, t.trig_EF_trigmuonef_n, t.trig_EF_trigmuonef_track_n, t.mu_staco_pt,
                        t.mu_staco_eta, t.mu_staco_phi, t.trig_EF_trigmuonef_track_CB_pt, t.trig_EF_trigmuonef_track_CB_eta,
                        t.trig_EF_trigmuonef_track_CB_phi, t.trig_EF_trigmuonef_track_CB_hasCB):
                  value_array = array('d',[t.mu_staco_pt.at(iMu),t.mu_staco_eta.at(iMu),t.mu_staco_phi.at(iMu),t.mu_staco_ptcone20.at(iMu),float(t.mu_staco_isCombinedMuon.at(iMu) == True)])
                  muon_weight = ROOT.APEvtWeight()
                  muon_weight.AddWeightToEvt(trigWeighter.GetWeight(value_array))
                  mu_SF = muon_weight.GetWeight()
                  print("Muon SF: %12.6f event %8d trigger_muon_pt = %12.6f" %(mu_SF, t.EventNumber, t.mu_staco_pt.at(iMu) ) )
                  wgt['mu'] *= mu_SF
                  trig_mu = True
                  break
              else: trig_mu = False
            else:
              trig_mu = t.EF_mu18
        
        if passel and trig_el: 
            OSeecuts['Trigger'] += wgt['el']
            OSemcuts['Trigger'] += wgt['el']
            SSeecuts['Trigger'] += wgt['el']
            SSemcuts['Trigger'] += wgt['el']
        else:
            passel = False
        if passmu and trig_mu: # mu triggers are simulated by weighting
            OSmecuts['Trigger'] += wgt['mu']
            OSmmcuts['Trigger'] += wgt['mu']
            SSmecuts['Trigger'] += wgt['mu']
            SSmmcuts['Trigger'] += wgt['mu']
        else:
            passmu = False
        # Jet/MET cleaning cut
        if len(jet_idx['bad']) == 0:
            if passel:
              OSeecuts['Jet/MET Cleaning'] += wgt['el']
              OSemcuts['Jet/MET Cleaning'] += wgt['el']
              SSeecuts['Jet/MET Cleaning'] += wgt['el']
              SSemcuts['Jet/MET Cleaning'] += wgt['el']
            if passmu: 
              OSmecuts['Jet/MET Cleaning'] += wgt['mu']
              OSmmcuts['Jet/MET Cleaning'] += wgt['mu']
              SSmecuts['Jet/MET Cleaning'] += wgt['mu']
              SSmmcuts['Jet/MET Cleaning'] += wgt['mu']
        else:
            if (passel or passmu) and verbose: print("BadJet: %8d; jet: %2d" %(t.EventNumber, jet_idx['bad'][0]) )
            if config.isdata: passel = passmu = False
        # Good primary vertex
        #if susyObjDef.IsGoodVertex(t.vx_nTracks):
        if t.vx_nTracks.at(0) > 4:
            if passel:
              OSeecuts['Primary vertex cut'] += wgt['el']
              OSemcuts['Primary vertex cut'] += wgt['el']
              SSeecuts['Primary vertex cut'] += wgt['el']
              SSemcuts['Primary vertex cut'] += wgt['el']
            if passmu: 
              OSmecuts['Primary vertex cut'] += wgt['mu']
              OSmmcuts['Primary vertex cut'] += wgt['mu']
              SSmecuts['Primary vertex cut'] += wgt['mu']
              SSmmcuts['Primary vertex cut'] += wgt['mu']
        else:
            if (passel or passmu) and verbose: print("BadVertex: %8d; %1d tracks" %(t.EventNumber, t.vx_nTracks.at(0) ) )
            passel = passmu = False
        # Cosmic muon event veto:
        if len(mu_idx['cosmic']) == 0:
            if passel:
              OSeecuts['Cosmic muon veto'] += wgt['el']
              OSemcuts['Cosmic muon veto'] += wgt['el']
              SSeecuts['Cosmic muon veto'] += wgt['el']
              SSemcuts['Cosmic muon veto'] += wgt['el']
            if passmu: 
              OSmecuts['Cosmic muon veto'] += wgt['mu']
              OSmmcuts['Cosmic muon veto'] += wgt['mu']
              SSmecuts['Cosmic muon veto'] += wgt['mu']
              SSmmcuts['Cosmic muon veto'] += wgt['mu']
        else:
          if (passel or passmu) and verbose: print("CosmicMuon: %d" %t.EventNumber)
          passel = passmu = False
        ## Now the dilepton cutflow diverges:
        ## Check leading lepton (as per trigger) is > 25GeV for electrons and > 20GeV for muons
        passOSee = passSSee = passel and len(mu_idx['signal']) == 0 and len(el_idx['baseline']) == 2
        if passOSee or passSSee:
          iE1 = el_idx['baseline'][0]
          iE2 = el_idx['baseline'][1]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lE2 = susyObjDef.GetElecTLV(iE2)
          if lE1.Pt() > 25000 or lE2.Pt() > 25000:
            OSeecuts['Dilepton requirement'] += wgt['el']
            SSeecuts['Dilepton requirement'] += wgt['el']
          else: passOSee = passSSee = False
        ## EMU channel with electron trigger. Electron must have 25 GeV
        passOSem = passSSem = passel and len(mu_idx['signal']) == 1 and len(el_idx['baseline']) == 1
        if passOSem or passSSem:
          iE1 = el_idx['baseline'][0]
          iM1 = mu_idx['signal'][0]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lM1 = susyObjDef.GetMuonTLV(iM1)
          if lE1.Pt() > 25000:
            OSemcuts['Dilepton requirement'] += wgt['el']
            SSemcuts['Dilepton requirement'] += wgt['el']
          else: passOSem = passSSem = False
        ## EMU channel with muon trigger. Muon must have 20 GeV
        ## and overlap with electron-triggered events must be removed
        passOSme = passSSme = passmu and len(mu_idx['signal']) == 1 and len(el_idx['baseline']) == 1
        if passOSme or passSSem:
          iE1 = el_idx['baseline'][0]
          iM1 = mu_idx['signal'][0]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lM1 = susyObjDef.GetMuonTLV(iM1)
          if lM1.Pt() > 20000 and (lE1.Pt() < 25000 or not trig_el):
            OSmecuts['Dilepton requirement'] += wgt['mu']
            SSmecuts['Dilepton requirement'] += wgt['mu']
          else: passOSme = passSSme = False
        ## MUMU
        passOSmm = passSSmm = passmu and len(mu_idx['signal']) == 2 and len(el_idx['baseline']) == 0
        if passOSmm or passSSmm:
          iM1 = mu_idx['signal'][0]
          iM2 = mu_idx['signal'][1]
          lM1 = susyObjDef.GetMuonTLV(iM1)
          lM2 = susyObjDef.GetMuonTLV(iM2)
          if lM1.Pt() > 20000 or lM2.Pt() > 20000:
            OSmmcuts['Dilepton requirement'] += wgt['mu']
            SSmmcuts['Dilepton requirement'] += wgt['mu']
          else: passOSmm = passSSmm = False

        # OS and M_ll requirement
        if passOSee or passSSee:
          iE1 = el_idx['baseline'][0]
          iE2 = el_idx['baseline'][1]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lE2 = susyObjDef.GetElecTLV(iE2)
          cE1 = t.el_charge.at(iE1)
          cE2 = t.el_charge.at(iE2)
          mDl = (lE1 + lE2).M()
          if cE1*cE2 < 0 and mDl > 12000:
            OSeecuts['Lepton signs & M_ll > 12GeV'] += wgt['el']
          else: passOSee = False
          if cE1*cE2 > 0 and mDl > 12000:
            SSeecuts['Lepton signs & M_ll > 12GeV'] += wgt['el']
          else: passSSee = False

        if passOSem or passSSem:
          iE1 = el_idx['baseline'][0]
          iM1 = mu_idx['signal'][0]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lM1 = susyObjDef.GetMuonTLV(iM1)
          cE1 = t.el_charge.at(iE1)
          cM1 = t.mu_staco_charge.at(iM1)
          mDl = (lE1 + lM1).M()
          if cE1*cM1 < 0 and mDl > 12000:
            OSemcuts['Lepton signs & M_ll > 12GeV'] += wgt['el']
          else: passOSem = False
          if cE1*cM1 > 0 and mDl > 12000:
            SSemcuts['Lepton signs & M_ll > 12GeV'] += wgt['el']
          else: passSSem = False

        if passOSme or passSSme:
          iE1 = el_idx['baseline'][0]
          iM1 = mu_idx['signal'][0]
          lE1 = susyObjDef.GetElecTLV(iE1)
          lM1 = susyObjDef.GetMuonTLV(iM1)
          cE1 = t.el_charge.at(iE1)
          cM1 = t.mu_staco_charge.at(iM1)
          mDl = (lE1 + lM1).M()
          if cE1*cM1 < 0 and mDl > 12000:
            OSmecuts['Lepton signs & M_ll > 12GeV'] += wgt['mu']
          else: passOSme = False
          if cE1*cM1 > 0 and mDl > 12000:
            SSmecuts['Lepton signs & M_ll > 12GeV'] += wgt['mu']
          else: passSSme = False

        if passOSmm or passSSmm:
          iM1 = mu_idx['signal'][0]
          iM2 = mu_idx['signal'][1]
          lM1 = susyObjDef.GetMuonTLV(iM1)
          lM2 = susyObjDef.GetMuonTLV(iM2)
          cM1 = t.mu_staco_charge.at(iM1)
          cM2 = t.mu_staco_charge.at(iM2)
          mDl = (lM1 + lM2).M()
          if cM1*cM2 < 0 and mDl > 12000:
            OSmmcuts['Lepton signs & M_ll > 12GeV'] += wgt['mu']
          else: passOSmm = False
          if cM1*cM2 > 0 and mDl > 12000:
            SSmmcuts['Lepton signs & M_ll > 12GeV'] += wgt['mu']
          else: passSSmm = False

        # Calculate the mT and meff
        pt = [0, 0]
        phi = [0, 0]
        if passOSee or passSSee:
            pt = [susyObjDef.GetElecTLV(el_idx['baseline'][0]).Pt(), susyObjDef.GetElecTLV(el_idx['baseline'][1]).Pt()]
            phi = [susyObjDef.GetElecTLV(el_idx['baseline'][0]).Phi(), susyObjDef.GetElecTLV(el_idx['baseline'][1]).Phi()]
        if passOSem or passSSem or passOSme or passSSme:
            pt = [susyObjDef.GetElecTLV(el_idx['baseline'][0]).Pt(), susyObjDef.GetMuonTLV(mu_idx['signal'][0]).Pt()]
            phi = [susyObjDef.GetElecTLV(el_idx['baseline'][0]).Phi(), susyObjDef.GetMuonTLV(mu_idx['signal'][0]).Phi()]
        if passOSmm or passSSmm:
            pt = [susyObjDef.GetMuonTLV(mu_idx['signal'][0]).Pt(), susyObjDef.GetMuonTLV(mu_idx['signal'][1]).Pt()]
            phi = [susyObjDef.GetMuonTLV(mu_idx['signal'][0]).Phi(), susyObjDef.GetMuonTLV(mu_idx['signal'][1]).Phi()]
        if pt[1] > pt[0]:
            pt.reverse()
            phi.reverse()
        mT = sqrt(2.*pt[0]*met.Mod()*(1. - cos(ROOT.TVector2.Phi_mpi_pi(phi[0] - met.Phi()))))
        meff = met.Mod() + pt[0] + pt[1]
        for jet in jet_idx['good']:
            meff += susyObjDef.GetJetTLV(jet).Pt()

        passOSee = passOSee and len(el_idx['signal']) == 2 and len(mu_idx['signal']) == 0
        if passOSee:
          OSeecuts['Tight iso electrons'] += wgt['el']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f OSEE" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        passSSee = passSSee and len(el_idx['signal']) == 2 and len(mu_idx['signal']) == 0
        if passSSee:
          SSeecuts['Tight iso electrons'] += wgt['el']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f SSEE" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        passOSem = passOSem and len(el_idx['signal']) == 1 and len(mu_idx['signal']) == 1
        if passOSem:
          OSemcuts['Tight iso electrons'] += wgt['el']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f OSEM" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        passSSem = passSSem and len(el_idx['signal']) == 1 and len(mu_idx['signal']) == 1
        if passSSem:
          SSemcuts['Tight iso electrons'] += wgt['el']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f SSEM" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        passOSme = passOSme and len(mu_idx['signal']) == 1 and len(el_idx['signal']) == 1
        if passOSme:
          OSmecuts['Tight iso electrons'] += wgt['mu']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f OSME" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        passSSme = passSSme and len(mu_idx['signal']) == 1 and len(el_idx['signal']) == 1
        if passSSme:
          SSmecuts['Tight iso electrons'] += wgt['mu']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f SSME" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        if passOSmm:
          OSmmcuts['Tight iso electrons'] += wgt['mu']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f OSMM" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )
        if passSSmm:
          SSmmcuts['Tight iso electrons'] += wgt['mu']
          if verbose: print("SignalEvent: (%6d;%4d;%8d) %2d muons, %2d electrons, M_ll: %8.3f, M_Et: %8.3f, M_Eff: %8.3f SSMM" %(t.RunNumber, t.lbn, t.EventNumber, len(mu_idx['signal']), len(el_idx['baseline']), mDl/1000, met.Mod()/1000, meff/1000) )

        ## Compute event weight for Monte Carlo when selecting leptons
        ## event weight is computed after overlap removal (could be debated)
        ## Still need to figure out whether noniso muons should also be rescaled ? (they are good combined muons) 
        ## WARNING: Do NOT reweight MC when comparing to new 2011 data
        if not config.isdata:
            lepton_SF = 1.0
            for iEl in el_idx['signal']:
                lepton_SF *= susyObjDef.GetSignalElecSF(t.el_cl_eta.at(iEl),t.el_cl_E.at(iEl)/cosh(t.el_cl_eta.at(iEl)))
            for iMu in mu_idx['signal']:
                lepton_SF *= susyObjDef.GetSignalMuonSF(iMu)
            wgt['el'] *= lepton_SF
            wgt['mu'] *= lepton_SF
        # Save current cut state for R&D signal cuts
        retainOSee = passOSee
        retainOSem = passOSem
        retainOSme = passOSme
        retainOSmm = passOSmm
        retainSSee = passSSee
        retainSSem = passSSem
        retainSSme = passSSme
        retainSSmm = passSSmm
        # MET cut
        if met.Mod() > 80000.:
            if passOSee: OSeecuts['MET > 80 GeV'] += wgt['el']
            if passOSem: OSemcuts['MET > 80 GeV'] += wgt['el']
            if passOSme: OSmecuts['MET > 80 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['MET > 80 GeV'] += wgt['mu']
            if passSSee: SSeecuts['MET > 80 GeV'] += wgt['el']
            if passSSem: SSemcuts['MET > 80 GeV'] += wgt['el']
            if passSSme: SSmecuts['MET > 80 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['MET > 80 GeV'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False
        # MET cut
        if met.Mod() > 250000.:
            if passOSee:
              OSeecuts['MET > 250 GeV'] += wgt['el']
            if passOSem: OSemcuts['MET > 250 GeV'] += wgt['el']
            if passOSme: OSmecuts['MET > 250 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['MET > 250 GeV'] += wgt['mu']
            if passSSee: SSeecuts['MET > 250 GeV'] += wgt['el']
            if passSSem: SSemcuts['MET > 250 GeV'] += wgt['el']
            if passSSme: SSmecuts['MET > 250 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['MET > 250 GeV'] += wgt['mu']

        # New R&D cuts start here
        passOSee = retainOSee
        passOSem = retainOSem
        passOSme = retainOSme
        passOSmm = retainOSmm
        passSSee = retainSSee
        passSSem = retainSSem
        passSSme = retainSSme
        passSSmm = retainSSmm

        # 3 jet signal region
        # Check jet pT > 80, 40, 40 GeV
        if len(jet_idx['good']) >= 3 and susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt() > 80000.0 \
          and susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt() > 40000.0 \
          and susyObjDef.GetJetTLV(jet_idx['good'][2]).Pt() > 40000.0:
            if verbose and (passOSee or passOSem or passOSme or passOSmm or passSSee or passSSem or passSSme or passSSmm):
              print("SignalEvent3j: (%6d;%4d;%8d) %2d jets, Pt1: %8.3f, Pt2: %8.3f, Pt3: %8.3f, M_Et: %8.3f, M_T: %8.3f" %(t.RunNumber, \
                    t.lbn, t.EventNumber, len(jet_idx['good']), susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt(), susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt(), \
                    susyObjDef.GetJetTLV(jet_idx['good'][2]).Pt(), met.Mod()/1000, mT/1000) )
            if passOSee: OSeecuts['Njet(80,40,40) > 3'] += wgt['el']
            if passOSem: OSemcuts['Njet(80,40,40) > 3'] += wgt['el']
            if passOSme: OSmecuts['Njet(80,40,40) > 3'] += wgt['mu']
            if passOSmm: OSmmcuts['Njet(80,40,40) > 3'] += wgt['mu']
            if passSSee: SSeecuts['Njet(80,40,40) > 3'] += wgt['el']
            if passSSem: SSemcuts['Njet(80,40,40) > 3'] += wgt['el']
            if passSSme: SSmecuts['Njet(80,40,40) > 3'] += wgt['mu']
            if passSSmm: SSmmcuts['Njet(80,40,40) > 3'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False
        # MET cut
        if met.Mod() > 220000.:
            if passOSee:
              OSeecuts['3jet + MET > 220 GeV'] += wgt['el']
            if passOSem: OSemcuts['3jet + MET > 220 GeV'] += wgt['el']
            if passOSme: OSmecuts['3jet + MET > 220 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['3jet + MET > 220 GeV'] += wgt['mu']
            if passSSee: SSeecuts['3jet + MET > 220 GeV'] += wgt['el']
            if passSSem: SSemcuts['3jet + MET > 220 GeV'] += wgt['el']
            if passSSme: SSmecuts['3jet + MET > 220 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['3jet + MET > 220 GeV'] += wgt['mu']

        # 4 jet signal region
        passOSee = retainOSee
        passOSem = retainOSem
        passOSme = retainOSme
        passOSmm = retainOSmm
        passSSee = retainSSee
        passSSem = retainSSem
        passSSme = retainSSme
        passSSmm = retainSSmm
        if len(jet_idx['good']) >= 4 and susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt() > 100000.0 \
          and susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt() > 70000.0 \
          and susyObjDef.GetJetTLV(jet_idx['good'][2]).Pt() > 70000.0 \
          and susyObjDef.GetJetTLV(jet_idx['good'][3]).Pt() > 70000.0:
            if verbose and (passOSee or passOSem or passOSme or passOSmm or passSSee or passSSem or passSSme or passSSmm):
              print("SignalEvent4j: (%6d;%4d;%8d) %2d jets, Pt1: %8.3f, Pt2: %8.3f, Pt3: %8.3f, Pt4: %8.3f, M_Et: %8.3f, M_T: %8.3f" %(t.RunNumber, \
                    t.lbn, t.EventNumber, len(jet_idx['good']), susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt(), susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt(), \
                    susyObjDef.GetJetTLV(jet_idx['good'][2]).Pt(), susyObjDef.GetJetTLV(jet_idx['good'][3]).Pt(), met.Mod()/1000, mT/1000) )
            if passOSee: OSeecuts['Njet(100,70,70,70) > 4'] += wgt['el']
            if passOSem: OSemcuts['Njet(100,70,70,70) > 4'] += wgt['el']
            if passOSme: OSmecuts['Njet(100,70,70,70) > 4'] += wgt['mu']
            if passOSmm: OSmmcuts['Njet(100,70,70,70) > 4'] += wgt['mu']
            if passSSee: SSeecuts['Njet(100,70,70,70) > 4'] += wgt['el']
            if passSSem: SSemcuts['Njet(100,70,70,70) > 4'] += wgt['el']
            if passSSme: SSmecuts['Njet(100,70,70,70) > 4'] += wgt['mu']
            if passSSmm: SSmmcuts['Njet(100,70,70,70) > 4'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False
        # MET cut
        if met.Mod() > 100000.:
            if passOSee: OSeecuts['4jet + MET > 100 GeV'] += wgt['el']
            if passOSem: OSemcuts['4jet + MET > 100 GeV'] += wgt['el']
            if passOSme: OSmecuts['4jet + MET > 100 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['4jet + MET > 100 GeV'] += wgt['mu']
            if passSSee: SSeecuts['4jet + MET > 100 GeV'] += wgt['el']
            if passSSem: SSemcuts['4jet + MET > 100 GeV'] += wgt['el']
            if passSSme: SSmecuts['4jet + MET > 100 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['4jet + MET > 100 GeV'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False

        # New SS signal region
        passOSee = retainOSee
        passOSem = retainOSem
        passOSme = retainOSme
        passOSmm = retainOSmm
        passSSee = retainSSee
        passSSem = retainSSem
        passSSme = retainSSme
        passSSmm = retainSSmm
        # MET cut
        if met.Mod() > 100000.:
            if passOSee: OSeecuts['MET > 100 GeV'] += wgt['el']
            if passOSem: OSemcuts['MET > 100 GeV'] += wgt['el']
            if passOSme: OSmecuts['MET > 100 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['MET > 100 GeV'] += wgt['mu']
            if passSSee: SSeecuts['MET > 100 GeV'] += wgt['el']
            if passSSem: SSemcuts['MET > 100 GeV'] += wgt['el']
            if passSSme: SSmecuts['MET > 100 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['MET > 100 GeV'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False

        # New SS signal region
        passOSee = retainOSee
        passOSem = retainOSem
        passOSme = retainOSme
        passOSmm = retainOSmm
        passSSee = retainSSee
        passSSem = retainSSem
        passSSme = retainSSme
        passSSmm = retainSSmm
        # Check jet pT > 50, 50 GeV
        if len(jet_idx['good']) >= 2 and susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt() > 50000.0 \
           and susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt() > 50000.:
            if verbose and (passOSee or passOSem or passOSme or passOSmm or passSSee or passSSem or passSSme or passSSmm):
              print("SignalEvent2j: (%6d;%4d;%8d) %2d jets, Pt1: %8.3f, Pt2: %8.3f, M_Et: %8.3f, M_T: %8.3f" %(t.RunNumber, t.lbn, t.EventNumber, len(jet_idx['good']), susyObjDef.GetJetTLV(jet_idx['good'][0]).Pt(), susyObjDef.GetJetTLV(jet_idx['good'][1]).Pt(), met.Mod()/1000, mT/1000) )
            if passOSee: OSeecuts['2jets (pt > 50,50)'] += wgt['el']
            if passOSem: OSemcuts['2jets (pt > 50,50)'] += wgt['el']
            if passOSme: OSmecuts['2jets (pt > 50,50)'] += wgt['mu']
            if passOSmm: OSmmcuts['2jets (pt > 50,50)'] += wgt['mu']
            if passSSee: SSeecuts['2jets (pt > 50,50)'] += wgt['el']
            if passSSem: SSemcuts['2jets (pt > 50,50)'] += wgt['el']
            if passSSme: SSmecuts['2jets (pt > 50,50)'] += wgt['mu']
            if passSSmm: SSmmcuts['2jets (pt > 50,50)'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False
        # MET cut
        if met.Mod() > 80000.:
            if passOSee: OSeecuts['2jet MET > 80 GeV'] += wgt['el']
            if passOSem: OSemcuts['2jet MET > 80 GeV'] += wgt['el']
            if passOSme: OSmecuts['2jet MET > 80 GeV'] += wgt['mu']
            if passOSmm: OSmmcuts['2jet MET > 80 GeV'] += wgt['mu']
            if passSSee: SSeecuts['2jet MET > 80 GeV'] += wgt['el']
            if passSSem: SSemcuts['2jet MET > 80 GeV'] += wgt['el']
            if passSSme: SSmecuts['2jet MET > 80 GeV'] += wgt['mu']
            if passSSmm: SSmmcuts['2jet MET > 80 GeV'] += wgt['mu']
        else:
            passOSee = passOSem = passOSme = passOSmm = False
            passSSee = passSSem = passSSme = passSSmm = False
    print("susycutflow_2lep.py: OS di-electron cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,OSeecuts[cut]) )
    print("susycutflow_2lep.py: OS electron+muon cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,OSemcuts[cut]) )
    print("susycutflow_2lep.py: SS di-electron cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,SSeecuts[cut]) )
    print("susycutflow_2lep.py: SS electron+muon cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,SSemcuts[cut]) )
    print("susycutflow_2lep.py: OS muon+electron cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,OSmecuts[cut]) )
    print("susycutflow_2lep.py: OS dimuon cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,OSmmcuts[cut]) )
    print("susycutflow_2lep.py: SS muon+electron cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,SSmecuts[cut]) )
    print("susycutflow_2lep.py: SS dimuon cutflow summary")
    for icut,cut in enumerate(cuts):
        print("%2d %25s %15.6f" % (icut,cut,SSmmcuts[cut]) )
    print("susycutflow_2lep.py: ending", datetime.now() )

if __name__ == '__main__':
    main()
