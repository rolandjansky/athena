################################################################################
# Package: InDetSimpleVisual
################################################################################

# Declare the package name:
atlas_subdir( InDetSimpleVisual )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          PRIVATE
                          DetectorDescription/Identifier
                          GaudiKernel
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
			  InnerDetector/InDetDetDescr/TRT_ReadoutGeometry)

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( InDetSimpleVisual
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives EventPrimitives Identifier GaudiKernel InDetIdentifier InDetReadoutGeometry TRT_ReadoutGeometry)

# Install files from the package:
atlas_install_headers( InDetSimpleVisual )
atlas_install_joboptions( share/*.py )

