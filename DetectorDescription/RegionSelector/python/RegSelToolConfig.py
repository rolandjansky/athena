#
#   @file    RegSelToolConfig.py
#
#            configuration functions for the new RegSelTools 
#
#   @author  sutt 
#
#   @date    Sun  8 Mar 2020 03:27:57 GMT
#                 
#   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration#                 
#

from AthenaCommon.Constants import INFO,ERROR,FALSE,TRUE,DEBUG,VERBOSE

def _makeRegSelTool( detector, enable, CondAlgConstructor ) :
                
    from RegionSelector.RegionSelectorConf import RegSelTool

    tool = RegSelTool(name="RegSelTool_"+detector)
    
    # should we enable the look up table access for this subsystem ?

    if ( enable ) :
        
        # add the lookup table to retrieve
        
        tool.RegSelLUT = "RegSelLUTCondData_"+detector

        tool.Initialised = True

        # add the conditions algorithm to create the lookup table

        from AthenaCommon.AlgSequence import AthSequencer
        condseq = AthSequencer('AthCondSeq')
        
        if not hasattr( condseq, 'RegSelCondAlg_'+detector ) :
            CondAlg = CondAlgConstructor( name = ("RegSelCondAlg_"+detector),
                                          ManagerName = detector,
                                          PrintTable  = False,
                                          RegSelLUT = ("RegSelLUTCondData_"+detector) )

            if detector == "Pixel":
                CondAlg.DetEleCollKey = "PixelDetectorElementCollection"
            elif detector == "SCT":
                CondAlg.DetEleCollKey = "SCT_DetectorElementCollection"

            condseq += CondAlg

    else:
        # detector not configured so don't enable 
        # lookup table access

        tool.Initialised = False
    
    return tool



# inner detector toold

def makeRegSelTool_Pixel() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.pixel_on()
    from InDetRegionSelector.InDetRegionSelectorConf import SiRegSelCondAlg
    return _makeRegSelTool( 'Pixel', enabled, SiRegSelCondAlg )

def makeRegSelTool_SCT() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.SCT_on()
    from InDetRegionSelector.InDetRegionSelectorConf import SiRegSelCondAlg
    return _makeRegSelTool( 'SCT', enabled, SiRegSelCondAlg )

def makeRegSelTool_TRT() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.TRT_on()
    from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegSelCondAlg
    return _makeRegSelTool( 'TRT', enabled, TRT_RegSelCondAlg )


# muon spectrimeter tools 

def makeRegSelTool_MDT() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.MDT_on()
    from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegSelCondAlg
    return _makeRegSelTool( "MDT", enabled, MDT_RegSelCondAlg )

def makeRegSelTool_RPC() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.RPC_on()
    from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegSelCondAlg
    return _makeRegSelTool( "RPC", enabled, RPC_RegSelCondAlg )

def makeRegSelTool_TGC() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.TGC_on()
    from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegSelCondAlg
    return _makeRegSelTool( "TGC", enabled, TGC_RegSelCondAlg )

def makeRegSelTool_CSC() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.CSC_on()
    from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegSelCondAlg
    return _makeRegSelTool( "CSC", enabled, CSC_RegSelCondAlg )


# new small wheel 

def makeRegSelTool_MM() :
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    from AthenaCommon.DetFlags import DetFlags
    enabled = False
    if MuonGeometryFlags.hasMM() :
        enabled = DetFlags.detdescr.Micromegas_on()
    from MuonRegionSelector.MuonRegionSelectorConf import MM_RegSelCondAlg
    return _makeRegSelTool( "MM", enabled, MM_RegSelCondAlg )

def makeRegSelTool_sTGC() :
    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    from AthenaCommon.DetFlags import DetFlags
    enabled = False
    if MuonGeometryFlags.hasSTGC() :
        enabled = DetFlags.detdescr.sTGC_on()
    from MuonRegionSelector.MuonRegionSelectorConf import sTGC_RegSelCondAlg
    return _makeRegSelTool( "sTGC", enabled, sTGC_RegSelCondAlg )



# calorimeter 

def makeRegSelTool_TTEM() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.Calo_on()
    from LArRegionSelector.LArRegionSelectorConf import RegSelCondAlg_LAr
    return _makeRegSelTool( "TTEM", enabled, RegSelCondAlg_LAr )

def makeRegSelTool_TTHEC() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.Calo_on()
    from LArRegionSelector.LArRegionSelectorConf import RegSelCondAlg_LAr
    return _makeRegSelTool( "TTHEC", enabled, RegSelCondAlg_LAr )
            
def makeRegSelTool_FCALEM() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.Calo_on()
    from LArRegionSelector.LArRegionSelectorConf import RegSelCondAlg_LAr
    return _makeRegSelTool( "FCALEM", enabled, RegSelCondAlg_LAr )

def makeRegSelTool_FCALHAD() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.Calo_on()
    from LArRegionSelector.LArRegionSelectorConf import RegSelCondAlg_LAr
    return _makeRegSelTool( "FCALHAD", enabled, RegSelCondAlg_LAr )

# tile calorimeter

def makeRegSelTool_TILE() :
    from AthenaCommon.DetFlags import DetFlags
    enabled = DetFlags.detdescr.Calo_on()
    from TileRawUtils.TileRawUtilsConf import RegSelCondAlg_Tile
    return _makeRegSelTool( "TILE", enabled, RegSelCondAlg_Tile )

