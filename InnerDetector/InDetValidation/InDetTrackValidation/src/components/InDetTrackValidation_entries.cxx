
#include "InDetTrackValidation/SCT_ClusterValidationNtupleWriter.h"
#include "InDetTrackValidation/InDetSplittedTracksCreator.h"
#include "InDetTrackValidation/HoleSearchValidation.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

//using namespace InDet;

DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, SCT_ClusterValidationNtupleWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetSplittedTracksCreator )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, HoleSearchValidation )

DECLARE_FACTORY_ENTRIES(InDetTrackValidation)
{ // in this package name these methods are registered 
    DECLARE_NAMESPACE_ALGORITHM( InDet, SCT_ClusterValidationNtupleWriter)
    DECLARE_NAMESPACE_ALGORITHM( InDet, InDetSplittedTracksCreator )
    DECLARE_NAMESPACE_ALGORITHM( InDet, HoleSearchValidation )
}

