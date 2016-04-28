/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/****************************************************************
   HGTD Sensitive Detector class
   Jalal Abdallah Oct 28th 2015.
 ****************************************************************/

#ifndef HGTDG4SD_HGTDSENSORSDTOOL_H
#define HGTDG4SD_HGTDSENSORSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

// for the LArHit
#include "StoreGate/WriteHandle.h"
#include "LArSimEvent/LArHitContainer.h"

// STL headers
#include <string>

class G4VSensitiveDetector;
class HGTDSensorSD;
class HGTD_ID;

class HGTDSensorSDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  HGTDSensorSDTool(const std::string& type, const std::string& name, const IInterface *parent);

  StatusCode initialize() override final;

  // Calls down to all the SDs to get them to pack their hits into a central collection
  StatusCode Gather() override final;
  
  // Make me an SD!
  G4VSensitiveDetector* makeSD() override final;

  // Destructor
  virtual ~HGTDSensorSDTool() {}

  void setupHelpers( HGTDSensorSD* ) const;

 protected:
  
  void setupAllSDs(std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration) const;
  bool match(const char *first, const char * second) const;

 private:
  // The actual hit container - here because the base class is for both calib and standard SD tools
  SG::WriteHandle<LArHitContainer> m_LArHitColl;
  
  // what type of treatment for the timing
  std::string m_timeBinType;
  
  // What time bin width for regular hits?
  double m_timeBinWidth;

  // Define the Time Window for the Hits 
  double m_timeWindowMin;
  double m_timeWindowMax;

  // the actual sensitive detector
  HGTDSensorSD* m_hgtdSD;

  // the helper for the HGTD Identifiers
  const HGTD_ID*      m_hgtdID;

};

#endif //HGTDG4SD_HGTDSENSORSDTOOL_H
