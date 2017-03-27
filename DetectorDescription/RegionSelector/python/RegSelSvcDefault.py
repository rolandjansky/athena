# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# default configuration of RegSelSvc
# example of a configuration in class deriving from a Configurable

from AthenaCommon.SystemOfUnits import *  # loads MeV etc...
    
# import the base class
from RegionSelector.RegionSelectorConf import RegSelSvc

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
        ftkTable  = None       
      

        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.DetFlags import DetFlags

        if DetFlags.detdescr.ID_on():
            # if DetFlags.detdescr.ftk_on(): ### is the ftk properly integrated yet ??? 
            from InDetRegionSelector.InDetRegionSelectorConf import FTK_RegionSelectorTable
            if DetFlags.detdescr.FTK_on():

                ftkTable = FTK_RegionSelectorTable(name        = "FTK_RegionSelectorTable",
                                                   ManagerName = "",
                                                   OutputFile  = "RoITableFTK.txt",
                                                   PrintHashId = True,
                                                   PrintTable  = False)
                ToolSvc += ftkTable
                mlog.debug(ftkTable)
    

            if DetFlags.detdescr.pixel_on():
                from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
                pixTable = SiRegionSelectorTable(name        = "PixelRegionSelectorTable",
                                                 ManagerName = "Pixel",
                                                 OutputFile  = "RoITablePixel.txt",
                                                 PrintHashId = True,
                                                 PrintTable  = False)
                ToolSvc += pixTable
                mlog.debug(pixTable)
        
            if DetFlags.detdescr.SCT_on():
                from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
                sctTable = SiRegionSelectorTable(name        = "SCT_RegionSelectorTable",
                                                 ManagerName = "SCT",
                                                 OutputFile  = "RoITableSCT.txt",
                                                 PrintHashId = True,
                                                 PrintTable  = False)
                ToolSvc += sctTable
                mlog.debug(sctTable)

            if DetFlags.detdescr.TRT_on():
                from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
                trtTable = TRT_RegionSelectorTable(name = "TRT_RegionSelectorTable",
                                                   ManagerName = "TRT",
                                                   OutputFile  = "RoITableTRT.txt",
                                                   PrintHashId = True,
                                                   PrintTable  = False)
                ToolSvc += trtTable
                mlog.debug(trtTable)
                
        if DetFlags.detdescr.Calo_on():
            from LArRegionSelector.LArRegionSelectorConf import LArRegionSelectorTable
            larTable =  LArRegionSelectorTable(name="LArRegionSelectorTable")
            ToolSvc += larTable
            mlog.debug(larTable)
            
            from TileRawUtils.TileRawUtilsConf import TileRegionSelectorTable
            tileTable =  TileRegionSelectorTable(name="TileRegionSelectorTable")
            ToolSvc += tileTable
            mlog.debug(tileTable)

        if DetFlags.detdescr.Muon_on():
            if DetFlags.detdescr.RPC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegionSelectorTable
                rpcTable = RPC_RegionSelectorTable(name = "RPC_RegionSelectorTable")

                ToolSvc += rpcTable
                mlog.debug(rpcTable)

            if DetFlags.detdescr.MDT_on():
                from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegionSelectorTable
                mdtTable = MDT_RegionSelectorTable(name = "MDT_RegionSelectorTable")

                ToolSvc += mdtTable
                mlog.debug(mdtTable)

            if DetFlags.detdescr.TGC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegionSelectorTable
                tgcTable = TGC_RegionSelectorTable(name = "TGC_RegionSelectorTable")

                ToolSvc += tgcTable
                mlog.debug(tgcTable)

            if DetFlags.detdescr.CSC_on():
                from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegionSelectorTable
                cscTable = CSC_RegionSelectorTable(name = "CSC_RegionSelectorTable")

                ToolSvc += cscTable
                mlog.debug(cscTable)


        self.PixelRegionLUT_CreatorTool  = pixTable
        self.SCT_RegionLUT_CreatorTool   = sctTable
        self.TRT_RegionLUT_CreatorTool   = trtTable
        self.FTK_RegionLUT_CreatorTool   = ftkTable

        self.LArRegionSelectorTable      = larTable
        self.TileRegionSelectorTable     = tileTable

        self.RPC_RegionLUT_CreatorTool   = rpcTable
        self.MDT_RegionLUT_CreatorTool   = mdtTable
        self.TGC_RegionLUT_CreatorTool   = tgcTable
        self.CSC_RegionLUT_CreatorTool   = cscTable

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
            if DetFlags.detdescr.FTK_on():
                self.enableFTK = True
            else:
                self.enableFTK = False
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
            if DetFlags.detdescr.CSC_on():
                self.enableCSC = True
            else:
                self.enableCSC = False
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
            self.enableFTK   = False
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

