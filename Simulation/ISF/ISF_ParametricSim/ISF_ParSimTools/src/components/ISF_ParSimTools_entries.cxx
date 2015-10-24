#include "GaudiKernel/DeclareFactoryEntries.h"
#include "ISF_ParSimTools/TrackParticleSmearer.h"
#include "ISF_ParSimTools/DefaultSmearer.h"
#include "ISF_ParSimTools/MuonSmearer.h"
#include "ISF_ParSimTools/ElectronSmearer.h"
#include "ISF_ParSimTools/PionSmearer.h"
#include "ISF_ParSimTools/ISPtoPerigeeTool.h"



DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , DefaultSmearer       )
DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , MuonSmearer          )
DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , ElectronSmearer      )
DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , PionSmearer          )
DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , TrackParticleSmearer )
DECLARE_NAMESPACE_TOOL_FACTORY( iParSim , ISPtoPerigeeTool	   )



DECLARE_FACTORY_ENTRIES( ISF_ParSimTools ) {
  DECLARE_NAMESPACE_TOOL( iParSim ,  DefaultSmearer       )
  DECLARE_NAMESPACE_TOOL( iParSim ,  MuonSmearer          )
  DECLARE_NAMESPACE_TOOL( iParSim ,  ElectronSmearer      )
  DECLARE_NAMESPACE_TOOL( iParSim ,  PionSmearer          )
  DECLARE_NAMESPACE_TOOL( iParSim ,  TrackParticleSmearer )
  DECLARE_NAMESPACE_TOOL( iParSim ,  ISPtoPerigeeTool     )

}


