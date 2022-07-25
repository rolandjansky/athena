# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_VertexReco )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core MathCore Hist RIO )
find_package( nlohmann_json )

atlas_add_component( AFP_VertexReco
                     src/*.cxx src/components/*.cxx AFP_VertexReco/*.h 
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AFP_DBToolsLib AthContainers AthenaBaseComps AthenaKernel GaudiKernel PathResolver StoreGateLib xAODForward nlohmann_json::nlohmann_json )


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )   
