#include "GaudiKernel/DeclareFactoryEntries.h"
#include "MuidCaloEnergyTools/MuidCaloEnergyTool.h"
#include "MuidCaloEnergyTools/MuidCaloEnergyMeas.h"
#include "MuidCaloEnergyTools/MuidCaloEnergyParam.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidCaloEnergyTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidCaloEnergyMeas )
DECLARE_NAMESPACE_TOOL_FACTORY( Rec, MuidCaloEnergyParam )

DECLARE_FACTORY_ENTRIES( MuidCaloEnergyTools )
{
    DECLARE_NAMESPACE_TOOL( Rec, MuidCaloEnergyTool )
    DECLARE_NAMESPACE_TOOL( Rec, MuidCaloEnergyMeas )
    DECLARE_NAMESPACE_TOOL( Rec, MuidCaloEnergyParam )
}

