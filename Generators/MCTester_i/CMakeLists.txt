# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MCTester_i )

# External dependencies:
find_package( MCTester )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint Graf Graf3d Gpad Html Postscript Gui GX11TTF GX11 )
set( MCTester_sources src/MCTesterAlg.cxx)
set( HEPMC_MCTester_INTERFACE_DIR ${MCTESTER_INCLUDE_DIRS})

# Component(s) in the package:
atlas_add_component( MCTester_i
                     ${MCTester_sources}
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${MCTESTER_INCLUDE_DIRS} ${HEPMC_MCTester_INTERFACE_DIR}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${MCTESTER_LIBRARIES} AtlasHepMCLib AthenaBaseComps GaudiKernel StoreGateLib EventInfo GeneratorObjects )


# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/*.sh share/*.C )

