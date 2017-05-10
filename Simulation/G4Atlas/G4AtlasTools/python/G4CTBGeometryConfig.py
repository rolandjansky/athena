# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

#########################################################################################
#--- CTB 2004 ----------------------------------------------------------
#########################################################################################

def getLArCTBGeoDetectorTool(name='LArCTB', **kwargs):
    kwargs.setdefault("DetectorName", "LArMgr")
    kwargs.setdefault("OffsetX",-10.)
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getTileDetCTB3B3EGeoDetectorTool(name='TileDetCTB3B3E', **kwargs):
    kwargs.setdefault("DetectorName", "Tile")
    kwargs.setdefault("OffsetZ",1780.25)
    return CfgMgr.GeoDetectorTool(name, **kwargs)

def getCTBTRTEnvelope(name="CTBTRT", **kwargs):
    kwargs.setdefault("DetectorName", "CTBTRT")
    kwargs.setdefault("InnerRadius", 550.) #FIXME Units?
    kwargs.setdefault("OuterRadius", 1090.) #FIXME Units?
    kwargs.setdefault("dZ", 712.75) #FIXME Units?
    import math
    kwargs.setdefault("StartPhi", math.radians(-19.0))
    kwargs.setdefault("DeltaPhi", math.radians(31.0))

    ## InnerDetector-CTB-04/05/06 Oracle tags use position1
    #kwargs.setdefault("OffsetX",1152.)
    #kwargs.setdefault("OffsetY",-8.)
    #kwargs.setdefault("OffsetZ",550.)

    ## InnerDetector-CTB-03/09 Oracle tags use position2
    #kwargs.setdefault("OffsetX",1152.)
    #kwargs.setdefault("OffsetY",112.)
    #kwargs.setdefault("OffsetZ",550.)

    ## InnerDetector-CTB-01 Oracle tag uses position3
    #kwargs.setdefault("OffsetX",1152.)
    #kwargs.setdefault("OffsetY",96.)
    #kwargs.setdefault("OffsetZ",550.)

    return CfgMgr.CylindricalEnvelope(name, **kwargs)

def getCTB_Beampipe1(name="BeamPipe1", **kwargs):
    kwargs.setdefault("DetectorName", "BEAMPIPE1")
    kwargs.setdefault("InnerRadius", 0.) #FIXME Units?
    kwargs.setdefault("OuterRadius", 50.) #FIXME Units?
    kwargs.setdefault("dZ", 1500.) #FIXME Units?
    kwargs.setdefault("Material", 'Vacuum')
    kwargs.setdefault("OffsetX",-15486.) #FIXME Units?
    import math
    kwargs.setdefault("RotateY", math.radians(-90.))
    return CfgMgr.CylindricalEnvelope(name, **kwargs)

def getCTB_Beampipe1_Photons(name="BeamPipe1_Photons", **kwargs):
    kwargs.setdefault("dZ", 999.) #FIXME Units?
    return getCTB_Beampipe1(name, **kwargs)

def getCTB_Beampipe2(name="BeamPipe2", **kwargs):
    kwargs.setdefault("DetectorName", "BEAMPIPE2")
    kwargs.setdefault("InnerRadius", 0.) #FIXME Units?
    kwargs.setdefault("OuterRadius", 50.) #FIXME Units?
    kwargs.setdefault("dZ", 1500.) #FIXME Units?
    kwargs.setdefault("Material", 'Vacuum')
    kwargs.setdefault("OffsetX",-10755.) #FIXME Units?
    import math
    kwargs.setdefault("RotateY", math.radians(-90.))
    return CfgMgr.CylindricalEnvelope(name, **kwargs)

def getCTB_Beampipe2_Photons(name="BeamPipe2_Photons", **kwargs):
    kwargs.setdefault("dZ", 999.) #FIXME Units?
    return getCTB_Beampipe1(name, **kwargs)

def getCTB_MYLAREQUIV(name="MYLAREQUIV", **kwargs):
    kwargs.setdefault("DetectorName", "MYLAREQUIV")
    kwargs.setdefault("InnerRadius", 0.) #FIXME Units?
    kwargs.setdefault("OuterRadius", 100.) #FIXME Units?
    kwargs.setdefault("dZ", 0.00168) #FIXME Units?
    kwargs.setdefault("Material", 'Mylar')
    kwargs.setdefault("OffsetX",-13980.) #FIXME Units?
    import math
    kwargs.setdefault("RotateY", math.radians(-90.))
    return CfgMgr.CylindricalEnvelope(name, **kwargs)

def getLArFarUpstreamMaterial(name="LArFarUpstreamMaterial", **kwargs):
    kwargs.setdefault("DetectorName", "LARFARUPSTREAMMATERIAL")
    kwargs.setdefault("dX", 6.675) #FIXME Units?
    kwargs.setdefault("dY", 50.) #FIXME Units?
    kwargs.setdefault("dZ", 50.) #FIXME Units?
    kwargs.setdefault("Material", 'Aluminum')
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getCTB_LArFarUpstreamMaterial(name="CTB_LArFarUpstreamMaterial", **kwargs):
    kwargs.setdefault("OffsetX",-20000.) #FIXME Units?
    return getLArFarUpstreamMaterial(name, **kwargs)

def getIDET_LArFarUpstreamMaterial(name="IDET_LArFarUpstreamMaterial", **kwargs):
    kwargs.setdefault("OffsetX",-1020.) #FIXME Units?
    return getLArFarUpstreamMaterial(name, **kwargs)

# - Magnets  -----------------------------------------------------------
def getMAGBOXMBPL12(name="MAGBOXMBPL12", **kwargs):
    kwargs.setdefault("DetectorName", "MAGBOXMBPL12")
    kwargs.setdefault("dX", 1335.) #FIXME Units?
    kwargs.setdefault("dY", 870.)  #FIXME Units?
    kwargs.setdefault("dZ", 565.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    kwargs.setdefault("OffsetX",-11702.) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['MBPL12HOLE'])
    kwargs.setdefault("Hole_dX",   [2670.]);
    kwargs.setdefault("Hole_dY",   [261.]);
    kwargs.setdefault("Hole_dZ",   [56.]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [0]);
    kwargs.setdefault("HolePosZ",  [0]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMBPL12(name="MBPL12", **kwargs):
    kwargs.setdefault("DetectorName", "MBPL12")
    kwargs.setdefault("dX", 1000.) #FIXME Units?
    kwargs.setdefault("dY", 260.)  #FIXME Units?
    kwargs.setdefault("dZ", 55.)  #FIXME Units?
    kwargs.setdefault("OffsetX",-11702.) #FIXME Units?

    # for the photon beams we reduce the size of the
    # beampipes and we centered them in the MBPL12 and
    # MBPL13 magnet
    SubDetectorList = ['BeamPipe2_Photons']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMAGBOXMBPL13(name="MAGBOXMBPL13", **kwargs):
    kwargs.setdefault("DetectorName", "MAGBOXMBPL13")
    kwargs.setdefault("dX", 1335.) #FIXME Units?
    kwargs.setdefault("dY", 565.)  #FIXME Units?
    kwargs.setdefault("dZ", 870.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    kwargs.setdefault("OffsetX",-15500.) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['MBPL13HOLE'])
    kwargs.setdefault("Hole_dX",   [2670.]);
    kwargs.setdefault("Hole_dY",   [56.]);
    kwargs.setdefault("Hole_dZ",   [261.]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [0]);
    kwargs.setdefault("HolePosZ",  [0]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMBPL13(name="MBPL13", **kwargs):
    kwargs.setdefault("DetectorName", "MBPL13")
    kwargs.setdefault("dX", 1000.) #FIXME Units?
    kwargs.setdefault("dY", 55.)  #FIXME Units?
    kwargs.setdefault("dZ", 260.)  #FIXME Units?
    kwargs.setdefault("OffsetX",-15500.) #FIXME Units?

    # for the photon beams we reduce the size of the
    # beampipes and we centered them in the MBPL12 and
    # MBPL13 magnet
    SubDetectorList = ['BeamPipe1_Photons']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMAGBOXMBPSID(name="MAGBOXMBPSID", **kwargs):
    kwargs.setdefault("DetectorName", "MAGBOXMBPSID")
    kwargs.setdefault("dX", 845.) #FIXME Units?
    kwargs.setdefault("dY", 870.)  #FIXME Units?
    kwargs.setdefault("dZ", 565.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    kwargs.setdefault("OffsetX",500.) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['MBPSIDHOLE'])
    kwargs.setdefault("Hole_dX",   [1690.]);
    kwargs.setdefault("Hole_dY",   [161.]);
    kwargs.setdefault("Hole_dZ",   [116.]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [0]);
    kwargs.setdefault("HolePosZ",  [0]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMBPSID(name="MBPSID", **kwargs):
    kwargs.setdefault("DetectorName", "MBPSID")
    kwargs.setdefault("dX", 500.) #FIXME Units?
    kwargs.setdefault("dY", 260.)  #FIXME Units?
    kwargs.setdefault("dZ", 115.)  #FIXME Units?
    kwargs.setdefault("OffsetX",500.) #FIXME Units?
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMAGBOXMBPL(name="MAGBOXMBPL", **kwargs):
    kwargs.setdefault("DetectorName", "MAGBOXMBPL")
    kwargs.setdefault("dX", 1335.) #FIXME Units?
    kwargs.setdefault("dY", 565.)  #FIXME Units?
    kwargs.setdefault("dZ", 870.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    #kwargs.setdefault("OffsetX",28296.) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['MBPLHOLE'])
    kwargs.setdefault("Hole_dX",   [26700.]);
    kwargs.setdefault("Hole_dY",   [56.]);
    kwargs.setdefault("Hole_dZ",   [261.]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [0]);
    kwargs.setdefault("HolePosZ",  [0]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMBPL(name="MBPL", **kwargs):
    kwargs.setdefault("DetectorName", "MBPL")
    kwargs.setdefault("dX", 1000.) #FIXME Units?
    kwargs.setdefault("dY", 55.)  #FIXME Units?
    kwargs.setdefault("dZ", 260.)  #FIXME Units?
    #kwargs.setdefault("OffsetX",28296.) #FIXME Units?
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMAGBOXMBPS2(name="MAGBOXMBPS2", **kwargs):
    kwargs.setdefault("DetectorName", "MAGBOXMBPS2")
    kwargs.setdefault("dX", 845.) #FIXME Units?
    kwargs.setdefault("dY", 565.)  #FIXME Units?
    kwargs.setdefault("dZ", 870.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    #kwargs.setdefault("OffsetX",45299.) #FIXME Units?
    # Subtraction Solid - has to be a better way to do this!!
    kwargs.setdefault("NumberOfHoles", 1)
    kwargs.setdefault("HoleNames", ['MBPS2HOLE'])
    kwargs.setdefault("Hole_dX",   [1690.]);
    kwargs.setdefault("Hole_dY",   [56.]);
    kwargs.setdefault("Hole_dZ",   [261.]);
    kwargs.setdefault("HolePosX",  [0]);
    kwargs.setdefault("HolePosY",  [0]);
    kwargs.setdefault("HolePosZ",  [0]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getMBPS2(name="MBPS2", **kwargs):
    kwargs.setdefault("DetectorName", "MBPS2")
    kwargs.setdefault("dX", 500.) #FIXME Units?
    kwargs.setdefault("dY", 55.)  #FIXME Units?
    kwargs.setdefault("dZ", 260.)  #FIXME Units?
    #kwargs.setdefault("OffsetX",45299.) #FIXME Units?
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getEquivUpstreamMaterial(name="EquivUpstreamMaterial", **kwargs):
    kwargs.setdefault("DetectorName", "EquivUpstreamMaterial")
    kwargs.setdefault("dX", 5.874) #FIXME Units?
    kwargs.setdefault("dY", 100.)  #FIXME Units?
    kwargs.setdefault("dZ", 100.)  #FIXME Units?
    kwargs.setdefault("OffsetX",-1000.) #FIXME Units?
    return CfgMgr.BoxEnvelope(name, **kwargs)

# - Muon-DUMP  ---------------------------------------------------------

def getDUMPCONCRETE(name="DUMPCONCRETE", **kwargs):
    kwargs.setdefault("DetectorName", "DUMPCONCRETE")
    kwargs.setdefault("dX", 1600.) #FIXME Units?
    kwargs.setdefault("dY", 2000.)  #FIXME Units?
    kwargs.setdefault("dZ", 1600.)  #FIXME Units?
    kwargs.setdefault("Material", 'Concrete')
    kwargs.setdefault("NumberOfHoles", 2)
    kwargs.setdefault("HoleNames", ['DUMPCONCRETEHOLE1','DUMPCONCRETEHOLE2'])
    kwargs.setdefault("Hole_dX",   [1601.,1601.]);
    kwargs.setdefault("Hole_dY",   [500.,800.]);
    kwargs.setdefault("Hole_dZ",   [800.,400.]);
    kwargs.setdefault("HolePosX",  [0.,0.]);
    kwargs.setdefault("HolePosY",  [800.,-400.]);
    kwargs.setdefault("HolePosZ",  [0.,0.]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getDUMPIRON(name="DUMPIRON", **kwargs):
    kwargs.setdefault("DetectorName", "DUMPIRON")
    kwargs.setdefault("dX", 1600.) #FIXME Units?
    kwargs.setdefault("dY",  600.)  #FIXME Units?
    kwargs.setdefault("dZ",  800.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    kwargs.setdefault("NumberOfHoles", 3)
    kwargs.setdefault("HoleNames", ['DUMPIRONHOLE1','DUMPIRONCORNER1','DUMPIRONCORNER2'])
    kwargs.setdefault("Hole_dX",   [1604.,1604.,1604.]);
    kwargs.setdefault("Hole_dY",   [400.,100.,100.]);
    kwargs.setdefault("Hole_dZ",   [400.,200.,200.]);
    kwargs.setdefault("HolePosX",  [0.,0.,0.]);
    kwargs.setdefault("HolePosY",  [0.,-501.,-501.]);
    kwargs.setdefault("HolePosZ",  [0.,601.,-601.]);
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getDUMP(name="DUMP", **kwargs):
    kwargs.setdefault("DetectorName", "DUMP")
    kwargs.setdefault("dX", 1600.) #FIXME Units?
    kwargs.setdefault("dY",  600.)  #FIXME Units?
    kwargs.setdefault("dZ",  800.)  #FIXME Units?
    kwargs.setdefault("Material", 'Iron')
    #dump_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(15449.7,800.,0.)
    return CfgMgr.BoxEnvelope(name, **kwargs)

# - Envelopes  ---------------------------------------------------------

def getCTB_IDETEnvelope(name="CTB_IDET", **kwargs):
    kwargs.setdefault("DetectorName", "IDET")
    kwargs.setdefault("dX", 2380.) #FIXME Units?
    kwargs.setdefault("dY", 1000.) #FIXME Units?
    kwargs.setdefault("dZ", 1300.) #FIXME Units?
    SubDetectorList=["MBPSID","MAGBOXMBPSID"]

    #from AthenaCommon.DetFlags import DetFlags
    #if DetFlags.geometry.pixel_on():
    #    SubDetectorList += ['Pixel']
    #if DetFlags.geometry.SCT_on():
    #    SubDetectorList += ['SCT']
    #if DetFlags.geometry.TRT_on() and not isUpgrade:
    #    SubDetectorList += ['TRT']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getCTBMode2_IDETEnvelope(name="CTBMode2_IDET", **kwargs):
    SubDetectorList=["MBPSID","MAGBOXMBPSID","EquivUpstreamMaterial"]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return getCTB_IDETEnvelope(name, **kwargs)

def getCTB_CALOEnvelope(name="CTB_CALO", **kwargs):
    kwargs.setdefault("DetectorName", "CALO")
    #calo.df.SetPhiMin(math.radians(-27)) #FIXME - missing functionality
    #calo.df.SetDeltaPhi(math.radians(60)) #FIXME - missing functionality
    kwargs.setdefault("NSurfaces", 3)
    kwargs.setdefault("InnerRadii", [2269.,950.,950.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [5140.,5140.,5140.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces",  [-2830.,-1050.,6310.]) #FIXME Units?
    SubDetectorList=[]
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def getCTB_MUONEnvelope(name="CTB_MUON", **kwargs):
    kwargs.setdefault("DetectorName", "MUON")
    kwargs.setdefault("dX", 22702.) #FIXME Units?
    kwargs.setdefault("dY", 4100.) #FIXME Units?
    kwargs.setdefault("dZ", 2800.) #FIXME Units?
    kwargs.setdefault("OffsetX", 34900.)
    return CfgMgr.BoxEnvelope(name, **kwargs)

def getCTB_WorldEnvelope(name="CTB_World", **kwargs):
    kwargs.setdefault("DetectorName", "CTB")
    kwargs.setdefault("dX", 58000.) #FIXME Units?
    kwargs.setdefault("dY", 7000.) #FIXME Units?
    kwargs.setdefault("dZ", 7000.) #FIXME Units?
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.SimLayout.get_Value()=='ctbh8_photon':
        SubDetectorList=['CTB_IDET','CTB_MUON','MBPL13','MAGBOXMBPL13','MBPL12','MAGBOXMBPL12','MYLAREQUIV']
    elif simFlags.BeamConditions.get_Value():
        SubDetectorList=["CTBMode2_IDET","CTB_MUON"]
    else:
        SubDetectorList=['CTB_IDET','CTB_MUON','BeamPipe1','BeamPipe2','MYLAREQUIV']
    if not DetFlags.Muon_on():
        pass
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)

