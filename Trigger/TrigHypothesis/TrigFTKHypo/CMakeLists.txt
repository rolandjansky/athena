################################################################################
# Package: TrigFTKHypo
################################################################################

# Declare the package name:
atlas_subdir( TrigFTKHypo )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          DetectorDescription/IRegionSelector
			  Event/xAOD/xAODEventInfo  
                          Trigger/TrigTools/TrigTimeAlgs )

# External dependencies:
find_package( tdaq-common )

# # Component(s) in the package:
# atlas_add_library( TrigFTKHypoLib
#                    src/*.cxx
#                    PUBLIC_HEADERS TrigFTKHypo
#                    INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
#                    LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  GaudiKernel TrigInterfacesLib TrigTimeAlgsLib
#                    PRIVATE_LINK_LIBRARIES IRegionSelector  )

atlas_add_component( TrigFTKHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES}  GaudiKernel xAODEventInfo TrigInterfacesLib IRegionSelector TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

