################################################################################
# Package: InDetAlignDBTools
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignDBTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkAlignment/TrkAlignInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Control/DataModel
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          InnerDetector/InDetAlignTools/InDetAlignGenTools
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          Tracking/TrkAlignment/TrkAlignEvent )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( InDetAlignDBTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives AthenaKernel DataModel AtlasDetDescr GaudiKernel TRT_ConditionsServicesLib InDetIdentifier InDetReadoutGeometry TrkAlignEvent )

# Install files from the package:
atlas_install_headers( InDetAlignDBTools )

