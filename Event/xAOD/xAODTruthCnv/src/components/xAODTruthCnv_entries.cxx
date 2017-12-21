#include "../xAODTruthCnvAlg.h"
#include "../RedoTruthLinksAlg.h"
#include "xAODTruthCnv/TruthMetaDataTool.h"
#include "../xAODTruthReader.h"
#include "../HepMCTruthReader.h"

DECLARE_COMPONENT( xAODMaker::xAODTruthCnvAlg )
DECLARE_COMPONENT( xAODMaker::RedoTruthLinksAlg )
DECLARE_COMPONENT( xAODMaker::TruthMetaDataTool )
DECLARE_COMPONENT( xAODReader::xAODTruthReader )
DECLARE_COMPONENT( HepMCTruthReader )

