# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
import cppyy
import ROOT
import math
cppyy.loadDictionary('egammaEnumsDict') # Needed for egammaParameters
from ROOT import egammaParameters
from ROOT import egammaPID
cppyy.loadDict("libTrkTrackSummaryDict")
cppyy.loadDict('libegammaAnalysisUtilsDict')
from ROOT import TLorentzVector

def isLoosePP(electron):
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/IsEMIdentification
    # 1.1 2012 data analyses with release 17.2 
    # 1. Standard Method
    # everything is OK for all the 2012 reprocessed data since the AODFix was used by default.
    return electron.passID(egammaPID.ElectronIDLoosePP) 

def isMediumPP(electron):
    # https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/IsEMIdentification
    if not electron.trackParticle():
        return False
    if not electron.trackParticle().trackSummary():
        return False

    eta = electron.cluster().etaBE(2)
    if abs(eta) > 300.:
        return False
    eT = electron.cluster().e()/math.cosh(eta)
    f3 = electron.detailValue(egammaParameters.f3)
    rHad = 0.
    rHad1 = 0. 
    if abs(eT) != 0.:
        rHad = electron.detailValue(egammaParameters.ethad)/eT
        rHad1 = electron.detailValue(egammaParameters.ethad1)/eT
    e277 = electron.detailValue(egammaParameters.e277)
    Reta = 0.
    if e277 != 0.:
        e237 = electron.detailValue(egammaParameters.e237)
        Reta = e237/e277
    w2 = electron.detailValue(egammaParameters.weta2)
    f1 = electron.detailValue(egammaParameters.f1)
    wstot = electron.detailValue(egammaParameters.wtots1)
    DEmaxs1 = 0.
    emaxs1 = electron.detailValue(egammaParameters.emaxs1)
    Emax2 = electron.detailValue(egammaParameters.e2tsts1)
    if abs(emaxs1+Emax2) > 0.:
        DEmaxs1 = (emaxs1-Emax2)/(emaxs1+Emax2)
    deltaEta = electron.detailValue(egammaParameters.deltaEta1)
    d0 = electron.detailValue(egammaParameters.trackd0_physics)
    nTRT = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHits)
    nTRTOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTOutliers)
    TRratio = 0.
    if (nTRT+nTRTOutliers)>0:
        nTRThigh = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdHits)
        nTRThighOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdOutliers) 
        TRratio = float(nTRThigh+nTRThighOutliers)/float(nTRT+nTRTOutliers)
    nPix = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelHits)
    nSi = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTHits)+nPix
    nPixOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelOutliers)
    nSiOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTOutliers)+nPixOutliers
    nBlayer = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfBLayerHits)
    nBlayerOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfBLayerOutliers)
    expectBlayer = (electron.detailValue(egammaParameters.expectHitInBLayer) != 0.)

    return ROOT.isMediumPlusPlus(eta, eT, f3,\
                                 rHad, rHad1, Reta, w2,\
                                 f1, wstot, DEmaxs1, deltaEta, d0,\
                                 TRratio, nTRT, nTRTOutliers,\
                                 nSi, nSiOutliers, nPix, nPixOutliers,\
                                 nBlayer, nBlayerOutliers, expectBlayer) 

def checkElectronQuality(qual, electron):
    if qual=='MediumPP':
        return isMediumPP(electron) 
    if qual=='LoosePP':
        return isLoosePP(electron) 
    return True

def checkMuonQuality(qual, muon):
    if qual=='combined+lowpt':
        return muon.isCombinedMuon() or muon.isSegmentTaggedMuon()
    if qual=='combined+lowpt+standalone':
        return muon.isCombinedMuon() or muon.isSegmentTaggedMuon() or (muon.isStandAloneMuon() and abs(muon.eta())>2.5 and abs(muon.eta())<2.7)
    if qual=='calo':
        return abs(muon.eta())<0.1 
    return True

def checkJetQuality(qual, jet):
    if qual=='barrel':
        return abs(jet.eta())<2.5 # |eta|<2.5
    return True

class HSG2_2L2QDPDFilter(PyAthena.AthFilterAlgorithm):

    def __init__(self, name="HSG2_2L2QDPDFilter",**kw):
        kw['name'] = name
        super(HSG2_2L2QDPDFilter, self).__init__(**kw)
        self.cutDict=dict() 
        # types are e or mu or jet 
        self.cutDict["types"] = kw.get("types", [])
        # pT cuts for muons and jets, ET cut for electrons 
        self.cutDict["pTCuts"] = kw.get("pTCuts", [])
        # quality cuts are MediumPP for electrons, combined+lowpt for muons, barrel for jets
        self.cutDict["qualityCuts"] = kw.get("qualityCuts", [])
        # Container names in AOD 
        self.cutDict["collections"] = kw.get("collections", [])
        # Di-lepton mass cut
        self.cutDict["diLeptonMassCut"] = kw.get("diLeptonMassCut", 5000.)
        # Electron-jet dR cut
        self.cutDict["electronJetDRCut"] = kw.get("electronJetDRCut", 0.05) # Negative value means no overlap removal
        # qualified stores the pointers of objects which pass the selections  
        self.qualified=dict()
        self.qualifiedFourMomenta=dict()
        self.nObj=len(self.cutDict["types"]) 
        for i in range(self.nObj):
            self.qualified[i]=list()
            self.qualifiedFourMomenta[i]=list()
        # The number of processed events     
        self.nProc=0
        # The number of rejected events
        self.nFail=0
        # The list to contain objects' pointer
        self.combination=list()
        # The list to contain objects' momenta
        self.fourMomenta=list()

    def initialize(self):
        # Check the validitly of self.cutDict
        if len(self.cutDict["pTCuts"])!=self.nObj:
               self.msg.fatal("Lengths of pTCuts and types are different!!")
               return StatusCode.Failure
        if len(self.cutDict["qualityCuts"])!=self.nObj:
               self.msg.fatal("Lengths of qualityCuts and types are different!!")
               return StatusCode.Failure
        if len(self.cutDict["collections"])!=self.nObj:
               self.msg.fatal("Lengths of collections and types are different!!")
               return StatusCode.Failure
        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.fatal("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        return StatusCode.Success

    def getFourMomentum(self,obj,number):
        fourMomentum = TLorentzVector(0,0,0,0)
        # Electron: calculate 4-momentum based on cluster energy and track direction
        # ET cut is used for electrons
        if self.cutDict["types"][number]=="e":
            if obj.cluster() and obj.trackParticle() and abs(obj.trackParticle().eta())<300.:
                fourMomentum.SetPtEtaPhiE(obj.cluster().e()/math.cosh(obj.trackParticle().eta()), \
                                          obj.trackParticle().eta(), obj.trackParticle().phi(), \
                                          obj.cluster().e())
            else:
                fourMomentum.SetPtEtaPhiE(obj.et(), obj.eta(), obj.phi(), obj.e())
        else: 
            fourMomentum.SetPtEtaPhiE(obj.pt(), obj.eta(), obj.phi(), obj.e())
        return fourMomentum    

    def checkPt(self,fourMomentum,number):
        # ET cut is used for electrons
        # pT cut is used for muons and jets
        # Pt() gives electron ET and muon/jet pT
        return (fourMomentum.Pt() > self.cutDict["pTCuts"][number])

    def checkQuality(self,obj,number):
        if self.cutDict["types"][number]=="e":
            return checkElectronQuality(self.cutDict["qualityCuts"][number], obj)
        if self.cutDict["types"][number]=="mu":
            return checkMuonQuality(self.cutDict["qualityCuts"][number], obj)
        if self.cutDict["types"][number]=="jet":
            return checkJetQuality(self.cutDict["qualityCuts"][number], obj)
        self.msg.warning("specified type is strange", self.cutDict["types"][number])
        return False

    def checkDiLeptonMass(self,fourMomentum,number):
        # Leptons are assumed to be 0th and 1st objects. 
        if number==1 and self.cutDict["types"][0]==self.cutDict["types"][1] \
           and (self.cutDict["types"][0]=="e" or self.cutDict["types"][0]=="mu"):
            dileptonFourMomentum = self.fourMomenta[0] + fourMomentum
            return (dileptonFourMomentum.M()>self.cutDict["diLeptonMassCut"]) 
        else:
            return True

    def checkElectronJetOverlap(self,fourMomentum,number):
        # Electrons are assumed to be 0th and 1st objects.
        # Jets are assumed to be 2nd and 3rd objects. 
        if (number==2 or number==3) and self.cutDict["types"][number]=="jet":
            isOverlapped = False
            jet_eta = fourMomentum.Eta()
            jet_phi = fourMomentum.Phi()
            for i in range(2):
                if self.cutDict["types"][i]=="e":
                    e_eta = self.fourMomenta[i].Eta()
                    e_phi = self.fourMomenta[i].Phi()
                    dEta = jet_eta-e_eta
                    dPhi = jet_phi-e_phi
                    if dPhi>+math.pi:
                        dPhi -= 2.*math.pi
                    if dPhi<-math.pi:
                        dPhi += 2.*math.pi
                    dR = math.sqrt(dEta*dEta + dPhi*dPhi)
                    if dR<self.cutDict["electronJetDRCut"]:
                        isOverlapped = True
                        continue
            return (not isOverlapped) # True if electron and jet are not overlapped. 
        else: # e-e, (e-mu), mu-mu, mu-jet, jet-jet cases  
            return True
        
    def findQualified(self):
        # Initialize the qualified array 
        for k in self.qualified.keys():
            self.qualified[k]=[]
            self.qualifiedFourMomenta[k]=[]
        # Loop over nObj objects 
        for i in range(self.nObj):
            # Set the collection type 
            collectionType="" 
            if self.cutDict["types"][i]=="e":
                collectionType="ElectronContainer"
            elif self.cutDict["types"][i]=="mu":
                collectionType="Analysis::MuonContainer"
            elif self.cutDict["types"][i]=="jet":    
                collectionType="JetCollection"
            else:
                self.msg.warning("specified type is strange", self.cutDict["types"][i])
                return StatusCode.Success
            # Retrieve the collection 
            collection=0
            try:
                collection = self.storeGateSvc.retrieve(collectionType, self.cutDict["collections"][i])
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.cutDict["collections"][i] )
                return StatusCode.Success
            # Loop over objects in the collection 
            for obj in collection:
                fourMomentum = self.getFourMomentum(obj, i)
                # pT or ET cut
                if not self.checkPt(fourMomentum, i):
                    continue
                # quality cut
                if not self.checkQuality(obj, i):
                    continue
                # If an object reaches here, the object passes the selection and is added to the qualified array. 
                self.qualified[i].append(obj)
                self.qualifiedFourMomenta[i].append(fourMomentum)
            # The lengths of self.qualified[i] and self.qualifiedFourMomenta[i] should always be identical.  
            if len(self.qualified[i])!=len(self.qualifiedFourMomenta[i]):
                self.msg.warning("len(self.qualified[i])!=len(self.qualifiedFourMomenta[i])")

    def subFindCombination(self):
        # The index of a new object is obtained by len. 
        i = len(self.combination)
        # The lengths of self.combination and self.fourMomenta should always be identical.  
        if i!=len(self.fourMomenta):
            self.msg.warning("len(self.combination)!=len(self.fourMomenta)")
        # Loop over i-th objects
        for j in range(len(self.qualified[i])):
            obj = self.qualified[i][j]
            fourMomentum = self.qualifiedFourMomenta[i][j]
            # Chech if this object is already used in the current combination or not. 
            isNew = True
            for k in range(i):
                if self.cutDict["types"][i]==self.cutDict["types"][k] and obj==self.combination[k]:
                    isNew = False
                    continue
            if not isNew:
                continue
            # Di-lepton mass cut
            if not self.checkDiLeptonMass(fourMomentum, i):
                continue
            # Electron-jet overlap removal
            if not self.checkElectronJetOverlap(fourMomentum, i):
                continue
            # obj is not used in this combination and is now added. 
            self.combination.append(obj)
            self.fourMomenta.append(fourMomentum)
            if len(self.combination)==self.nObj:
                # This combination consists of required objects.  
                return True
            # Try next object in a recursive way. 
            if self.subFindCombination():
                return True
            # Remove the last object 
            self.combination.pop()
            self.fourMomenta.pop()
        return False

    def findCombination(self):
        nCombinations = 1
        # If at least one object is not found, the event is rejected. 
        for i in range(self.nObj):
            if len(self.qualified[i])==0:
                return False
            nCombinations *= len(self.qualified[i])
        # If there are really many (100,000) combinations to be checked, the event is accepted
        if nCombinations >100000:
            return True 
        self.combination = []
        self.fourMomenta = []
        return self.subFindCombination() 

    def execute(self):
        self.nProc+=1
        # Find objects
        self.findQualified()
        # Find at least one combination
        if self.findCombination():
            self.setFilterPassed(True)
        else:    
            self.setFilterPassed(False)
            self.nFail+=1
        return StatusCode.Success

    def finalize(self):
        self.msg.info( 'finalizing: processed %i passed %i failed %i' % (self.nProc, self.nProc-self.nFail, self.nFail) )
        return StatusCode.Success
