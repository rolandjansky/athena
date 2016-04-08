################################################################################
# Package: InDetSurveyConstraintTool
################################################################################

# Declare the package name:
atlas_subdir( InDetSurveyConstraintTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/EventPrimitives
                          GaudiKernel
                          PRIVATE
                          Control/AthenaKernel
                          Database/RegistrationServices
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/DetDescrCond/DetDescrConditions
                          InnerDetector/InDetAlignTools/InDetAlignGenTools
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry )

# External dependencies:
find_package( AIDA )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( InDetSurveyConstraintTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives Identifier EventPrimitives GaudiKernel AthenaKernel AtlasDetDescr DetDescrConditions InDetIdentifier InDetReadoutGeometry )

# Install files from the package:
atlas_install_headers( InDetSurveyConstraintTool )

