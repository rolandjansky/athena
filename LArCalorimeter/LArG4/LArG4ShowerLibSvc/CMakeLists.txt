################################################################################
# Package: LArG4ShowerLibSvc
################################################################################

# Declare the package name:
atlas_subdir( LArG4ShowerLibSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          LArCalorimeter/LArG4/LArG4Code
                          PRIVATE
                          Control/AthenaKernel
                          LArCalorimeter/LArG4/LArG4ShowerLib
                          Tools/PathResolver )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

atlas_add_library( LArG4ShowerLibSvcLib
                   LArG4ShowerLibSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS LArG4ShowerLibSvc
                   LINK_LIBRARIES GaudiKernel LArG4Code AthenaBaseComps )

# Component(s) in the package:
atlas_add_component( LArG4ShowerLibSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES}  LArG4ShowerLibSvcLib AthenaKernel LArG4ShowerLib PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.py )

