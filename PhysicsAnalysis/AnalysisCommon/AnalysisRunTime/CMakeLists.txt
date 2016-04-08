################################################################################
# Package: AnalysisRunTime
################################################################################

# Declare the package name:
atlas_subdir( AnalysisRunTime )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaSealSvc
                          Control/MinimalRunTime
                          Database/AthenaPOOL/EventSelectorAthenaPool
                          Database/AthenaPOOL/RDBAccessSvc
                          Event/EventAthenaPool
                          Generators/GeneratorObjectsAthenaPool
                          InnerDetector/InDetEventCnv/InDetEventAthenaPool
                          LArCalorimeter/LArCnv/LArAthenaPool
                          PhysicsAnalysis/AnalysisCommon/ParticleEventAthenaPool
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEventAthenaPool
                          PhysicsAnalysis/TruthParticleID/McParticleEventAthenaPool
                          Reconstruction/RecAthenaPool
                          RootHistCnv
                          TileCalorimeter/TileSvc/TileEventAthenaPool
                          Tracking/TrkEventCnv/TrkEventAthenaPool )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Oracle )
find_package( ROOT COMPONENTS Core PyROOT Tree MathCore Hist RIO pthread )
find_package( pytools )
find_package( tdaq-common )

