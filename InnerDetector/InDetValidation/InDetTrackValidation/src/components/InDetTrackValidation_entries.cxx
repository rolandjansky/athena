
#include "InDetTrackValidation/TRT_DriftTimeNtupleHelper.h"
#include "InDetTrackValidation/SCT_ClusterValidationNtupleWriter.h"
#include "InDetTrackValidation/PixelClusterValidationNtupleWriter.h"
#include "InDetTrackValidation/TRT_DriftCircleValidationNtupleWriter.h"
#include "InDetTrackValidation/SiResidualValidationNtupleHelper.h"
#include "InDetTrackValidation/InDetSplittedTracksCreator.h"
#include "InDetTrackValidation/HoleSearchValidation.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

//using namespace InDet;

DECLARE_NAMESPACE_TOOL_FACTORY( InDet, TRT_DriftTimeNtupleHelper )
DECLARE_NAMESPACE_TOOL_FACTORY( InDet, SiResidualValidationNtupleHelper )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, SCT_ClusterValidationNtupleWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, PixelClusterValidationNtupleWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, TRT_DriftCircleValidationNtupleWriter )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, InDetSplittedTracksCreator )
DECLARE_NAMESPACE_ALGORITHM_FACTORY( InDet, HoleSearchValidation )

DECLARE_FACTORY_ENTRIES(InDetTrackValidation)
{ // in this package name these methods are registered 
    DECLARE_NAMESPACE_TOOL(InDet, TRT_DriftTimeNtupleHelper)
    DECLARE_NAMESPACE_TOOL(InDet, SiResidualValidationNtupleHelper)
    DECLARE_NAMESPACE_ALGORITHM( InDet, SCT_ClusterValidationNtupleWriter)
    DECLARE_NAMESPACE_ALGORITHM( InDet, PixelClusterValidationNtupleWriter)    
    DECLARE_NAMESPACE_ALGORITHM( InDet, TRT_DriftCircleValidationNtupleWriter)
    DECLARE_NAMESPACE_ALGORITHM( InDet, InDetSplittedTracksCreator )
    DECLARE_NAMESPACE_ALGORITHM( InDet, HoleSearchValidation )
}

