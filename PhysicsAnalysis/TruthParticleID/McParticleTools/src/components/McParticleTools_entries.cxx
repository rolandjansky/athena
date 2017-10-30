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

DECLARE_TOOL_FACTORY( TruthParticleCnvTool )
DECLARE_TOOL_FACTORY( McVtxFilterTool )
		      
DECLARE_TOOL_FACTORY( NoopFilterTool )
DECLARE_TOOL_FACTORY( OldSpclMcFilterTool )
DECLARE_TOOL_FACTORY( PileupFilterTool )
DECLARE_TOOL_FACTORY( EtaPtFilterTool )
DECLARE_TOOL_FACTORY( VtxBasedFilterTool )

DECLARE_TOOL_FACTORY( TruthIsolationTool )

DECLARE_TOOL_FACTORY( GenAodValidationTool )
DECLARE_TOOL_FACTORY( SpclMcValidationTool )

DECLARE_TOOL_FACTORY( HepMcTupleWriterTool )
DECLARE_TOOL_FACTORY( HepMcFloatWriterTool )
DECLARE_TOOL_FACTORY( HepMcWriterTool )
DECLARE_TOOL_FACTORY( HepMcReaderTool )
DECLARE_TOOL_FACTORY( McAodWriterTool )
DECLARE_TOOL_FACTORY( McAodTupleWriterTool )

