# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

#########################################################################################
#--- Tile TB 2000-2003  ------------------------------------------------
#########################################################################################

def getTileTB_CALOEnvelope(name="TileTB_CALO", **kwargs):
    kwargs.setdefault("DetectorName", "CALO")
    import math
    kwargs.setdefault("StartPhi", math.radians(-27))
    kwargs.setdefault("DeltaPhi", math.radians(60))
    kwargs.setdefault("NSurfaces", 3)
    kwargs.setdefault("InnerRadii", [2269.,950.,950.]) #FIXME Units?
    kwargs.setdefault("OuterRadii", [5140.,5140.,5140.]) #FIXME Units?
    kwargs.setdefault("ZSurfaces",  [-2830.,-1050.,6310.]) #FIXME Units?
    from G4AtlasApps.SimFlags import simFlags
    # Check the consistency of the flags
    if simFlags.Eta.statusOn and (simFlags.Theta.statusOn or simFlags.Z.statusOn):
        raise ValueError('THE ETA PARAMETER CAN NOT BE SET TOGETHER WITH THETA AND Z')
    elif not(simFlags.Theta.statusOn and simFlags.Z.statusOn) and not simFlags.Eta.statusOn:
        raise ValueError('THETA AND Z ARE NOT SET')
    import math
    from AthenaCommon import PhysicalConstants
    DeltaY=0.0
    if simFlags.Y.statusOn :
        DeltaY=-simFlags.Y.get_Value()
    PhiZ=0.0
    if simFlags.Phi.statusOn:
        PhiZ=-math.radians(simFlags.Phi.get_Value())
    if simFlags.Eta.statusOn:
        # Mode 1 -> User enters only eta
        eta=simFlags.Eta.get_Value()
        ThetaY=-(PhysicalConstants.pi*0.5)+2*math.atan(math.exp(-eta))
        DeltaX=float(2298-6208)/math.cosh(eta)+6208
        DeltaF=0.0
        DeltaZ=0.0
        #ctb_calo_rotation=math.degrees(ThetaY)
    elif(simFlags.Theta.statusOn and simFlags.Z.statusOn):
        theta=simFlags.Theta.get_Value()
        z=simFlags.Z.get_Value()
        eta=0.0
        if abs(theta) < 60.01:
            # Mode 2 -> User enters theta!=+/-90 and Z
            # Z is the distance from center of the module to the desired
            # entrace point calculated at R=2290 (start of tilecal
            # module)
            #ctb_calo_rotation=theta
            ThetaY=math.radians(theta)
            DeltaX=2298.0+3910.0*(1.0-math.cos(ThetaY))
            DeltaF=(2290.0*math.tan(-ThetaY)-z)
            DeltaZ=DeltaF*math.cos(ThetaY)
        elif abs(abs(theta)-90.0) < 0.01:
            # Mode 3 -> User enters theta=(+/-)90 and Z
            # Z is the distance from ATLAS center to corresponding
            # tilerow
            # e.g center of first tile row is at 2300 + 100/2 = 2350
            sign=int(theta>0)*2-1
            #ctb_calo_rotation=sign*90.0
            ThetaY=sign*math.radians(90.0)
            # DeltaX = ctb shift + tile_rmin + module_length/2
            DeltaX=2298.0+2290.0+5640.0/2
            DeltaF=0.0
            DeltaZ=-sign*math.fabs(z)
    else:
        print 'Tile table rotation: ERROR unknown rotation mode'
        raise ValueError('UNKNOWN MODE - NEITHER ETA NOR THETA AND Z ARE SET')
    kwargs.setdefault("RotateZ",PhiZ)
    kwargs.setdefault("RotateY",ThetaY)
    kwargs.setdefault("OffsetX",DeltaX)
    kwargs.setdefault("OffsetY",DeltaY)
    kwargs.setdefault("OffsetZ",DeltaZ)
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.Tile_on():
        SubDetectorList += ['Tile']
    SubDetectorList += ['MuonWallTileTB']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.PolyconicalEnvelope(name, **kwargs)

def getTileTB_WorldEnvelope(name="TileTB_World", **kwargs):
    kwargs.setdefault("DetectorName", "CTB")
    kwargs.setdefault("dX", 20000.) #FIXME Units?
    kwargs.setdefault("dY", 20000.) #FIXME Units?
    kwargs.setdefault("dZ", 20000.) #FIXME Units?
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.Calo_on():
        SubDetectorList += ['TileTB_CALO']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)

#########################################################################################
#--- LAr TB 2002-2004  ------------------------------------------------
#########################################################################################

def getLArTB_WorldEnvelope(name="LArTB_World", **kwargs):
    kwargs.setdefault("DetectorName", "LARTBWORLD")
    from G4AtlasApps.SimFlags import simFlags
    mode = simFlags.SimLayout.get_Value()
    if (mode=='tb_LArH6_2003'):
        kwargs.setdefault("dx", 25500.) #FIXME Units?
        kwargs.setdefault("dy", 25500.) #FIXME Units?
        kwargs.setdefault("dz", 50500.) #FIXME Units?
    elif (mode=='tb_LArH6_2002' or mode=='tb_LArH6_2004'):
        kwargs.setdefault("dx", 14500.) #FIXME Units?
        kwargs.setdefault("dy", 14500.) #FIXME Units?
        kwargs.setdefault("dz", 50500.) #FIXME Units?
    elif (mode=='tb_LArH6EC_2002'):
        kwargs.setdefault("dx", 5500.) #FIXME Units?
        kwargs.setdefault("dy", 5500.) #FIXME Units?
        kwargs.setdefault("dz", 15500.) #FIXME Units?
    else:
        raise ValueError('Unsupported GeometryVersion: %s' % mode)
    SubDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.geometry.Calo_on():
        SubDetectorList += ['LArMgr']
    kwargs.setdefault("SubDetectors", SubDetectorList)
    return CfgMgr.BoxEnvelope(name, **kwargs)
