from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
if commonGeoFlags.Run()=="RUN4":
    include ("eflowRec/eflowRec_config_HLLHC.py")
else:
    include("eflowRec/eflowRec_config_MC15_JetETMiss.py")
