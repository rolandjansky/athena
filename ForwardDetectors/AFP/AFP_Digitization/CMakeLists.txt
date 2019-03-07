################################################################################
# Package: AFP_Digitization
################################################################################

# Declare the package name:
atlas_subdir( AFP_Digitization )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/PileUpTools
                          Event/xAOD/xAODEventInfo
                          ForwardDetectors/AFP/AFP_DigiEv
                          ForwardDetectors/AFP/AFP_SimEv
                          GaudiKernel
                          Simulation/HitManagement
                          PRIVATE
                          DetectorDescription/Identifier )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( AFP_Digitization
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel PileUpToolsLib xAODEventInfo AFP_DigiEv AFP_SimEv GaudiKernel HitManagement Identifier )

# Install files from the package:
atlas_install_headers( AFP_Digitization )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

