doMuonHoleSearch=True

if doMuonHoleSearch:
    # muon active geometry options
    theApp.Dlls+=["TrkDetDescrTools"]
    theApp.Dlls+=["MuonTrackingGeometry"]
    MuonStationBuilder=Service("ToolSvc.MuonStationBuilder")
    MuonStationBuilder.BuildBarrelStations = True
    MuonStationBuilder.BuildEndcapStations = True
    MuonStationBuilder.BuildCSCStations = True
    MuonStationBuilder.BuildTGCStations = True

    # muon tracking geometry options
    include( "MuonTrackingGeometry/ConfiguredMuonGeometryBuilder.py" )
    MuonTrackingGeometryBuilder = ConfiguredMuonGeometryBuilder('MuonTrackingGeometryBuilder',None)
    ToolSvc.MuonTrackingGeometryBuilder.BuildActiveMaterial = True
    ToolSvc.MuonTrackingGeometryBuilder.BuildInertMaterial = False
    ToolSvc.MuonTrackingGeometryBuilder.PhiPartitions = 8
    ToolSvc.MuonTrackingGeometryBuilder.EtaBarrelPartitions = 8

    # combined tracking geometry builder options
    include ( 'TrkDetDescrTools/ConfiguredGeometryBuilder.py' )
    MyMuonHolesOnTrackToolGeometryBuilder = ConfiguredGeometryBuilder('MuonHolesOnTrackToolGeometryBuilder', \
                                                                        None,None,None,'MuonTrackingGeometryBuilder',0)
    MyMuonHolesOnTrackToolGeometryBuilder.switchOffGeometry('ID') 
    MyMuonHolesOnTrackToolGeometryBuilder.switchOffGeometry('Calo') 
    MyMuonHolesOnTrackToolGeometryBuilder.switchOnGeometry('Muon',0) 
    MyMuonHolesOnTrackToolGeometryBuilder.printInfo() 
    
    # navigator
    include ( 'TrkExTools/ConfiguredNavigator.py' )
    MyMuonHolesOnTrackToolNavigator    = ConfiguredNavigator('MuonHolesOnTrackToolNavigator', \
                                        MyMuonHolesOnTrackToolGeometryBuilder,'MuonTest')
    MyMuonHolesOnTrackToolNavigator.printInfo()
    MyMuonHolesOnTrackToolNavigator.msgStreamLevel(0)
    # material updator
    include ( 'TrkExTools/ConfiguredUpdator.py' )
    MyMuonHolesOnTrackToolMatUpdator      = ConfiguredUpdator('MatUpdator')
    # propagator
    include ( 'TrkExTools/ConfiguredPropagator.py' )
    MyMuonHolesOnTrackToolPropagator   = ConfiguredPropagator('Trk::STEP_Propagator','STEP_Propagator')
    ToolSvc.STEP_Propagator.MaterialEffects = True
    MyMuonHolesOnTrackToolPropagator.printInfo()
    # extrapolator
    include ( 'TrkExTools/ConfiguredExtrapolator.py' )
    MyMuonHolesOnTrackToolExtrapolator = ConfiguredExtrapolator('MuonHolesOnTrackToolExtrapolator', \
                                          MyMuonHolesOnTrackToolNavigator, \
                                          MyMuonHolesOnTrackToolMatUpdator, \
                                          [ MyMuonHolesOnTrackToolPropagator ] )

    MyMuonHolesOnTrackToolExtrapolator.printInfo()
    MyMuonHolesOnTrackToolExtrapolator.msgStreamLevel(0)


theApp.Dlls   += [ "MuonTrackSummaryHelperTool" ]
myMuonTrackSummaryHelperTool = Algorithm("ToolSvc.Muon::MuonTrackSummaryHelperTool")
myMuonTrackSummaryHelperTool.DoHolesOnTrack=doMuonHoleSearch
#myMuonTrackSummaryHelperTool.OutputLevel=VERBOSE

