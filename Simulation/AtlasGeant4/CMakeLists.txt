
################################################################################
# Package: AtlasGeant4
################################################################################

# Declare the package name:
atlas_subdir( AtlasGeant4 )

# Generations correspond to topologically sorted list:
# - https://its.cern.ch/jira/secure/attachment/282935/topologically_sorted_package_generations.txt
set( AtlasGeant4_TARGET_OBJECTS
    # Generation 0
    G4AtlasInterfaces
    SimHelpers
    # Generation 1
    G4AtlasToolsLib
    MCTruth
    # Generation 2
    CaloG4SimLib 
    ISF_Geant4Event
    # Generation 3 
    LArG4Code
    MCTruthBaseLib
    # Generation 4
    LArG4ShowerLib 
    G4AtlasAlgLib
    TileG4InterfacesLib
    # Generation 5
    TRT_G4Utilities
    LArG4ShowerLibSvcLib 
    G4PhysicsLists
    GeoMaterial2G4
    TrackWriteFastSimLib
    ISF_Geant4ToolsLib
    TileGeoG4SDLib
    # Generation 6
    BCM_G4_SDLib
    BLM_G4_SDLib
    PixelG4_SDLib
    SCT_G4_SDLib
    TRT_G4_SDLib
    MuonG4SDLib
    G4DebuggingHelperLib
    G4ProfilingToolsLib
    Geo2G4Lib )

# Components
# Generations correspond to topologically sorted list:
# - https://its.cern.ch/jira/secure/attachment/282935/topologically_sorted_package_generations.txt
set( AtlasGeant4Component_TARGET_OBJECTS
    # Generation 1
    G4AtlasTools
    # Generation 2
    CaloG4Sim
    ISF_Geant4CommonTools
    # Generation 3
    MCTruthBase
    # Generation 4
    G4AtlasAlg
    # Generation 5
    LArG4ShowerLibSvc
    TrackWriteFastSim
    ISF_Geant4Tools
    TileGeoG4SD
    # Generation 6 
    HGTD_G4_SD
    BCM_G4_SD
    BLM_G4_SD
    PixelG4_SD
    SCT_G4_SD
    TRT_G4_SD
    TRT_TR_Process
    LArG4Barrel
    LArG4EC
    LArG4FCAL
    LArG4FastSimulation
    LArG4HEC
    LArG4SD
    MagFieldUtils
    MuonG4SD
    G4AtlasServices
    Charginos
    ExtraParticles
    G4CosmicFilter
    G4ExtraProcesses
    G4ScoringProcess
    Gauginos
    Monopole
    Quirks
    Sleptons
    G4HiveEx
    G4DebuggingTools
    G4FastSimulation
    G4ProfilingTools
    G4StepLimitation
    G4UserActions
    Geo2G4
    ISF_Services
    ISF_Geant4CommonServices
    ISF_Geant4Services
    DeadMaterial
    MinBiasScintillator
    TileGeoG4Calib )


# Add additional libs and components used only in Athena
if( NOT SIMULATIONBASE )
    list( APPEND AtlasGeant4_TARGET_OBJECTS
        # Generation 5
        ForwardTransportSvcLib
        G4ExternalDecay
        # Generation 6
        AFP_G4_SDLib
        ALFA_G4_SDLib
        LUCID_G4_SDLib
        ZDC_SDLib
        LArG4GenShowerLibLib )
    
    list( APPEND AtlasGeant4Component_TARGET_OBJECTS
        # Generation 2
        ActsGeantFollowing
	# Generation 5
	ForwardTransportSvc
	# Generation 6
        AFP_G4_SD
	ALFA_G4_SD
	ForwardRegionMgField
	ForwardTransport
	LUCID_G4_SD
	LUCID_OpProcess
	ZDC_SD
	LArG4FastSimSvc
	LArG4GenShowerLib
	LArG4H6SD
	G4AtlasTests
	G4HitFilter
	RHadrons
	ISF_FastCaloSimParametrization
	ISF_FastCaloSimSD
	ISF_FatrasToolsG4
	TBDetDescrCnv
	CombinedScintillator
	LarCrate
	MuonWall
	PhantomCalorimeter
	TrkG4UserActions )
endif() 

# Need to disable as-needed, as libraries are not necessarily
# ordered correctly with respect to objects.
atlas_disable_as_needed()

# Shared library that only tests should link to, composed of library OBJECTs
atlas_add_library( AtlasGeant4Lib
                   dummy.cc
                   SHARED
                   NO_PUBLIC_HEADERS
                   LINK_LIBRARIES ${AtlasGeant4_TARGET_OBJECTS} )


# Primary "big" component composed of all library/component OBJECT libs
atlas_add_component( AtlasGeant4
                     dummy.cc
                     LINK_LIBRARIES "-Wl,--exclude-libs,ALL" ${AtlasGeant4Component_TARGET_OBJECTS} ${AtlasGeant4_TARGET_OBJECTS} )

