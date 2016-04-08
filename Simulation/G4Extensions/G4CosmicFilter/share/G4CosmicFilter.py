#from G4AtlasApps import PyG4Atlas, AtlasG4Eng
#from G4AtlasApps.SimFlags import simFlags
#MyAction = PyG4Atlas.UserAction('G4CosmicFilter','G4CosmicFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
#
#volumeName="CaloEntryLayer"
#if simFlags.CosmicFilterVolumeName=="Muon":
#    volumeName="MuonExitLayer"
#if simFlags.CosmicFilterVolumeName=="Calo":
#    volumeName="MuonEntryLayer"
#if simFlags.CosmicFilterVolumeName=="TRT_Barrel":
#    volumeName="TRTBarrelEntryLayer"
#if simFlags.CosmicFilterVolumeName=="TRT_EC":
#    volumeName="TRTECAEntryLayer"
#if simFlags.CosmicFilterVolumeName=="SCT_Barrel":
#    volumeName="SCTBarrelEntryLayer"
#if simFlags.CosmicFilterVolumeName=="Pixel":
#    volumeName="PixelEntryLayer"
#print 'G4CosmicFilter: Filter volume is %s' % volumeName
#
#if simFlags.CosmicFilterVolumeName=="TRT_EC":
#    print 'G4CosmicFilter: adding TRTECBEntryLayer'
#    MyAction= PyG4Atlas.UserAction('G4CosmicFilter','G4CosmicAndFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
#    actionProperties={"VolumeName":volumeName,"VolumeName2":"TRTECBEntryLayer"}
#else:
#    actionProperties={"VolumeName":volumeName}
#
#if simFlags.CosmicFilterVolumeName2.statusOn:
#    if simFlags.CosmicFilterVolumeName2=="Muon":
#        volumeName2="MuonExitLayer"
#    if simFlags.CosmicFilterVolumeName=="Calo":
#        volumeName="MuonEntryLayer"
#    if simFlags.CosmicFilterVolumeName2=="InnerDetector":
#        volumeName2="CaloEntryLayer"
#    if simFlags.CosmicFilterVolumeName2=="TRT_Barrel":
#        volumeName2="TRTBarrelEntryLayer"
#    if simFlags.CosmicFilterVolumeName2=="TRT_EC":
#        volumeName2="TRTECAEntryLayer"
#    if simFlags.CosmicFilterVolumeName2=="SCT_Barrel":
#        volumeName2="SCTBarrelEntryLayer"
#    if simFlags.CosmicFilterVolumeName2=="Pixel":
#        volumeName2="PixelEntryLayer"
#    if "volumeName2" in dir():
#        print 'G4CosmicFilter: Filter volume #2 is %s' % volumeName2
#        if simFlags.CosmicFilterVolumeName2=="TRT_EC":
#            print 'G4CosmicFilter: adding TRTECBEntryLayer'
#            MyAction = PyG4Atlas.UserAction('G4CosmicFilter','G4CosmicOrFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
#            actionProperties = {"VolumeName":volumeName,"VolumeName2":volumeName2,"VolumeName3":"TRTECBEntryLayer"}
#        else:
#            MyAction = PyG4Atlas.UserAction('G4CosmicFilter','G4CosmicAndFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
#            actionProperties = {"VolumeName":volumeName,"VolumeName2":volumeName2 }
#
#if simFlags.CosmicFilterID.statusOn:
#    actionProperties["PDG_ID"] = simFlags.CosmicFilterID.get_Value()
#if simFlags.CosmicFilterPTmin.statusOn:
#    actionProperties["pTmin"] = simFlags.CosmicFilterPTmin.get_Value()
#if simFlags.CosmicFilterPTmax.statusOn:
#    actionProperties["pTmax"] = simFlags.CosmicFilterPTmax.get_Value()
#
#MyAction.set_Properties(actionProperties)
#AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
