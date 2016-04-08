################################################################################
# Package: ZdcEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ZdcEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          ForwardDetectors/ZDC/ZDC_SimEvent
                          ForwardDetectors/ZDC/ZdcCnv/ZdcEventTPCnv
                          ForwardDetectors/ZDC/ZdcEvent
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( ZdcEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES ZdcEvent/ZdcDigitsCollection.h ZdcEvent/ZdcRawChannelCollection.h ZDC_SimEvent/ZDC_SimStripHit_Collection.h ZDC_SimEvent/ZDC_SimPixelHit_Collection.h
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities ZDC_SimEvent ZdcEventTPCnv ZdcEvent GaudiKernel )

# Install files from the package:
atlas_install_headers( ZdcEventAthenaPool )
atlas_install_joboptions( share/*.py )

