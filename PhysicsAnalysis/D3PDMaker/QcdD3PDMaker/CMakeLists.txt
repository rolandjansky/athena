################################################################################
# Package: QcdD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( QcdD3PDMaker )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          GaudiKernel
                          PhysicsAnalysis/D3PDMaker/D3PDMakerUtils
                          PRIVATE
                          Control/AthenaKernel
                          Control/SGTools
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/VxVertex )

# Component(s) in the package:
atlas_add_library( QcdD3PDMakerLib
                   src/fillers/*.cxx
                   src/preprocessing/*.cxx
                   PUBLIC_HEADERS QcdD3PDMaker
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel D3PDMakerUtils StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES AthenaKernel SGTools muonEvent VxVertex )

atlas_add_component( QcdD3PDMaker
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests GaudiKernel D3PDMakerUtils AthenaKernel SGTools muonEvent VxVertex QcdD3PDMakerLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

