# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# CI test definitions for the Athena project
# --> README.md before you modify this file
#

#################################################################################
# General
#################################################################################
atlas_add_citest( DuplicateClass
   SCRIPT python -c 'import ROOT'
   PROPERTIES FAIL_REGULAR_EXPRESSION "class .* is already in" )

#################################################################################
# Digitization/Simulation
#################################################################################

atlas_add_citest( G4ExHive
   SCRIPT athena.py --threads=4 --evtMax=50 G4AtlasApps/jobOptions.G4AtlasMT.py
   PROPERTIES PROCESSORS 4 )

atlas_add_citest( FastChain
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/FastChain.sh )

atlas_add_citest( SimulationRun2AF3
   SCRIPT RunWorkflowTests_Run2.py --CI -s -w AF3 )

atlas_add_citest( SimulationRun4FullSim
   SCRIPT RunWorkflowTests_Run4.py --CI -s -w FullSim -e '--maxEvents 5'
   LOG_IGNORE_PATTERN "WARNING FPE INVALID" )  # ignore FPEs from Geant4

atlas_add_citest( PileUpPresamplingRun2
   SCRIPT RunWorkflowTests_Run2.py --CI -p -w PileUpPresampling -e '--maxEvents 5' --no-output-checks )

atlas_add_citest( OverlayRun2MC
   SCRIPT RunWorkflowTests_Run2.py --CI -o -w MCOverlay )

atlas_add_citest( OverlayRun2Data
   SCRIPT RunWorkflowTests_Run2.py --CI -o -w DataOverlay )


#################################################################################
# Standard reconstruction workflows
#################################################################################
atlas_add_citest( RecoRun2Data
   SCRIPT RunWorkflowTests_Run2.py --CI -r -w DataReco -e '--maxEvents 500' --threads 8 --no-output-checks
   PROPERTIES PROCESSORS 8 )

atlas_add_citest( RecoRun2Data_CAConfig
   SCRIPT RunWorkflowTests_Run2.py --CI -r -w DataReco -e '--CA --maxEvents 5' --no-output-checks )

atlas_add_citest( RecoRun2Data_DAODPHYS
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/DAODPhys.sh PHYS ../RecoRun2Data/run_q442/myAOD.pool.root
   PROPERTIES REQUIRED_FILES ../RecoRun2Data/run_q442/myAOD.pool.root
   DEPENDS RecoRun2Data )

atlas_add_citest( RecoRun2Data_DAODPHYSLite
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/DAODPhys.sh PHYSLITE ../RecoRun2Data/run_q442/myAOD.pool.root
   PROPERTIES REQUIRED_FILES ../RecoRun2Data/run_q442/myAOD.pool.root
   DEPENDS RecoRun2Data )

atlas_add_citest( RecoRun2MC
   SCRIPT RunWorkflowTests_Run2.py --CI -r -w MCReco -e '--maxEvents 25' --threads 0 --no-output-checks )

atlas_add_citest( RecoRun2MC_CAConfig
   SCRIPT RunWorkflowTests_Run2.py --CI -r -w MCReco -e '--CA --maxEvents 5' --no-output-checks )

atlas_add_citest( RecoRun2MC_PileUp
   SCRIPT RunWorkflowTests_Run2.py --CI -p -w MCPileUpReco -e '--maxEvents 5 --inputRDO_BKGFile=../../PileUpPresamplingRun2/run_d1730/myRDO.pool.root' --no-output-checks  # go two levels up as the test runs in a subfolder
   PROPERTIES REQUIRED_FILES ../PileUpPresamplingRun2/run_d1730/myRDO.pool.root
   DEPENDS PileUpPresamplingRun2 )

atlas_add_citest( RecoRun3MC
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/RecoRun3MC.sh )

atlas_add_citest( RecoRun3MC_CAConfig
   SCRIPT RunWorkflowTests_Run3.py --CI -r -w MCReco -e '--CA --maxEvents 5' --no-output-checks )

atlas_add_citest( RecoRun4MC
   SCRIPT RunWorkflowTests_Run4.py --CI -r -w MCReco -e '--maxEvents 5 --inputHITSFile=../../SimulationRun4FullSim/run_s3761/myHITS.pool.root'  # go two levels up as the test runs in a subfolder
   PROPERTIES REQUIRED_FILES ../SimulationRun4FullSim/run_s3761/myHITS.pool.root
   DEPENDS SimulationRun4FullSim )


#################################################################################
# Data Quality
#################################################################################

atlas_add_citest( DataQuality_r21ESD
   SCRIPT Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root"]' DQ.Steering.doHLTMon=False --threads=1 )

atlas_add_citest( DataQuality_Run3MC
   SCRIPT Run3DQTestingDriver.py 'Input.Files=["../RecoRun3MC/run_q445/myAOD.pool.root"]' DQ.Environment=AOD --threads=1
   PROPERTIES REQUIRED_FILES ../RecoRun3MC/run_q445/myAOD.pool.root
   DEPENDS RecoRun3MC )


#################################################################################
# Special reconstruction
#################################################################################

atlas_add_citest( EgammaCAConfig
   SCRIPT Reco_tf.py --CA --steering doRAWtoALL --inputRDOFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/WorkflowReferences/master/q443/v1/myRDO.pool.root --preInclude egammaConfig.egammaOnlyFromRawFlags.egammaOnlyFromRaw --outputAODFile=AOD.pool.root --maxEvents=1 )

atlas_add_citest( Egamma
   SCRIPT ut_egammaARTJob_test.sh )


#################################################################################
# Trigger
#################################################################################

atlas_add_citest( TriggerMC
   SCRIPT test_trigAna_RDOtoRDOTrig_v1Dev_build.py )

atlas_add_citest( TriggerData
   SCRIPT test_trig_data_v1Dev_build.py )

atlas_add_citest( TriggerDataCAConfig
   SCRIPT test_trig_data_newJO_build.py )

atlas_add_citest( Trigger_athenaHLT_v1Dev
   SCRIPT test_trigP1_v1Dev_decodeBS_build.py )

atlas_add_citest( Trigger_athenaHLT_v1PhysP1
   SCRIPT test_trigP1_v1PhysP1_build.py )

atlas_add_citest( Trigger_athenaHLT_v1Cosmic
   SCRIPT test_trigP1_v1Cosmic_build.py )

# TODO: We stop here for now (migration ongoing...)
return()


#################################################################################
# DQ
#################################################################################
atlas_add_citest( Run3DQr21ESD
   SCRIPT Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root"]' DQ.Steering.doHLTMon=False --threads=1 )

atlas_add_citest( q221_Run3DQ
   PRE_EXEC_SCRIPT "cp ../q221/HLTMonitoring*.json ."
   SCRIPT Run3DQTestingDriver.py 'Input.Files=["../q221/myAOD.pool.root"]' DQ.Environment=AOD --threads=1
   PROPERTIES REQUIRED_FILES ../q221/myAOD.pool.root
   DEPENDS q221 )

#################################################################################
# Digitization/Simulation
#################################################################################
atlas_add_citest( MuonDigiReco_digi
   SCRIPT Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/HITS/AsymmetricLayout_HITS_v2.root --imf False --outputRDOFile OUT_RDO.root --conditionsTag OFLCOND-MC16-SDR-RUN3-02 )

atlas_add_citest( MuonDigiReco_reco
   SCRIPT Reco_tf.py --inputRDOFile ../MuonDigiReco_digi/OUT_RDO.root --autoConfiguration everything --imf False --outputESDFile OUT_ESD.root
   PROPERTIES REQUIRED_FILES ../MuonDigiReco_digi/OUT_RDO.root
   DEPENDS MuonDigiReco_digi
   POST_EXEC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/checkMuonDigiReco.sh )
