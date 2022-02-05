# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( VP1TrkAuxAlgs )

# External dependencies:
find_package( Qt5 COMPONENTS Core  )
find_package( SoQt )

# Component(s) in the package:
atlas_add_component( VP1TrkAuxAlgs src/*.cxx src/components/*.cxx
   LINK_LIBRARIES Qt5::Core AthenaBaseComps GaudiKernel TrkExInterfaces ${SOQT_LIBRARIES}
   TrkFitterInterfaces VP1Base VP1Utils )
