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


# TODO: We stop here for now (migration ongoing...)
return()






#################################################################################
# Reconstruction
#################################################################################
atlas_add_citest( q431
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/q431.sh
   ENVIRONMENT ATHENA_CORE_NUMBER=8
   PROPERTIES PROCESSORS 8
   POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/checkReco.sh master_q431_AOD_digest_500events.ref master_q431_AOD_content_500events.ref" )

atlas_add_citest( q431_DAODPHYS
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/DAODPhys.sh PHYS ../q431/myAOD.pool.root
   PROPERTIES REQUIRED_FILES ../q431/myAOD.pool.root
   DEPENDS q431 )

atlas_add_citest( q431_DAODPHYSLite
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/DAODPhys.sh PHYSLITE ../q431/myAOD.pool.root
   PROPERTIES REQUIRED_FILES ../q431/myAOD.pool.root
   DEPENDS q431 )

atlas_add_citest( q221
   SCRIPT Reco_tf.py --AMI q221 --athenaopts='--threads=1' --outputAODFile=myAOD.pool.root
   POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/checkReco.sh master_q221_AOD_digest.ref master_q221_AOD_content.ref" )

atlas_add_citest( q440
   SCRIPT Reco_tf.py --AMI q440 --maxEvents=5 --athenaopts='RDOtoRDOTrigger:--threads=1' )

atlas_add_citest( MuonReconstructionConfig
   SCRIPT python -m MuonConfig.MuonReconstructionConfig --run )

atlas_add_citest( MuonCombinedReconstructionConfig
   SCRIPT python -m MuonCombinedConfig.MuonCombinedReconstructionConfig --run )

atlas_add_citest( CaloTopoClusterConfig
   SCRIPT python -m CaloRec.CaloTopoClusterConfig --run )

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
atlas_add_citest( DigitizationNewConfig
   SCRIPT DigitizationConfigNew_test.py )

atlas_add_citest( FastChain
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/FastChain.sh )

atlas_add_citest( MuonDigiReco_digi
   SCRIPT Digi_tf.py --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MuonRecRTT/Run3/HITS/AsymmetricLayout_HITS_v2.root --imf False --outputRDOFile OUT_RDO.root --conditionsTag OFLCOND-MC16-SDR-RUN3-02 )

atlas_add_citest( MuonDigiReco_reco
   SCRIPT Reco_tf.py --inputRDOFile ../MuonDigiReco_digi/OUT_RDO.root --autoConfiguration everything --imf False --outputESDFile OUT_ESD.root
   PROPERTIES REQUIRED_FILES ../MuonDigiReco_digi/OUT_RDO.root
   DEPENDS MuonDigiReco_digi
   POST_EXEC_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/test/checkMuonDigiReco.sh )

atlas_add_ciTest( Overlay_digi
   SCRIPT Digi_tf.py --AMI d1609 )

atlas_add_ciTest( Overlay_reco
   SCRIPT Reco_tf.py --AMI r12276 --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3126/HITS.12860054._032508.pool.root.1 --inputRDO_BKGFile ../Overlay_digi/MC_premixing_MT.RDO.pool.root --athenaopts='--threads=1'
   PROPERTIES REQUIRED_FILES ../Overlay_digi/MC_premixing_MT.RDO.pool.root
   DEPENDS Overlay_digi )

atlas_add_citest( OverlayTier0
   SCRIPT RunTier0Tests.py -o -n )

#################################################################################
# Trigger
#################################################################################
atlas_add_citest( Trigger_athenaHLT_decodeBS
   SCRIPT test_trigP1_v1Dev_decodeBS_build.py )

atlas_add_citest( Trigger_athenaHLT_data
   SCRIPT test_trigP1_v1PhysP1_build.py )

atlas_add_citest( Trigger_athena_MC
   SCRIPT test_trigAna_RDOtoRDOTrig_v1Dev_build.py )

atlas_add_citest( Trigger_athena_data
   SCRIPT test_trig_data_v1Dev_build.py )

atlas_add_citest( Trigger_athena_cosmic
   SCRIPT test_trig_data_v1Cosmic_build.py )

atlas_add_citest( Trigger_athena_data_NewConfig
   SCRIPT test_trig_data_newJO_build.py )

#################################################################################
# Upgrade
#################################################################################
atlas_add_citest( PhaseIIUpgrade
   SCRIPT RunWorkflowTests_Run4.py --CI -e '--maxEvents 5' )
