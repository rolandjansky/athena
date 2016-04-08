#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Digitization
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-MC12-ITK-27-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=-1
jobproperties.AthenaCommonFlags.PoolHitsInput=["singleMuon_test_slhc_Hits.pool.root"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="singleMuon_test_slhc_Digits.pool.root"

from AthenaCommon.GlobalFlags import jobproperties
# jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-00-00'
# jobproperties.Global.DetDescrVersion='ATLAS-SLHC-01-03-00'
jobproperties.Global.DetDescrVersion='ATLAS-P2-ITK-01-00-00'

from Digitization.DigitizationFlags import jobproperties 
jobproperties.Digitization.doInDetNoise=False
jobproperties.Digitization.dataRunNumber=240000

include ( "Digitization/Digitization.py" )

include("InDetSLHC_Example/postInclude.SLHC_Digitization_lowthresh.py")

MessageSvc = Service( "MessageSvc" )
#increase the number of letter reserved to the alg/tool name from 18 to 30
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message per alg
MessageSvc.defaultLimit = 9999999  # all messages
# Set output level threshold among DEBUG, INFO, WARNING, ERROR, FATAL 
MessageSvc.OutputLevel = INFO

include("InDetSLHC_Example/postInclude.SLHC_Setup.py")
