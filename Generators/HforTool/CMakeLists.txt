# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HforTool )

# Component(s) in the package:
atlas_add_component( HforTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AtlasHepMCLib AthenaBaseComps EventInfo JetEvent GeneratorObjects TruthUtils GenInterfacesLib )
