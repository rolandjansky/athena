# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ALFA_GloRec )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO HistPainter Graf Gpad )

# Component(s) in the package:
atlas_add_component( ALFA_GloRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps StoreGateLib EventInfo ALFA_Geometry ALFA_RawEv ALFA_GloRecEv ALFA_LocRecCorrEv ALFA_LocRecEv GaudiKernel GeneratorObjects )

# Install files from the package:
atlas_install_joboptions( share/*.py share/*.C )
