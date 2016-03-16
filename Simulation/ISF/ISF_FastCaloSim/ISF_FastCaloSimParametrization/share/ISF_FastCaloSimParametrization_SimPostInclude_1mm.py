def add_my_user_action():
    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
    MyAction = PyG4Atlas.UserAction( 'ISF_Geant4UserActions','FastCaloSimParamAction',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
    ## Radii include:  MaxRadius MaxRadiusLAr MaxRadiusHEC MaxRadiusFCAL MaxRadiusTile
    #MyAction.set_Properties({"shift_lar_subhit":"1","shorten_lar_step":"1","MaxRadius":"1","MaxRadiusLAr":"1"})
    MyAction.set_Properties({"shift_lar_subhit":"1","shorten_lar_step":"1","MaxRadiusLAr":"1"})
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
simFlags.InitFunctions.add_function("postInit", add_my_user_action)
                           
#from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimParamAlg
#topSeq += FastCaloSimParamAlg()
ISF_HITSStream.stream1.ItemList += ["ISF_FCS_Parametrization::FCS_StepInfoCollection#ZHMergedEventSteps"]
