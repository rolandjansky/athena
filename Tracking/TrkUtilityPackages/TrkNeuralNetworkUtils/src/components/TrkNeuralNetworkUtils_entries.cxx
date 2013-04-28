#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkNeuralNetworkUtils/NeuralNetworkToHistoTool.h"

using namespace Trk ;

DECLARE_NAMESPACE_TOOL_FACTORY( Trk, NeuralNetworkToHistoTool )

/** factory entries need to have the name of the package */
DECLARE_FACTORY_ENTRIES( TrkNeuralNetworkUtils )
{
  DECLARE_NAMESPACE_TOOL( Trk, NeuralNetworkToHistoTool );
}
