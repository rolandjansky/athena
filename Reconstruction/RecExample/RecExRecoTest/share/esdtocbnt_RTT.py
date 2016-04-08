# steering file for ESD->CBNT step
# see myTopOptions.py for more info

# jp.AthenaCommonFlags.PoolESDInput=["theesdfile"]

rec.RootNtupleOutput="ntuple_fromESD.root"

include ("RecExCommon/esdtocbnt.py")

include("RecExRecoTest/rttchainhack.py")
