# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# D3PDMaker
#
# Author: Sven Menke
#
# this python alg creates a D3PD and fills it
# 
from __future__ import print_function
import AthenaPython.PyAthena as PyAthena
from ROOT import TFile, TTree 
from array import array
import os

class HECNoiseD3PDMaker(PyAthena.Alg):
    def __init__ ( self, name, **kw ) :
        PyAthena.Alg.__init__(self,name)
        self.name = name
        self.det = None
        self.ped = None

        ## cell cuts
        self.MinDigitADC      = kw.get('MinDigitADC',     20)
        self.MaxDeltaT        = kw.get('MaxDeltaT',        5)
        self.NtupleFileName   = kw.get('NtupleFileName', 'HECNoiseD3PD.root')
        self.TriggerLines     = kw.get('TriggerLines',   ['L1_J5',
                                                          'L1_J10',
                                                          'L1_J12',
                                                          'L1_J30',
                                                          'L1_TAU5',
                                                          'L1_TAU8',
                                                          'L1_J5_EMPTY',
                                                          'L1_J10_EMPTY',
                                                          'L1_J12_EMPTY',
                                                          'L1_J30_EMPTY',
                                                          'L1_TAU5_EMPTY',
                                                          'L1_TAU8_EMPTY',
                                                          'L1_J5_FIRSTEMPTY',
                                                          'L1_J10_FIRSTEMPTY',
                                                          'L1_J12_FIRSTEMPTY',
                                                          'L1_J30_FIRSTEMPTY',
                                                          'L1_TAU5_FIRSTEMPTY',
                                                          'L1_TAU8_FIRSTEMPTY'])

    def initialize(self):
        print ("==> initializing ", self.name)
        print ("MinDigitADC:     ", self.MinDigitADC)
        print ("MaxDeltaT:       ", self.MaxDeltaT)
        print ("NtupleFileName:  ", self.NtupleFileName)
        print ("TriggerLines:    ", self.TriggerLines)
        #
        self.sg = PyAthena.py_svc("StoreGateSvc")
        self.det = PyAthena.StoreGate.pointer("DetectorStore")
        self.LArOID = self.det.retrieve("LArOnlineID","LArOnlineID")
        self.lcs = PyAthena.py_tool('LArCablingService')
        self.cdd = PyAthena.CaloDetDescrManager.instance()
        self.cid = self.cdd.getCaloCell_ID()
        self.tdt = PyAthena.py_tool('Trig::TrigDecisionTool/TrigDecisionTool')
        self.ntfile =  TFile(self.NtupleFileName,"RECREATE")
        self.hectree = TTree("HECNoise","HECNoise")
        self.iRun = array('i',[0])
        self.iEvent = array('L',[0])            
        self.iEventCount = array('i',[0])            
        self.fPrescale = {}
        self.iTrigger = {}
        for tl in self.TriggerLines:
            self.fPrescale[tl] = array('f',[0.0])
            self.iTrigger[tl] = array('i',[0])
            pass
        self.iTime = array('i',[0])
        self.iLB = array('i',[0])
        self.iBCID = array('i',[0])
        self.avgMu = array('f',[0.0])
        self.actMu = array('f',[0.0])
        self.iGain = array('i',[0])
        self.iOID = array('L',[0])
        self.iSide = array('i',[0])
        self.iSamp = array('i',[0])
        self.iReg = array('i',[0])
        self.iEta = array('i',[0])
        self.iPhi = array('i',[0])
        self.iQuality = array('i',[0])            
        self.e = array('f',[0.0])
        self.t = array('f',[0.0])
        self.eta = array('f',[0.0])
        self.phi = array('f',[0.0])
        self.z = array('f',[0.0])
        self.r = array('f',[0.0])
        self.Ped = array('f',[0.0])
        self.PedRMS = array('f',[0.0])
        self.iDigi = array('i',32*[0])
        self.iMax = array('i',[0])
        self.iMin = array('i',[0])
        #
        self.hectree.Branch("iRun",self.iRun,"iRun/I")
        self.hectree.Branch("iEvent",self.iEvent,"iEvent/I")
        self.hectree.Branch("iEventCount",self.iEventCount,"iEventCount/I")

        for tl in self.TriggerLines:
            self.hectree.Branch(tl+"_Prescale",self.fPrescale[tl],tl+"_Prescale/F")
            self.hectree.Branch(tl+"_Trigger",self.iTrigger[tl],tl+"_Trigger/I")
            pass
        self.hectree.Branch("iTime",self.iTime,"iTime/I")
        self.hectree.Branch("iLB",self.iLB,"iLB/I")
        self.hectree.Branch("iBCID",self.iBCID,"iBCID/I")
        self.hectree.Branch("avgMu",self.avgMu,"avgMu/F")
        self.hectree.Branch("actMu",self.actMu,"actMu/F")
        self.hectree.Branch("iGain",self.iGain,"iGain/I")
        self.hectree.Branch("iOID",self.iOID,"iOID/l")
        self.hectree.Branch("iSide",self.iSide,"iSide/I")
        self.hectree.Branch("iSamp",self.iSamp,"iSamp/I")
        self.hectree.Branch("iReg",self.iReg,"iReg/I")
        self.hectree.Branch("iEta",self.iEta,"iEta/I")
        self.hectree.Branch("iPhi",self.iPhi,"iPhi/I")
        self.hectree.Branch("iQuality",self.iQuality,"iQuality/I")
        self.hectree.Branch("e",self.e,"e/F")
        self.hectree.Branch("t",self.t,"t/F")
        self.hectree.Branch("eta",self.eta,"eta/F")
        self.hectree.Branch("phi",self.phi,"phi/F")
        self.hectree.Branch("z",self.z,"z/F")
        self.hectree.Branch("r",self.r,"r/F")
        self.hectree.Branch("Ped",self.Ped,"Ped/F")
        self.hectree.Branch("PedRMS",self.PedRMS,"PedRMS/F")
        self.hectree.Branch("iDigi",self.iDigi,"iDigi[32]/I")
        self.hectree.Branch("iMax",self.iMax,"iMax/I")
        self.hectree.Branch("iMin",self.iMin,"iMin/I")
        #
        return True

    def execute(self):
        #for some obscure reason, we need run dump before we can retrieve the flat objects using their abstract interface
        garbagedump = open(os.devnull, 'w')
        self.det.dump(garbagedump)
        garbagedump.close()
        self.ped = self.det.retrieve("ILArPedestal","Pedestal")
        # filter low gain cells
        passedTrigger = False
        foundLowCell = False
        for tl in self.TriggerLines:
            self.fPrescale[tl][0] = self.tdt.getPrescale(tl)
            if self.tdt.isPassed(tl):
                passedTrigger = True
                self.iTrigger[tl][0] = 1
                pass
            else:
                self.iTrigger[tl][0] = 0
                pass
            pass
        if passedTrigger or not passedTrigger: # take all events with LG Cells
            self.iEventCount[0] = 0
            ldc = self.sg.retrieve("LArDigitContainer","LArDigitContainer_Thinned")
            for ld in ldc:
                hid = ld.hardwareID()
                if self.LArOID.isHECchannel(hid):# and ld.gain() == 2:
                    sigmax=0
                    sigmin=0
                    imax=0
                    imin=0
                    isamp=0
                    samp0= ld.samples()[0]
                    for samp in ld.samples():
                        if isamp < 32:
                            self.iDigi[isamp] = samp
                            pass
                        if samp-samp0 > sigmax:
                            sigmax = samp-samp0
                            imax=isamp
                            pass
                        if samp-samp0 < sigmin:
                            sigmin = samp-samp0
                            imin=isamp
                            pass
                        isamp = isamp+1
                        pass
                    if sigmax > self.MinDigitADC and sigmin < -self.MinDigitADC and ( (imin-imax) < self.MaxDeltaT or imin < imax):
                        foundLowCell = True
                        ei = self.sg.retrieve("EventInfo","ByteStreamEventInfo")
                        cc = self.sg.retrieve("CaloCellContainer","AllCalo")
                        self.iRun[0] = ei.event_ID().run_number()
                        self.iEvent[0] = ei.event_ID().event_number()
                        self.iEventCount[0] = self.iEventCount[0]+1
                        self.iTime[0] = ei.event_ID().time_stamp()
                        self.iLB[0] = ei.event_ID().lumi_block()
                        self.iBCID[0] = ei.event_ID().bunch_crossing_id()
                        self.avgMu[0] = ei.averageInteractionsPerCrossing()
                        self.actMu[0] = ei.actualInteractionsPerCrossing()
                        self.iGain[0] = ld.gain()
                        oid = self.lcs.cnvToIdentifier(hid)
                        self.iOID[0] = ld.channelID().get_compact()
                        self.Ped[0] = self.ped.pedestal(ld.channelID(),ld.gain())
                        self.PedRMS[0] = self.ped.pedestalRMS(ld.channelID(),ld.gain())
                        self.iSide[0] = self.cid.pos_neg(oid)
                        self.iSamp[0] = self.cid.sampling(oid)
                        self.iReg[0] = self.cid.region(oid)
                        self.iEta[0] = self.cid.eta(oid)
                        self.iPhi[0] = self.cid.phi(oid)
                        self.iMax[0] = imax
                        self.iMin[0] = imin
                        ihash = self.cid.calo_cell_hash(oid)
                        rcell = cc.findCell(ihash)
                        self.e[0] = 0.0
                        self.t[0] = 0.0
                        self.iQuality[0] = 0
                        if rcell.ID() != oid:
                            print ("Cell iHash does not match ...")
                            pass
                        else:
                            self.e[0] = rcell.e()
                            self.t[0] = rcell.time()
                            self.iQuality[0] = rcell.quality()
                            pass
                        cdde = self.cdd.get_element(oid)
                        self.eta[0] = cdde.eta()
                        self.phi[0] = cdde.phi()
                        self.z[0] = cdde.z()
                        self.r[0] = cdde.r()
                        self.hectree.Fill()
                        self.setFilterPassed(True)
                        pass
                    pass
                pass
            if foundLowCell:
                print ('Event passed HECNoise Filter')
                return True
            pass
        print ('Event failed HECNoise Filter')
        self.setFilterPassed(False)
        return True

    def finalize(self):
        self.ntfile.Write()
        self.ntfile.Close()
        return True
    
