include ("RecExRecoTest/RecExRecoTest_RTT_common.py")

include ("RecExCond/RecExCommon_flags.py")
rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())


# main jobOption
include ("RecExCommon/rdotoesd.py")

include ("RecExRecoTest/RecExRecoTest_RTT_common_postOptions.py")



