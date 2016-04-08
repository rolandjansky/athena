#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# AtlDataSumNtuple
#
# Eric Torrence - September 2011
#
# Contents:
# NtupleHandler - Utility to define and fill the per-BCID ntuple
#

import os
import array
from ROOT import TObject, TFile, TH1D, TTree, gROOT, AddressOf

class TrigNtupleHandler:

    def __init__(self):

        self.fileName = 'lumi.root'
        self.treeName = 'lumiData'
        self.file = None
        self.tree = None
        self.updatemode = False

        # Flag showing whether BCID data is initialized
        self.bcidData = False

        # Flag showing whether L1 trigger counts are initialized
        self.l1TrigData = True
        
    def open(self, update=True):
        print 'NtupleHandler.open() called'

        if os.path.exists(self.fileName) and update:
            print 'Opening %s for updating' % self.fileName
            self.updatemode = True
            self.file = TFile(self.fileName, 'update')
            self.tree = self.file.Get(self.treeName)
            
        else:
            print 'Creating %s for writing' % self.fileName
            self.updatemode = False
            self.file = TFile(self.fileName, 'create')
            self.tree = TTree(self.treeName, self.treeName)
            

    def close(self):
        print 'ScanNtupleHandler.close() called'
        
        self.tree.Write('', TObject.kOverwrite)
        self.file.Close()
        
    def init(self):
        print 'ScanNtupleHandler.init() called'

        self.initLBData()
        self.initBCIDData()

    def save(self):
        self.tree.Fill()

    def readLastEntry(self):
        entries = self.tree.GetEntries()
        self.tree.GetEntry(entries-1)
        
    # Fill information from LumiLBData object
    def fillLumi(self, lumi):

        # Erase any old data
        self.clear()

        # Time in COOL format (ns)
        self.lbData.coolStartTime = lumi.startTime.timerunlb()
        self.lbData.coolEndTime = lumi.endTime.timerunlb()
        # Time in seconds
        self.lbData.startTime = lumi.startTime.timerunlb()/1.E9
        self.lbData.endTime = lumi.endTime.timerunlb()/1.E9
        # LB duration in seconds
        self.lbData.lbTime = (lumi.endTime.timerunlb() - lumi.startTime.timerunlb())/1.E9
        
        self.lbData.run = lumi.runLB.run
        self.lbData.lb = lumi.runLB.lb

        # Need to fill these elsewhere
        # self.lbData.fill = 0
        # self.lbData.eBeam = 0.
        # self.lbData.stable = False
        # self.lbData.ready = False
        # self.lbData.physics = False        

        # if lumi.onlEvtsPerBX > 0.:
        #    self.lbData.muToLumi = lumi.onlInstLumi/lumi.onlEvtsPerBX
        
        self.lbData.onlInstLum = lumi.onlInstLumi
        self.lbData.onlInstLumAll = lumi.onlInstLumiAll
        self.lbData.onlEvtsPerBX = lumi.onlEvtsPerBX

        self.lbData.onlPrefChan = lumi.onlPrefChan
        self.lbData.onlValid = lumi.onlValid
        self.lbData.olcValid = lumi.olcValid
        
        self.lbData.nColl = lumi.bcid.nbcol()
        self.lbData.nBeam1 = lumi.bcid.nb1()
        self.lbData.nBeam2 = lumi.bcid.nb2()
        self.lbData.qBeam1Col = lumi.IBeam1
        self.lbData.qBeam2Col = lumi.IBeam2
        self.lbData.qBeam1All = lumi.IBeam1All
        self.lbData.qBeam2All = lumi.IBeam2All

        self.lbData.specLumi = lumi.specLumi
        self.lbData.geomLumi = lumi.geomLumi
        self.lbData.maxEvtsPerBX = lumi.maxEvtsPerBX

        self.lbData.l1LiveFrac = lumi.l1Livefrac

        # Get this from the veto folders
        # self.lbData.avgLiveFrac = -1.
        # self.lbData.lumiWtLiveFrac = -1.

        self.lbData.matched = lumi.matched

        if not self.bcidData: return
        
        # And fill the per-BCID values
        for (bcid, caliLumi) in lumi.bcid.caliLumi.iteritems():
            self.lumiDel[int(bcid)] = caliLumi

        for (bcid, q) in lumi.bcid.b1Curr.iteritems():
            self.qBeam1[int(bcid)] = q

        for (bcid, q) in lumi.bcid.b2Curr.iteritems():
            self.qBeam2[int(bcid)] = q

        i=0
        for bcid in sorted(list(lumi.bcid.b1BCID)):
            self.b1BCID[i] = bcid
            i += 1

        i=0
        for bcid in sorted(list(lumi.bcid.b2BCID)):
            self.b2BCID[i] = bcid
            i += 1

        i=0
        for bcid in sorted(list(lumi.bcid.colBCID)):
            self.colBCID[i] = bcid
            i += 1

        # Still need live fraction

    # Pass TriggerL1Data object for lumi block filled by TriggerHandler
    # Also need mapping of channel names to channel values
    def fillL1Trig(self, trigData, trigChan):
        for (trig, chan) in trigChan.iteritems():
            self.l1TBP[chan] = trigData.TBP[trig]
            self.l1TAP[chan] = trigData.TAP[trig]
            self.l1TAV[chan] = trigData.TAV[trig]

    def defineBranch(self, name, type):
        self.tree.Branch(name, AddressOf(self.lbData, name), name+'/'+type)

    def loadBranch(self, name):
        branch = self.tree.GetBranch(name)
        branch.SetAddress(AddressOf(self.lbData, name))
        
    def initLBData(self):

        # Define the main lumiblock data
        # Time is in ns
        
#            ULong64_t startTime;\
#            ULong64_t endTime;\

        LBDataStructStr = "struct LBDataStruct {\
            ULong64_t coolStartTime;\
            ULong64_t coolEndTime;\
            Double_t startTime;\
            Double_t endTime;\
            Float_t lbTime;\
            UInt_t fill;\
            UInt_t run;\
            UInt_t lb;\
            Float_t eBeam;\
            Bool_t stable;\
            Bool_t ready;\
            Bool_t physics;\
            Bool_t larVeto;\
            \
            UInt_t onlValid;\
            UInt_t olcValid;\
            UInt_t onlPrefChan;\
            Float_t muToLumi;\
            Float_t onlInstLum;\
            Float_t onlInstLumAll;\
            Float_t onlEvtsPerBX;\
            \
            UInt_t nColl;\
            UInt_t nBeam1;\
            UInt_t nBeam2;\
            Float_t qBeam1Col;\
            Float_t qBeam2Col;\
            Float_t qBeam1All;\
            Float_t qBeam2All;\
            \
            Float_t specLumi;\
            Float_t geomLumi;\
            Float_t maxEvtsPerBX;\
            \
            Float_t l1LiveFrac;\
            Float_t avgLiveFrac;\
            Float_t lumiWtLiveFrac;\
            \
            UInt_t matched;\
        };"
        
        # Replace sizes if needed
        gROOT.ProcessLine(LBDataStructStr)
        from ROOT import LBDataStruct
        self.lbData = LBDataStruct()

        self.varList = []

        self.varList.append(('startTime', 'D'))
        self.varList.append(('endTime', 'D'))
        self.varList.append(('coolStartTime', 'l'))
        self.varList.append(('coolEndTime', 'l'))
        self.varList.append(('lbTime', 'F'))
        
        self.varList.append(('fill', 'i'))
        self.varList.append(('run', 'i'))
        self.varList.append(('lb', 'i'))
        self.varList.append(('eBeam', 'F'))

        # Boolean status flags
        self.varList.append(('stable', 'O'))
        self.varList.append(('ready', 'O'))
        self.varList.append(('physics', 'O'))
        self.varList.append(('larVeto', 'O'))
        
        # Luminosity information
        self.varList.append(('onlPrefChan', 'i'))
        self.varList.append(('muToLumi', 'F'))
        self.varList.append(('onlInstLum', 'F'))
        self.varList.append(('onlInstLumAll', 'F'))
        self.varList.append(('onlEvtsPerBX', 'F'))
        self.varList.append(('onlValid', 'i'))  # From LBLESTONL & 0x3FF
        self.varList.append(('olcValid', 'i'))  # From LUMINOSITY
        
        # Total bunch information
        self.varList.append(('nColl', 'i'))
        self.varList.append(('nBeam1', 'i'))
        self.varList.append(('nBeam2', 'i'))
        self.varList.append(('qBeam1Col', 'F')) # Total charge colliding
        self.varList.append(('qBeam2Col', 'F'))
        self.varList.append(('qBeam1All', 'F')) # Total charge in all BCIDs
        self.varList.append(('qBeam2All', 'F'))

        self.varList.append(('specLumi', 'F'))
        self.varList.append(('geomLumi', 'F'))
        self.varList.append(('maxEvtsPerBX', 'F'))

        # Livetime information
        self.varList.append(('l1LiveFrac', 'F'))
        self.varList.append(('avgLiveFrac', 'F'))
        self.varList.append(('lumiWtLiveFrac', 'F')) # lumi-weighted per-BCID livefraction

        # Where the lumi information came from
        self.varList.append(('matched', 'i'))

        for (var, type) in self.varList:
            if self.updatemode:
                self.loadBranch(var)

            else:
                self.defineBranch(var, type)
            
    def initBCIDData(self):

        self.bcidData = True
        
        # Delivered luminosity
        self.lumiDel = array.array('f', (0.,)*3564)
        self.qBeam1  = array.array('f', (0.,)*3564)         # Charge per BCID
        self.qBeam2  = array.array('f', (0.,)*3564)
        self.liveFrac = array.array('f', (0.,)*3564)        # Deadtime

        if self.updatemode:
            self.tree.GetBranch('lumiDel').SetAddress(self.lumiDel)
            self.tree.GetBranch('qBeam1').SetAddress(self.qBeam1)
            self.tree.GetBranch('qBeam2').SetAddress(self.qBeam2)
            self.tree.GetBranch('liveFrac').SetAddress(self.liveFrac)
            
            
        else:
            self.tree.Branch('lumiDel',  self.lumiDel,  'lumiDel[3564]/F')
            self.tree.Branch('qBeam1',   self.qBeam1,   'qBeam1[3564]/F')
            self.tree.Branch('qBeam2',   self.qBeam2,   'qBeam2[3564]/F')
            self.tree.Branch('liveFrac', self.liveFrac, 'liveFrac[3564]/F') # Per-BCID livetime from lumi counters
            
        # BCID arrays (unsigned shorts)
        self.b1BCID = array.array('H', (0,)*3564)
        self.b2BCID = array.array('H', (0,)*3564)
        self.colBCID = array.array('H', (0,)*3564)

        if self.updatemode:
            self.tree.GetBranch('b1BCID').SetAddress(self.b1BCID)
            self.tree.GetBranch('b2BCID').SetAddress(self.b2BCID)
            self.tree.GetBranch('colBCID').SetAddress(self.colBCID)
        else:
            self.tree.Branch('b1BCID', self.b1BCID, 'b1BCID[nBeam1]/s') # Unsigned short
            self.tree.Branch('b2BCID', self.b2BCID, 'b2BCID[nBeam2]/s') # Unsigned short
            self.tree.Branch('colBCID', self.colBCID, 'colBCID[nColl]/s') # Unsigned short

    def initL1TrigData(self):

        self.l1TrigData = True
        
        # Counts by channel ID
        self.l1TBP = array.array('I', (0,)*256)
        self.l1TAP = array.array('I', (0,)*256)
        self.l1TAV = array.array('I', (0,)*256)

        if self.updatemode:
            self.tree.GetBranch('l1TBP').SetAddress(self.l1TBP)
            self.tree.GetBranch('l1TAP').SetAddress(self.l1TAP)
            self.tree.GetBranch('l1TAV').SetAddress(self.l1TAV)
            
        else:
            self.tree.Branch('l1TBP',  self.l1TBP, 'l1TBP[256]/i')
            self.tree.Branch('l1TAP',  self.l1TAP, 'l1TAP[256]/i')
            self.tree.Branch('l1TAV',  self.l1TAV, 'l1TAV[256]/i')
            

    # Set all ntuple variables to default values
    def clear(self):

        self.lbData.startTime = 0
        self.lbData.endTime = 0
        self.lbData.lbTime = 0.
        self.lbData.fill = 0
        self.lbData.run = 0
        self.lbData.lb = 0
        self.lbData.eBeam = 0.

        self.lbData.stable = False
        self.lbData.ready = False
        self.lbData.physics = False        
        self.lbData.larVeto = False
        
        self.lbData.onlPrefChan = 0
        self.lbData.muToLumi = 0.
        self.lbData.onlInstLum = -1.
        self.lbData.onlInstLumAll = -1.
        self.lbData.onlEvtsPerBX = -1.
        self.lbData.onlValid = 0xFFFFFF
        self.lbData.olcValid = 0xFFFFFF
        
        self.lbData.nColl = 0
        self.lbData.nBeam1 = 0
        self.lbData.nBeam2 = 0
        self.lbData.qBeam1Col = -1.
        self.lbData.qBeam2Col = -1.
        self.lbData.qBeam1All = -1.
        self.lbData.qBeam2All = -1.

        self.lbData.specLumi = -1.
        self.lbData.geomLumi = -1.
        self.lbData.maxEvtsPerBX = -1.

        self.lbData.l1LiveFrac = -1.
        self.lbData.avgLiveFrac = -1.
        self.lbData.lumiWtLiveFrac = -1.

        self.lbData.matched = 0

        if self.bcidData:
                    
            # Per-BCID arrays
            for i in range(3564):
                self.lumiDel[i] = 0.
                self.qBeam1[i] = 0.
                self.qBeam2[i] = 0.
                self.liveFrac[i] = 0.
                self.b1BCID[i] = 0
                self.b2BCID[i] = 0
                self.colBCID[i] = 0

        if self.l1TrigData:

            # L1 trigger counts
            for i in range(256):
                self.l1TBP[i] = 0
                self.l1TAP[i] = 0
                self.l1TAV[i] = 0
