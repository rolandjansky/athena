# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( NavFourMom )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( NavFourMom
                   src/*.cxx
                   PUBLIC_HEADERS NavFourMom
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthLinks AthenaKernel CxxUtils EventKernel FourMom Navigation )

atlas_add_dictionary( NavFourMomDict
                      NavFourMom/NavFourMomDict.h
                      NavFourMom/selection.xml
		              LINK_LIBRARIES NavFourMom
                      NAVIGABLES  DataVector<INavigable4Momentum> DataVector<IParticle>
                      DATA_LINKS INav4MomAssocs
                      ELEMENT_LINKS  DataVector<I4Momentum> )

