/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelPresamplerCalculator.hh 
// Prepared 05-Dec-2002 Bill Seligman

// A first pass at determing hit cell ID in the LAr barrel presampler.

#ifndef __LArBarrelPresamplerCalculator_H__
#define __LArBarrelPresamplerCalculator_H__

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"

#include "globals.hh"
#include <stdexcept>
// Forward declarations.
class G4Step;
class PsMap;
class LArG4BirksLaw;

namespace LArG4 {
  namespace BarrelPresampler {
    // Forward declaration
    class Geometry;
  }
}

class LArBarrelPresamplerCalculator : public LArVCalculator {

public:

  // Accessor for pointer to the singleton.
  static LArBarrelPresamplerCalculator* GetCalculator();
  ~LArBarrelPresamplerCalculator();

  /////////////////////////////////////////////
  // The interface for LArVCalculator.

  virtual G4float OOTcut() const { return m_OOTcut; }

  virtual G4bool Process(const G4Step* a_step){return  Process(a_step, m_hdata);}
  virtual G4bool Process(const G4Step* a_step, std::vector<LArHitData>& hdata);

  virtual int getNumHits() const {return m_nhits;}
  virtual const LArG4Identifier& identifier(int i=0) const { 
    if (i<0||i>=m_nhits) throw std::range_error("Hit asked is out of range");
    if(static_cast<int>(m_hdata.size())<=i) throw std::range_error("No such hit yet");
    return m_hdata[i].id; }
  virtual G4double time(int i=0) const { 
    if (i<0||i>=m_nhits) throw std::range_error("Hit asked is out of range");
    if(static_cast<int>(m_hdata.size())<=i) throw std::range_error("No such hit yet");
    return m_hdata[i].time; }
  virtual G4double energy(int i=0) const { 
    if (i<0||i>=m_nhits) throw std::range_error("Hit asked is out of range");
    if(static_cast<int>(m_hdata.size())<=i) throw std::range_error("No such hit yet");
    return m_hdata[i].energy; };
  virtual G4bool isInTime(int i=0) const    { 
    if (i<0||i>=m_nhits) throw std::range_error("Hit asked is out of range");
    return     m_isInTime[i]; }
  virtual G4bool isOutOfTime(int i=0) const { 
    if (i<0||i>=m_nhits) throw std::range_error("Hit asked is out of range");
    return ( ! m_isInTime[i] ); }

  inline void detectorName(std::string name) { m_detectorName=name; }

protected:
  // The constructor is protected according to the singleton design
  // pattern.
  LArBarrelPresamplerCalculator();

private:

  //copy constructor
  LArBarrelPresamplerCalculator(const LArBarrelPresamplerCalculator&);//coverity issue fix. Declared, but not implemented
  LArBarrelPresamplerCalculator& operator=(const  LArBarrelPresamplerCalculator&);//coverity issue fix. Declared, but not implemented
  static LArBarrelPresamplerCalculator* m_instance;

  // Pointer to detector parameters routine.
  LArVG4DetectorParameters* m_parameters;

   // Class for calculating the identifier.
  LArG4::BarrelPresampler::Geometry* m_geometry;

  PsMap* m_psmap;

  // RUN Options
  bool IflCur;

  const LArG4BirksLaw *birksLaw;

  // Store the out-of-time cut from the description:
  G4float m_OOTcut;

  // The results of the Process calculation:
  int m_nhits;                                 // number of hits
  //std::vector<LArG4Identifier> m_identifier;   // hit identifier
  //std::vector<G4double> m_energy;                // energy (or current)
  //std::vector<G4double> m_time;                  // time
  std::vector<LArHitData> m_hdata;

  std::vector<G4bool> m_isInTime;                // hit in time ?
  LArG4Identifier m_identifier2;

  // detector name, for translated geometry
  std::string m_detectorName;

  bool m_testbeam;

};

#endif // __LArBarrelPresamplerCalculator_H__
