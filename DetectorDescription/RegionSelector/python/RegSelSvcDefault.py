# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# default configuration of RegSelSvc
# example of a configuration in class deriving from a Configurable

from AthenaCommon.SystemOfUnits import mm
    
# import the base class
from RegionSelector.RegionSelectorConf import RegSelSvc

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

class RegSelSvcDefault ( RegSelSvc )  :

    def __init__(self, name="RegSelSvcDefault"):
        # call base class constructor
        RegSelSvc.__init__( self, name="RegSelSvc")

        from AthenaCommon.Logging import logging
        mlog = logging.getLogger( 'RegSelSvc::__init__ ' )
        mlog.info('entering')

        # z vertex range for old style methods and now propogation
        # even for ne style methods
        # old value - 168
        #        self.DeltaZ = 168 * mm
        # new value as of 03-08-2011 for the width LHC beam spot of 75mm 
        self.DeltaZ = 225 * mm

        # Configure LUT creator tools
        pixTable  = None
        sctTable  = None
        trtTable  = None
        larTable  = None
        tileTable = None
        rpcTable  = None
        mdtTable  = None
        tgcTable  = None
        cscTable  = None
        mmTable   = None
        stgcTable = None
      
        from AthenaCommon.DetFlags import DetFlags

        if DetFlags.detdescr.Calo_on():
            from LArRegionSelector.LArRegionSelectorConf import LArRegionSelectorTable
            larTable =  LArRegionSelectorTable(name="LArRegionSelectorTable")
            mlog.debug(larTable)
            
            from TileRawUtils.TileRawUtilsConf import TileRegionSelectorTable
            tileTable =  TileRegionSelectorTable(name="TileRegionSelectorTable")
            mlog.debug(tileTable)

        if DetFlags.detdescr.Muon_on():
            if DetFlags.detdescr.RPC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegionSelectorTable
                rpcTable = RPC_RegionSelectorTable(name = "RPC_RegionSelectorTable")
                mlog.debug(rpcTable)

            if DetFlags.detdescr.MDT_on():
                from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegionSelectorTable
                mdtTable = MDT_RegionSelectorTable(name = "MDT_RegionSelectorTable")
                mlog.debug(mdtTable)

            if DetFlags.detdescr.TGC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegionSelectorTable
                tgcTable = TGC_RegionSelectorTable(name = "TGC_RegionSelectorTable")
                mlog.debug(tgcTable)

            # could avoid first check in case DetFlags.detdescr.CSC_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasCSC() and DetFlags.detdescr.CSC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegionSelectorTable
                cscTable = CSC_RegionSelectorTable(name = "CSC_RegionSelectorTable")
                mlog.debug(cscTable)

            # could avoid first check in case DetFlags.detdescr.Micromegas_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasMM() and DetFlags.detdescr.Micromegas_on():
                from MuonRegionSelector.MuonRegionSelectorConf import MM_RegionSelectorTable
                mmTable = MM_RegionSelectorTable(name = "MM_RegionSelectorTable")
                mlog.debug(mmTable)

            # could avoid first check in case DetFlags.detdescr.sTGC_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasSTGC() and DetFlags.detdescr.sTGC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import sTGC_RegionSelectorTable
                stgcTable = sTGC_RegionSelectorTable(name = "sTGC_RegionSelectorTable")
                mlog.debug(stgcTable)


        self.PixelRegionLUT_CreatorTool  = pixTable
        self.SCT_RegionLUT_CreatorTool   = sctTable
        self.TRT_RegionLUT_CreatorTool   = trtTable

        self.LArRegionSelectorTable      = larTable
        self.TileRegionSelectorTable     = tileTable

        self.RPCRegionSelectorTable   = rpcTable
        self.MDTRegionSelectorTable   = mdtTable
        self.TGCRegionSelectorTable   = tgcTable
        self.CSCRegionSelectorTable   = cscTable
        self.MMRegionSelectorTable    = mmTable
        self.sTGCRegionSelectorTable  = stgcTable

        # have some job options to *disable* robs and modules
        # but also have an *enable* list from OKS, so, first, 
        # - OKS can *enable* robs, then  
        # - it *disables* any robs from the rob list, then
        # - it *disables* any modules from the module lists.
        # so be careful !!!
        self.readSiROBListFromOKS  = False        
        self.DeleteSiRobList       = []
        self.DeletePixelHashList   = []
        self.DeleteSCTHashList     = []
        self.DeleteTRTHashList     = []
        
        # set geometry and detector flags based on global properties
        # now obtained by default from GeoModelSvc at C++ init.
        #from AthenaCommon.GlobalFlags import globalflags
        #self.GeometryLayout = globalflags.DetDescrVersion()
        
        if DetFlags.detdescr.ID_on():
            self.enableID = True
            if DetFlags.detdescr.SCT_on():
                self.enableSCT = True
            else:
                self.enableSCT = False
            if DetFlags.detdescr.pixel_on():
                self.enablePixel = True
            else:
                self.enablePixel = False
            if DetFlags.detdescr.TRT_on():
                self.enableTRT = True
            else:
                self.enableTRT = False
        else:
            self.enableID = False

        if DetFlags.detdescr.Calo_on():
            self.enableCalo = True
        else:
            self.enableCalo = False

        if DetFlags.detdescr.Muon_on():
            self.enableMuon = True
            if DetFlags.detdescr.RPC_on():
                self.enableRPC = True
            else:
                self.enableRPC = False
            if DetFlags.detdescr.MDT_on():
                self.enableMDT = True
            else:
                self.enableMDT = False
            if DetFlags.detdescr.TGC_on():
                self.enableTGC = True
            else:
                self.enableTGC = False
            # could avoid first check in case DetFlags.detdescr.CSC_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasCSC() and DetFlags.detdescr.CSC_on():
                self.enableCSC = True
            else:
                self.enableCSC = False
            # could avoid first check in case DetFlags.detdescr.sTGC_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasSTGC() and DetFlags.detdescr.sTGC_on():
                self.enablesTGC = True
            else:
                self.enablesTGC = False
            # could avoid first check in case DetFlags.detdescr.Micromegas_on() would take into account MuonGeometryFlags already
            if MuonGeometryFlags.hasMM() and DetFlags.detdescr.Micromegas_on():
                self.enableMM = True
            else:
                self.enableMM = False
        else:
            self.enableMuon = False

        # now check if the trigger is actually enabled and if not, disable eveything anyway.
        # so any non-trigger algorithm has to turn it on itself, and *also* has to check that
        # the relevant detector is actually enabled
        from RecExConfig.RecAlgsFlags import recAlgs
        if not recAlgs.doTrigger():
            self.enableID    = True
            self.enablePixel = False
            self.enableSCT   = False
            self.enableTRT   = False
            self.enableCalo  = False
            self.enableMuon  = False
                                                                     
        # no longer print the summary information as this may confuse people if the configuration
        # is subsequently changed
        # print config summary for info
        # mlog.info('RegSelSvc.GeometryLayout = %s' % self.GeometryLayout)
        # if self.enableID: 
        #    mlog.info('RegSelSvc detector switches configured: indet=%s (sct=%s pixel=%s trt=%s) calo=%s muon=%s'
        #              % (self.enableID,self.enableSCT,self.enablePixel,self.enableTRT,self.enableCalo,self.enableMuon))
        # else:
        #    mlog.info('RegSelSvc detector switches configured: indet=%s calo=%s muon=%s'
        #              % (self.enableID,self.enableCalo,self.enableMuon))

