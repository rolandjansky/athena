# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_VertexReco )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist RIO )

atlas_add_component( AFP_VertexReco
                     src/*.cxx src/components/*.cxx AFP_VertexReco/*.h 
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} AthContainers AthenaBaseComps AthenaKernel GaudiKernel PathResolver StoreGateLib xAODForward )


# Install files from the package:
atlas_install_joboptions( share/*.py )
