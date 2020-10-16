# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBDetDescr )

# Component(s) in the package:
atlas_add_library( TBDetDescr
                   src/TB*.cxx
                   PUBLIC_HEADERS TBDetDescr
                   LINK_LIBRARIES AthContainers AthenaKernel GeoPrimitives
                   PRIVATE_LINK_LIBRARIES GaudiKernel )

atlas_add_dictionary( TBDetDescrDict
                      TBDetDescr/TBDetDescrDict.h
                      TBDetDescr/selection.xml
                      LINK_LIBRARIES TBDetDescr )
