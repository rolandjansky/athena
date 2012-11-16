/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArHitMerger class used by LArG4SD and FastSimulation to merge
// LArHits in the same time bins

#ifndef __LArHitMerger_H__
#define __LArHitMerger_H__

#include "LArSimEvent/LArHit.h"
#include "LArG4Code/LArVHitMerger.h"
#include "LArG4HitManagement/LArHitsEventAction.h"

#include "G4Types.hh"

#include <set>
#include <map>
#include <string>

class LArHitMerger : public LArVHitMerger
{
public:

  enum LArHitTimeBins
  {
    HitTimeBinDefault = 0,
    HitTimeBinUniform = 1
  };

  // Constructors and destructors.
  LArHitMerger(StoreGateSvc* detStore,
	       LArHitsEventAction* action,
	       std::string timeBinType,
	       G4float timeBinWidth);    // This has some effect for the uniform binning only

  virtual ~LArHitMerger();

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
  // Pointer to the action object which holds global collections
  LArHitsEventAction* m_action;

  // Time binning type
  LArHitMerger::LArHitTimeBins m_timeBinType;

  // Get the time bin
  G4int getTimeBin(G4double time);

  // Width of the time bins for summing hits - for the uniform binning
  G4float m_timeBinWidth;

  timeBins_t                          m_timeBins;
};

#endif // _LArHitMerger_H_
