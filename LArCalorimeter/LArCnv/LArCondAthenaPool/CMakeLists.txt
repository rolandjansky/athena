################################################################################
# Package: LArCondAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( LArCondAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          LArCalorimeter/LArCnv/LArCondTPCnv
                          LArCalorimeter/LArRawConditions )

# Component(s) in the package:
atlas_add_poolcnv_library( LArCondAthenaPoolPoolCnv
                           src/*.cxx
                           FILES LArRawConditions/LArAutoCorrComplete.h LArRawConditions/LArAutoCorrMC.h LArRawConditions/LArCaliPulseParamsComplete.h LArRawConditions/LArDAC2uAComplete.h LArRawConditions/LArDAC2uAMC.h LArRawConditions/LArDetCellParamsComplete.h LArRawConditions/LArfSamplComplete.h LArRawConditions/LArfSamplMC.h LArRawConditions/LArH6OscillationComplete.h LArRawConditions/LArMinBiasComplete.h LArRawConditions/LArMinBiasMC.h LArRawConditions/LArMinBiasAverageComplete.h LArRawConditions/LArMinBiasAverageMC.h LArRawConditions/LArMphysOverMcalComplete.h LArRawConditions/LArMphysOverMcalMC.h LArRawConditions/LArNoiseComplete.h LArRawConditions/LArNoiseMC.h LArRawConditions/LArOFCComplete.h LArRawConditions/LArOFCMC.h LArRawConditions/LArPedestalComplete.h LArRawConditions/LArPedestalMC.h LArRawConditions/LArPhysCaliTdiffComplete.h LArRawConditions/LArRampComplete.h LArRawConditions/LArRampMC.h LArRawConditions/LArShapeComplete.h LArRawConditions/LArShapeMC.h LArRawConditions/LArShape32MC.h LArRawConditions/LArTdriftComplete.h LArRawConditions/LAruA2MeVComplete.h LArRawConditions/LAruA2MeVMC.h LArRawConditions/LArWFParamsContainer.h LArRawConditions/LArOFCBinComplete.h LArRawConditions/LArRinjComplete.h LArRawConditions/LArCableLengthComplete.h LArRawConditions/LArCableAttenuationComplete.h LArRawConditions/LArTshaperComplete.h LArRawConditions/LArEMEC_CphiComplete.h LArRawConditions/LArEMEC_HValphaComplete.h LArRawConditions/LArEMEC_HVbetaComplete.h LArRawConditions/LArCaliWaveContainer.h LArRawConditions/LArPhysWaveContainer.h LArRawConditions/LArHVScaleCorrComplete.h LArRawConditions/LArDSPThresholdsComplete.h LArRawConditions/LArFEBTimeOffset.h LArRawConditions/LArCalibParams.h LArRawConditions/LArGlobalTimeOffset.h 
                           MULT_CHAN_TYPES LArAutoCorrComplete LArAutoCorrMC LArCaliPulseParamsComplete LArDAC2uAComplete LArDAC2uAMC LArDetCellParamsComplete LArfSamplComplete LArfSamplMC LArH6OscillationComplete LArMinBiasComplete LArMinBiasMC LArMinBiasAverageComplete LArMinBiasAverageMC LArMphysOverMcalComplete LArMphysOverMcalMC LArNoiseComplete LArNoiseMC LArOFCComplete LArOFCMC LArPedestalComplete LArPhysCaliTdiffComplete LArRampComplete LArRampMC LArShapeComplete LArShapeMC LArShape32MC LArTdriftComplete LAruA2MeVComplete LAruA2MeVMC LArOFCBinComplete LArRinjComplete LArCableLengthComplete LArCableAttenuationComplete LArTshaperComplete LArEMEC_CphiComplete LArEMEC_HValphaComplete LArEMEC_HVbetaComplete LArCaliWaveContainer LArPhysWaveContainer LArHVScaleCorrComplete LArDSPThresholdsComplete
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib LArCondTPCnv LArRawConditions )

# Install files from the package:
atlas_install_joboptions( share/*.py )

