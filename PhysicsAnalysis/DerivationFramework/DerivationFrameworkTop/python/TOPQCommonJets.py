# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#===================================================================================
# Common file used for TOPQ jet setup
# Call with:
#   import DerivationFrameworkTop.TOPQCommonJets
#   DerivationFrameworkTop.TOPQCommonJets.buildTOPQCA15jets(TOPQXSequence)
#   DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("JetCollection")
#   DerivationFrameworkTop.TOPQCommonJets.TOPQupdateJVT("JetCollection")
#===================================================================================

#========
# IMPORTS
#========
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.ObjKeyStore import cfgKeyStore
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

#==============
# JET MODIFIERS
#==============
from JetRec.JetRecStandard import jtm
jtm.modifiersMap["dfgroomed"] += [jtm.ktsplitter]
jtm.modifiersMap["calib"] += [jtm.jvf]
jtm.modifiersMap["lctopo"] += [jtm.jvf]
jtm.modifiersMap["emtopo"] += [jtm.jvf]
jtm.modifiersMap["calib_topo_ungroomed"] += [jtm.jvf]
jtm.modifiersMap["topo_ungroomed"] += [jtm.jvf]

#==================
# CamKt15LCTopoJets
#==================
# uses DerivationFrameworkJetEtMiss.ExtendedJetCommon
def buildTOPQCA15jets(algseq):
  if not cfgKeyStore.isInInput("xAOD::JetContainer","CamKt15LCTopoJets"):
    from JetRec.JetRecConf import JetAlgorithm
    jtm.modifiersMap["lctopoCA15"] = list(jtm.modifiersMap["lctopo"])
    if globalflags.DataSource()=='geant4':
      jtm.modifiersMap["lctopoCA15"].remove('truthassoc')
    if hasattr(jtm,"CamKt15LCTopoJets"):
      TOPQCA15LC = getattr(jtm,"CamKt15LCTopoJets")
    else:
      TOPQCA15LC = jtm.addJetFinder("CamKt15LCTopoJets", "CamKt", 1.5, "lctopo","lctopoCA15", ghostArea=0.01, ptmin= 2000, ptminFilter=100000, calibOpt="aro")
    if not hasattr(algseq,"jetalgCamKt15LCTopo"):
      TOPQCA15LCalg = JetAlgorithm("jetalgCamKt15LCTopo", Tools = [TOPQCA15LC] )
      algseq += TOPQCA15LCalg
      print "Running jet finding algorithm for CamKt15LCTopoJets"
# end buildTOPQCA15jets(algseq)

#================
# JET CALIBRATION
#================
# uses DerivationFrameworkJetEtMiss.ExtendedJetCommon
def applyTOPQJetCalibration(jetcollection, algseq=None):
  supportedJets = ['AntiKt4EMTopo', 'AntiKt10LCTopoTrimmedPtFrac5SmallR20']
  if not jetcollection in supportedJets:
    print "TOPQCommonJets:",jetcollection, "is an unsupported collection for calibration!" 
    return
  elif jetcollection == 'AntiKt4EMTopo':
    applyJetCalibration_xAODColl(jetcollection,algseq)
  elif jetcollection == 'AntiKt10LCTopoTrimmedPtFrac5SmallR20':
    applyJetCalibration_CustomColl(jetcollection, algseq)
# end applyTOPQJetCalibration(jetcollection, algseq=None)
    
#=======================
# JVT AUGMENTATION
#=======================
# uses DerivationFrameworkJetEtMiss.ExtendedJetCommon
def TOPQupdateJVT(jetcollection, algseq=None):
  supportedJets = ['AntiKt4EMTopo']
  if not jetcollection in supportedJets:
    print "TOPQCommonJets:", jetcollection, "is an unsupported collection for JVT augmentation!"
    return
  else:
    updateJVT_xAODColl(jetcollection,algseq)
# end TOPQupdateJVT(jetcollection, algseq=None)
