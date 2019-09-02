#############################################################################################
# Fabien.Tarrade@cern.ch                                                                    #
#                                                                                           #
# this is a OLD jobOptions which can maybe work but only for the barrel                     #
# can be used as a skeleton if needed but                                                   #
# need to be updated for the barrel and the patterns for EMEC, HEC and FCAL need to be add   #
#                                                                                           #
#############################################################################################

from LArCalibUtils.LArCalibUtilsConf import LArCalibDigitMaker
LArCalibDigitMaker = LArCalibDigitMaker("LArCalibDigitMaker")
LArCalibDigitMaker.KeyList += GainList
LArCalibDigitMaker.KeyList += ["FREE"]
LArCalibDigitMaker.DontRun  = False

from LArROD.LArRODConf import LArCalibDigitsAccumulator
LArCalibDigitsAccumulator = LArCalibDigitsAccumulator("LArCalibDigitsAccumulator")
LArCalibDigitsAccumulator.KeyList += GainList

# DelayScale example: in CTB 2004 240 TTCrx steps in a sampling period
LArCalibDigitsAccumulator.DelayScale = (25./240.)*ns

LArCalibDigitMaker.BoardIDs += [ 1007091712, 1007616000, 1008140288, 1008664576, 1009188864, 1009713152, 1010237440, 1010761728, 1011286016, 1011810304, 1012334592, 1012858880, 1013383168, 1013907456,
                                 1014431744, 1014956032, 1015480320, 1016004608, 1016528896, 1017053184, 1017577472, 1018101760, 1018626048, 1019150336, 1019674624, 1020198912, 1020723200, 1021247488,
                                  021771776, 1022296064, 1022820352, 1023344640, 1023868928, 1024393216, 1024917504, 1025441792, 1025966080, 1026490368, 1027014656, 1027538944, 1028063232, 1028587520,
                                 1029111808, 1029636096, 1030160384, 1030684672, 1031208960, 1031733248, 1032257536, 1032781824, 1033306112, 1033830400, 1034354688, 1034878976, 1035403264, 1035927552,
                                 1036451840, 1036976128, 1037500416, 1038024704, 1038548992, 1039073280, 1039597568, 1040121856, 1040646144, 1041170432, 1041825792, 1041858560, 1042120704, 1042743296,
                                 1043267584, 1043824640, 1044316160, 1044840448, 1045495808, 1045528576, 1045790720, 1046413312, 1046937600, 1047461888, 1047986176, 1048641536, 1048674304, 1048936448,
                                 1049559040, 1050083328, 1050607616, 1051131904, 1051787264, 1051820032, 1052082176, 1052704768, 1053229056, 1057423360, 1057947648, 1058603008, 1058635776, 1058897920,
                                 1059520512, 1060044800, 1060601856, 1061093376, 1061617664, 1062273024, 1062305792, 1062567936, 1063190528, 1063714816, 1064239104, 1064763392, 1065418752, 1065451520,
                                 1065713664, 1066336256, 1066860544, 1067384832, 1067909120, 1068564480, 1068597248, 1068859392, 1069481984, 1070006272 ]
   
#
# Calibration patterns
#

LArCalibDigitMaker.NTrigger = 100
LArCalibDigitMaker.DAC     += DACs
LArCalibDigitMaker.Delay   += Delays
LArCalibDigitMaker.Pattern += [0x10001,0x10001,0x10001,0x0000001]
LArCalibDigitMaker.Pattern += [0x20002,0x20002,0x20002,0x0000002]
LArCalibDigitMaker.Pattern += [0x40004,0x40004,0x40004,0x0000004]
LArCalibDigitMaker.Pattern += [0x80008,0x80008,0x80008,0x0000008]
LArCalibDigitMaker.Pattern += [0x100010,0x100010,0x100010,0x10010]
LArCalibDigitMaker.Pattern += [0x200020,0x200020,0x200020,0x20020]
LArCalibDigitMaker.Pattern += [0x400040,0x400040,0x400040,0x40040]
LArCalibDigitMaker.Pattern += [0x800080,0x800080,0x800080,0x80080]
LArCalibDigitMaker.Pattern += [0x1000100,0x1000100,0x1000100,0x100100]
LArCalibDigitMaker.Pattern += [0x2000200,0x2000200,0x2000200,0x200200]
LArCalibDigitMaker.Pattern += [0x4000400,0x4000400,0x4000400,0x400400]
LArCalibDigitMaker.Pattern += [0x8000800,0x8000800,0x8000800,0x800800]
LArCalibDigitMaker.Pattern += [0x10001000,0x10001000,0x10001000,0x0001000]
LArCalibDigitMaker.Pattern += [0x20002000,0x20002000,0x20002000,0x0002000]
LArCalibDigitMaker.Pattern += [0x40004000,0x40004000,0x40004000,0x0004000]
LArCalibDigitMaker.Pattern += [0x80008000,0x80008000,0x80008000,0x0008000]
   
topSequence += LArCalibDigitMaker
topSequence += LArCalibDigitsAccumulator

svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/HIGH"  ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/MEDIUM"]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/LOW"   ]
svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]

