#print "now executing CaloDetMgrDetDescrCnv_CaloDetDescrCnv"
#
#  Joboptions for the loading of the of CaloDetDescrMgr
#

#theApp.Dlls += [ "CaloDetMgrDetDescrCnv" ]

#DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
#DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]
#DetDescrCnvSvc.DetectorManagers += [ "CaloTTMgr" ]
include( "CaloConditions/CaloConditions_jobOptions.py" ) 

# must be included once only :
#include.block ("CaloDetMgrDetDescrCnv/CaloDetDescr_joboptions.py")

