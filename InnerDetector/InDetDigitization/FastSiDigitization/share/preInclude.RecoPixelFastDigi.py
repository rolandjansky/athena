from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.pixelClusterSplittingType = 'AnalogClus'
InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False)
InDetFlags.doSCT_PRDFormation.set_Value_and_Lock(True)
InDetFlags.doTRT_PRDFormation.set_Value_and_Lock(True)
InDetFlags.doPixelPRDFormation.set_Value_and_Lock(False)
#InDetFlags.doPhysValMon  = True
