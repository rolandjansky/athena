# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TauTools/python/TauAssocdump.py
## @purpose TauEDMdumper dumps the tau EDM contents. However, we also need to 
## dump the contents of some associated objects (eg. tracks). This class 
## contains the methods needed to dump those associated objects 
## @author Dugan O'Neil with lots of help from Sebastien Binet and Doug Schouten

import inspect
from array import array
import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena

# add dictionary x and dictionary y into dictionary z
# z = dict(x, **y)
# OR
# z.update(y) should add dictionary y to existing dictionary z (overwriting 
# any repeated keys)

maxTracks = 8 
maxPiZero = 4
trackVarList = ["pt","eta","phi"]
piZeroVarList = ["pt","eta","phi"]
secVtxPosList = ["x","y","z"]
secVtxFitList = ["chiSquared","numberDoF"]
sumPi0VecList = ["e","px","py","pz"]
tauPIDVarList = ["SafeLikelihood", "Likelihood", "BDTJetScore", "BDTEleScore", "BDTJetScoreSigTrans", "BDTJetScoreBkgTrans"]
tauPIDisTauList = ["TauCutLoose", "TauCutMedium", "TauCutTight", "MuonVeto", "TauLlhTight", "TauLlhMedium", "TauLlhLoose", 
                   "ElectronVetoLoose", "ElectronVetoMedium", "ElectronVetoTight", "JetBDTSigLoose", "JetBDTSigMedium", 
                   "JetBDTSigTight", "EleBDTLoose", "EleBDTMedium", "EleBDTTight", "JetBDTBkgLoose", "JetBDTBkgMedium", "JetBDTBkgTight"]
assocKey = "Tau"


##############################################
# init secondary vertex dump
def initSecVtx(maxTaus):
    localVarDict  = {}
    for method in secVtxPosList:
        secvtxPosKey = assocKey+"_secVtx_position_"+method
        localVarDict[secvtxPosKey] = array('f',maxTaus*[0.])
    for col in range(3):
        for row in range(3):
            covKey = assocKey+"_secVtx_errorPosition_covariance_"+str(row)+str(col)
            localVarDict[covKey] = array('f',maxTaus*[0.])
    for method in secVtxFitList:    
        fitKey = assocKey+"_secVtx_fitQuality_"+method
        localVarDict[fitKey] = array('f',maxTaus*[0.])    
    return localVarDict
##
def dumpSecVtx(tauCounter,detail,varDict):
    myVtx = PyAthena.Trk.RecVertex()
    myVtx = getattr(detail,"secVtx")()
    if str(type(myVtx)).find("NoneType") is not -1:
        pass
    else:
        # first get position            
        for pos in secVtxPosList:
            posKey = assocKey+"_secVtx_position_"+pos
            varDict[posKey][tauCounter] = float(getattr(myVtx.position(),pos)())
            # now get covariance matrix
        for col in range(1,4):
            for row in range(1,4):
                covKey = assocKey+"_secVtx_errorPosition_covariance_"+str(row-1)+str(col-1)
                varDict[covKey][tauCounter] = float(myVtx.errorPosition().covariance()(row,col))
            # now get fit quality
        for method in secVtxFitList:    
            fitKey = assocKey+"_secVtx_fitQuality_"+method
            varDict[fitKey][tauCounter] = float(getattr(myVtx.fitQuality(),method)())

    return varDict
##############################################
# init TauPID dump
def initTauPID(maxTaus):
   localVarDict = {}
   for IDtype in tauPIDVarList:
       idKey = assocKey+"_TauPID_"+IDtype
       localVarDict[idKey] = array('f',maxTaus*[0.])
   return localVarDict    
##
def dumpTauPID(tauCounter,pid,varDict):   
    for IDtype in tauPIDVarList:
        idKey = assocKey+"_TauPID_"+IDtype
        discNo = eval("PyAthena.TauJetParameters."+IDtype)
        varDict[idKey][tauCounter] = float(pid.discriminant(discNo))
    return varDict    
    
##############################################
# init isTau dump
def initIsTau(maxTaus):
   localVarDict = {}
   for IDtype in tauPIDisTauList:
       idKey = assocKey+"_TauPID_"+IDtype
       localVarDict[idKey] = array('f',maxTaus*[0.])
   return localVarDict    
##
def dumpIsTau(tauCounter,pid,varDict):   
    for IDtype in tauPIDisTauList:
        idKey = assocKey+"_TauPID_"+IDtype
        isTauLabel = eval("PyAthena.TauJetParameters."+IDtype)
        varDict[idKey][tauCounter] = float(pid.isTau(isTauLabel))
    return varDict        

    
##############################################
# track dump. Dump up to maxTracks tracks which are
# associated with the tau.
def initTrkDump(maxTaus):
    localVarDict  = {}
    for itrack in range(maxTracks):
        for trackVar in trackVarList:
            trackKey = assocKey+"_Trk"+str(itrack)+"_"+trackVar
            localVarDict[trackKey] = array('f',maxTaus*[0.])
    return localVarDict
##
def dumpTrkLink(tauCounter, tau, varDict):
    trackCounter = 0
    for track in enumerate(tau[1].trackLinkVector()):
        if trackCounter < maxTracks:
            for trackMethod in trackVarList:
                trackKey = assocKey+"_Trk"+str(trackCounter)+"_"+trackMethod
                varDict[trackKey][tauCounter] = getattr(track[1],trackMethod)()
            trackCounter += 1
    return varDict
def initSeedCaloTrkDump(maxTaus):
    localVarDict  = {}
    for itrack in range(maxTracks):
        for trackVar in trackVarList:
            seedCaloTrackKey = assocKey+"_SeedCaloTrk"+str(itrack)+"_"+trackVar
            localVarDict[seedCaloTrackKey] = array('f',maxTaus*[0.])
    return localVarDict
##
def dumpSeedCaloTrkLink(tauCounter, tau, varDict):
    trackCounter = 0
    for track in enumerate(tau[1].trackLinkVector()):
        if trackCounter < maxTracks:
            for trackMethod in trackVarList:
                trackKey = assocKey+"_SeedCaloTrk"+str(trackCounter)+"_"+trackMethod
                varDict[trackKey][tauCounter] = getattr(track[1],trackMethod)()
            trackCounter += 1
    return varDict    
    
## SeedCalo wide tracks are in details, not in TauJet. Save same variables.
def initSeedCaloWideTrkDump(maxTaus):
    localVarDict  = {}
    for itrack in range(maxTracks):
        for trackVar in trackVarList:
            seedCaloWideTrackKey = assocKey+"_SeedCaloWideTrk"+str(itrack)+"_"+trackVar
            localVarDict[seedCaloWideTrackKey] = array('f',maxTaus*[0.])
    return localVarDict
#
def dumpSeedCaloWideTrks(tauCounter, detail, varDict):
    for itrack in range(detail.seedCalo_nWideTrk()):
        if itrack < maxTracks:
            for trackMethod in trackVarList:
                trackKey = assocKey+"_SeedCaloWideTrk"+str(itrack)+"_"+trackMethod
                varDict[trackKey][tauCounter] = getattr(detail.seedCalo_wideTrk(itrack),trackMethod)()

    return varDict
    
## Other tracks are in details, not in TauJet. Save same variables.
def initOtherTrkDump(maxTaus):
    localVarDict  = {}
    for itrack in range(maxTracks):
        for trackVar in trackVarList:
            otherTrackKey = assocKey+"_OtherTrk"+str(itrack)+"_"+trackVar
            localVarDict[otherTrackKey] = array('f',maxTaus*[0.])
    return localVarDict
#
def dumpOtherTrks(tauCounter, detail, varDict):
    for itrack in range(detail.nOtherTrk()):
        if itrack < maxTracks:
            for trackMethod in trackVarList:
                trackKey = assocKey+"_OtherTrk"+str(itrack)+"_"+trackMethod
                varDict[trackKey][tauCounter] = getattr(detail.otherTrk(itrack),trackMethod)()

    return varDict    
##############################################
# store the 4-vectors for sumEM
def initSumPi0Vec(maxTaus):
    localVarDict  = {}
    for method in sumPi0VecList:
        sumPi0VecKey = assocKey+"_sumPi0Vec_"+method
        localVarDict[sumPi0VecKey] = array('f',maxTaus*[0.])
    return localVarDict

def dumpSumPi0Vec(tauCounter, detail, varDict):
    for sumPi0VecMethod in sumPi0VecList:
        sumPi0VecKey = assocKey+"_sumPi0Vec_"+sumPi0VecMethod
        varDict[sumPi0VecKey][tauCounter] = getattr(detail.sumPi0Vec(),sumPi0VecMethod)()

    return

##############################################
# store a maximum of 4 piZeros associated with each tau
def initPiZero(maxTaus):
    localVarDict  = {}
    for ipiZero in range(maxPiZero):
        for piZeroVar in piZeroVarList:
            piZeroKey = assocKey+"_PiZero"+str(ipiZero)+"_"+piZeroVar
            localVarDict[piZeroKey] = array('f',maxTaus*[0.])
    return localVarDict

def dumpPiZero(tauCounter, detail, varDict):
    # loop through the PiZeros, store their variables
    piZeroCounter = 0
    for piZero in enumerate(detail.pi0LinkVec()):
        if piZeroCounter < maxPiZero:
            for piZeroMethod in piZeroVarList:
                piZeroKey = assocKey+"_PiZero"+str(piZeroCounter)+"_"+piZeroMethod
                varDict[piZeroKey][tauCounter] = getattr(piZero[1],piZeroMethod)()
            piZeroCounter += 1
    return
        
