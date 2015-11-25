/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4BARRELSDTOOL_H
#define LARG4BARRELSDTOOL_H

// Base class
#include "G4AtlasTools/SensitiveDetectorBase.h"

#include <string>
#include <vector>
class G4VSensitiveDetector;

// For the identifier helpers
class LArEM_ID;
class LArFCAL_ID;
class LArHEC_ID;
class LArMiniFCAL_ID;
class CaloDM_ID;
class LArG4SimpleSD;
class LArG4CalibSD;

class LArG4SDTool : public SensitiveDetectorBase
{
 public:
  // Constructor
  LArG4SDTool(const std::string& type, const std::string& name, const IInterface *parent);
    
  // Destructor
  virtual ~LArG4SDTool() {}

  StatusCode initialize() override final;

  void setupHelpers( LArG4SimpleSD* ) const;
  void setupHelpers( LArG4CalibSD* ) const;

 protected:

  G4VSensitiveDetector* makeSD() override final { return nullptr; }

  // Are we set up to run with PID hits?
  G4bool m_doPID;
  // What time binning type for regular hits?
  std::string m_timeBinType;
  // What time bin width for regular hits?
  float m_timeBinWidth;
  // Is there going to be a fast simulation coming into this SD?
  G4bool m_useFrozenShowers;

  // Method actually doing the work to assign all of these SDs
  void setupAllSDs(std::map<G4VSensitiveDetector*,std::vector<std::string>*> configuration) const;

  // Pointers to the identifier helpers 
  const LArEM_ID*       m_larEmID; 
  const LArFCAL_ID*     m_larFcalID; 
  const LArHEC_ID*      m_larHecID; 
  const LArMiniFCAL_ID* m_larMiniFcalID;
  const CaloDM_ID*      m_caloDmID;
  bool match(const char *first, const char * second) const;

};

#endif
