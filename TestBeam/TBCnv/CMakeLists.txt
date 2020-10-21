# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TBCnv )

# External dependencies:
find_package( tdaq-common COMPONENTS eformat eformat_write )

# Component(s) in the package:
atlas_add_library( TBCnvLib
   TBCnv/*.h TBCnv/*.icc src/*.cxx
   PUBLIC_HEADERS TBCnv
   INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
   LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps
   ByteStreamData EventInfo GaudiKernel LArIdentifier TBEvent
   ByteStreamCnvSvcLib ByteStreamCnvSvcBaseLib LArCablingLib
   PRIVATE_LINK_LIBRARIES Identifier LArRawEvent StoreGateLib )

atlas_add_component( TBCnv
   src/components/*.cxx
   LINK_LIBRARIES TBEvent TBCnvLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
