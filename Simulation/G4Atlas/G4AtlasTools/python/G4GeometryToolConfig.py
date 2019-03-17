# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

from Geo2G4.Geo2G4Conf import GeoDetectorTool
from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
from G4AtlasTools.G4AtlasToolsConf import CylindricalEnvelope

from AthenaCommon.SystemOfUnits import mm, cm, m

#tool cfg or just cfg?
def BeamPipeGeoDetectorToolCfg(ConfigFlags, name='BeamPipe', **kwargs):
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "BeamPipe")
    return result, GeoDetectorTool(name, **kwargs)

def PixelGeoDetectorToolCfg(ConfigFlags, name='Pixel', **kwargs):    
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "Pixel")
    return result, GeoDetectorTool(name, **kwargs)

def SCTGeoDetectorToolCfg(ConfigFlags, name='SCT', **kwargs):
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "SCT")
    return result, GeoDetectorTool(name, **kwargs)

def TRTGeoDetectorToolCfg(ConfigFlags, name='TRT', **kwargs):
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "TRT")
    return result, GeoDetectorTool(name, **kwargs)

def IDetServicesMatGeoDetectorToolCfg(ConfigFlags, name='IDetServicesMat', **kwargs):
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "InDetServMat")
    return result, GeoDetectorTool(name, **kwargs)

def getLArMgrGeoDetectorTool(name='LArMgr', **kwargs):
    kwargs.setdefault("DetectorName", "LArMgr")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getTileGeoDetectorTool(name='Tile', **kwargs):
    kwargs.setdefault("DetectorName", "Tile")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getLucidGeoDetectorTool(name='Lucid', **kwargs):
    kwargs.setdefault("DetectorName", "LUCID")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getALFAGeoDetectorTool(name='ALFA', **kwargs):
    kwargs.setdefault("DetectorName", "ALFA")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getZDCGeoDetectorTool(name='ZDC', **kwargs):
    kwargs.setdefault("DetectorName", "ZDC")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getAFPGeoDetectorTool(name='AFP', **kwargs):
    kwargs.setdefault("DetectorName", "AFP")
    kwargs.setdefault("GeoDetectorName", "AFP_GeoModel")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getFwdRegionGeoDetectorTool(name='FwdRegion', **kwargs):
    kwargs.setdefault("DetectorName", "FwdRegion")
    kwargs.setdefault("GeoDetectorName", "ForwardRegionGeoModel")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getMuonGeoDetectorTool(name='Muon', **kwargs):
    kwargs.setdefault("DetectorName", "Muon")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getCavernInfraGeoDetectorTool(name='CavernInfra', **kwargs):
    kwargs.setdefault("DetectorName", "CavernInfra")
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def IDETEnvelopeCfg(ConfigFlags, name="IDET", **kwargs):
    result = ComponentAccumulator()

    isUpgrade = ConfigFlags.GeoModel.Run =="RUN4" 
    isRUN2 = ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]
    isRUN1 = not (isRUN2 or isUpgrade)

    kwargs.setdefault("DetectorName", "IDET")
    innerRadius = 37.*mm # RUN1 default
    if isRUN2:
        innerRadius = 28.9*mm #29.15*mm
    if isUpgrade:
        innerRadius = 32.15*mm
    kwargs.setdefault("InnerRadius", innerRadius)
    kwargs.setdefault("OuterRadius", 1.148*m)
    kwargs.setdefault("dZ", 347.5*cm)

    SubDetectorList=[]
    if ConfigFlags.Detector.GeometryPixel:
        accPixel, toolPixel = PixelGeoDetectorToolCfg(ConfigFlags)
        SubDetectorList += [toolPixel] 
        result.merge(accPixel)
    if ConfigFlags.Detector.GeometrySCT:
        accSCT, toolSCT = SCTGeoDetectorToolCfg(ConfigFlags)
        SubDetectorList += [toolSCT]
        result.merge(accSCT)
    if ConfigFlags.Detector.GeometryTRT and not isUpgrade:
        accTRT, toolTRT = TRTGeoDetectorToolCfg(ConfigFlags)
        SubDetectorList += [toolTRT]
        result.merge(accTRT)

    accIDetServices, toolIDetServices = IDetServicesMatGeoDetectorToolCfg(ConfigFlags)
    SubDetectorList += [toolIDetServices]
    result.merge(accIDetServices)
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return result, CylindricalEnvelope(name, **kwargs)

def getCALOEnvelope(name="CALO", **kwargs):
    kwargs.setdefault("DetectorName", "CALO")
    kwargs.setdefault("NSurfaces", 18)
    kwargs.setdefault("InnerRadii", [41.,41.,41.,41.,41.,41.,120.,120.,1148.,1148.,120.,120.,41.,41.,41.,41.,41.,41.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [415.,415,3795.,3795.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,3795.,3795.,415.,415.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-6781.,-6735.,-6735.,-6530.,-6530.,-4587.,-4587.,-3475.,-3475.,3475.,3475.,4587.,4587.,6530.,6530.,6735.,6735.,6781.]) #FIXME Units?
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.LAr_on():
        SubDetectorList += ['LArMgr']
    if DetFlags.geometry.Tile_on():
        SubDetectorList += ['Tile']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def getForwardRegionEnvelope(name='ForwardRegion', **kwargs):
    kwargs.setdefault("DetectorName", "ForDetEnvelope")
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.FwdRegion_on():
        SubDetectorList += ['FwdRegion']
        # FIXME Temporary solution - looking for a better place to add this one.
        from AthenaCommon.CfgGetter import getPublicTool
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += getPublicTool("ForwardRegionProperties")
    if DetFlags.geometry.ZDC_on():
        SubDetectorList += ['ZDC']
    if DetFlags.geometry.ALFA_on():
        SubDetectorList += ['ALFA']
    if DetFlags.geometry.AFP_on():
        SubDetectorList += ['AFP']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.GeoDetectorTool(name, **kwargs) ##FIXME Should this really be a GeoDetectorTool???

def getMUONEnvelope(name="MUONQ02", **kwargs): #FIXME rename to MUON when safe
    kwargs.setdefault("DetectorName", "MUONQ02") #FIXME rename to MUON when safe
    kwargs.setdefault("NSurfaces", 34)
    kwargs.setdefault("InnerRadii", [1050.,1050.,1050.,1050.,436.7,436.7,279.,279.,70.,70.,420.,420.,3800.,3800.,4255.,4255.,4255.,4255.,4255.,4255.,3800.,3800.,420.,420.,70.,70.,279.,279.,436.7,436.7,1050.,1050.,1050.,1050.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [1500.,1500.,2750.,2750.,12650.,12650.,13400.,13400.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,13000.,13000.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,13400.,13400.,12650.,12650.,2750.,2750.,1500.,1500.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-26046.,-23001.,-23001.,-22030.,-22030.,-18650.,-18650.,-12900.,-12900.,-6783.,-6783.,-6736.,-6736.,-6550.,-6550.,-4000.,-4000.,4000.,4000.,6550.,6550.,6736.,6736.,6783.,6783.,12900.,12900.,18650.,18650.,22030.,22030.,23001.,23001.,26046.]) #FIXME Units?
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.Muon_on():
        SubDetectorList += ['Muon']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def getCosmicShortCut(name="CosmicShortCut", **kwargs):
    kwargs.setdefault("DetectorName", "TTR_BARREL")
    kwargs.setdefault("NSurfaces", 14)
    kwargs.setdefault("InnerRadii", [70.,70.,12500.,12500.,12500.,12500.,13000.,13000.,12500.,12500.,12500.,12500.,70.,70.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [12501.,12501.,12501.,12501.,13001.,13001.,13001.,13001.,13001.,13001.,12501.,12501.,12501.,12501.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-22031.,-22030.,-22030.,-12901.,-12901.,-12900.,-12900., 12900.,12900.,12901.,12901.,22030.,22030.,22031.]) #FIXME Units?
    SubDetectorList=[]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def generateSubDetectorList():
    SubDetectorList=[]
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics' or \
            (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
        if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, "ReadTR"):
            SubDetectorList += ['CosmicShortCut']
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.Muon_on():
        SubDetectorList += ['MUONQ02'] #FIXME rename to MUON when safe
    if DetFlags.ID_on():
        SubDetectorList += ['IDET']
    if DetFlags.Calo_on():
        SubDetectorList += ['CALO']
    if DetFlags.ID_on(): #HACK
        if DetFlags.bpipe_on(): #HACK
            SubDetectorList += ['BeamPipe'] #HACK
    if DetFlags.geometry.Lucid_on():
        SubDetectorList += ['Lucid']
    if simFlags.ForwardDetectors.statusOn:
        SubDetectorList += ['ForwardRegion']
    #if DetFlags.Muon_on(): #HACK
    #    SubDetectorList += ['MUONQ02'] #FIXME rename to MUON when safe #HACK
    #SubDetectorList += generateFwdSubDetectorList() #FIXME Fwd Detectors not supported yet.
    return SubDetectorList

def getATLAS(name="Atlas", **kwargs):
    kwargs.setdefault("DetectorName", "Atlas")
    kwargs.setdefault("NSurfaces", 18)
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.BeamFlags import jobproperties
    from AthenaCommon.DetFlags import DetFlags
    ## InnerRadii
    innerRadii = [0.0] * 18
    kwargs.setdefault("InnerRadii", innerRadii)

    ## Shrink the global ATLAS envelope to the activated detectors,
    ## except when running on special setups.

    ## OuterRadii
    AtlasForwardOuterR = 2751.
    AtlasOuterR1 = 14201.
    AtlasOuterR2 = 14201.
    AtlasOuterR3 =  1501.
    if jobproperties.Beam.beamType() != 'cosmics' and not DetFlags.Muon_on() and not \
       (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
        AtlasOuterR1 = 4251.
        AtlasOuterR2 = 4251.
        if not DetFlags.Calo_on():
            AtlasOuterR1 = 1150.
            AtlasOuterR2 = 1150.

    outerRadii = [0.0] * 18
    for i in (0, 1, 16, 17):
        outerRadii[i] = 1501.
    for i in (2, 3, 14, 15):
        outerRadii[i] = AtlasForwardOuterR
    for i in (4, 5, 12, 13):
        outerRadii[i] = AtlasOuterR2
    for i in xrange(6, 12):
        outerRadii[i] = AtlasOuterR1

    ## World R range
    if simFlags.WorldRRange.statusOn:
        if simFlags.WorldRRange.get_Value() > max(AtlasOuterR1, AtlasOuterR2):
            routValue = simFlags.WorldRRange.get_Value()
            for i in xrange(4, 14):
                outerRadii[i] = routValue
        else:
            raise RuntimeError('getATLASEnvelope: ERROR simFlags.WorldRRange must be > %f. Current value %f' % max(AtlasOuterR1, AtlasOuterR2), routValue)
    kwargs.setdefault("OuterRadii", outerRadii)

    ## ZSurfaces
    zSurfaces = [-26046., -23001., -23001., -22031., -22031., -12899., -12899., -6741., -6741.,  6741.,  6741.,  12899., 12899., 22031., 22031., 23001., 23001., 26046.] # FIXME units mm??

    if simFlags.ForwardDetectors.statusOn:
        zSurfaces[0]  = -400000.
        zSurfaces[17] =  400000.

    if simFlags.WorldZRange.statusOn:
        if simFlags.WorldZRange.get_Value() < 26046.:
              AtlasG4Eng.G4Eng.log.error('')
              raise RuntimeError('getATLASEnvelope: ERROR simFlags.WorldZRange must be > 26046. Current value: %f' % simFlags.WorldZRange.get_Value())
        zSurfaces[17] =  simFlags.WorldZRange.get_Value() + 100.
        zSurfaces[16] =  simFlags.WorldZRange.get_Value() + 50.
        zSurfaces[15] =  simFlags.WorldZRange.get_Value() + 50.
        zSurfaces[14] =  simFlags.WorldZRange.get_Value()
        zSurfaces[13] =  simFlags.WorldZRange.get_Value()
        zSurfaces[0] =  -simFlags.WorldZRange.get_Value() - 100.
        zSurfaces[1] =  -simFlags.WorldZRange.get_Value() - 50.
        zSurfaces[2] =  -simFlags.WorldZRange.get_Value() - 50.
        zSurfaces[3] =  -simFlags.WorldZRange.get_Value()
        zSurfaces[4] =  -simFlags.WorldZRange.get_Value()
    kwargs.setdefault("ZSurfaces", zSurfaces)
    kwargs.setdefault("SubDetectors", generateSubDetectorList())
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def getCavernWorld(name="Cavern", **kwargs):
    kwargs.setdefault("DetectorName", "World")
    bedrockDX = 302700
    bedrockDZ = 301000
    from G4AtlasApps.SimFlags import simFlags
    if not (hasattr(simFlags,'CavernBG') and simFlags.CavernBG.statusOn ):
        ## Be ready to resize bedrock if the cosmic generator needs more space
        if simFlags.ISFRun:
            # for ISF cosmics simulation, set world volume to biggest possible case
            bedrockDX = 1000.*3000 # 3 km
            bedrockDZ = 1000.*3000 # 3 km
        else:
            from CosmicGenerator.CosmicGeneratorConfig import CavernPropertyCalculator
            theCavernProperties = CavernPropertyCalculator()
            if theCavernProperties.BedrockDX() > bedrockDX:
                bedrockDX = theCavernProperties.BedrockDX()
            if theCavernProperties.BedrockDZ() > bedrockDZ:
                bedrockDZ = theCavernProperties.BedrockDZ()
    kwargs.setdefault("dX", bedrockDX) #FIXME Units?
    kwargs.setdefault("dY", 57300 + 41000 + 1000) # 1 extra metre to help voxelization... #FIXME Units?
    kwargs.setdefault("dZ", bedrockDZ) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['BelowCavern'])
    kwargs.setdefault("Hole_dX",   [bedrockDX]);
    kwargs.setdefault("Hole_dY",   [41000]);
    kwargs.setdefault("Hole_dZ",   [bedrockDZ]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [-58300]);
    kwargs.setdefault("HolePosZ",  [0]);
    kwargs.setdefault("SubDetectors", ['CavernInfra', 'Atlas'])
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getG4AtlasDetectorConstructionTool(name="G4AtlasDetectorConstructionTool", **kwargs):
    return CfgMgr.G4AtlasDetectorConstructionTool(name, **kwargs)

def getMaterialDescriptionTool(name="MaterialDescriptionTool", **kwargs):
    ## kwargs.setdefault("SomeProperty", aValue)
    return CfgMgr.MaterialDescriptionTool(name, **kwargs)