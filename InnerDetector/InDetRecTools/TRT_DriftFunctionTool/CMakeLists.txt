################################################################################
# Package: TRT_DriftFunctionTool
################################################################################

# Declare the package name:
atlas_subdir( TRT_DriftFunctionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          Event/EventInfo
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_DriftFunctionTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry GeoModelUtilities EventInfo TRT_ConditionsServicesLib InDetIdentifier )

# Install files from the package:
atlas_install_headers( TRT_DriftFunctionTool )

