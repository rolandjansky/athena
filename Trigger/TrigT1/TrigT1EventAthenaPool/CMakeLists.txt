# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1EventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( TrigT1EventAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES TrigT1Result/RoIBResult.h TrigT1Result/MuCTPI_RDO.h
         TrigT1Result/CTP_RDO.h TrigT1CaloEvent/TriggerTowerCollection.h
         TrigT1CaloEvent/JetElementCollection.h
         TrigT1CaloEvent/CPMTowerCollection.h
         TrigT1CaloEvent/CPMTobRoICollection.h
         TrigT1CaloEvent/CMXCPTobCollection.h
         TrigT1CaloEvent/CMXCPHitsCollection.h
         TrigT1CaloEvent/CMXJetTobCollection.h
         TrigT1CaloEvent/CMXJetHitsCollection.h
         TrigT1CaloEvent/CMXEtSumsCollection.h TrigT1CaloEvent/CMXRoI.h
         TrigT1CaloEvent/JEMEtSumsCollection.h
         TrigT1CaloEvent/JEMTobRoICollection.h
         TrigT1CaloEvent/CPMHitsCollection.h TrigT1CaloEvent/CPMRoICollection.h
         TrigT1CaloEvent/CMMCPHitsCollection.h
         TrigT1CaloEvent/CMMJetHitsCollection.h
         TrigT1CaloEvent/CMMEtSumsCollection.h TrigT1CaloEvent/CMMRoI.h
         TrigT1CaloEvent/JEMHitsCollection.h TrigT1CaloEvent/JEMRoICollection.h
         TrigT1CaloEvent/RODHeaderCollection.h
   TYPES_WITH_NAMESPACE ROIB::RoIBResult
                        LVL1::TriggerTowerCollection LVL1::JetElementCollection
                        LVL1::CPMTowerCollection LVL1::CPMTobRoICollection
                        LVL1::CMXCPTobCollection LVL1::CMXCPHitsCollection
                        LVL1::CMXJetTobCollection LVL1::CMXJetHitsCollection
                        LVL1::CMXEtSumsCollection LVL1::CMXRoI
                        LVL1::JEMEtSumsCollection LVL1::JEMTobRoICollection
                        LVL1::CPMHitsCollection LVL1::CPMRoICollection
                        LVL1::CMMCPHitsCollection LVL1::CMMJetHitsCollection
                        LVL1::CMMEtSumsCollection LVL1::CMMRoI
                        LVL1::JEMHitsCollection LVL1::JEMRoICollection
                        LVL1::RODHeaderCollection
   LINK_LIBRARIES AthenaPoolUtilities GaudiKernel AthenaPoolCnvSvcLib
                  TrigT1CaloEventLib TrigT1EventTPCnv TrigT1Interfaces
                  TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/*.py )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  set( TRIGT1EVENTATHENAPOOL_REFERENCE_TAG
       TrigT1EventAthenaPoolReference-01-00-00 )
  run_tpcnv_legacy_test( TrigT1EventTPCnv_18.0.0   ESD-18.0.0
                  REFERENCE_TAG ${TRIGT1EVENTATHENAPOOL_REFERENCE_TAG} )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
