/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArBarrelCalculator.hh
// The Cell Identifier for the EM Barrel readout cells

// Adapted from code written by Gaston Parrour
// Adaptation by Sylvain Negroni

// 12-Jul-2002 WGS: Added LArG4Identifier.

// 18-03-2005  G.Unal: major revision to include new current maps
class LArG4BirksLaw;
#ifndef LARBARRELCALCULATOR_H
#define LARBARRELCALCULATOR_H

#include "LArG4Code/LArG4Identifier.h"
#include "LArG4Code/LArVCalculator.h"
#include "LArG4Code/LArVG4DetectorParameters.h"
#include "LArG4Barrel/LArBarrelGeometry.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>

class G4Step;
class AccMap;
class MapEta;

class LArBarrelCalculator : public LArVCalculator 
{
public: 

  // Accessor method for singleton pattern.
  static LArBarrelCalculator* GetCalculator();

  // Destructor
  ~LArBarrelCalculator();

  LArBarrelCalculator (const LArBarrelCalculator&) = delete;
  LArBarrelCalculator& operator= (const LArBarrelCalculator&) = delete;
  
  virtual G4bool Process(const G4Step* a_step){return  Process(a_step, m_hdata);}
  virtual G4bool Process(const G4Step* a_step, std::vector<LArHitData>& hdata);
  
  //
  // get functions:
  //
  virtual G4float OOTcut() const { return m_OOTcut; }  
 
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
  // The constructor is protected according to the 'singleton' design
  // pattern.
  LArBarrelCalculator();
 
private:

  static LArBarrelCalculator* m_instance;

  LArVG4DetectorParameters* m_parameters;
  LArG4::Barrel::Geometry* m_geometry;
  AccMap*   m_accmap;
  std::unique_ptr<MapEta>   m_etamap1;
  std::unique_ptr<MapEta>   m_etamap2;
  std::unique_ptr<MapEta>   m_etamap3;

  G4float m_OOTcut;

  // RUN Options
  bool m_IflCur;
  bool m_IflMapTrans;
  bool m_IflXtalk;

  double m_dstep;
  
  const LArG4BirksLaw *m_birksLaw;
  bool  m_doHV;


  // detector name, for translated geometry
  std::string m_detectorName;

  // global EMBarrel dimensions
  double s_etaMaxBarrel;
  double s_zMinBarrel;
  double s_zMaxBarrel;
  // global Accordion dimensions
  double s_rMinAccordion;
  double s_rMaxAccordion;
  // half thickness of absorber and electrode
  double s_ThickAbs;
  double s_ThickEle;
  // GU 11/06/2003  total number of cells in phi
  int s_NCellTot;
  int s_NCellMax;
  // to handle small difference (mostly phi wrapping and +-z symmetry)
  // between atlas and test beam
  bool m_testbeam;
  
  // The results of the Process calculation:
  int m_nhits;                                 // number of hits
  //std::vector<LArG4Identifier> m_identifier;   // hit identifier
  //std::vector<G4double> m_energy;                // energy (or current)
  //std::vector<G4double> m_time;                  // time
  std::vector<LArHitData> m_hdata;

  std::vector<G4bool> m_isInTime;                // hit in time ?
  LArG4Identifier m_identifier2;
  LArG4Identifier m_identifier_xt1;
  LArG4Identifier m_identifier_xt2;

// Hv values
//   0,1 = positive/negative barrel
//   0->1023 = electrode number
//   0->6 = eta region number (0.2 granularity) (1.2 to 1.475 in same eta bin)
//   0,1 = below, above the electrode (according to phi in global Atlas frame) 

  double m_hv[2][1024][7][2];

  G4bool FiducialCuts(G4double,G4double,G4double);

  void InitHV();
  double ScaleHV(double, double, double, double);

};

#endif // LARBARRELCALCULATOR_H
