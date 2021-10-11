

#--------------------------------------------------------------
# Import config
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

athenaCommonFlags.FilesInput = ["/tmp/amorley/mc14_13TeV.187039.Herwigpp_UEEE4_CTEQ6L1_Zmumu.merge.AOD.e2770_s1982_s2008_r5787_r5853_tid01598243_00/AOD.01598243._000545.pool.root.1"]

import MagFieldServices.SetupField

# --- number of events to process
athenaCommonFlags.EvtMax     = 10
athenaCommonFlags.SkipEvents = 0#122-2 #33

doVP1 = False

# --- disable error protection of RecExCommon
athenaCommonFlags.AllowIgnoreConfigError.set_Value_and_Lock(False)


#--------------------------------------------------------------
# Additional Detector Setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 

# --- switch on InnerDetector
DetFlags.ID_setOn()

# --- turn off Calo
DetFlags.Calo_setOff()

# --- and switch off Muons
DetFlags.Muon_setOff()

#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
rec.OutputLevel.set_Value_and_Lock       (INFO)

# --- controls what is written out. ESD includes AOD, so it's normally enough
# --- ESD writing temporarily disabled post 2013 migration - to be reinstated! 

from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.TrackParticleSlimmer=False 
AODFlags.TrackParticleLastHitAndPerigeeSlimmer=False

rec.doWriteESD.set_Value_and_Lock        (False)###
rec.doAOD.set_Value_and_Lock             (False)
rec.doWriteAOD.set_Value_and_Lock        (False)
rec.doDPD.set_Value_and_Lock             (False)
rec.doCBNT.set_Value_and_Lock            (False)
rec.doWriteTAG.set_Value_and_Lock        (False)

# --- turn on InDet
rec.doInDet.set_Value_and_Lock           (True)
# --- turn off calo
rec.doCalo.set_Value_and_Lock           (False)
# --- turn off muons
rec.doMuon.set_Value_and_Lock            (False) 
# --- turn off forward detectors
rec.doForwardDet.set_Value_and_Lock      (False)
# --- turn off trigger
rec.doTrigger.set_Value_and_Lock         (False)

# --- turn off combined reconstruction
rec.doEgamma.set_Value_and_Lock          (False)
rec.doMuonCombined.set_Value_and_Lock    (False)
rec.doTau.set_Value_and_Lock             (False)
rec.doJetMissingETTag.set_Value_and_Lock (False)

# --- turn of calo stuff we don't need anyway
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock  (False)
jobproperties.CaloRecFlags.doCaloEMTopoCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoTower.set_Value_and_Lock    (False)

# --- turn of jets (Hack!!!)
from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled.set_Value_and_Lock          (False)

# --- turn off egamma Brem
recAlgs.doEgammaBremReco.set_Value_and_Lock  (False)
# --- turn off Eflow and missing ET
recAlgs.doEFlow.set_Value_and_Lock           (False)
recAlgs.doEFlowJet.set_Value_and_Lock        (False)
recAlgs.doMissingET.set_Value_and_Lock       (False)
recAlgs.doMissingETSig.set_Value_and_Lock    (False)
recAlgs.doObjMissingET.set_Value_and_Lock    (False)
# --- turn off combined muons
recAlgs.doMuGirl.set_Value_and_Lock          (False)
recAlgs.doMuTag.set_Value_and_Lock           (False)
recAlgs.doMuidLowPt.set_Value_and_Lock       (False)
recAlgs.doMuonIDCombined.set_Value_and_Lock  (False)
recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
recAlgs.doMuonSpShower.set_Value_and_Lock    (False)
recAlgs.doStaco.set_Value_and_Lock           (False)
recAlgs.doCaloTrkMuId.set_Value_and_Lock     (False)
recAlgs.doTileMuID.set_Value_and_Lock        (False)


#--------------------------------------------------------------
# Printout config
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
print "globalflags configuration:"
print globalflags

print "detflags configuration:"
DetFlags.Print()

print "rec configuration:"
print rec

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------


# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include ("RecExCommon/RecExCommon_topOptions.py")

from AthenaCommon import CfgMgr

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("MySeq2")

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


# Add the track dressing alg
streamName = "TESTSTREAM_"

from InDetPhysValMonitoring.InDetPhysValJobProperties import isMC
if isMC() :
  meta_data=''
  import InDetPhysValMonitoring.InDetPhysValDecoration
  if len(streamName) == 0 :
     # add track parameter decorations to truth particles but only if the decorations have not been applied already
     meta_data = InDetPhysValMonitoring.InDetPhysValDecoration.getMetaData()
     from AthenaCommon.Logging import logging

  logger = logging.getLogger( "DerivationFramework" )
  if len(meta_data) == 0 :
    kwargs_in = {'Prefix' : streamName } if len(streamName)>0 else {}
    truth_track_param_decor_alg = InDetPhysValMonitoring.InDetPhysValDecoration.getInDetPhysValTruthDecoratorAlg(**kwargs_in)
    if  InDetPhysValMonitoring.InDetPhysValDecoration.findAlg([truth_track_param_decor_alg.getName()]) == None :
      DerivationFrameworkJob += truth_track_param_decor_alg
    else :
      logger.info('Decorator %s already present not adding again.' % (truth_track_param_decor_alg.getName() ))
  else :
    logger.info('IDPVM decorations applied to input file. That should compris truth particle decorations. Not adding them again .')




topSequence += DerivationFrameworkJob


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr

fileName   = "test.pool.root"
TestStream = MSMgr.NewPoolRootStream( streamName, fileName)
TestStream.AddItem("xAOD::EventInfo_v1#*")
TestStream.AddItem("xAOD::EventAuxInfo_v1#*")
TestStream.AddItem("xAOD::TruthParticleContainer_v1#*")
TestStream.AddItem("xAOD::TruthParticleAuxContainer_v1#*")
TestStream.AddItem("xAOD::TruthVertexContainer_v1#*")
TestStream.AddItem("xAOD::TruthVertexAuxContainer_v1#*")
TestStream.AddItem("xAOD::TruthEventContainer_v1#*")
TestStream.AddItem("xAOD::TruthEventAuxContainer_v1#*")


if doVP1:
  from VP1Algs.VP1AlgsConf import VP1Alg
  topSequence += VP1Alg()

