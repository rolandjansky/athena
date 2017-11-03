include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

rec.doTrigger=True
rec.doWriteESD=False # uncomment if do not write ESD

include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['FieldAndGeo']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


include ("RecExCommon/rdotoesd.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")
