# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCondAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( LArCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArRawConditions/LArAutoCorrComplete.h LArRawConditions/LArAutoCorrMC.h LArRawConditions/LArCaliPulseParamsComplete.h LArRawConditions/LArDAC2uAComplete.h LArRawConditions/LArDAC2uAMC.h LArRawConditions/LArDetCellParamsComplete.h LArRawConditions/LArfSamplComplete.h LArRawConditions/LArfSamplMC.h LArRawConditions/LArH6OscillationComplete.h LArRawConditions/LArMinBiasComplete.h LArRawConditions/LArMinBiasMC.h LArRawConditions/LArMinBiasAverageComplete.h LArRawConditions/LArMinBiasAverageMC.h LArRawConditions/LArMphysOverMcalComplete.h LArRawConditions/LArMphysOverMcalMC.h LArRawConditions/LArNoiseComplete.h LArRawConditions/LArNoiseMC.h LArRawConditions/LArOFCComplete.h LArRawConditions/LArPedestalComplete.h LArRawConditions/LArPedestalMC.h LArRawConditions/LArPhysCaliTdiffComplete.h LArRawConditions/LArRampComplete.h LArRawConditions/LArRampMC.h LArRawConditions/LArShapeComplete.h LArRawConditions/LArShape32MC.h LArRawConditions/LArTdriftComplete.h LArRawConditions/LAruA2MeVComplete.h LArRawConditions/LAruA2MeVMC.h LArRawConditions/LArWFParamsContainer.h LArRawConditions/LArOFCBinComplete.h LArRawConditions/LArRinjComplete.h LArRawConditions/LArCableLengthComplete.h LArRawConditions/LArCableAttenuationComplete.h LArRawConditions/LArTshaperComplete.h LArRawConditions/LArEMEC_CphiComplete.h LArRawConditions/LArEMEC_HValphaComplete.h LArRawConditions/LArEMEC_HVbetaComplete.h LArRawConditions/LArCaliWaveContainer.h LArRawConditions/LArPhysWaveContainer.h LArRawConditions/LArHVScaleCorrComplete.h LArRawConditions/LArDSPThresholdsComplete.h LArRawConditions/LArFEBTimeOffset.h LArRawConditions/LArCalibParams.h LArRawConditions/LArGlobalTimeOffset.h 
                           MULT_CHAN_TYPES LArAutoCorrComplete LArAutoCorrMC LArCaliPulseParamsComplete LArDAC2uAComplete LArDAC2uAMC LArDetCellParamsComplete LArfSamplComplete LArfSamplMC LArH6OscillationComplete LArMinBiasComplete LArMinBiasMC LArMinBiasAverageComplete LArMinBiasAverageMC LArMphysOverMcalComplete LArMphysOverMcalMC LArNoiseComplete LArNoiseMC LArOFCComplete LArPedestalComplete LArPhysCaliTdiffComplete LArRampComplete LArRampMC LArShapeComplete LArShape32MC LArTdriftComplete LAruA2MeVComplete LAruA2MeVMC LArOFCBinComplete LArRinjComplete LArCableLengthComplete LArCableAttenuationComplete LArTshaperComplete LArEMEC_CphiComplete LArEMEC_HValphaComplete LArEMEC_HVbetaComplete LArCaliWaveContainer LArPhysWaveContainer LArHVScaleCorrComplete LArDSPThresholdsComplete
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )


atlas_add_test( LArAutoCorrCompleteCnv_test
                SOURCES
                test/LArAutoCorrCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArCaliWaveContainerCnv_test
                SOURCES
                test/LArCaliWaveContainerCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArOFCompleteCnv_test
                SOURCES
                test/LArOFCCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArPedestalCompleteCnv_test
                SOURCES
                test/LArPedestalCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArPhysWaveContainerCnv_test
                SOURCES
                test/LArPhysWaveContainerCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArRampCompleteCnv_test
                SOURCES
                test/LArRampCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArShapeCompleteCnv_test
                SOURCES
                test/LArShapeCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArDAC2uACompleteCnv_test
                SOURCES
                test/LArDAC2uACompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArDAC2uAMCCnv_test
                SOURCES
                test/LArDAC2uAMCCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArMphysOverMcalCompleteCnv_test
                SOURCES
                test/LArMphysOverMcalCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LArMphysOverMcalMCCnv_test
                SOURCES
                test/LArMphysOverMcalMCCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LAruA2MeVCompleteCnv_test
                SOURCES
                test/LAruA2MeVCompleteCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )

atlas_add_test( LAruA2MeVMCCnv_test
                SOURCES
                test/LAruA2MeVMCCnv_test.cxx
                LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions TestTools )
