# This jO is to test writing RDOs in as "standard" setup as possible.
# It uses RecExCommon to configure athena job with all the current idiosyncrasies.
#

#DetDescrVersion = "ATLAS-DC3-07"
#PoolRDOInput = ["/afs/cern.ch/user/a/andr/scratch0/data/testIdeal_07.007233.singlepart_mu100.digit.RDO.v12000201_tid002765._00001.pool.root.1"]

DetDescrVersion = "ATLAS-CSC-02-00-00"
PoolRDOInput = ["/afs/cern.ch/user/a/andr/scratch0/ideal2_valid1.007211.singlepart_mu10.digit.RDO.v13003002/RDO.016155._00001.pool.root.1"]



doTrigger = False # for example do not run trigger simulation
# doTruth=False
# number of event to process
EvtMax=2

# By default write ESD, AOD and TAG simultenaously, but note that in productio
#      these should be done in separate steps (results might not be bit-by-bit identical).
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG
doAOD=False
doAODall=False
doCBNT=False
doHist=False
doJiveXML=False

doWriteRDO=True


# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


sg = Service ("StoreGateSvc" )
#StoreGateSvc.ActivateHistory=False
sg.Dump = True


# 20080102 dev val 3: muons crash again.
# Redefine output list so it does not include muon stuff

print "AG: original StreamRDO.ItemList = ", StreamRDO.ItemList

StreamRDO.ItemList = []

# Define output stream
StreamRDO.ItemList += [ "EventInfo#*" ];
StreamRDO.ItemList += [ "McEventCollection#*" ]
# InDet RDO
StreamRDO.ItemList += [ "PixelRDO_Container#*" ]
StreamRDO.ItemList += [ "SCT_RDO_Container#*" ]
StreamRDO.ItemList += [ "TRT_RDO_Container#*" ]
StreamRDO.ItemList += [ "InDetSimDataCollection#*" ]
# Calo RDO
StreamRDO.ItemList += [ "LArRawChannelContainer#*" ]
StreamRDO.ItemList += [ "TileRawChannelContainer#*" ]
# Calo calib hit
StreamRDO.ItemList += [ "TileHitVector#*" ]
StreamRDO.ItemList += [ "CaloCalibrationHitContainer#*" ]

#AG: # Muon RDO
#AG: StreamRDO.ItemList += [ "CscRawDataContainer#*" ]
#AG: StreamRDO.ItemList += [ "MdtCsmContainer#*" ]
#AG: StreamRDO.ItemList += [ "RpcPadContainer#*" ]
#AG: StreamRDO.ItemList += [ "TgcRdoContainer#*" ]
#AG: StreamRDO.ItemList += [ "TrackRecordCollection#*" ]
#AG: StreamRDO.ItemList += [ "MuonSimDataCollection#*" ]
#AG: # from LVL1
#AG: StreamRDO.ItemList += [ "ROIB::RoIBResult#*", "MuCTPI_RDO#*" ]
#AG: # Muon digit
#AG: StreamRDO.ItemList += [ "MdtDigitContainer#*"]
#AG: StreamRDO.ItemList += [ "RpcDigitContainer#*"]
#AG: StreamRDO.ItemList += [ "TgcDigitContainer#*"]
#AG: StreamRDO.ItemList += [ "CscDigitContainer#*"]
#AG: #StreamRDO.ItemList += [ "CscDigitCollection#*"]
#AG: #StreamRDO.ItemList += [ "CscClusterCollection#*"]

print "AG: final StreamRDO.ItemList = ", StreamRDO.ItemList


