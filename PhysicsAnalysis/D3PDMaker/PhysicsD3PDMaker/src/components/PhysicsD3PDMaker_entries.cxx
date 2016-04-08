/******************************************************************************
 *   Entries file for SMEWTrilepSkim
 *
 *   @author Steven Kaneti <StevenKanetiSPAMNOTcern.ch>
 *           Nick Edwards
 *
 *   @version $Id: PhysicsD3PDMaker_entries.cxx 606012 2014-07-11 09:40:36Z krasznaa $
 *
 *****************************************************************************/

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "../TrileptonSkimmer.h"

DECLARE_ALGORITHM_FACTORY(TrileptonSkimmer)

DECLARE_FACTORY_ENTRIES(PhysicsD3PDMaker) {
  DECLARE_ALGORITHM(TrileptonSkimmer)
}
