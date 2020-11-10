# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBCaloGeometry )

# Component(s) in the package:
atlas_add_library( TBCaloGeometryLib
                   src/*.cxx
                   PUBLIC_HEADERS TBCaloGeometry
                   LINK_LIBRARIES AthenaBaseComps CaloDetDescrLib
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers GaudiKernel GeoPrimitives TBCaloConditionsLib TBDetDescr )

atlas_add_component( TBCaloGeometry
                     src/components/*.cxx
                     LINK_LIBRARIES TBCaloGeometryLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

