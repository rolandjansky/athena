#ifndef XAOD_ANALYSIS
#include "../HIClusterMaker.h"
#include "../HIJetCellSubtractorTool.h"
#include "../HIClusterSubtraction.h"
#include "../HISubtractedCellMakerTool.h"
#endif

#include "HIJetRec/HIEventShapeJetIteration.h"
#include "HIJetRec/HIJetClusterSubtractorTool.h"
#include "HIJetRec/HIJetConstituentSubtractionTool.h"
#include "HIJetRec/HIJetDRAssociationTool.h"
#include "HIJetRec/HIJetMaxOverMeanTool.h"
#include "HIJetRec/HIJetDiscriminatorTool.h"
#include "HIJetRec/HIJetSignificanceTool.h"
#include "HIJetRec/HIUEModulatorTool.h"
#include "HIJetRec/HIClusterPseudoJetGetter.h"

#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( HIClusterMaker )
DECLARE_COMPONENT( HIJetCellSubtractorTool )
DECLARE_COMPONENT( HIClusterSubtraction )
DECLARE_COMPONENT( HISubtractedCellMakerTool )
#endif


DECLARE_COMPONENT( HIEventShapeJetIteration )
DECLARE_COMPONENT( HIJetConstituentSubtractionTool )
DECLARE_COMPONENT( HIJetClusterSubtractorTool )
DECLARE_COMPONENT( HIJetDRAssociationTool )
DECLARE_COMPONENT( HIJetMaxOverMeanTool )
DECLARE_COMPONENT( HIJetDiscriminatorTool )
DECLARE_COMPONENT( HIJetSignificanceTool )
DECLARE_COMPONENT( HIUEModulatorTool )
DECLARE_COMPONENT( HIClusterPseudoJetGetter )

