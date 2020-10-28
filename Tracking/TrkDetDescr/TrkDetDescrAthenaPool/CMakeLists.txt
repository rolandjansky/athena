# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDetDescrAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkDetDescrAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkGeometry/MaterialStepCollection.h TrkGeometry/LayerMaterialMap.h TrkGeometry/ElementTable.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities GaudiKernel TrkDetDescrTPCnv TrkGeometry )

