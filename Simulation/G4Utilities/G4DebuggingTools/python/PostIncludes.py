# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from G4DebuggingTools.G4DebuggingToolsConfigNew import VolumeDebugger

def VolumeDebuggerAtlas(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    kwargs.setdefault("TargetVolume","Atlas::Atlas")
    result = VolumeDebugger(configFlags,name,**kwargs)
    return result

def VolumeDebuggerITk(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    kwargs.setdefault("TargetVolume","ITK::ITK")
    result = VolumeDebugger(configFlags,name,**kwargs)
    return result

def VolumeDebuggerITkPixel(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    kwargs.setdefault("TargetVolume","ITkPixel::ITkPixel")
    result = VolumeDebugger(configFlags,name,**kwargs)
    return result

def VolumeDebuggerITkStrip(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    kwargs.setdefault("TargetVolume","ITkStrip::SCT")
    #change this eventually away from SCT once geometry tag is updated...
    result = VolumeDebugger(configFlags,name,**kwargs)
    return result

def VolumeDebuggerHGTD(configFlags, name="G4UA::ISFFullUserActionSvc", **kwargs):
    kwargs.setdefault("TargetVolume","HGTD::HGTD")
    result = VolumeDebugger(configFlags,name,**kwargs)
    return result  
