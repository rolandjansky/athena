################################################################################
# Package: NavFourMom
################################################################################

# Declare the package name:
atlas_subdir( NavFourMom )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          PRIVATE
                          Event/FourMom )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( NavFourMom
                   src/*.cxx
                   PUBLIC_HEADERS NavFourMom
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} DataModel Navigation SGTools EventKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} FourMom )

atlas_add_dictionary( NavFourMomDict
                      NavFourMom/NavFourMomDict.h
                      NavFourMom/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} DataModel Navigation SGTools EventKernel FourMom NavFourMom
                      NAVIGABLES  DataVector<INavigable4Momentum> DataVector<IParticle>
                      DATA_LINKS INav4MomAssocs
                      ELEMENT_LINKS  DataVector<I4Momentum> )

