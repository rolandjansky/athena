/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Converter from TrigInDetTrackTruthMap_old to TrigInDetTrackTruthMap.
// $Id: TrigInDetTrackTruthMap_old_cnv.cxx,v 1.2 2009-02-11 02:33:13 ssnyder Exp $

#include "TrigTruthEventTPCnv/TrigInDetTrackTruthMap_old_cnv.h"

void
TrigInDetTrackTruthMap_old_cnv::Convert (TrigInDetTrackTruthMap* newobj,
                                         const TrigInDetTrackTruthMap_old*
                                            oldobj)
{
  newobj->m_truth_vec = oldobj->m_truth_vec;
  newobj->m_elink_vec.clear();
  for (size_t i=0; i < oldobj->m_elink_vec.size(); i++) {
    // Note: this was originally a push_back of m_elink_vec[i] directly.
    // This didn't work though.  The ELs in the vector are default-constructed;
    // then the data member is overwritten by root.  Thus, the store
    // pointer doesn't get set.  (Note that ELStreamer won't do that any more.)
    // If we explicitly construct a new EL here, the store pointer will
    // get set correctly.
    //
    // Could use a push_back with emplacement here...
    const ElementLink<TrigInDetTrackCollection>& el0= oldobj->m_elink_vec[i];
    ElementLink<TrigInDetTrackCollection> el (el0.key(), el0.index());
    newobj->m_elink_vec.push_back (el);
  }
}
