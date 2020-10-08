# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloTTDetDescr )

# Component(s) in the package:
atlas_add_library( CaloTTDetDescr
                   src/*.cxx
                   PUBLIC_HEADERS CaloTTDetDescr
                   LINK_LIBRARIES AthenaKernel Identifier
                   PRIVATE_LINK_LIBRARIES CaloIdentifier )

