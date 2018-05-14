from AthenaCommon.GlobalFlags import globalflags;
globalflags.DataSource.set_Value_and_Lock("geant4");
DetDescrVersion="ATLAS-P2-ITK-13-00-00"; 
ConditionsTag="OFLCOND-MC15c-SDR-13"; 
#DetDescrVersion="ATLAS-P2-ITK-13-00-00" 
globalflags.DetDescrVersion.set_Value_and_Lock(DetDescrVersion);
include("InDetSLHC_Example/preInclude.SLHC.py")
include('InDetSLHC_Example/preInclude.SLHC_Setup_Strip_GMX.py')
include('InDetSLHC_Example/preInclude.SLHC_Setup_InclBrl_4.py')
