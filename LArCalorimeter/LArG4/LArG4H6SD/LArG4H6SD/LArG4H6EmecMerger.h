/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4H6EmecMerger class used by LArG4SD and FastSimulation to merge
// LArHits in the same time bins

#ifndef __LArG4H6EmecMerger_H__
#define __LArG4H6EmecMerger_H__

#include "LArSimEvent/LArHit.h"
#include "LArG4Code/LArVHitMerger.h"

#include "G4Types.hh"

#include <set>
#include <map>

class LArG4H6EmecMerger : public LArVHitMerger
{
public:

  // Constructors and destructors.
  LArG4H6EmecMerger(StoreGateSvc* detStore,
	       G4float timeBinWidth);

  virtual ~LArG4H6EmecMerger();

  // Clean up at the beginning of event
  void BeginOfEvent();

  // Do the actual job here
  bool process(G4Step* step,
	       LArG4Identifier ident,
	       G4double time,
	       G4double energy);

  // Distribute hits created during the event between global containers
  void EndOfEvent();

  // Clear the map
  void clear();

  // We need two types containers for hits:

  // The set defined below is used to tell us if we've already had a
  // hit in a cell.  We store these hits in a set, so we can quickly
  // search it.  Note the use of a custom definition of a "less"
  // function for the set, so we're not just comparing hit pointers.

  class LessHit {
  public:
    bool operator() ( LArHit* const& p, LArHit* const& q ) const
    {
      return p->Less(q);
    }
  };
  
  typedef std::set< LArHit*, LessHit >  hits_t;
  typedef hits_t::iterator              hits_pointer;
  
  // The hits are grouped into time bins, with the width of a bin
  // determined by a user parameter.  This map is used to associate a
  // time bin with its corresponding set of hits.
  
  typedef std::map < G4int, hits_t* >   timeBins_t;
  typedef timeBins_t::iterator          timeBins_pointer;
  typedef timeBins_t::const_iterator    timeBins_const_pointer;

private:

  // Width of the time bins for summing hits:
  G4float m_timeBinWidth;

  timeBins_t                          m_timeBins;

};

#endif // _LArG4H6EmecMerger_H_
