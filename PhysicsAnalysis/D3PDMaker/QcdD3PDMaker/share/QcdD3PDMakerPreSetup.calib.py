from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Logging import logging

#Suppressing output for warning (especially for jet tools)
svcMgr.MessageSvc.warningLimit=10

#==================================
# PrimaryDPDMaker specific options
#==================================

#no skimming
primDPD.ApplySkimming=False

#not make DESD for default
primDPD.MakePrimaryDPD=False

#==================================
# RecExCommon specific options
#==================================

if QcdD3PDMakerFlags.doRecJet() or QcdD3PDMakerFlags.doRecMET() or QcdD3PDMakerFlags.doRecTruthJet():
  rec.oldFlagTopSteering.set_Value_and_Lock(True)

#==================================
#Setting AOD flags. Not much needs to be done if you read an ESD 'cause everything's already built.
#See https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
#==================================
#other settings
jetFlags.noStandardConfig=True

if rec.readAOD() or not (QcdD3PDMakerFlags.doRecJet() or QcdD3PDMakerFlags.doRecMET()):
  jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(False)
  jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
  jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)
else:
  jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
  jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
  jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Disable AODFix and run _local
rec.doApplyAODFix.set_Value_and_Lock(False)
#rec.UserAlgs += ["QcdD3PDMaker/AODFix_local.py"]

#==================================
# reconstruction of jet, etmiss
#==================================
#if not primDPD.UseMCTruth():
#    UserAlgs += ["QcdD3PDMaker/GoodRunsListsUser_preEventSelector.py"]

#on-the-fly jet reconstruction
if QcdD3PDMakerFlags.doRecJet() or QcdD3PDMakerFlags.doRecTruthJet():
  rec.UserAlgs += ["QcdD3PDMaker/myJets_calib.py"]
#  rec.UserAlgs += ["QcdD3PDMaker/myJets_new.py"]
#  rec.UserAlgs += ["QcdD3PDMaker/myJets.py"]

#on-the-fly met reconstruction
if QcdD3PDMakerFlags.doRecMET():
  recAlgs.doMissingET.set_Value_and_Lock(True)
  #additional MET
  if QcdD3PDMakerFlags.doAdditionalMissingET():
    rec.UserAlgs += ["QcdD3PDMaker/metAlgs_new.py"]
#    rec.UserAlgs += ["QcdD3PDMaker/metAlgs.py"]

#fix EMJES
if QcdD3PDMakerFlags.doEMJESFix():
  rec.UserAlgs += ['QcdD3PDMaker/EMJESFix.py']
