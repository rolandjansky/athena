################################################################################
# Package: MagFieldUtils
################################################################################

# Declare the package name:
atlas_subdir( MagFieldUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MagneticField/MagFieldInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS RIO MathCore Core Tree Hist pthread )
find_package( XercesC )

# Component(s) in the package:
atlas_add_component( MagFieldUtils
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MagFieldInterfaces )

# Install files from the package:
atlas_install_headers( MagFieldUtils )
atlas_install_joboptions( share/*.py )

