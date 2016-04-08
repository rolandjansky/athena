###############################################################
#
# Job options fragment to write LArTTCell map to POOL (no IOV)
#
#==============================================================

# LAr Ids/Tools  (loads Calo Ids/Tools as well)
include( "LArIdCnv/LArIdCnv_joboptions.py" )
# this lib
theApp.Dlls   += [ "CaloCondAthenaPoolPoolCnv" ]

include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )
Stream1.Store="DetectorStore"
Stream1.ItemList += [ "LArTTCellMap#*" ]






