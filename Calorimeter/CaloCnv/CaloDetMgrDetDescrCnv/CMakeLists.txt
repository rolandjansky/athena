# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloDetMgrDetDescrCnv )

# Component(s) in the package:
atlas_add_component( CaloDetMgrDetDescrCnv
                     src/*.cxx
                     LINK_LIBRARIES DetDescrCnvSvcLib Identifier GaudiKernel CaloDetDescrLib CaloIdentifier AthenaKernel StoreGateLib SGtests GeoModelUtilities LArReadoutGeometry TileDetDescr )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

