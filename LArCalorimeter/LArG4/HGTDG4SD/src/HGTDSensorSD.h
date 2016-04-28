/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   HGTD Sensitive Detector class
   Jalal Abdallah Oct 28th 2015.
 ****************************************************************/

#ifndef HGTDG4SD_HGTDSENSORSD_H
#define HGTDG4SD_HGTDSENSORSD_H

// Base class
#include "G4VSensitiveDetector.hh"

// use of the hits
#include "StoreGate/WriteHandle.h"

//new LAr stuff
#include "CaloIdentifier/HGTD_ID.h"
#include "LArSimEvent/LArHit.h"

// G4 needed classes
class G4Step;
class G4HCofThisEvent;
class LArHitContainer;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....

class HGTDSensorSD : public G4VSensitiveDetector
{
 public:
  // Constructor
  HGTDSensorSD(const std::string& name, std::string , double, double, double);

  // Destructor
  virtual ~HGTDSensorSD() { /* I don't own myHitColl if all has gone well */ }

  // Process the hits from G4
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

  // Initialize from G4
  void Initialize(G4HCofThisEvent *) override final;

  // called by the tool to gather everything
  void EndOfAthenaEvent( LArHitContainer * hitContainer );

  void setupHelpers( const HGTD_ID* HGTD ) {
    m_hgtdID = HGTD;
  }

  double getTruncatedTime(double time) const;
  double getMinSubtractedMaxTruncatedTime(double time) const;
  G4bool SimpleHit(Identifier id , G4double time , G4double energy );
  G4int getTimeBin(G4double time) const;


 private:

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
  // The hits are grouped into time bins, with the width of a bin
  // determined by a user parameter.  This map is used to associate a
  // time bin with its corresponding set of hits.
  typedef std::map < G4int, hits_t* >   timeBins_t;
  timeBins_t m_timeBins;

  const HGTD_ID*      m_hgtdID;

  std::string m_timeBinType;
  double      m_timeBinWidth;
  double      m_timeWindowMin;
  double      m_timeWindowMax;

};

#endif //HGTDG4SD_HGTDSENSORSD_H
