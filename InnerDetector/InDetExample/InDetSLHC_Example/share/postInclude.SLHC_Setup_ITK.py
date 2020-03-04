#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_ITK import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

if SLHC_Flags.UseLocalGeometry():
    print "Using local Dictionary XML generated on the fly: InDetIdDictFiles/IdDictInnerDetector_ITK.xml"
    print "!!!!!This configuration only works for stand-alone ITk so turn off other subdetectors!!!!"
    svcMgr.DetDescrCnvSvc.IdDictFromRDB = False
    svcMgr.DetDescrCnvSvc.InDetIDFileName = "InDetIdDictFiles/IdDictInnerDetector_ITK.xml"
    #This needs to be added as well, since the Muon system doesn't define a sensible default ID dictionary, and the ID Helpers are constructed regardless of whether the subdetector is built or not... this should be fixed for future!
    svcMgr.DetDescrCnvSvc.MuonIDFileName = "IdDictParser/IdDictMuonSpectrometer_R.01.xml"

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")
