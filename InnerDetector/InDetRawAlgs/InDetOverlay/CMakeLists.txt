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
                          InnerDetector/InDetRawEvent/InDetSimData 
                          InnerDetector/InDetRecTools/TRT_ElectronPidTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          Tracking/TrkEvent/TrkTrack
                          )

find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( InDetOverlay
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps IDC_OverlayBase GaudiKernel InDetRawData StoreGateLib SGtests CxxUtils GeneratorObjects InDetIdentifier InDetSimData TrkTrack TRT_ConditionsServicesLib)

# Install files from the package:
atlas_install_headers( InDetOverlay )
atlas_install_joboptions( share/*.py )

