################################################################################
# Package: TrigSteeringEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( TrigSteeringEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Trigger/TrigEvent/TrigSteeringEventTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigSteeringEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES TrigSteeringEvent/TrigRoiDescriptor.h TrigSteeringEvent/TrigRoiDescriptorCollection.h TrigSteeringEvent/TrigPassBits.h TrigSteeringEvent/TrigPassBitsCollection.h TrigSteeringEvent/TrigPassFlags.h TrigSteeringEvent/TrigPassFlagsCollection.h TrigSteeringEvent/GenericResult.h TrigSteeringEvent/Lvl1Result.h TrigSteeringEvent/HLTResult.h TrigSteeringEvent/TrigOperationalInfo.h TrigSteeringEvent/TrigOperationalInfoCollection.h
                           TYPES_WITH_NAMESPACE HLT::HLTResult LVL1CTP::Lvl1Result
                           LINK_LIBRARIES AthenaPoolUtilities GaudiKernel TrigSteeringEvent AthenaPoolCnvSvcLib TrigSteeringEventTPCnv )

# Install files from the package:
atlas_install_joboptions( share/*.py )
