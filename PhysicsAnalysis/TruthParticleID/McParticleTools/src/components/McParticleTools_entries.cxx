#include "../TruthParticleCnvTool.h"
#include "../McVtxFilterTool.h"

#include "../NoopFilterTool.h"
#include "../OldSpclMcFilterTool.h"
#include "../PileupFilterTool.h"
#include "../EtaPtFilterTool.h"
#include "../VtxBasedFilterTool.h"

#include "../TruthIsolationTool.h"

#include "../GenAodValidationTool.h"
#include "../SpclMcValidationTool.h"

#include "../HepMcTupleWriterTool.h"
#include "../HepMcFloatWriterTool.h"
#include "../HepMcWriterTool.h"
#include "../HepMcReaderTool.h"
#include "../McAodWriterTool.h"
#include "../McAodTupleWriterTool.h"

DECLARE_COMPONENT( TruthParticleCnvTool )
DECLARE_COMPONENT( McVtxFilterTool )
		      
DECLARE_COMPONENT( NoopFilterTool )
DECLARE_COMPONENT( OldSpclMcFilterTool )
DECLARE_COMPONENT( PileupFilterTool )
DECLARE_COMPONENT( EtaPtFilterTool )
DECLARE_COMPONENT( VtxBasedFilterTool )

DECLARE_COMPONENT( TruthIsolationTool )

DECLARE_COMPONENT( GenAodValidationTool )
DECLARE_COMPONENT( SpclMcValidationTool )

DECLARE_COMPONENT( HepMcTupleWriterTool )
DECLARE_COMPONENT( HepMcFloatWriterTool )
DECLARE_COMPONENT( HepMcWriterTool )
DECLARE_COMPONENT( HepMcReaderTool )
DECLARE_COMPONENT( McAodWriterTool )
DECLARE_COMPONENT( McAodTupleWriterTool )

