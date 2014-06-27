from LArConditionsCommon.LArCondFlags import larCondFlags
larCondFlags.SingleVersion=True
larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

from LArROD.LArRODFlags import larRODFlags
if not larRODFlags.doDSP:
    larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("3:238,306,313,319,325,331,338,344,350,1001:1012,1021,1022")
else:
    larCondFlags.LArCoolChannelSelection.set_Value_and_Lock("")

