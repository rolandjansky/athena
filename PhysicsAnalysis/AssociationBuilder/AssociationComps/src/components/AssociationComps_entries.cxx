#include "../AssocBuilder.h"
#include "../AssocDumper.h"
#include "../DeltaROverlapTool.h"
#include "../AssocBuilderTool.h"
#include "../DeltaRAssociationTool.h"
#include "../AthenaBarCodeAssociationTool.h"
#include "../TriggerAssociationTool.h"
#include "../AssociationAlgorithm.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( AssocBuilder )
DECLARE_ALGORITHM_FACTORY( AssocDumper )
DECLARE_TOOL_FACTORY( DeltaROverlapTool )
DECLARE_TOOL_FACTORY( AssocBuilderTool )
DECLARE_TOOL_FACTORY( DeltaRAssociationTool )
DECLARE_TOOL_FACTORY( AthenaBarCodeAssociationTool )
DECLARE_TOOL_FACTORY( TriggerAssociationTool )
DECLARE_ALGORITHM_FACTORY( AssociationAlgorithm )


DECLARE_FACTORY_ENTRIES(AssociationComps) {
  DECLARE_ALGORITHM( AssocBuilder )
  DECLARE_ALGORITHM( AssocDumper )
  DECLARE_TOOL( DeltaROverlapTool )
  DECLARE_TOOL( AssocBuilderTool )
  DECLARE_TOOL( DeltaRAssociationTool )
  DECLARE_TOOL( AthenaBarCodeAssociationTool )
  DECLARE_TOOL( TriggerAssociationTool )
  DECLARE_ALGORITHM( AssociationAlgorithm )
}
