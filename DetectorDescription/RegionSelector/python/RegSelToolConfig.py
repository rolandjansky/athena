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

from AthenaConfiguration.ComponentFactory import CompFactory # CompFactory creates old or new configs depending on the enva

def _condAlgName(detector):
    return "RegSelCondAlg_"+detector

def _createRegSelCondAlg( detector,  CondAlgConstructor ):
    """
    Cretes conditions alg that provides dat to a RegSel Tool
    """
    condAlg = CondAlgConstructor( name = _condAlgName( detector ),
                                  ManagerName = detector,
                                  PrintTable  = False,
                                  RegSelLUT = ("RegSelLUTCondData_"+detector) )

    if detector == "Pixel":
        condAlg.DetEleCollKey = "PixelDetectorElementCollection"
        condAlg.PixelCablingCondData = "PixelCablingCondData"
    elif detector == "SCT":
        condAlg.DetEleCollKey = "SCT_DetectorElementCollection"
        condAlg.SCT_CablingData = "SCT_CablingData"
    return condAlg

def _createRegSelTool( detector, enable ):
    """
    Creates RegSelTool and corresponding cond tool that is needed for its function

    If the enable flag is set - the tool is properly configured, else it is configured NOT to provide the data.

    """

    
    tool = CompFactory.RegSelTool(name="RegSelTool_"+detector)

    # should we enable the look up table access for this subsystem ?

    if not enable:
        # detector not configured so don't enable
        # lookup table access
        tool.Initialised = False
        return tool
        
    # add the lookup table to retrieve
        
    tool.RegSelLUT = "RegSelLUTCondData_"+detector # has to match wiht appropriate RegSelCondAlg
    tool.Initialised = True
    return tool



def _makeRegSelTool( detector, enable, CondAlgConstructor ):

    from AthenaCommon.AlgSequence import AthSequencer
    condseq = AthSequencer('AthCondSeq')

    if enable and not hasattr( condseq, _condAlgName( detector ) ):
        condseq += _createRegSelCondAlg( detector, CondAlgConstructor )

    return _createRegSelTool( detector, enable )



# inner detector tools

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


##### new JO counterparts

def regSelToolCfg(flags, detector, CondAlg, CablingConfigCfg=0):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    ca = ComponentAccumulator()
    if(CablingConfigCfg != 0):
        ca.merge(CablingConfigCfg(flags))
    ca.setPrivateTools(_createRegSelTool(detector, True))
    ca.addCondAlgo(_createRegSelCondAlg(detector, CondAlg))
    return ca

# inner detector

def regSelTool_Pixel_Cfg(flags):
    return regSelToolCfg(flags, "Pixel", CompFactory.SiRegSelCondAlg)

def regSelTool_SCT_Cfg(flags):
    return regSelToolCfg(flags, "SCT", CompFactory.SiRegSelCondAlg)

def regSelTool_TRT_Cfg(flags):
    return regSelToolCfg(flags, "TRT", CompFactory.TRT_RegSelCondAlg)


# muon spectrometer

def regSelTool_MDT_Cfg(flags):
    from MuonConfig.MuonCablingConfig import MDTCablingConfigCfg
    return regSelToolCfg(flags, "MDT", CompFactory.MDT_RegSelCondAlg, MDTCablingConfigCfg)

def regSelTool_RPC_Cfg(flags):
    from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
    return regSelToolCfg(flags, "RPC", CompFactory.RPC_RegSelCondAlg, RPCCablingConfigCfg)

def regSelTool_TGC_Cfg(flags):
    from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
    return regSelToolCfg(flags, "TGC", CompFactory.TGC_RegSelCondAlg, TGCCablingConfigCfg)

def regSelTool_CSC_Cfg(flags):
    return regSelToolCfg(flags, "CSC", CompFactory.CSC_RegSelCondAlg)

def regSelTool_STGC_Cfg(flags):
    return regSelToolCfg(flags, "STGC", CompFactory.STGC_RegSelCondAlg)

def regSelTool_MM_Cfg(flags):
    return regSelToolCfg(flags, "MM", CompFactory.MM_RegSelCondAlg)
