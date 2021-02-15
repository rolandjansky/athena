# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiDigitization )

# Component(s) in the package:
atlas_add_library( SiDigitization
                   src/SiChargedDiode.cxx
                   src/SiChargedDiodeCollection.cxx
                   src/SiSurfaceCharge.cxx
                   PUBLIC_HEADERS SiDigitization
                   LINK_LIBRARIES AthenaKernel AthAllocators Identifier GaudiKernel InDetReadoutGeometry ReadoutGeometryBase InDetSimEvent )

