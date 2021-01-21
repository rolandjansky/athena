# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getAFP_SensitiveDetector(name="AFP_SensitiveDetector", **kwargs):
    volumeList=[] #["AFP::AFP*_LogSIDSensor*" , "AFP::AFP*_LogSIDVacuumSensor*" , "AFP::AFP*_LogTDSensor*"]
    for det in range(4):
        for num in range(4):
            volumeList += ["AFP::AFP0"+str(det)+"_LogSIDSensor["+str(num)+"]"]
#    for det in range(2,3):
#        for num in range(5):
#            volumeList += ["AFP::AFP::AFP0"+str(det)+"_LogSIDSensor["+str(num)+"]_refl"]
    for det in range(4):
        volumeList += ["AFP::AFP0"+str(det)+"_LogSIDVacuumSensor[11]"]
    for det in [0,3]:
        for Q in range(1,2):
            for num in range(11,45):
                volumeList += ["AFP::AFP0"+str(det)+"_Q"+str(Q)+"_LogTDSensor["+str(num)+"]"]
                volumeList += ["AFP::AFP0"+str(det)+"_Q"+str(Q)+"_LogRadiator["+str(num)+"]"]
                volumeList += ["AFP::AFP0"+str(det)+"_Q"+str(Q)+"_LGuide["+str(num)+"]"]
#    for Q in range(1):
#        for num in range(31):
#            volumeList += ["AFP::AFP::AFP03_Q"+str(Q)+"_LogTDSensor["+str(num)+"]_refl"]
    kwargs.setdefault("LogicalVolumeNames", volumeList)
    kwargs.setdefault("OutputCollectionNames", ["AFP_TDSimHitCollection", "AFP_SIDSimHitCollection"])
    return CfgMgr.AFP_SensitiveDetectorTool(name, **kwargs)
def getAFP_SiDSensitiveDetector(name="AFP_SiDSensitiveDetector", **kwargs):
    volumeList=[] #["AFP::AFP*_LogSIDSensor*" , "AFP::AFP*_LogSIDVacuumSensor*"]
    for det in range(4):
        for num in range(4):
            volumeList += ["AFP::AFP0"+str(det)+"_LogSIDSensor["+str(num)+"]"]
#    for det in range(2,3):
#        for num in range(5):
#            volumeList += ["AFP::AFP::AFP0"+str(det)+"_LogSIDSensor["+str(num)+"]_refl"]
    for det in range(4):
        volumeList += ["AFP::AFP0"+str(det)+"_LogSIDVacuumSensor[11]"]
    kwargs.setdefault("LogicalVolumeNames", volumeList)
    kwargs.setdefault("OutputCollectionNames", ["AFP_SIDSimHitCollection"])
    return CfgMgr.AFP_SiDSensitiveDetectorTool(name, **kwargs)
def getAFP_TDSensitiveDetector(name="AFP_TDSensitiveDetector", **kwargs):
    volumeList=[] #["AFP::AFP*_LogTDSensor*"]
##    for det in [0,3]:
##        for Q in range(1):
##            for num in range(32):
##                volumeList += ["AFP::AFP0"+str(det)+"_Q"+str(Q)+"_LogTDSensor["+str(num)+"]"]
    for det in [0,3]:
        for Q in range(1,2):
            for num in range(11,45):
                volumeList += ["AFP::AFP0"+str(det)+"_Q"+str(Q)+"_LogTDSensor["+str(num)+"]"]
#    for Q in range(1):
#        for num in range(31):
#            volumeList += ["AFP::AFP::AFP03_Q"+str(Q)+"_LogTDSensor["+str(num)+"]_refl"]
    kwargs.setdefault("LogicalVolumeNames", volumeList)
    kwargs.setdefault("OutputCollectionNames", ["AFP_TDSimHitCollection"])
    return CfgMgr.AFP_TDSensitiveDetectorTool(name, **kwargs)
