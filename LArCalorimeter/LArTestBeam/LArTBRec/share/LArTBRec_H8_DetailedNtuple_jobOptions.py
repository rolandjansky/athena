#*******************************************************************
#
# JobOption to analyse LAr testbeam data :
#      detailed version dumping the digits, for experts
#
#*******************************************************************

ByteStreamAddressProviderSvc.TypeNames+=["LArFebHeaderContainer/LArFebHeader"]
if CBNTAthenaAware:
   from LArROD.LArRODConf import CBNTAA_LArDigits
   CBNT_LArDigits = CBNTAA_LArDigits("CBNT_LArDigit")
else:   
   CBNT_Athena.Members += [ "CBNT_LArDigits" ]
   CBNT_LArDigits = Algorithm( "CBNT_LArDigits" )
   CBNT_LArDigits.NtupleLocID="/FILE1/CALO/502";

CBNT_LArDigits.NEvents=100
CBNT_LArDigits.ContainerKey = "FREE"
CBNT_LArDigits.Calo = "EM" # which calo will be saved
CBNT_LArDigits.DumpGains = "MEDIUMHIGH" # which gains will be saved
CBNT_LArDigits.DumpCut = 0 # lower cut for max sample in channel
CBNT_LArDigits.SaveSCAAddress = True

if CBNTAthenaAware:
   CBNT_AthenaAware += CBNT_LArDigits
