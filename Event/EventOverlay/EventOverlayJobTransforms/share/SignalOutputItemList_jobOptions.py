include.block ( "EventOverlayJobTransforms/SignalOutputItemList_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags

# TODO: migrate old configuration
# include ( "EventOverlayJobTransforms/MuonMcSignal_jobOptions.py" )
# include ( "EventOverlayJobTransforms/LArMcSignal_jobOptions.py" )
# include ( "EventOverlayJobTransforms/TileMcSignal_jobOptions.py" )
# include ( "EventOverlayJobTransforms/L1Signal_jobOptions.py" ) 

# TODO: some event info update will be needed

# The output - signal
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
signalStream = AthenaPoolOutputStream("StreamRDO_SGNL", overlayFlags.PoolSignalRDOOutput(), asAlg=True)
signalStream.ForceRead = True


from AthenaCommon.Logging import logging
logg = logging.getLogger('SignalOutputItemList')
logg.info( '****************** MN:  Creating SignalOutputItemList *****************' )

list = []

list += [ "xAOD::EventInfo#*", "xAOD::EventAuxInfo#*" ]
list += [ "xAOD::EventInfoContainer#*", "xAOD::EventInfoAuxContainer#*"]
list += [ "EventStreamInfo#*" ]

if DetFlags.overlay.Truth_on():
    list += ["McEventCollection#" + overlayFlags.sigPrefix() + "TruthEvent"]
    if DetFlags.overlay.SCT_on():
        list += ["InDetSimDataCollection#" + overlayFlags.sigPrefix() + "SCT_SDO_Map"]

if DetFlags.overlay.SCT_on():
    list += ["SCT_RDO_Container#" + overlayFlags.sigPrefix() + "SCT_RDOs"]

signalStream.ItemList = list
signalStream.TransientItems = list

# Write all IOV meta data containers
signalStream.MetadataItemList += ["IOVMetaDataContainer#*"]
signalStream.MetadataItemList += ["LumiBlockCollection#*"]
