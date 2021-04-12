# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiDigitization )

# Component(s) in the package:
atlas_add_library( SiDigitization
                   src/SiChargedDiode.cxx
                   src/SiChargedDiodeCollection.cxx
                   src/SiSurfaceCharge.cxx
		   src/InducedChargeModel.cxx
                   PUBLIC_HEADERS SiDigitization
                   LINK_LIBRARIES AthenaKernel AthAllocators Identifier GaudiKernel ReadoutGeometryBase InDetSimEvent 
		   #These are needed only because of InducedChargeModel
		   #Can be removed once this is moved elsewhere
		   InDetConditionsSummaryService MagFieldConditions PathResolver SiPropertiesToolLib)

