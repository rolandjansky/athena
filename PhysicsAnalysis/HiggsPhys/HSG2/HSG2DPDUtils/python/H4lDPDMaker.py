# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# event filter for H4l DPD
# author: Andrea Di Simone

import AthenaPython.PyAthena as PyAthena
from PrimaryDPDMaker import PrimaryDPDHelpers
import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
from AthenaPython.PyAthena import EventInfo
from copy import copy
import cppyy
import ROOT
import math
# Get enum for tracking enum
cppyy.loadDict("libTrkTrackSummaryDict")
cppyy.loadDict('libTrkEventPrimitivesDict')
cppyy.loadDictionary('egammaEnumsDict')
from ROOT import egammaPID
from ROOT import egammaParameters
from ROOT import TLorentzVector
cppyy.loadDict('libegammaAnalysisUtilsDict')

import os
import os.path

def checkPhotonQuality(qual,gamma):

    if qual=='any':
        if gamma.author(egammaParameters.AuthorRConv)==0:
            return True

    return False

def checkMuonQuality(qual, muon):
    
    if qual=='any':
         return True
    elif qual=='combined':
        if muon.isCombinedMuon():
            return True
    elif qual=='standalone':
        if muon.isStandAloneMuon():
             return True
    elif qual=='lowpt':
        if muon.isSegmentTaggedMuon():
             return True
    elif qual=='combined+lowpt':
        if muon.isCombinedMuon() or muon.isSegmentTaggedMuon():
             return True
    elif qual=='inMS':

        if (muon.author()==11 or muon.author()==5) and (abs(abs(muon.eta()) - 2.6)>0.12):
            return False
        else:
            return True
        
    else:
        self.msg.error( 'Muon quality %s not defined' % self.cutDict["quality"][i])
        return False

    return False
                        
def checkElectronQuality(qual, electron, acceptFrwd=True):
        
    good=True

    if qual!='any':
        good=PrimaryDPDHelpers.checkElectronIsEM(electron, qual)
            
    if (electron.author()==1 or electron.author()==3):
        good = good and (electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTHits)+\
                         electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelHits)>0)
    elif electron.author()==8 :
        good= good and acceptFrwd
    else:
        #discard other authors
        good = False

    return good

class H4lMultiLeptonFilter(PyAthena.AthFilterAlgorithm):

    def __init__(self, name="H4lMultiLeptonFilter",**kw):
        ## initialize base class
        kw['name'] = name
        super(H4lMultiLeptonFilter, self).__init__(**kw)
        self.choice=list()
        self.cutDict=dict()
        self.cutDict["kinematicsPtCuts"] = kw.get('kinematicsPtCuts', [])
        self.cutDict["kinematicsPtCuts"].sort(reverse=True)
        self.cutDict["kinematicsMassCuts"] = kw.get('kinematicsMassCuts', [])
        self.cutDict["kinematicsMassCuts"].sort(reverse=True)
        self.cutDict["kinematicsDeltaRCut"] = kw.get('kinematicsDeltaRCut', 0.)
        # counters for efficiency
        self.nProc=0
        self.nFail=0
        self.qualified=dict()
        self.nLept=0
        self.mass=0
        self.massUp=0
        self.massDo=0
        self.doOS=False
        self.doPhoton=False
        self.photonPt=0
        self.photonEta=0
        self.space='**'
        self.checkLArError = kw.get('checkLArError', True)

    def setCut(self, whichLepton,cutName,cutValue):

        # cutDict initialized in __init__ of daughter classes

        try:
            self.cutDict.get(cutName)[whichLepton]=cutValue
        except IndexError:
            self.msg.error('setting property for lepton out of range')
        except KeyError:
            self.msg.error('setting unknown property '+cutName)

    def initialize(self):
        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Recoverable

        # additionalMassAndQualityCuts is only for 4 leptons
        self.electronLikelihoodTool = None
        if self.nLept==4:
            self.electronLikelihoodTool = PyAthena.py_tool('AthElectronLikelihoodTool/AthElectronLikelihoodTool_Loose')
            if self.electronLikelihoodTool is None:
                self.msg.error("Problem retrieving AthElectronLikelihoodTool/AthElectronLikelihoodTool_Loose pointer !!")
                return StatusCode.Recoverable
            self.msg.info("AthElectronLikelihoodTool/AthElectronLikelihoodTool_Loose retrieved.")

        return StatusCode.Success

    def findQualified(self):
        
        # this is lepton-type dependent, leave it to daughter classes
        pass


    def execute(self):
        
        # must clean up qualified list from previous event
        for k in self.qualified.keys():
            self.qualified[k]=[]

        self.nProc+=1

        # LAr error check
        if self.checkLArError:
            try:
                eventInfo = self.storeGateSvc.retrieve("EventInfo")
            except LookupError:
                self.msg.warning("EventInfo is not found")           
                self.setFilterPassed(False)
                self.nFail+=1
                return StatusCode.Success   
            if eventInfo.errorState(EventInfo.LAr)==EventInfo.Error:
                self.setFilterPassed(False)
                self.nFail+=1
                return StatusCode.Success

        self.findQualified()

        # need to see whether a proper combination of qualified electrons allows to satisfy all conditions
        # for the time being let's check all combinations...
        # first check the easiest case: whether any cut was never passed
        
        for i in range(self.nLept):
            if not len(self.qualified[i]):
                self.setFilterPassed(False)
                self.nFail+=1 
                return StatusCode.Success

        # If the filter uses 4 (quadruplet) or more leptons and if there are more than 50000 combinations, 
        # the event is passed without further checking to reduce the CPU time. 
        if self.nLept>=4:
            nCombinations = 1
            for i in range(self.nLept):
                nCombinations = nCombinations * len(self.qualified[i])
            if nCombinations>50000:
                self.setFilterPassed(True)
                return StatusCode.Success
          
        # store here "final" list
        chosen=list()
        
        chosen=self.addChoice(chosen,'***')

        if len(chosen)!=self.nLept:
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success

        ###### add a photon requirement #####
        if self.doPhoton:
            try:
                collection = self.storeGateSvc.retrieve("PhotonContainer" , "PhotonAODCollection")
            except LookupError:
                self.msg.warning( 'Collection %s not found' % "PhotonAODCollection" )
                self.setFilterPassed(False)
                self.nFail+=1
                return StatusCode.Success
            
            passedCuts=False
            for photon in collection :
                if photon.cluster().e()/math.cosh(photon.cluster().eta()) > self.photonPt and math.fabs(photon.cluster().eta())<self.photonEta:
                    passedCuts=True
            if not passedCuts:
                self.setFilterPassed(False)
                self.nFail+=1
                return StatusCode.Success
 
        self.setFilterPassed(True)
        return StatusCode.Success

    def chargeCheck(self,leptons):

        chargetmp=1.
        for iLept in range(len(leptons)):
            chargetmp = chargetmp*leptons[iLept].charge()

        if chargetmp<0:
            return True
        else:
            return False

    def invMass(self,leptons,types):
        if(len(leptons)!=len(types)):
            self.msg.warning('invMass len(leptons)!=len(types) %i != %i' % (len(leptons), len(types)))
            return -9999.

        result = TLorentzVector(0,0,0,0)

        for iLept in range(len(leptons)):
            fourMomentum = TLorentzVector()
            # Electron: calculate 4-momentum based on cluster energy and track direction  
            # Muon: use 4-momentum of the muon object
            if types[iLept]=="e":
                if leptons[iLept].cluster() and leptons[iLept].trackParticle():
                    fourMomentum.SetPtEtaPhiE(leptons[iLept].cluster().e()/math.cosh(leptons[iLept].trackParticle().eta()), \
                                              leptons[iLept].trackParticle().eta(), \
                                              leptons[iLept].trackParticle().phi(), \
                                              leptons[iLept].cluster().e())
                else:
                    fourMomentum.SetPtEtaPhiE(leptons[iLept].et(), leptons[iLept].eta(), leptons[iLept].phi(), leptons[iLept].e())
            else:
                fourMomentum.SetPtEtaPhiE(leptons[iLept].pt(), leptons[iLept].eta(), leptons[iLept].phi(), leptons[iLept].e())
            result = result + fourMomentum

        return result.M()
    
    def additionalPtCuts(self, leptons):
        # Check the validity of "kinematicsPtCuts"
        if self.nLept!=len(self.cutDict["kinematicsPtCuts"]):
            return True
        # Check the consistency between "kinematicsPtCuts" and "type"
        if len(self.cutDict["kinematicsPtCuts"])!=len(self.cutDict["type"]):
            self.msg.warning('additionalPtCuts len(self.cutDict["kinematicsPtCuts"])=%i != len(self.cutDict["type"])=%i' % \
                             (len(self.cutDict["kinematicsPtCuts"]), len(self.cutDict["type"])))
            return True
        # Count the number of leptons above each threshold (threshold values should be sorted)
        nLeptonsAboveThresholds = [0] * self.nLept
        for iLept in range(len(leptons)):
            for iThreshold in range(self.nLept):
                EPT = 0.
                # Electron: calculate 4-momentum based on cluster energy and track direction
                # Muon: use 4-momentum of the muon object
                if self.cutDict["type"][iLept]=="e":
                    if leptons[iLept].cluster() and leptons[iLept].trackParticle():
                        EPT = leptons[iLept].cluster().e()/math.cosh(leptons[iLept].trackParticle().eta())
                    else:
                        EPT = leptons[iLept].et()
                else:
                    EPT = leptons[iLept].pt()
                if EPT > self.cutDict["kinematicsPtCuts"][iThreshold]:
                    nLeptonsAboveThresholds[iThreshold] += 1
        # Check the number of leptons which satisfy each threshold 
        for iThreshold in range(self.nLept):
            if not (nLeptonsAboveThresholds[iThreshold] > iThreshold):
                return False
        return True

    def additionalMassAndQualityCuts(self, leptons):
        # Only for 4 leptons 
        if self.nLept!=4:
            return True
        # Check the validity of "kinematicsMassCuts"
        if len(self.cutDict["kinematicsMassCuts"])!=2:
            return True

        # Check if self.electronLikelihoodTool is available
        if self.electronLikelihoodTool is None:
            self.msg.error("self.electronLikelihoodTool pointer is NULL!")
            return True

        # Run passLikelihood for electrons  
        resultsEID = []
        nBadElectrons = 0
        for iLept in range(4):
            if self.cutDict["type"][iLept]=="e":
                if not bool(self.electronLikelihoodTool.accept(leptons[iLept])):
                    nBadElectrons += 1
                    resultsEID.append(False)
                else:
                    resultsEID.append(True)
            else:
                resultsEID.append(True)
        # In 4e case, at least two electrons should pass eID
        if nBadElectrons>=3:
            return False

        # Set di-lepton mass thresholds     
        MthreshH = self.cutDict["kinematicsMassCuts"][0]
        MthreshL = self.cutDict["kinematicsMassCuts"][1]

        # There are 3 combinations 
        order = [[0,1,2,3], [0,2,1,3], [0,3,1,2]]
        for iOrder in range(len(order)):
            # Calculate di-lepton masses
            pair01 = [leptons[order[iOrder][0]], leptons[order[iOrder][1]]]
            type01 = [self.cutDict["type"][order[iOrder][0]], self.cutDict["type"][order[iOrder][1]]]
            pair23 = [leptons[order[iOrder][2]], leptons[order[iOrder][3]]]
            type23 = [self.cutDict["type"][order[iOrder][2]], self.cutDict["type"][order[iOrder][3]]]
            M01 = self.invMass(pair01, type01)
            M23 = self.invMass(pair23, type23)
            # Check the masses are greater than thresholds 
            if M01 > M23:
                if not (M01>MthreshH and M23>MthreshL):
                    continue
            else:
                if not (M23>MthreshH and M01>MthreshL):
                    continue
            # eID is applied to electron(s) of the primary pair closer to MZ
            MZ = 91187.6 # PDG (2010) in MeV 
            if abs(M01-MZ) < abs(M23-MZ):
                if resultsEID[order[iOrder][0]] and resultsEID[order[iOrder][1]]:
                    return True
            else:
                if resultsEID[order[iOrder][2]] and resultsEID[order[iOrder][3]]:
                    return True
        return False

    def additionalDeltaRCut(self, leptons):
        # If the threshold is equal to or less than 0, nothing is done. 
        if self.cutDict["kinematicsDeltaRCut"]<=0.:
            return True
        minimumDeltaR = 9999.
        etaValues = []
        phiValues = []
        # Fill eta and phi values 
        for iLept in range(len(leptons)):
            if self.cutDict["type"][iLept]=="e" and leptons[iLept].trackParticle() and leptons[iLept].cluster():
                etaValues.append(leptons[iLept].trackParticle().eta())
                phiValues.append(leptons[iLept].trackParticle().phi())
            else:
                etaValues.append(leptons[iLept].eta())
                phiValues.append(leptons[iLept].phi())
        # Calculate minimum delta R value  
        for iLept in range(len(leptons)):
            for jLept in range(len(leptons)):
                if iLept>=jLept:
                    continue
                deltaEta = etaValues[iLept]-etaValues[jLept]
                deltaPhi = phiValues[iLept]-phiValues[jLept]
                if deltaPhi > +math.pi:
                    deltaPhi -= 2.*math.pi
                if deltaPhi < -math.pi:    
                    deltaPhi += 2.*math.pi
                deltaR = math.sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi)    
                if deltaR < minimumDeltaR:
                    minimumDeltaR = deltaR
        # Compare delta R values     
        return (minimumDeltaR > self.cutDict["kinematicsDeltaRCut"])
    
    def addChoice(self,chosen,space):
        
        # make a local copy of the input
        localChosen=copy(chosen)
        
        # recursively create combinations of leptons

        # check escape condition
        if len(localChosen)==self.nLept:
            # no need to continue with the loop
            # must check here for invariant mass

            # Additional kinematics cuts and quality cuts
            passedKinematicsCut = False
            if self.additionalMassAndQualityCuts(localChosen):
                if self.additionalPtCuts(localChosen):
                    if self.additionalDeltaRCut(localChosen):
                        passedKinematicsCut = True 
            if (self.nLept<2 \
                    or ((self.mass==0 or self.invMass(localChosen,self.cutDict["type"])>self.mass) and self.massUp==0 and self.massDo==0) \
                    or (self.mass==0 and self.invMass(localChosen,self.cutDict["type"])>self.massDo and self.invMass(localChosen,self.cutDict["type"])<self.massUp)) \
                    and passedKinematicsCut and (not self.doOS or self.chargeCheck(localChosen)):
                # success: recursion will stop at all levels and resulting set will be returned
                return localChosen
            else:
                # chosen set not good, mark that inv mass failed by popping last element
                # this way recursion will keep on
                localChosen.pop()
                return localChosen

        # not escaped, so let's add another lepton
        chosen2=list()
        counter=0
        
        listToPickFrom=self.qualified[len(localChosen)]
        # look for additional lepton amongst the qualified for the next position
        for anotherLepton in listToPickFrom:
            # one more copy to append to
            chosen2=copy(localChosen)

            if anotherLepton not in localChosen:
                # must also check duplicate leptons
                duplicate=False
                for c in localChosen:
                    dEta = abs( c.eta() - anotherLepton.eta() )
                    dPhi = abs( c.phi() - anotherLepton.phi() )
                    if dEta < 0.0001 and dPhi < 0.0001:
                        duplicate=True
                        
                if not duplicate:
                    chosen2.append(anotherLepton)
                    chosen3=self.addChoice(chosen2,space+'***')
                    if len(chosen3)==len(chosen2)-1:
                        # present anotherLpeton was discarded due to mass check, continue the loop
                        continue
                    if len(chosen3)==self.nLept:
                        # got back a complete set, nothing to do but pass it above
                        return chosen3
                    # otherwise there is nothing to do in this for loop.
            counter+=1
            
        # if we couldn't attach any new lepton, return the original list 
        return localChosen
            
    def finalize(self):

        self.msg.info( 'finalizing: processed %i passed %i failed %i' % (self.nProc, self.nProc-self.nFail, self.nFail) )           
                    
        return StatusCode.Success

class H4lElectronFilter(H4lMultiLeptonFilter):

    def __init__(self, name="H4lElectronFilter",**kw):
        ## initialize base class
        kw['name'] = name
        super(H4lElectronFilter, self).__init__(**kw)
        self.nLept = kw.get('nElec', 4)
        self.mass = kw.get('mass', 0)
        self.cutDict["et"]= kw.get('eTCuts', [])
        self.cutDict["eta"]= kw.get('etaCuts', [])
        self.cutDict["type"] = kw.get('types',[])
        self.cutDict["quality"] = kw.get('qualityCuts', [])
        self.collectionName= kw.get('collectionName', "ElectronAODCollection")
        self.acceptFrwdE=kw.get('acceptFrwdE', True)

        # If type is not given, fill "e" for the given number of leptons 
        if len(self.cutDict["type"])==0 and self.nLept>0:
            for iLept in range(self.nLept):
                self.cutDict["type"].append("e")

        for i in range(self.nLept):
            self.qualified[i]=list()

    def findQualified(self):

        try:
            electronCollection = self.storeGateSvc.retrieve( "ElectronContainer", self.collectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.collectionName )           
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success   

        for electron in electronCollection :
            for i in range(self.nLept):

                passedCuts=True
                if len(self.cutDict["et"]):
                    if electron.cluster() and electron.trackParticle():
                        passedCuts =  electron.cluster().e()/math.cosh(electron.trackParticle().eta()) > self.cutDict["et"][i]
                    else:
                        passedCuts= passedCuts and electron.et() > self.cutDict["et"][i]
                
                if len(self.cutDict["eta"]):
                    passedCuts= passedCuts and abs(electron.eta()) < self.cutDict["eta"][i]

                passedCuts= passedCuts and checkElectronQuality(self.cutDict["quality"][i],electron, self.acceptFrwdE)

                if passedCuts:
                    self.qualified[i].append(electron)

class H4lMuonFilter(H4lMultiLeptonFilter):

    def __init__(self, name="H4lMuonFilter",**kw):
        ## initialize base class
        kw['name'] = name
        super(H4lMuonFilter, self).__init__(**kw)
        self.mass = kw.get('mass', 0)
        self.nLept = kw.get('nMu', 4)
        self.cutDict["pt"] = kw.get('pTCuts', [])
        self.cutDict["eta"] = kw.get('etaCuts',[])
        self.cutDict["type"] = kw.get('types',[])
        self.cutDict["quality"] = kw.get('qualityCuts',[])
        self.collectionName= kw.get('collectionName', "StacoMuonCollection")

        # initialize cut dict with same values

        # If type is not given, fill "mu" for the given number of leptons 
        if len(self.cutDict["type"])==0 and self.nLept>0:
            for iLept in range(self.nLept):
                self.cutDict["type"].append("mu")

        for i in range(self.nLept):
            self.qualified[i]=list()

    def findQualified(self):
        
        try:
            muonCollection = self.storeGateSvc.retrieve("Analysis::MuonContainer" , self.collectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.collectionName )           
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success   

        
        for muon in muonCollection :

            for i in range(self.nLept):

                passedCuts=True
                
                if len(self.cutDict["pt"]):
                    passedCuts=passedCuts and muon.pt() > self.cutDict["pt"][i]
                if len(self.cutDict["eta"]):
                    passedCuts=passedCuts and abs(muon.eta()) < self.cutDict["eta"][i]

                passedCuts = passedCuts and checkMuonQuality(self.cutDict["quality"][i],muon)
                
                if passedCuts:
                    self.qualified[i].append(muon)

class H4lJpsiMuonFilter(H4lMultiLeptonFilter):

    def __init__(self, name="H4lJpsiMuonFilter",**kw):
        ## initialize base class
        kw['name'] = name
        super(H4lJpsiMuonFilter, self).__init__(**kw)
        self.nLept = kw.get('nMu', 2)
        self.massUp = kw.get('massUp', 0)
        self.massDo = kw.get('massDo', 0)
        self.doOS= kw.get('doOS',False)
        self.doPhoton= kw.get('doPhoton',False)
        self.photonPt= kw.get('photonPt',0)
        self.photonEta=kw.get('photonEta',5);
        self.cutDict["pt"] = kw.get('pTCuts', [])
        self.cutDict["eta"] = kw.get('etaCuts',[])
        self.cutDict["type"] = kw.get('types',[])
        self.cutDict["quality"] = kw.get('qualityCuts',[])
        self.collectionName= kw.get('collectionName', "StacoMuonCollection")

        # initialize cut dict with same values

        # If type is not given, fill "mu" for the given number of leptons 
        if len(self.cutDict["type"])==0 and self.nLept>0:
            for iLept in range(self.nLept):
                self.cutDict["type"].append("mu")

        for i in range(self.nLept):
            self.qualified[i]=list()

    def findQualified(self):
        
        try:
            muonCollection = self.storeGateSvc.retrieve("Analysis::MuonContainer" , self.collectionName )
        except LookupError:
            #if self.nProcessed <100:
            self.msg.warning( 'Collection %s not found' % self.collectionName )           
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success   

        
        for muon in muonCollection :

            for i in range(self.nLept):

                passedCuts=True
                
                if len(self.cutDict["pt"]):
                    passedCuts=passedCuts and muon.pt() > self.cutDict["pt"][i]
                if len(self.cutDict["eta"]):
                    passedCuts=passedCuts and abs(muon.eta()) < self.cutDict["eta"][i]

                passedCuts = passedCuts and checkMuonQuality(self.cutDict["quality"][i],muon)
                
                if passedCuts:
                    #print 'muon',muon,'passed cuts',i
                    self.qualified[i].append(muon)
                #else:
                    #print 'muon',muon,'failed cuts',i
     #   print self.qualified



class H4lMixedFilter(H4lMultiLeptonFilter):

    def __init__(self, name="H4lMixedFilter",**kw):
        ## initialize base class
        kw['name'] = name
        super(H4lMixedFilter, self).__init__(**kw)
        self.mass = kw.get('mass', 0)
        self.nLept = kw.get('nLept', 4)
        self.cutDict["pt"] = kw.get('pTCuts', [])
        self.cutDict["et"] = kw.get('eTCuts', [])
        self.cutDict["eta"] = kw.get('etaCuts',[])
        self.cutDict["type"] = kw.get('types',[])
        self.cutDict["quality"] = kw.get('qualityCuts',[])
        self.cutDict["coll"] = kw.get('collections',[])
        # initialize cut dict with same values
        self.acceptFrwdE=kw.get('acceptFrwdE', True)

        for i in range(self.nLept):
            self.qualified[i]=list()


    def checkQuality(self,lepton,number):

        if self.cutDict["type"][number]=="mu":

            return checkMuonQuality(self.cutDict["quality"][number], lepton)

        elif self.cutDict["type"][number]=="e":

            return checkElectronQuality(self.cutDict["quality"][number], lepton,self.acceptFrwdE)

        else:
            return checkPhotonQuality(self.cutDict["quality"][number], lepton)


    def findQualified(self):

        for i in range(self.nLept):

            collection=0
            
            if self.cutDict["type"][i]=="mu":
                
                try:
                    collection = self.storeGateSvc.retrieve("Analysis::MuonContainer" , self.cutDict["coll"][i])
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.cutDict["coll"][i] )           
                    self.setFilterPassed(False)
                    self.nFail+=1
                    return StatusCode.Success

            elif self.cutDict["type"][i]=="e":
                
                try:
                    collection = self.storeGateSvc.retrieve("ElectronContainer" , self.cutDict["coll"][i])
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.cutDict["coll"][i] )           
                    self.setFilterPassed(False)
                    self.nFail+=1
                    return StatusCode.Success

            else:
                try:
                    collection = self.storeGateSvc.retrieve("PhotonContainer" , self.cutDict["coll"][i])
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.cutDict["coll"][i] )           
                    self.setFilterPassed(False)
                    self.nFail+=1
                    return StatusCode.Success
                
        
            for lepton in collection :

                passedCuts=True

                if len(self.cutDict["pt"]):
                    passedCuts= passedCuts and lepton.pt() > self.cutDict["pt"][i]

                if len(self.cutDict["eta"]):
                    passedCuts= passedCuts and abs(lepton.eta()) < self.cutDict["eta"][i]

                if len(self.cutDict["et"]):
                    if self.cutDict["type"][i]=="e":
                        if lepton.cluster() and lepton.trackParticle():
                            passedCuts =  lepton.cluster().e()/math.cosh(lepton.trackParticle().eta()) > self.cutDict["et"][i]
                        else:
                            passedCuts= passedCuts and lepton.et() > self.cutDict["et"][i]
                    elif self.cutDict["type"][i]=="gamma":
                        if lepton.cluster():
                            passedCuts =  lepton.cluster().pt() > self.cutDict["et"][i]
                        else:
                            passedCuts =  lepton.et() > self.cutDict["et"][i]

                    else:
                        passedCuts= passedCuts and lepton.et() > self.cutDict["et"][i]

                passedCuts=passedCuts and self.checkQuality(lepton, i)

                if passedCuts:
                    self.qualified[i].append(lepton)
