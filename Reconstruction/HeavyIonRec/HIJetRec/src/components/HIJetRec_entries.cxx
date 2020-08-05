#ifndef XAOD_ANALYSIS
#include "../HIClusterMaker.h"
#include "../HIJetCellSubtractorTool.h"
#include "../HIClusterSubtraction.h"
#include "../HISubtractedCellMakerTool.h"
#endif

#include "../../Root/HIEventShapeJetIteration.h"
#include "../../Root/HIJetClusterSubtractorTool.h"
#include "../../Root/HIJetConstituentSubtractionTool.h"
#include "../../Root/HIJetConstituentModifierTool.h"
#include "../../Root/HIJetDRAssociationTool.h"
#include "../../Root/HIJetMaxOverMeanTool.h"
#include "../../Root/HIJetDiscriminatorTool.h"
#include "../../Root/HIJetSignificanceTool.h"
#include "../../Root/HIUEModulatorTool.h"

#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( HIClusterMaker )
DECLARE_COMPONENT( HIJetCellSubtractorTool )
DECLARE_COMPONENT( HIClusterSubtraction )
DECLARE_COMPONENT( HISubtractedCellMakerTool )
#endif

DECLARE_COMPONENT( HIJetConstituentModifierTool )
DECLARE_COMPONENT( HIEventShapeJetIteration )
DECLARE_COMPONENT( HIJetConstituentSubtractionTool )
DECLARE_COMPONENT( HIJetClusterSubtractorTool )
DECLARE_COMPONENT( HIJetDRAssociationTool )
DECLARE_COMPONENT( HIJetMaxOverMeanTool )
DECLARE_COMPONENT( HIJetDiscriminatorTool )
DECLARE_COMPONENT( HIJetSignificanceTool )
DECLARE_COMPONENT( HIUEModulatorTool )
