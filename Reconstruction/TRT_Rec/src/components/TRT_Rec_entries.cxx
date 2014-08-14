/***************************************************************************
 TRT_Rec
 ATLAS Collaboration
 ***************************************************************************/

// $Id: TRT_Rec_entries.cxx,v 1.5 2006-10-11 10:45:02 pop Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_Rec/TRT_DataSelector.h"
#include "TRT_Rec/TRT_Histogrammer.h"
#include "TRT_Rec/TRT_Predictor.h"
#include "TRT_Rec/TRT_Recon.h"

DECLARE_TOOL_FACTORY( TRT_DataSelector )
DECLARE_TOOL_FACTORY( TRT_Histogrammer )
DECLARE_TOOL_FACTORY( TRT_Predictor )
DECLARE_TOOL_FACTORY( TRT_Recon )

DECLARE_FACTORY_ENTRIES( TRT_Rec ) 
{
    DECLARE_TOOL( TRT_DataSelector )
    DECLARE_TOOL( TRT_Histogrammer )
    DECLARE_TOOL( TRT_Predictor )	
    DECLARE_TOOL( TRT_Recon )
}

