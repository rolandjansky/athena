/***************************************************************************
 ATLAS Collaboration
 ***************************************************************************/

// $Id: TrkAmbiguitySolver_entries.cxx,v 1.1.1.1 2007-02-21 13:36:44 msiebel Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrkAmbiguitySolver/TrkAmbiguitySolver.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY( Trk, TrkAmbiguitySolver )

DECLARE_FACTORY_ENTRIES( TrkAmbiguitySolver )
{
	DECLARE_NAMESPACE_ALGORITHM( Trk, TrkAmbiguitySolver );
}





