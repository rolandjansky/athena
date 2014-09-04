#include "GaudiKernel/DeclareFactoryEntries.h"
#include "AnalysisUtils/KinematicSelector.h"
#include "AnalysisUtils/ElectronIDSelector.h"
#include "AnalysisUtils/ISelector.h"
#include "AnalysisUtils/ContainerFilter.h"
#include "AnalysisUtils/ParticleBaseSelector.h"
#include "AnalysisUtils/IParticleSelector.h"

DECLARE_TOOL_FACTORY( KinematicSelector )
DECLARE_TOOL_FACTORY( ElectronIDSelector )
DECLARE_TOOL_FACTORY( ISelector )
DECLARE_TOOL_FACTORY( ContainerFilter )
DECLARE_ALGORITHM_FACTORY( ParticleBaseSelector )
DECLARE_ALGORITHM_FACTORY( IParticleSelector )

DECLARE_FACTORY_ENTRIES( AnalysisUtils ) {
  DECLARE_ALGTOOL( KinematicSelector )
  DECLARE_ALGTOOL( ElectronIDSelector )
  DECLARE_ALGTOOL( ISelector )
  DECLARE_ALGTOOL( ContainerFilter )
  DECLARE_ALGORITHM( ParticleBaseSelector )
  DECLARE_ALGORITHM( IParticleSelector )

}
