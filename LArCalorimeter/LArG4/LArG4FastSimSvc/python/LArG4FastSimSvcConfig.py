# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from LArG4FastSimSvc.LArG4FastSimSvcConf import LArG4FastSimSvc
from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging

class DefaultLArG4FastSimSvc(LArG4FastSimSvc):
    __slots__ = []
    log = logging.getLogger( 'LArG4FastSimSvc' )

    def __init__(self, name = "DefaultLArG4FastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        # get parametrisation level
        from G4AtlasApps.SimFlags import simFlags
        
        if not hasattr(simFlags, 'LArParameterization'):
            self.log.warning("JobProperty LArParameterization is not defined! Can not set up fast simulation!")
            return

        if simFlags.LArParameterization() == 0:
            self.log.warning("JobProperty LArParameterization is zero! No fast simulation requested!")
            return

    def setDefaults(cls, handle):
        pass

class LArG4EMBFastSimSvc(DefaultLArG4FastSimSvc):
    __slots__ = []

    def __init__(self, name = "LArG4EMBFastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        from AthenaCommon.Logging import logging
        self.log = logging.getLogger( 'LArG4EMBFastSimSvc' )

        # set defaults
        self.EFlagToShowerLib=True
        self.GFlagToShowerLib=True
        self.NeutFlagToShowerLib=True
        self.PionFlagToShowerLib=True

        self.ContainHigh=True
        self.ContainCrack=True
        self.ContainLow=True

        # get parametrisation level
        from G4AtlasApps.SimFlags import simFlags
        
        # adjust defaults for parametrisation level
        if simFlags.LArParameterization() >= 2:
            pass
        
        if simFlags.LArParameterization() >= 3:
            # switch off any EMB speed up
            self.EFlagToShowerLib=False
            self.GFlagToShowerLib=False
            self.NeutFlagToShowerLib=False
            self.PionFlagToShowerLib=False

    def setDefaults(cls, handle):

        # get service manager
        from AthenaCommon.AppMgr import ServiceMgr

        if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
            cls.log.warning("ShowerLibSvc not setup")

        if handle.EFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMB.11.root"]
        if handle.GFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMB.22.root"]
        if handle.NeutFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMB.2112.root"]
        if handle.PionFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMB.211.root"]

class LArG4EMECFastSimSvc(DefaultLArG4FastSimSvc):
    __slots__ = []
    log = logging.getLogger( 'LArG4EMECFastSimSvc' )

    def __init__(self, name = "LArG4EMECFastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        # set defaults
        self.EFlagToShowerLib=True
        self.GFlagToShowerLib=True
        self.NeutFlagToShowerLib=True
        self.PionFlagToShowerLib=True

        self.ContainHigh=True
        self.ContainCrack=True
        self.ContainLow=True

        # get parametrisation level
        from G4AtlasApps.SimFlags import simFlags
        
        # adjust defaults for parametrisation level
        if simFlags.LArParameterization() >= 2:
            pass

        if simFlags.LArParameterization() >= 3:
            # switch off any EMEC speed up
            self.EFlagToShowerLib=False
            self.GFlagToShowerLib=False
            self.NeutFlagToShowerLib=False
            self.PionFlagToShowerLib=False

    def setDefaults(cls, handle):
        # get service manager
        from AthenaCommon.AppMgr import ServiceMgr

        if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
            cls.log.warning("ShowerLibSvc not setup")

        if handle.EFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMEC.11.root"]
        if handle.GFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMEC.22.root"]
        if handle.NeutFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMEC.2112.root"]
        if handle.PionFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.EMEC.211.root"]


class LArG4FCALFastSimSvc(DefaultLArG4FastSimSvc):
    __slots__ = []
    log = logging.getLogger( 'LArG4FCALFastSimSvc' )

    def __init__(self, name = "LArG4FCALFastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        # set defaults
        self.EFlagToShowerLib=True
        self.GFlagToShowerLib=True
        self.NeutFlagToShowerLib=True
        self.PionFlagToShowerLib=True

        self.ContainHigh=True
        self.ContainLow=True

        # get parametrisation level
        from G4AtlasApps.SimFlags import simFlags
        
        # adjust defaults for parametrisation level
        if simFlags.LArParameterization() >= 2:
            pass
            
        if simFlags.LArParameterization() >= 3:
            pass

    def setDefaults(cls, handle):
        # get service manager
        from AthenaCommon.AppMgr import ServiceMgr

        if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
            cls.log.warning("ShowerLibSvc not setup")

        if handle.EFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL1.11.root"]
        if handle.GFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL1.22.root"]
        if handle.NeutFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL1.2112.root"]
        if handle.PionFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL1.211.root"]


class LArG4FCAL2FastSimSvc(DefaultLArG4FastSimSvc):
    __slots__ = []
    log = logging.getLogger( 'LArG4FCAL2FastSimSvc' )

    def __init__(self, name = "LArG4FCAL2FastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        # set defaults
        self.EFlagToShowerLib=True
        self.GFlagToShowerLib=True
        self.NeutFlagToShowerLib=True
        self.PionFlagToShowerLib=True

        self.ContainHigh=True
        self.ContainLow=True

        # get parametrisation level
        from G4AtlasApps.SimFlags import simFlags
        
        # adjust defaults for parametrisation level
        if simFlags.LArParameterization() >= 2:
            pass
                
        if simFlags.LArParameterization() >= 3:
            pass

    def setDefaults(cls, handle):
        # get service manager
        from AthenaCommon.AppMgr import ServiceMgr

        if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
            cls.log.warning("ShowerLibSvc not setup")

        if handle.EFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL2.11.root"]
        if handle.GFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL2.22.root"]
        if handle.NeutFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL2.2112.root"]
        if handle.PionFlagToShowerLib:
            ServiceMgr.LArG4ShowerLibSvc.FileNameList+=["LArG4ShowerLib.FCAL2.211.root"]


class DeadMaterialFastSimSvc(DefaultLArG4FastSimSvc):
    __slots__ = []
    log = logging.getLogger( 'DeadMaterialFastSimSvc' )

    def __init__(self, name = "DeadMaterialFastSimSvc"):
        super( DefaultLArG4FastSimSvc, self ).__init__( name )

        self.EMinEneShowerLib = 0.*GeV # electrons with this or lower energy will be killed on sight
        self.EMaxEneShowerLib = 1000.*GeV # particles with energy higher than this will be left alive
        self.GMaxEneShowerLib = 5698. # particles with Z coord this or more will be killed

    def setDefaults(cls, handle):
        pass
