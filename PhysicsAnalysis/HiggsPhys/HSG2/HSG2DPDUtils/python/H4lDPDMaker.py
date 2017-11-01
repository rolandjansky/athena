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
from ROOT import ElectronLikelihoodTool
from ROOT import MultiLeptonMenu

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

def passMultiLepton(electron, multiLeptonMenu):
    # Check objects are available 
    if not electron.trackParticle():
        return False
    if not electron.trackParticle().trackSummary():
        return False
    if not electron.trackParticle().measuredPerigee():
        return False

    # double etas2           =  el_etas2;
    etas2 = electron.cluster().etaBE(2)
    if abs(etas2) > 300.:
        return False
    # double Et_cl           =  el_cl_E/cosh(el_etas2);
    Et_cl = electron.cluster().e()/math.cosh(etas2) 
    if Et_cl == 0.:
        return False
    # double DEmaxs1         = fabs(el_emaxs1+el_Emax2)>0. ? (el_emaxs1-el_Emax2)/(el_emaxs1+el_Emax2) : 0.;
    DEmaxs1 = 0.
    emaxs1 = electron.detailValue(egammaParameters.emaxs1)
    Emax2 = electron.detailValue(egammaParameters.e2tsts1)
    if abs(emaxs1+Emax2) > 0.:
        DEmaxs1 = (emaxs1-Emax2)/(emaxs1+Emax2)

    # double rHad            =  el_Ethad/Et_cl;
    rHad = electron.detailValue(egammaParameters.ethad)/Et_cl
    # double rHad1           =  el_Ethad1/Et_cl;
    rHad1 = electron.detailValue(egammaParameters.ethad1)/Et_cl
    # double Reta            =  el_reta;
    e237 = electron.detailValue(egammaParameters.e237)
    e277 = electron.detailValue(egammaParameters.e277)
    Reta = 0.
    if e277 != 0.:
        Reta = e237/e277
    # double w2              =  el_weta2;
    w2 = electron.detailValue(egammaParameters.weta2)
    # double f1              =  el_f1;
    f1 = electron.detailValue(egammaParameters.f1)
    # double f3              =  el_f3;
    f3 = electron.detailValue(egammaParameters.f3)
    # double wstot           =  el_wstot;
    wstot = electron.detailValue(egammaParameters.wtots1)
    # double deltaEta        = el_deltaeta1;
    deltaEta = electron.detailValue(egammaParameters.deltaEta1)
    # double deltaPhiRescaled = el_deltaphiRescaled;
    deltaPhiRescaled = electron.detailValue(egammaParameters.deltaPhiRescaled)

    # int    nPix             = el_nPixHits;
    nPix = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelHits)
    # int    nSi              = el_nSiHits;
    nSi = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTHits)+nPix
    # int    nPixDeadSensors  = el_nPixelDeadSensors;
    nPixDeadSensors = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelDeadSensors)
    # int    nSiDeadSensors   = el_nSCTDeadSensors+el_nPixelDeadSensors;
    nSiDeadSensors = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTDeadSensors)+nPixDeadSensors
    # int    nTRThigh         = el_nTRTHighTHits;
    nTRThigh = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdHits)
    # int    nTRThighOutliers = el_nTRTHighTOutliers;
    nTRThighOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdOutliers) 
    # int    nTRT             = el_nTRTHits;
    nTRT = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHits)
    # int    nTRTOutliers     = el_nTRTOutliers;
    nTRTOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTOutliers)
    # bool   expectBlayer     = el_expectBLayerHit;
    expectBlayer = electron.trackParticle().trackSummary().get(ROOT.Trk.expectBLayerHit)
    # int    nBlayerHits      = el_nBLHits;
    nBlayerHits = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfBLayerHits)
        
    nTRTTotal = nTRT+nTRTOutliers 
    rTRT = 0.;
    if nTRTTotal > 0:
        rTRT = float(nTRThigh+nTRThighOutliers)/float(nTRTTotal)
    
    dpOverp =0.
    trackqoverp = electron.trackParticle().measuredPerigee().parameters()[ROOT.Trk.qOverP]
    for id in range(electron.nDetails()):
        if not electron.detailElementLink(id).isValid():
            continue
        if electron.detailName(id)!="egDetailAOD": 
            print 'H4lDPDMaker::passMultiLepton WARNING electron.detailName(%i) = %s != egDetailAOD' % (id, electron.detailName(id))
            continue
        if not isinstance(electron.detail(id), PyAthena.EMTrackFit): # This corresponds to dynamic_cast in C++
            continue
        if electron.detail(id).linkIndex()>=electron.nTrackParticles():
            continue
        if electron.detail(id).bremTrackAuthor()!=4:
            continue
        dpOverp = 1.-(trackqoverp/electron.detail(id).track_LastM_qOverP())
        break

    return multiLeptonMenu.passMultiLepton(etas2, Et_cl,\
                                           rHad, rHad1, Reta, w2,\
                                           f1,f3, wstot, DEmaxs1,\
                                           deltaEta, nSi,nSiDeadSensors, nPix,\
                                           nPixDeadSensors,deltaPhiRescaled,dpOverp,\
                                           rTRT,nTRTTotal,nBlayerHits,expectBlayer);

def passLikelihood(electron, electronLikelihoodTool, trackToVertexTool, primaryVertexPosition, numberOfPrimaryVertices):
    # Check objects are available 
    if not electron.trackParticle():
        return False
    if not electron.trackParticle().trackSummary():
        return False
    if not electron.trackParticle().measuredPerigee():
        return False

    # eta              : el_etas2
    eta = electron.cluster().etaBE(2)
    if abs(eta) > 300.:
        return False
    # eT (in MeV)      : el_cl_E/cosh(el_etas2)
    eT = electron.cluster().e()/math.cosh(eta) 
    if eT == 0.:
        return False
    # f3               : el_f3
    f3 = electron.detailValue(egammaParameters.f3)
    # rHad             : el_Ethad / eT   
    rHad = electron.detailValue(egammaParameters.ethad)/eT
    # rHad1            : el_Ethad1/ eT
    rHad1 = electron.detailValue(egammaParameters.ethad1)/eT
    # Reta             : el_reta
    e237 = electron.detailValue(egammaParameters.e237)
    e277 = electron.detailValue(egammaParameters.e277)
    Reta = 0.
    if e277 != 0.:
        Reta = e237/e277
    # w2               : el_weta2
    w2 = electron.detailValue(egammaParameters.weta2)
    # f1               : el_f1
    f1 = electron.detailValue(egammaParameters.f1)
    # wstot            : el_wstot
    # wstot = electron.detailValue(egammaParameters.wtots1)
    # DEmaxs1          : (el_emaxs1+el_Emax2 == 0.) ? 0. : (el_emaxs1-el_Emax2)/(el_emaxs1+el_Emax2)
    DEmaxs1 = 0.
    emaxs1 = electron.detailValue(egammaParameters.emaxs1)
    Emax2 = electron.detailValue(egammaParameters.e2tsts1)
    if abs(emaxs1+Emax2) > 0.:
        DEmaxs1 = (emaxs1-Emax2)/(emaxs1+Emax2)
    # deltaEta         : el_deltaeta1
    deltaEta = electron.detailValue(egammaParameters.deltaEta1)
    # d0               : el_trackd0pv defined in PhysicsAnalysis/D3PDMaker/egammaD3PDAnalysis/src/egammaTrackImpactAlg.cxx
    d0 = -9999. 
    newMeasPerigee = trackToVertexTool.perigeeAtVertex(electron.trackParticle(), primaryVertexPosition) 
    ROOT.SetOwnership(newMeasPerigee, True) 
    if newMeasPerigee != None:
        d0 = newMeasPerigee.parameters()[ROOT.Trk.d0];
    # TRratio          : el_TRTHighTOutliersRatio
    # Defined in PhysicsAnalysis/D3PDMaker/TrackD3PDMaker/src/TrackTrackSummaryFillerTool.cxx
    # if (ntrtxenon > 0)
    #   *m_TRTHighTOutliersRatio = (float)(nhightrt + nhightrt_outl) / (float)(ntrtxenon);
    # else if (ntrtxenon < 0 && (ntrt + ntrt_outl) > 0) {
    #   // Backwards compatibility if xenon information isn't present.
    #   *m_TRTHighTOutliersRatio = (float)(nhightrt + nhightrt_outl) / (float)(ntrt + ntrt_outl);
    TRratio = 0.
    # ntrtxenon = numberOfTRTXenonHits
    ntrtxenon = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTXenonHits)
    # nhightrt = numberOfTRTHighThresholdHits
    nhightrt = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdHits)
    # nhightrt_outl = numberOfTRTHighThresholdOutliers
    nhightrt_outl = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHighThresholdOutliers) 
    # ntrt = numberOfTRTHits
    ntrt = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTHits)
    # ntrt_outl = numberOfTRTOutliers
    ntrt_outl = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfTRTOutliers)
    if ntrtxenon>0:
        TRratio = float(nhightrt + nhightrt_outl) / float(ntrtxenon)
    elif ntrtxenon<0 and (ntrt+ntrt_outl)>0:
        TRratio = float(nhightrt + nhightrt_outl) / float(ntrt + ntrt_outl)
    # d0sigma          : el_tracksigd0pv
    d0sigma = -9999.
    if newMeasPerigee != None:
        d0sigma = newMeasPerigee.localErrorMatrix().error(ROOT.Trk.d0)
    # fside            : el_fside defined in PhysicsAnalysis/D3PDMaker/egammaD3PDMaker/python/ElectronD3PDObject.py
    # fside = electron.detailValue(egammaParameters.fracs1)
    # rphi             : el_rphi defined in PhysicsAnalysis/D3PDMaker/egammaD3PDMaker/src/egammaRetaphiFillerTool.cxx
    rphi = 0. 
    if e237 != 0.:
        e233 = electron.detailValue(egammaParameters.e233)
        rphi = e233/e237
    # ws3              : el_ws3 defined in PhysicsAnalysis/D3PDMaker/egammaD3PDMaker/python/ElectronD3PDObject.py 
    ws3 = electron.detailValue(egammaParameters.weta1)
    # deltaPoverP      : see below.
    # double dpOverp = 0.;
    # for (unsigned int i = 0; i<el_refittedTrack_LMqoverp[el].size();++i) {
    #        if((el_refittedTrack_author[el]).at(i)== 4) {
    #          dpOverp= 1.-(el_trackqoverp[el]/(el_refittedTrack_LMqoverp[el].at(i)));
    #             }
    # }
    # return dpOverp;           
    deltaPoverP = 0. 
    trackqoverp = electron.trackParticle().measuredPerigee().parameters()[ROOT.Trk.qOverP]
    for id in range(electron.nDetails()):
        if not electron.detailElementLink(id).isValid():
            continue
        if electron.detailName(id)!="egDetailAOD": 
            print 'H4lDPDMaker::passLikelihood WARNING electron.detailName(%i) = %s != egDetailAOD' % (id, electron.detailName(id))
            continue
        if not isinstance(electron.detail(id), PyAthena.EMTrackFit): # This corresponds to dynamic_cast in C++
            continue
        if electron.detail(id).linkIndex()>=electron.nTrackParticles():
            continue
        if electron.detail(id).bremTrackAuthor()!=4:
            continue
        deltaPoverP = 1.-(trackqoverp/electron.detail(id).track_LastM_qOverP())
        break
    # deltaphires      : el_deltaphiRescaled 
    deltaphires = electron.detailValue(egammaParameters.deltaPhiRescaled)
    # nSi              : el_nSiHits
    nSi = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTHits)+electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelHits)
    # nSiDeadSensors   : el_nPixelDeadSensors + el_nSCTDeadSensors
    nSiDeadSensors = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfSCTDeadSensors)+electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelDeadSensors)
    # nPix             : el_nPixHits
    nPix = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelHits)
    # nPixDeadSensors  : el_nPixelDeadSensors
    nPixDeadSensors = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfPixelDeadSensors)
    # nBlayer          : el_nBLHits
    nBlayer = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfBLayerHits)
    # nBlayerOutliers  : el_nBLayerOutliers
    nBlayerOutliers = electron.trackParticle().trackSummary().get(ROOT.Trk.numberOfBLayerOutliers)
    # expectBlayer     : el_expectHitInBLayer
    expectBlayer = bool(electron.detailValue(egammaParameters.expectHitInBLayer))
    # convBit          : el_isEM & (0x1 << egammaPID::ConversionMatch_Electron)
    # el_isEM is defined in PhysicsAnalysis/D3PDMaker/egammaD3PDMaker/src/egammaIsEMoneFillerTool.cxx
    convBit = long(electron.egammaID(egammaPID.IsEM)) & (0x1 << egammaPID.ConversionMatch_Electron)
    # ip               : Count number of vertices in vxp_n with >= 2 tracks in vxp_trk_n
    ip = numberOfPrimaryVertices

    result = electronLikelihoodTool.passLikelihood(ROOT.LikeEnum.Loose_BL_Pix, # LikeEnum::Menu operating_point, Loose_BL_Pix=100 
                                                   eta, eT, f3, rHad, rHad1,
                                                   Reta, w2, f1, DEmaxs1,
                                                   deltaEta, d0, TRratio,
                                                   d0sigma,
                                                   rphi, ws3,
                                                   deltaPoverP, deltaphires,
                                                   nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                                                   nBlayer, nBlayerOutliers, expectBlayer,
                                                   convBit, ip)
    return result

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
        self.space='**'
        self.checkLArError = kw.get('checkLArError', True)
        self.primaryVertexPosition = None
        self.numberOfPrimaryVertices = 0

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
            return StatusCode.Failure
             
        self.trackToVertexTool = PyAthena.py_tool('Reco::TrackToVertex', iface='Reco::ITrackToVertex')
        if self.trackToVertexTool is None:
            self.msg.error("Problem retrieving Reco::TrackToVertex pointer !!")
            return StatusCode.Failure
                                
        if not os.path.exists("./ElectronLikelihoodPdfs.root"):
            os.system("get_files -data ElectronLikelihoodPdfs.root")
        self.electronLikelihoodTool = ElectronLikelihoodTool("./ElectronLikelihoodPdfs.root")
        self.multiLeptonMenu = MultiLeptonMenu()

        return StatusCode.Success

    def findQualified(self):
        
        # this is lepton-type dependent, leave it to daughter classes
        pass


    def execute(self):
        
        # must clean up qualified list from previous event

        for k in self.qualified.keys():
            self.qualified[k]=[]


        #print 'EXECUTING',self.name()
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

        # Primary vertex position and the number of primary vertices 
        try:
            vxcontainer = self.storeGateSvc.retrieve("VxContainer", "VxPrimaryCandidate")
        except LookupError:
            self.msg.warning("VxPrimaryCandidate is not found")
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success
        self.primaryVertexPosition = vxcontainer[0].recVertex().position()
        self.numberOfPrimaryVertices = 0
        for primaryVertex in vxcontainer:
            trackAtVertex = primaryVertex.vxTrackAtVertex()
            if trackAtVertex and trackAtVertex.size()>=2:
                self.numberOfPrimaryVertices += 1

                
        self.findQualified()

        #print 'found qualified',self.qualified 

        # need to see whether a proper combination of qualified electrons allows to satisfy all conditions
        # for the time being let's check all combinations...

#        print self.name(),self.qualified    


#        for  k in self.qualified.keys():
#
#            for l in self.qualified[k]:
#                
#                print l,l.author(),l.eta(),l.pt()
#
        # first check the easiest case: whether any cut was never passed
        
        for i in range(self.nLept):
            if not len(self.qualified[i]):
                self.setFilterPassed(False)
                self.nFail+=1 
                #print self.name(), "FAIL 1 ",self.cutDict, 'qualified: ',self.qualified 
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

        #print self.name,chosen
        #for l in chosen:
        #    print l.eta(),l.phi(),l.pt()
        
        if len(chosen)!=self.nLept:
            #print self.name(), "FAIL 2 ",self.cutDict,'chosen:', chosen

            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success

        #print self.name(), "SUCCESS ",self.cutDict, 'chosen:',chosen

        self.setFilterPassed(True)

        return StatusCode.Success

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

        #print "total TLV is ", result.Px(),result.Py(),result.Pz(),result.E()
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

        # Run (passLikelihood OR passMultiLepton) for electrons  
        resultsEID = []
        nBadElectrons = 0
        for iLept in range(4):
            if self.cutDict["type"][iLept]=="e":
                if not (passLikelihood(leptons[iLept], self.electronLikelihoodTool, self.trackToVertexTool, self.primaryVertexPosition, self.numberOfPrimaryVertices) or \
                        passMultiLepton(leptons[iLept], self.multiLeptonMenu)):
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
        
        #print space,"adding choice to chosen",chosen

        # make a local copy of the input
        localChosen=copy(chosen)
        
        # recursively create combinations of leptons

        # check escape condition
        if len(localChosen)==self.nLept:
            #print space,'check escape with',localChosen
            # no need to continue with the loop
            # must check here for invariant mass

            # Additional kinematics cuts and quality cuts
            passedKinematicsCut = False
            if self.additionalMassAndQualityCuts(localChosen):
                if self.additionalPtCuts(localChosen):
                    if self.additionalDeltaRCut(localChosen):
                        passedKinematicsCut = True 

            if (self.nLept<2 or self.mass==0 or self.invMass(localChosen,self.cutDict["type"])>self.mass) and passedKinematicsCut:
                # success: recursion will stop at all levels and resulting set will be returned
                return localChosen
            else:
                # chosen set not good, mark that inv mass failed by popping last element
                # this way recursion will keep on
                #print space,'bad mass',self.nLept,self.mass,self.invMass(localChosen,self.cutDict["type"]),localChosen
                localChosen.pop()
                #print space,'returning popped chosen',localChosen
                return localChosen

        # not escaped, so let's add another lepton
        #print space,'not complete yet, adding one more lepton'
        chosen2=list()
        counter=0
        
        listToPickFrom=self.qualified[len(localChosen)]
        # look for additional lepton amongst the qualified for the next position
        for anotherLepton in listToPickFrom:
            #print space,'starting iteration. chosen is',localChosen,'anotherLepton now',anotherLepton
            # one more copy to append to
            chosen2=copy(localChosen)

            #print space,'try to add a lepton from', listToPickFrom ,'(',anotherLepton,') to the existing',len(localChosen)
            #print space,'chosen2 is now ',chosen2,'chosen is',localChosen
            if anotherLepton not in localChosen:
                #print space,'now considering' ,anotherLepton,'to be put in',localChosen
                # must also check duplicate leptons
                duplicate=False
                for c in localChosen:
                    dEta = abs( c.eta() - anotherLepton.eta() )
                    dPhi = abs( c.phi() - anotherLepton.phi() )
                    if dEta < 0.0001 and dPhi < 0.0001:
                        duplicate=True
                        
                if not duplicate:
                    #print space,'appending',anotherLepton
                    chosen2.append(anotherLepton)
                    chosen3=self.addChoice(chosen2,space+'***')
                    #print space,'got back ',chosen3, 'started from',chosen2
                    if len(chosen3)==len(chosen2)-1:
                        # present anotherLpeton was discarded due to mass check, continue the loop
                        continue
                    if len(chosen3)==self.nLept:
                        # got back a complete set, nothing to do but pass it above
                        return chosen3
                    # otherwise there is nothing to do in this for loop.
                    #elif len(chosen2)==len(chosen):
                        # recursion couldn't find any other lepton to complete the combination
                        # this combination is sterile, pass to the next anotherLepton
                        # remove the one we just appended, since it is not working
            #print space,'now finished',anotherLepton,counter,'moving to the next one. chosen2 is now',chosen2,'chosen was',localChosen
            counter+=1
            
        #print space,'no more leptons. returning back. chosen2',chosen2,'chosen was',localChosen
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
#        self.sameCuts = kw.get('sameCuts', False)
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
#
    def findQualified(self):

        try:
            electronCollection = self.storeGateSvc.retrieve( "ElectronContainer", self.collectionName )
        except LookupError:
            #if self.nProc <100:
            self.msg.warning( 'Collection %s not found' % self.collectionName )           
            self.setFilterPassed(False)
            self.nFail+=1
            return StatusCode.Success   

        #print "looking into electrons from  ",self.collectionName, "there are",len(electronCollection)

        for electron in electronCollection :

            #if electron.cluster() and electron.trackParticle():
            #    print "checking electron ",electron," Et1 ", electron.cluster().e()/math.cosh(electron.trackParticle().eta())," eta ",electron.eta()
            #else:
            #    print "checking electron ",electron," Et2 ", electron.et()," eta ",electron.eta()

            for i in range(self.nLept):

                passedCuts=True
                if len(self.cutDict["et"]):
                    if electron.cluster() and electron.trackParticle():
                        passedCuts =  electron.cluster().e()/math.cosh(electron.trackParticle().eta()) > self.cutDict["et"][i]
                        #print electron.cluster().e()/math.cosh(electron.trackParticle().eta()),electron.et()
                    else:
                        passedCuts= passedCuts and electron.et() > self.cutDict["et"][i]
                #print "after et ",passedCuts
                
                if len(self.cutDict["eta"]):
                    passedCuts= passedCuts and abs(electron.eta()) < self.cutDict["eta"][i]

                #print "after eta ",passedCuts
                
                passedCuts= passedCuts and checkElectronQuality(self.cutDict["quality"][i],electron, self.acceptFrwdE)

                #print "after qual ",passedCuts
                
                if passedCuts:
                    self.qualified[i].append(electron)
                #else:
                #    print 'electron',electron,'failed cuts',i
                    
        #print self.qualified

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
                #print "looking into electrons from  ",self.cutDict["coll"][i], "there are",len(collection)

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
                            #print lepton.cluster().e()/math.cosh(lepton.trackParticle().eta()),lepton.et()
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
                    #print 'lepton',lepton,'passed cuts',i
                    self.qualified[i].append(lepton)
                #else:
                    #print 'lepton',lepton,'failed cuts',i
