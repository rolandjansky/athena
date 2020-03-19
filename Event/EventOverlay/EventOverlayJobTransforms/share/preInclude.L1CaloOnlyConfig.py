if not 'DetFlags' in dir():
    #if you configure one detflag, you're responsible for configuring them all!
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOff()
DetFlags.Calo_setOn()
DetFlags.LVL1_setOn()
DetFlags.digitize.LVL1_setOff()
