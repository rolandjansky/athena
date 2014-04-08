# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from LumiBlockComps.LumiBlockCompsConf import LuminosityTool

class LuminosityToolDefault(LuminosityTool):
    """Default LuminosityTool for offline"""
    
    __slots__ = []
    def __init__(self, name='LuminosityTool'):
        super (LuminosityToolDefault, self).__init__(name)
        
        mlog = logging.getLogger(name)

        # Set up DB configuration
        from IOVDbSvc.CondDB import conddb
        from InDetRecExample.InDetJobProperties import InDetFlags

        # Check if this is express stream or bulk
        if not InDetFlags.useBeamConstraint():
            folder  = "/TRIGGER/LUMI/LBLESTONL"
            conddb.addFolder('TRIGGER_ONL', folder)
        else:
            folder = "/TRIGGER/OFLLUMI/LBLESTOFL"
            conddb.addFolder('TRIGGER_OFL', folder)

        # Folders for per-BCID calculation (all online folders)
        conddb.addFolder('TDAQ', '/TDAQ/OLC/CALIBRATIONS')
        conddb.addFolder('TDAQ', '/TDAQ/OLC/LHC/FILLPARAMS')
        conddb.addFolder('TDAQ', '/TDAQ/OLC/BUNCHLUMIS')  # This is now in COMP200
        # conddb.addFolder('', '<db>COOLONL_TDAQ/MONP200</db> /TDAQ/OLC/BUNCHLUMIS')

        # Mistakenly created as multi-version folder, must specify HEAD
        if not conddb.folderRequested( '/TRIGGER/LVL1/BunchGroupContent' ):
            conddb.addFolderWithTag('TRIGGER', '/TRIGGER/LVL1/BunchGroupContent', 'HEAD')
            pass

        # Other folders needed by LuminosityTool
        conddb.addFolder('TRIGGER', '/TRIGGER/LUMI/LBLB')

        self.LumiFolderName = folder

        mlog.info("Created default %s using folder %s" % (name, folder))



class LuminosityToolOnline(LuminosityTool):
    """LuminosityTool for use in the online/HLT"""
    
    __slots__ = []
    def __init__(self, name='LuminosityTool'):
        super (LuminosityToolOnline, self).__init__(name)

        mlog = logging.getLogger(name)
        
        from IOVDbSvc.CondDB import conddb

        folder  = "/TRIGGER/LUMI/LBLESTONL"
        
        conddb.addFolder('TRIGGER_ONL', folder)
        conddb.addFolder('TDAQ', '/TDAQ/OLC/CALIBRATIONS')
        conddb.addFolder('TDAQ', '/TDAQ/OLC/BUNCHLUMIS')

        self.LumiFolderName = folder
        self.FillparamsFolderName = ''
        self.BunchgroupFolderName = ''
        self.LBLBFolderName = ''
        
        mlog.info("Created online %s using folder %s" % (name, folder))
