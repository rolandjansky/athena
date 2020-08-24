# Converters:
include ( "EventAthenaPool/EventAthenaPool_joboptions.py")

include ( "RecAthenaPool/RecAthenaPool_joboptions.py")

include( "ParticleEventAthenaPool/ParticleEventAthenaPool_joboptions.py" )
include( "MuonIDEvent/MuonIDEventDict_joboptions.py" )

if rec.doTruth():
    include ( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py")
    include ("TrackParticleTruthAthenaPool/TrackParticleTruthAthenaPool_joboptions.py")

    
# for LAr or Tile since Calo Cell converter is there


if DetFlags.Calo_on():

    include ("EventCommonAthenaPool/EventCommonAthenaPool_joboptions.py")

    include ( "LArAthenaPool/LArAthenaPool_joboptions.py")
    
    # do not use Service("ToolSvc") anymore (from sebastien)
    # ToolSvc = Service("ToolSvc")
    # ToolSvc.LArTowerEMHEC.IncludedCalos = ["LAREM","LARHEC"]
    from LArClusterRec.LArClusterRecConf import LArTowerBuilderTool
    svcMgr.ToolSvc += LArTowerBuilderTool("LArTowerEMHEC",
                                          IncludedCalos = ["LAREM","LARHEC"])

    include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )

if DetFlags.ID_on():
    include ( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )
    include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
    include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )
    if rec.doTruth():
        include ("TrackParticleTruthAthenaPool/TrackParticleTruthAthenaPool_joboptions.py")


if DetFlags.Muon_on():
    include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
    include( "MuonRIO_OnTrack/MuonRIO_OnTrackDict_joboptions.py" )
    include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )


