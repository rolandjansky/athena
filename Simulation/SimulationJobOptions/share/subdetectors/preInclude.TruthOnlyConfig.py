if not 'DetFlags' in dir():
    #if you configure one detflag, you're responsible for configuring them all!
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.all_setOff()
DetFlags.Truth_setOn()
DetFlags.Print()
