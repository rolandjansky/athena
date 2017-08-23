#====================================================================
# DAPR2.py 
# reductionConf flag DAPR2 in Reco_tf.py   
# Special event picking format, to be used in case of event index
# failure
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# EVENT PICKING   
#====================================================================
from DerivationFrameworkDataPrep.DAPR2EventList import EventList
seq = CfgMgr.AthSequencer("DFEventPickingSequence") 
from GaudiSequencer.PyComps import PyEvtFilter
seq += PyEvtFilter(
   'DFEventPickingAlg',
   # the store-gate key. leave as an empty string to take any eventinfo instance
   evt_info='',
   OutputLevel=Lvl.WARNING)
seq.DFEventPickingAlg.evt_list = EventList
DerivationFrameworkJob += seq
#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_DAPR2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_DAPR2Stream )
DAPR2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
DAPR2Stream.AcceptAlgs(["DFEventPickingAlg"])
#====================================================================
# Store all containers
#====================================================================
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
excludeList = []
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )
