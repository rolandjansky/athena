################################################################################
# Package: ISF_AthenaPool
################################################################################

# Declare the package name:
atlas_subdir( ISF_AthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( ISF_AthenaPoolPoolCnv
                           src/*.cxx
                           FILES ISF_FastCaloSimEvent/FCS_StepInfoCollection.h
                           TYPES_WITH_NAMESPACE ISF_FCS_Parametrization::FCS_StepInfo ISF_FCS_Parametrization::FCS_StepInfoCollection
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib GaudiKernel ISF_FastCaloSimEvent )

