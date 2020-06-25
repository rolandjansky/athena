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
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry
                          PRIVATE
                          DetectorDescription/GeoModel/GeoModelInterfaces
                          DetectorDescription/GeoModel/GeoModelUtilities
                          Event/EventInfo
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )

atlas_add_library( TRT_DriftFunctionToolLib
                   TRT_DriftFunctionTool/*..h
                   INTERFACE
                   PUBLIC_HEADERS TRT_DriftFunctionTool
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( TRT_DriftFunctionTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry GeoModelUtilities EventInfo TRT_ConditionsServicesLib InDetIdentifier TRT_DriftFunctionToolLib )

