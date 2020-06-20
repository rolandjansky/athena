################################################################################
# Package: ForwardTransportSvc
################################################################################

# Declare the package name:
atlas_subdir( ForwardTransportSvc )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          ForwardDetectors/ForwardTracker
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Generators/GeneratorObjects 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( Geant4 )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( XercesC )

atlas_add_library( ForwardTransportSvcLib
                   ForwardTransportSvc/*.h
                   INTERFACE
                   PUBLIC_HEADERS ForwardTransportSvc
                   INCLUDE_DIRS ${GEANT4_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel AtlasHepMCLib ForwardTracker )

# Component(s) in the package:
atlas_add_component( ForwardTransportSvc
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${XERCESC_INCLUDE_DIRS} ${GEANT4_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${XERCESC_LIBRARIES} ${GEANT4_LIBRARIES} ${CLHEP_LIBRARIES} ForwardTransportSvcLib AthenaBaseComps GeneratorObjects ForwardTracker )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

