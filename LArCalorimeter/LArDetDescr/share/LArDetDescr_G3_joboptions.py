print "now initializing the LAr readout geometry : G3 flavour"

GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.LAr_VersionOverride = "LAr-G3-00"
       
# ------------------------------------------------
#  FIXME : setup online identifier & mapping
#    ( should be done somewhere else )
# ------------------------------------------------
include( "CaloConditions/CaloConditions_jobOptions.py" ) 
include ("LArCondCnv/LArCondCnv_G3_jobOptions.py")

# must be included once only :
include.block ("LArDetDescr/LArDetDescr_G3_joboptions.py")
