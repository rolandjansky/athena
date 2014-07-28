#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtx.h"
#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtxAllTE.h"
#include "TrigT2HistoPrmVtx/TrigT2HistoPrmVtxCombo.h"

DECLARE_ALGORITHM_FACTORY( TrigT2HistoPrmVtx )
DECLARE_ALGORITHM_FACTORY( TrigT2HistoPrmVtxAllTE )
DECLARE_ALGORITHM_FACTORY( TrigT2HistoPrmVtxCombo )

DECLARE_FACTORY_ENTRIES( TrigT2HistoPrmVtx ) { 

    DECLARE_ALGORITHM( TrigT2HistoPrmVtx )
    DECLARE_ALGORITHM( TrigT2HistoPrmVtxAllTE )
    DECLARE_ALGORITHM( TrigT2HistoPrmVtxCombo )

}
