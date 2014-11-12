/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArCalibHitMerger class used by LArG4CalibSD to merge CaloCalibrationHit-s

#ifndef __LArCalibHitMerger_H__
#define __LArCalibHitMerger_H__

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "LArG4Code/LArVCalibHitMerger.h"
#include "LArG4HitManagement/LArHitsEventAction.h"

#include "G4Types.hh"

#include <set>
#include <vector>

class LArCalibHitMerger : public LArVCalibHitMerger
{
public:

  // Constructors and destructors.
  LArCalibHitMerger(StoreGateSvc* detStore,
		    LArHitsEventAction* action);

  virtual ~LArCalibHitMerger();

  // Clean up at the beginning of event
  void BeginOfEvent();

  // Do the actual job here
  bool process(const LArG4Identifier& ident,
               const std::vector<G4double>& energies);

  // Do the actual job here
  bool process(const LArG4Identifier& ident,
               const std::vector<G4double>& energies, 
	       unsigned int particleID);

  // Distribute hits created during the event between global containers
  void EndOfEvent(std::string detectorName);

  // Clear the map
  void clear();

  // We need a container for the hits.  "calibrationHits" is used to
  // tell us if we've already had a hit in a cell.  We store these
  // hits in a set, so we can quickly search it.  Note the use of a
  // custom definition of a "less" function for the set, so we're
  // not just comparing hit pointers.
 
  class LessHit {
  public:
    bool operator() ( CaloCalibrationHit* const& p, CaloCalibrationHit* const& q ) const
      {
	return p->Less(q);
      }
  };

  typedef std::set< CaloCalibrationHit*, LessHit >  m_calibrationHits_t;
  typedef m_calibrationHits_t::iterator             m_calibrationHits_ptr_t;
  

private:
  // Pointer to the action object which holds global collections
  LArHitsEventAction* m_action;

  m_calibrationHits_t                               m_calibrationHits;

};

#endif // _LArCalibHitMerger_H_
