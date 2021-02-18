#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup_ITK import SLHC_Setup
SLHC_Setup = SLHC_Setup()

from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags

if (SLHC_Flags.UseLocalGeometry()) or (SLHC_Flags.ForceDictionaryGeneration()):
    fileSuffix = ""
    #To distinguish file generated from that present in ITKLayouts
    if SLHC_Flags.ForceDictionaryGeneration():
        fileSuffix = "_GEN"
    print "Using local Dictionary XML: ITKLayouts/IdDictInnerDetector_ITK_LOCAL"+fileSuffix+".xml"
    svcMgr.DetDescrCnvSvc.IdDictFromRDB = False
    svcMgr.DetDescrCnvSvc.InDetIDFileName = "ITKLayouts/IdDictInnerDetector_ITK_LOCAL"+fileSuffix+".xml"
    #Its all or nothing with local dictionary files, so these are currently sensible choices, but may need updating some time
    svcMgr.DetDescrCnvSvc.MuonIDFileName = "IdDictParser/IdDictMuonSpectrometer_R.09.NSW.xml"
    svcMgr.DetDescrCnvSvc.LArIDFileName = "IdDictParser/IdDictLArCalorimeter_HGTD_TDR_01.xml"
    svcMgr.DetDescrCnvSvc.TileIDFileName = "IdDictParser/IdDictTileCalorimeter.xml"
    svcMgr.DetDescrCnvSvc.CaloIDFileName = "IdDictParser/IdDictCalorimeter_L1Onl.xml"
    svcMgr.DetDescrCnvSvc.ForwardIDFileName = "IdDictParser/IdDictForwardDetectors_2010.xml"

include("InDetSLHC_Example/postInclude.SLHC_Setup_Common.py")
