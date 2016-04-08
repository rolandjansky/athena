if rec.doLArg() or rec.doTile():
    if globalflags.DataSource()=='geant4':
        include("LArConditionsCommon/LArIdMap_MC_jobOptions.py")
        include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
    else :
        include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")
        include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

    include( "LArIdCnv/LArIdCnv_joboptions.py" )
    include ("CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
 
if rec.doTile():
    include( "TileIdCnv/TileIdCnv_jobOptions.py" )
    include( "TileConditions/TileConditions_jobOptions.py" )        

if rec.doMuon():
    #MW/RN
   
    import MuonRecExample.MuonReadCalib
    #    GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"
    #if not globalflags.DataSource()=='geant4':
    #    MuDetCosmicFlags.doCalibCOOLprod=True        
    #    include("MuonCommRecExample/MuDetCosmicConDB_jobOptions.py")

