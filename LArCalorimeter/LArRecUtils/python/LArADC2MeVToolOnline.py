# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags

def LArADC2MeVToolOnline (name="LArADC2MeVToolOnline", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArADC2MeVTool (**kw)

    mlog = logging.getLogger( 'LArADC2MeVToolOnline::__init__ ' )
    mlog.info("entering")

    # do the configuration
    tool.MCSym = False
    tool.UseMphysOverMcal = True
    tool.UseHVScaleCorr = True

    # setup old ADC2MeVTool
    key_suffix = "_online"
    oldlarCondDBFolders =[  "/LAR/ElecCalibOnl/DAC2uA<key>DAC2uA"+key_suffix+"</key>",
                            "/LAR/ElecCalibOnl/MphysOverMcal<key>MphysOverMcal"+key_suffix+"</key>",
                            "/LAR/ElecCalibOnl/Ramp<key>Ramp"+key_suffix+"</key>",
                            "/LAR/ElecCalibOnl/uA2MeV<key>uA2MeV"+key_suffix+"</key>",
                            "/LAR/ElecCalibOnl/HVScaleCorr<key>HVScaleCorr"+key_suffix+"</key>",
                         ]
    from IOVDbSvc.CondDB import conddb

    forceRN=""
    selection=""
    LArDBConnection=""
    from LArConditionsCommon.LArCondFlags import larCondFlags
    if larCondFlags.LArDBConnection.statusOn:
        LArDBConnection = larCondFlags.LArDBConnection()
    if larCondFlags.LArForceIOVRunNumber.statusOn : 
        forceRN="<forceRunNumber>" +larCondFlags.LArForceIOVRunNumber() + "</forceRunNumber>"
    if larCondFlags.LArCoolChannelSelection.statusOn  : 
        selection="<channelSelection>"+larCondFlags.LArCoolChannelSelection()+"</channelSelection>"


    for i in oldlarCondDBFolders :
        if i.find("DAC2uA")>0 or  i.find("uA2MeV")>0 or i.find("HVScale")>0:
            conddb.addFolder("LAR",i+LArDBConnection+forceRN)
        else:
            conddb.addFolder("LAR",i+LArDBConnection+forceRN+selection) 

    tool.keyADC2DAC  = "Ramp"+key_suffix
    tool.keyDAC2uA   = "DAC2uA"+key_suffix
    tool.keyuA2MeV   = "uA2MeV"+key_suffix
    tool.keyMphysMcal = "MphysOverMcal"+key_suffix
    tool.keyHVScaleCorr = "HVScaleCorr"+key_suffix

    return tool
