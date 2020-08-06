# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IdDictDetDescrCnv )

# Component(s) in the package:
atlas_add_component( IdDictDetDescrCnv
                     src/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AtlasDetDescr DetDescrCnvSvcLib GaudiKernel GeoModelInterfaces GeoModelUtilities IdDictDetDescr IdDictParser Identifier RDBAccessSvcLib StoreGateLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

