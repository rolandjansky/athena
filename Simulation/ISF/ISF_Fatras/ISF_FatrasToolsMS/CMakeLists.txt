# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ISF_FatrasToolsMS )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( ISF_FatrasToolsMS
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel MuonSimEvent ISF_FatrasInterfaces TrkParameters TrkExInterfaces MuonReadoutGeometry MuonIdHelpersLib ISF_Event ISF_InterfacesLib TrkTrack MuonTGRecToolsLib )
