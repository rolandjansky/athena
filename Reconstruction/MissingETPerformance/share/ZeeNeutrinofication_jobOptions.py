######################################################
# Main Flags
#
# doNeutrinofy:
#    True: neutrinofication will be done.
#    False: re-run default MET reconstruction
#           using the same event selection as
#           in the case True.
#
# invSel:
#    Used when running for the second time on the
#    event sample.
#    True: inversion of the selection of the electron
#          to be neutrinofied.
#
# isMC:
#    Analyse MC truth information
#
######################################################
doNeutrinofy = True
doInvSel     = False
isMC         = True
#############
# Input files
PoolAODInput=["/some/location/AOD.pool.root"]

#if not 'RootNtupleOutput' in dir():
#   RootNtupleOutput="aant.neutrinofication.root"
   
################################
# RecExCommon Flags
# (needed because we want re-run
# Jet finding and MissingET)
EvtMax=1000

from RecExConfig.RecFlags  import rec   
rec.readAOD=True
rec.doCBNT=False
rec.doWriteESD=False
rec.doWriteAOD=False
rec.doAOD=False
rec.doWriteTAG=False
rec.doTruth=False
doMissingET=False

include ("RecExCommon/RecExCommon_topOptions.py")

###############
# Top algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

##########
# GeoModel
GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.IgnoreTagDifference = True

#################################
# Re-run Local Hadron Jets
# (collection not present in AOD)
from JetRec.JetGetters import *            
make_StandardJetGetter('Cone',0.4,'LCTopo')

############################
# Neutrinofication Sequencer
include ( "MissingETPerformance/ZeeSequencer_jobOptions.py" )

###################
# Histogram Service
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()

if doNeutrinofy:
   
   ##########################
   # Split Electron Container
   include ( "MissingETPerformance/SplitElectronContainer_jobOptions.py" )
   topSequence.ZeeSequence.SplitElectronContainer.inverseSelection=doInvSel
   topSequence.ZeeSequence.SplitElectronContainer.doTruth=isMC
   topSequence.ZeeSequence.SplitElectronContainer.OutputLevel=WARNING

   ##################################
   # Re-run MET with neutrinofication
   from MissingETPerformance.METRefGetter_Neutrinofication import METRefGetter
   METRefGetter()

   if doInvSel:
      aanName="aant.neutrinofy.invSel.root"
      ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='aant.neutrinofy.invSel.root' OPT='RECREATE'" ]

   else:
      aanName="aant.neutrinofy.norSel.root"
      ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='aant.neutrinofy.norSel.root' OPT='RECREATE'" ]

else:
   ######################
   # Re-run MET "default"
   from MissingETPerformace.METRefGetter import METRefGetter
   METRefGetter()

   aanName="aant.default.root"
   ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='aant.default.root' OPT='RECREATE'" ]

#########################################
# Sum up all different MET RefFinal terms
from MissingETPerformance.METGetterAOD_Neutrinofication import METGetterAOD
METGetterAOD()

################
# Ntuple Service
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence.ZeeSequence += AANTupleStream()

AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = aanName
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

##########################
# Default MissingET ntuple
from MissingET.MissingETConf import CBNTAA_MissingET
topSequence.ZeeSequence += CBNTAA_MissingET()

###################################
# Neutrinofication MissingET ntuple
include ( "MissingETPerformance/CBNTAA_MissingET_Neutrinofication_jobOptions.py" )
topSequence.ZeeSequence.CBNTAA_MissingET_Neutrinofication.OutputLevel=WARNING
