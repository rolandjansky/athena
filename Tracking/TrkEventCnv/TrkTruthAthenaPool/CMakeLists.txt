################################################################################
# Package: TrkTruthAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Tracking/TrkEvent/TrkTruthData
                          Tracking/TrkEventCnv/TrkTruthTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrkTruthAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrkTruthData/PRD_MultiTruthCollection.h TrkTruthData/TrackTruthCollection.h TrkTruthData/DetailedTrackTruthCollection.h
                           LINK_LIBRARIES GaudiKernel StoreGateLib SGtests AthenaPoolCnvSvcLib AthenaPoolUtilities TrkTruthData TrkTruthTPCnv )

