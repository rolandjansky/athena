################################################################################
# Package: InDetOverlay
################################################################################

# Declare the package name:
atlas_subdir( InDetOverlay )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Event/EventOverlay/IDC_OverlayBase
                          GaudiKernel
                          InnerDetector/InDetRawEvent/InDetRawData
                          PRIVATE
                          Control/StoreGate
 			  Control/CxxUtils	  
                          Generators/GeneratorObjects
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRawEvent/InDetSimData )

# Component(s) in the package:
atlas_add_component( InDetOverlay
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests CxxUtils GeneratorObjects InDetIdentifier InDetSimData )

# Install files from the package:
atlas_install_headers( InDetOverlay )
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
