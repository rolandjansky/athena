#include "AnalysisJiveXML/BJetRetriever.h"
#include "AnalysisJiveXML/ElectronRetriever.h"
#include "AnalysisJiveXML/MuonRetriever.h"
#include "AnalysisJiveXML/AODJetRetriever.h"
#include "AnalysisJiveXML/PhotonRetriever.h"
#include "AnalysisJiveXML/TauJetRetriever.h"
#include "AnalysisJiveXML/TrackParticleRetriever.h"
#include "AnalysisJiveXML/AODCaloClusterRetriever.h"
#include "AnalysisJiveXML/CompositeParticleRetriever.h"
#include "AnalysisJiveXML/TruthParticleRetriever.h"
#include "AnalysisJiveXML/TrigEMClusterRetriever.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

using namespace JiveXML;

DECLARE_TOOL_FACTORY(BJetRetriever)
DECLARE_TOOL_FACTORY(ElectronRetriever)
DECLARE_TOOL_FACTORY(MuonRetriever)
DECLARE_TOOL_FACTORY(AODJetRetriever)
DECLARE_TOOL_FACTORY(PhotonRetriever)
DECLARE_TOOL_FACTORY(TauJetRetriever)
DECLARE_TOOL_FACTORY(TrackParticleRetriever)
DECLARE_TOOL_FACTORY(AODCaloClusterRetriever)
DECLARE_TOOL_FACTORY(CompositeParticleRetriever)
DECLARE_TOOL_FACTORY(TruthParticleRetriever)
DECLARE_TOOL_FACTORY(TrigEMClusterRetriever)

DECLARE_FACTORY_ENTRIES(AnalysisJiveXML) {
  DECLARE_ALGTOOL(BJetRetriever)
  DECLARE_ALGTOOL(ElectronRetriever)
  DECLARE_ALGTOOL(MuonRetriever)
  DECLARE_ALGTOOL(AODJetRetriever)
  DECLARE_ALGTOOL(PhotonRetriever)
  DECLARE_ALGTOOL(TauJetRetriever)
  DECLARE_ALGTOOL(TrackParticleRetriever)
  DECLARE_ALGTOOL(AODCaloClusterRetriever)
  DECLARE_ALGTOOL(CompositeParticleRetriever)
  DECLARE_ALGTOOL(TruthParticleRetriever)
  DECLARE_ALGTOOL(TrigEMClusterRetriever)
}
