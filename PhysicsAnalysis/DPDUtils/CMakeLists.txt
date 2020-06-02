################################################################################
# Package: DPDUtils
################################################################################

# Declare the package name:
atlas_subdir( DPDUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/AthenaPython
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/NavFourMom
                          GaudiKernel
                          PhysicsAnalysis/AnalysisCommon/UserAnalysisUtils
                          PhysicsAnalysis/EventTag/TagEvent
                          Reconstruction/Jet/JetEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Reconstruction/egamma/egammaEvent
                          Trigger/TrigAnalysis/TrigDecisionTool
                          PRIVATE
                          Database/AthenaPOOL/DBDataModel
                          Event/EventBookkeeperMetaData
                          Reconstruction/tauEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( DPDUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps AthenaKernel StoreGateLib SGtests AthenaPoolUtilities NavFourMom GaudiKernel UserAnalysisUtilsLib TagEvent JetEvent muonEvent Particle egammaEvent TrigDecisionToolLib DBDataModel EventBookkeeperMetaData tauEvent )

# Install files from the package:
atlas_install_headers( DPDUtils )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-ignore=F401,F821 )
