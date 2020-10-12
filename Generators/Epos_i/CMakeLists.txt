# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( Epos_i )

# External dependencies:
find_package( CLHEP )
find_package( Crmc COMPONENTS Epos )

# Remove the --as-needed linker flags:
atlas_disable_as_needed()

# Component(s) in the package:
atlas_add_library( Epos_iLib
                   src/Epos.cxx
                   src/Ranfini.F
                   src/DRanf.F
                   src/Rangen.F
                   PUBLIC_HEADERS Epos_i
                   INCLUDE_DIRS ${CRMC_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CRMC_LIBRARIES} GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AtlasHepMCfioLib AthenaKernel GaudiKernel TruthUtils )

atlas_add_component( Epos_i
                     src/components/*.cxx
                     LINK_LIBRARIES Epos_iLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/epos_crmc.param ${CRMC_LCGROOT}/tabs/epos.inics
   ${CRMC_LCGROOT}/tabs/epos.inidi.lhc ${CRMC_LCGROOT}/tabs/epos.inirj.lhc
   ${CRMC_LCGROOT}/tabs/epos.inirj ${CRMC_LCGROOT}/tabs/epos.iniev
   ${CRMC_LCGROOT}/tabs/epos.initl ${CRMC_LCGROOT}/tabs/dpmjet.dat )
