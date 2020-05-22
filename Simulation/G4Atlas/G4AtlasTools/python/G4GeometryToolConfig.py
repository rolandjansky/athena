# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

GeoDetectorTool=CompFactory.GeoDetectorTool
from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
from AtlasGeoModel.InDetGMConfig import InDetGeometryCfg, InDetServiceMaterialCfg
from LArGeoAlgsNV.LArGMConfig import LArGMCfg
from TileGeoModel.TileGMConfig import TileGMCfg
from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
from AtlasGeoModel.ForDetGeoModelConfig import ForDetGeometryCfg

CylindricalEnvelope, PolyconicalEnvelope, MaterialDescriptionTool,G4AtlasDetectorConstructionTool=CompFactory.getComps("CylindricalEnvelope","PolyconicalEnvelope","MaterialDescriptionTool","G4AtlasDetectorConstructionTool",)

from AthenaCommon.SystemOfUnits import mm, cm, m

#ToDo - finish migrating this (dnoel)
#to still migrate: getCavernWorld, getCavernInfraGeoDetectorTool
#from ForwardRegionProperties.ForwardRegionPropertiesToolConfig import ForwardRegionPropertiesCfg


#put it here to avoid circular import?
G4GeometryNotifierSvc=CompFactory.G4GeometryNotifierSvc
def G4GeometryNotifierSvcCfg(ConfigFlags, name="G4GeometryNotifierSvc", **kwargs):
    kwargs.setdefault("ActivateLVNotifier", True)
    kwargs.setdefault("ActivatePVNotifier", False)
    return G4GeometryNotifierSvc(name, **kwargs)


def BeamPipeGeoDetectorToolCfg(ConfigFlags, name='BeamPipe', **kwargs):
    #set up geometry
    result=BeamPipeGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "BeamPipe")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def PixelGeoDetectorToolCfg(ConfigFlags, name='Pixel', **kwargs):
    #set up geometry
    result=InDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "Pixel")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def SCTGeoDetectorToolCfg(ConfigFlags, name='SCT', **kwargs):
    #set up geometry
    result=InDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "SCT")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def TRTGeoDetectorToolCfg(ConfigFlags, name='TRT', **kwargs):
    #set up geometry
    result=InDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "TRT")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def IDetServicesMatGeoDetectorToolCfg(ConfigFlags, name='IDetServicesMat', **kwargs):
    #set up geometry
    result=InDetServiceMaterialCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "InDetServMat")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def LArMgrGeoDetectorToolCfg(ConfigFlags, name='LArMgr', **kwargs):
    #set up geometry
    result=LArGMCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "LArMgr")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def TileGeoDetectorToolCfg(ConfigFlags, name='Tile', **kwargs):
    #set up geometry
    result=TileGMCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "Tile")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def LucidGeoDetectorToolCfg(ConfigFlags, name='Lucid', **kwargs):
    #set up geometry
    result=ForDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "LUCID")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def ALFAGeoDetectorToolCfg(ConfigFlags, name='ALFA', **kwargs):
    #set up geometry
    result=ForDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "ALFA")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def ZDCGeoDetectorToolCfg(ConfigFlags, name='ZDC', **kwargs):
    #set up geometry
    result=ForDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "ZDC")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def AFPGeoDetectorToolCfg(ConfigFlags, name='AFP', **kwargs):
    #set up geometry
    result=ForDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "AFP")
    kwargs.setdefault("GeoDetectorName", "AFP_GeoModel")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def FwdRegionGeoDetectorToolCfg(ConfigFlags, name='FwdRegion', **kwargs):
    #set up geometry
    result=ForDetGeometryCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "FwdRegion")
    kwargs.setdefault("GeoDetectorName", "ForwardRegionGeoModel")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def MuonGeoDetectorToolCfg(ConfigFlags, name='Muon', **kwargs):
    #set up geometry
    result=MuonGeoModelCfg(ConfigFlags)
    kwargs.setdefault("DetectorName", "Muon")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


#todo - set this up
def getCavernInfraGeoDetectorTool(ConfigFlags, name='CavernInfra', **kwargs):
    result = ComponentAccumulator() #needs geometry setting up!
    kwargs.setdefault("DetectorName", "CavernInfra")
    #add the GeometryNotifierSvc
    result.addService(G4GeometryNotifierSvcCfg(ConfigFlags))
    kwargs.setdefault("GeometryNotifierSvc", result.getService("G4GeometryNotifierSvc"))
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def IDETEnvelopeCfg(ConfigFlags, name="IDET", **kwargs):
    result = ComponentAccumulator()

    isUpgrade = ConfigFlags.GeoModel.Run =="RUN4"
    isRUN2 = ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]

    #isRUN1 = not (isRUN2 or isUpgrade) #not used, remove?

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
        toolPixel = result.popToolsAndMerge(PixelGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [toolPixel]
    if ConfigFlags.Detector.GeometrySCT:
        toolSCT = result.popToolsAndMerge(SCTGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [toolSCT]
    if ConfigFlags.Detector.GeometryTRT and not isUpgrade:
        toolTRT = result.popToolsAndMerge(TRTGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [toolTRT]

    toolIDetServices = result.popToolsAndMerge(IDetServicesMatGeoDetectorToolCfg(ConfigFlags))
    SubDetectorList += [toolIDetServices]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    result.setPrivateTools(CylindricalEnvelope(name, **kwargs))
    return result


def CALOEnvelopeCfg(ConfigFlags, name="CALO", **kwargs):
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    calolim = 6735.
    try:
        if LArDetectorToolNV.ActivateFeedThrougs:
            calolim = 6747.
    except AttributeError:
        pass
    kwargs.setdefault("DetectorName", "CALO")
    kwargs.setdefault("NSurfaces", 18)
    kwargs.setdefault("InnerRadii", [41.,41.,41.,41.,41.,41.,120.,120.,1148.,1148.,120.,120.,41.,41.,41.,41.,41.,41.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [415.,415,3795.,3795.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,4251.,3795.,3795.,415.,415.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-6781.,-calolim,-calolim,-6530.,-6530.,-4587.,-4587.,-3475.,-3475.,3475.,3475.,4587.,4587.,6530.,6530.,calolim,calolim,6781.]) #FIXME Units?
    SubDetectorList=[]
    if ConfigFlags.Detector.GeometryLAr:
        toolLArMgr = result.popToolsAndMerge(LArMgrGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolLArMgr ]
    if ConfigFlags.Detector.GeometryTile:
        toolTile = result.popToolsAndMerge(TileGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolTile ]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    result.setPrivateTools(PolyconicalEnvelope(name, **kwargs))
    return result


def ForwardRegionEnvelopeCfg(ConfigFlags, name='ForwardRegion', **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", "ForDetEnvelope")
    SubDetectorList=[]

    if ConfigFlags.Detector.SimulateFwdRegion:
        toolFwdRegion = result.popToolsAndMerge(FwdRegionGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolFwdRegion ]

        #TODO - migrate this over (WIP at the moment) (dnoel)
        #toolFwdRegionProperties = ForwardRegionPropertiesCfg(ConfigFlags)
        #result.addPublicTool(toolFwdRegionProperties) #add this as a service later?
    if ConfigFlags.Detector.GeometryZDC:
        toolZDC = result.popToolsAndMerge(ZDCGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolZDC ]
    if ConfigFlags.Detector.GeometryALFA:
        toolALFA = result.popToolsAndMerge(ALFAGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolALFA ]
    if ConfigFlags.Detector.GeometryAFP:
        toolAFP = result.popToolsAndMerge(AFPGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolAFP ]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    ##FIXME Should this really be a GeoDetectorTool???
    result.setPrivateTools(GeoDetectorTool(name, **kwargs))
    return result


def MUONEnvelopeCfg(ConfigFlags, name="MUONQ02", **kwargs): #FIXME rename to MUON when safe (IS IT SAFE?))
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    calolim = 6736.
    try:
        if LArDetectorToolNV.ActivateFeedThrougs:
            calolim = 6748.
    except AttributeError:
        pass
    kwargs.setdefault("DetectorName", "MUONQ02") #FIXME rename to MUON when safe
    kwargs.setdefault("NSurfaces", 34)
    kwargs.setdefault("InnerRadii", [1050.,1050.,1050.,1050.,436.7,436.7,279.,279.,70.,70.,420.,420.,3800.,3800.,4255.,4255.,4255.,4255.,4255.,4255.,3800.,3800.,420.,420.,70.,70.,279.,279.,436.7,436.7,1050.,1050.,1050.,1050.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [1500.,1500.,2750.,2750.,12650.,12650.,13400.,13400.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,13000.,13000.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,14200.,13400.,13400.,12650.,12650.,2750.,2750.,1500.,1500.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-26046.,-23001.,-23001.,-22030.,-22030.,-18650.,-18650.,-12900.,-12900.,-6783.,-6783.,-calolim,-calolim,-6550.,-6550.,-4000.,-4000.,4000.,4000.,6550.,6550.,calolim,calolim,6783.,6783.,12900.,12900.,18650.,18650.,22030.,22030.,23001.,23001.,26046.]) #FIXME Units?
    SubDetectorList=[]
    if ConfigFlags.Detector.SimulateMuon:
        toolMuon = result.popToolsAndMerge(MuonGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolMuon ]

    kwargs.setdefault("SubDetectors", SubDetectorList)
    result.setPrivateTools(PolyconicalEnvelope(name, **kwargs))
    return result


def CosmicShortCutCfg(ConfigFlags, name="CosmicShortCut", **kwargs):
    kwargs.setdefault("DetectorName", "TTR_BARREL")
    kwargs.setdefault("NSurfaces", 14)
    kwargs.setdefault("InnerRadii", [70.,70.,12500.,12500.,12500.,12500.,13000.,13000.,12500.,12500.,12500.,12500.,70.,70.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [12501.,12501.,12501.,12501.,13001.,13001.,13001.,13001.,13001.,13001.,12501.,12501.,12501.,12501.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces", [-22031.,-22030.,-22030.,-12901.,-12901.,-12900.,-12900., 12900.,12900.,12901.,12901.,22030.,22030.,22031.]) #FIXME Units?
    SubDetectorList=[]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return PolyconicalEnvelope(name, **kwargs)


def generateSubDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SubDetectorList=[]

    if ConfigFlags.Beam.Type == 'cosmics' or ConfigFlags.Sim.CavernBG != 'Signal':
        if ConfigFlags.Beam.Type == 'cosmics' and ConfigFlags.Sim.ReadTR:
            SubDetectorList += [ CosmicShortCutCfg(ConfigFlags) ]

    if ConfigFlags.Detector.SimulateMuon:
        accMuon = MUONEnvelopeCfg(ConfigFlags)
        toolMuon = accMuon.popPrivateTools()
        SubDetectorList += [ toolMuon ] #FIXME rename to MUON when safe
    if ConfigFlags.Detector.SimulateID:
        toolIDET = result.popToolsAndMerge(IDETEnvelopeCfg(ConfigFlags))
        SubDetectorList += [ toolIDET ]
    if ConfigFlags.Detector.SimulateCalo:
        toolCALO = result.popToolsAndMerge(CALOEnvelopeCfg(ConfigFlags))
        SubDetectorList += [ toolCALO ]
    if ConfigFlags.Detector.SimulateMuon:
        result.merge(accMuon) #add the acc later to match the old style config
    if ConfigFlags.Detector.SimulateBpipe:
        toolBpipe = result.popToolsAndMerge(BeamPipeGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolBpipe ]
    if ConfigFlags.Detector.GeometryLucid:
        toolLucid = result.popToolsAndMerge(LucidGeoDetectorToolCfg(ConfigFlags))
        SubDetectorList += [ toolLucid ]
    if ConfigFlags.Detector.SimulateForward:
        toolForward = result.popToolsAndMerge(ForwardRegionEnvelopeCfg(ConfigFlags))
        SubDetectorList += [ toolForward ]

    #if DetFlags.Muon_on(): #HACK
    #    SubDetectorList += ['MUONQ02'] #FIXME rename to MUON when safe #HACK
    #SubDetectorList += generateFwdSubDetectorList() #FIXME Fwd Detectors not supported yet.
    result.setPrivateTools(SubDetectorList)
    return result


def ATLASEnvelopeCfg(ConfigFlags, name="Atlas", **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("DetectorName", "Atlas")
    kwargs.setdefault("NSurfaces", 18)
    ## InnerRadii
    innerRadii = [0.0] * 18
    kwargs.setdefault("InnerRadii", innerRadii)

    ## Shrink the global ATLAS envelope to the activated detectors,
    ## except when running on special setups.

    ## OuterRadii
    AtlasForwardOuterR = 2751.
    AtlasOuterR1 = 14201.
    AtlasOuterR2 = 14201.
    if ConfigFlags.Beam.Type != 'cosmics' and not ConfigFlags.Detector.SimulateMuon and not \
       (ConfigFlags.Sim.CavernBG != 'Signal'):
        AtlasOuterR1 = 4251.
        AtlasOuterR2 = 4251.
        if not ConfigFlags.Detector.SimulateCalo:
            AtlasOuterR1 = 1150.
            AtlasOuterR2 = 1150.

    outerRadii = [0.0] * 18
    for i in (0, 1, 16, 17):
        outerRadii[i] = 1501.
    for i in (2, 3, 14, 15):
        outerRadii[i] = AtlasForwardOuterR
    for i in (4, 5, 12, 13):
        outerRadii[i] = AtlasOuterR2
    for i in range(6, 12):
        outerRadii[i] = AtlasOuterR1

    ## World R range
    if ConfigFlags.Sim.WorldRRange:
        routValue = ConfigFlags.Sim.WorldRRange
        if ConfigFlags.Sim.WorldRRange > max(AtlasOuterR1, AtlasOuterR2):
            for i in range(4, 14):
                outerRadii[i] = routValue
        else:
            raise RuntimeError('getATLASEnvelope: ERROR ConfigFlags.Sim.WorldRRange must be > %f. Current value %f' % (max(AtlasOuterR1, AtlasOuterR2), routValue) )
    kwargs.setdefault("OuterRadii", outerRadii)

    ## ZSurfaces
    zSurfaces = [-26046., -23001., -23001., -22031., -22031., -12899., -12899., -6741., -6741.,  6741.,  6741.,  12899., 12899., 22031., 22031., 23001., 23001., 26046.] # FIXME units mm??

    if ConfigFlags.Detector.SimulateForward:
        zSurfaces[0]  = -400000.
        zSurfaces[17] =  400000.

    #leave a check in for WorldRrange and WorldZrange?
    if ConfigFlags.Sim.WorldZRange:
        print (ConfigFlags.Sim.WorldZRange)
        if ConfigFlags.Sim.WorldZRange < 26046.:
              raise RuntimeError('getATLASEnvelope: ERROR ConfigFlags.Sim.WorldZRange must be > 26046. Current value: %f' % ConfigFlags.Sim.WorldZRange)
        zSurfaces[17] =  ConfigFlags.Sim.WorldZRange + 100.
        zSurfaces[16] =  ConfigFlags.Sim.WorldZRange + 50.
        zSurfaces[15] =  ConfigFlags.Sim.WorldZRange + 50.
        zSurfaces[14] =  ConfigFlags.Sim.WorldZRange
        zSurfaces[13] =  ConfigFlags.Sim.WorldZRange
        zSurfaces[0] =  -ConfigFlags.Sim.WorldZRange - 100.
        zSurfaces[1] =  -ConfigFlags.Sim.WorldZRange - 50.
        zSurfaces[2] =  -ConfigFlags.Sim.WorldZRange - 50.
        zSurfaces[3] =  -ConfigFlags.Sim.WorldZRange
        zSurfaces[4] =  -ConfigFlags.Sim.WorldZRange

    kwargs.setdefault("ZSurfaces", zSurfaces)
    SubDetectorList = result.popToolsAndMerge(generateSubDetectorList(ConfigFlags))
    kwargs.setdefault("SubDetectors", SubDetectorList)
    result.setPrivateTools(PolyconicalEnvelope(name, **kwargs))
    return result


def G4AtlasDetectorConstructionToolCfg(ConfigFlags, name="G4AtlasDetectorConstructionTool", **kwargs):
    return G4AtlasDetectorConstructionTool(name, **kwargs)


def MaterialDescriptionToolCfg(ConfigFlags, name="MaterialDescriptionTool", **kwargs):
    ## kwargs.setdefault("SomeProperty", aValue)
    return MaterialDescriptionTool(name, **kwargs)
