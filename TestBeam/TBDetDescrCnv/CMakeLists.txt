################################################################################
# Package: TBDetDescrCnv
################################################################################

# Declare the package name:
atlas_subdir( TBDetDescrCnv )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/DetDescrCnvSvc
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          TestBeam/TBDetDescr )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( Geant4 )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( TBDetDescrCnv
                     src/TB*.cxx
                     INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${GEANT4_LIBRARIES} ${XERCESC_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} DetDescrCnvSvcLib GaudiKernel StoreGateLib SGtests GeoPrimitives TBDetDescr )

# Install files from the package:
atlas_install_headers( TBDetDescrCnv )
atlas_install_joboptions( share/*.py )

