/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_JETFILTER_H
#define GENERATORFILTERS_JETFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <cmath>


/// @brief Applies the generator level filter for jet candidates
/// @author I Hinchliffe, May 2002
class JetFilter : public GenFilter {
public:

  JetFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterEvent();


  struct McObj {
    // Constructors and destructor
    McObj();
    McObj(CLHEP::HepLorentzVector& p)
      : m_p (p),
        m_Nobj (0)
    { }
    ~McObj() {}

    // Get functions
    CLHEP::HepLorentzVector& P() {return m_p;}
    double P(int i) {return m_p[i];}
    int Nobj() {return m_Nobj;}

    // Set functions
    void SetP(const CLHEP::HepLorentzVector& p) {m_p=p;}
    void SetP(const double&,const double&,const double&,const double&);
    void SetXYZM(const double&,const double&,const double&, const double&);
    void SetNobj(int& nn) {m_Nobj = nn;}

    // Overloaded operators for sorting on Pt
    bool operator<(const McObj& rhs) const {return m_p.perp()<rhs.m_p.perp();}
    bool operator>(const McObj& rhs) const {return m_p.perp()>rhs.m_p.perp();}

  protected:
    CLHEP::HepLorentzVector m_p;
    int m_Nobj;
  };


private:

  static const int m_grphi = 105 ; // -CLHEP::pi to CLHEP::pi in units of 0.06 (approx)
  static const int m_greta = 200 ; // -6.0 to 6.0 in units 0.06 {approx}

  double m_UserEta;  //range over which triggers are searched for
  int m_UserNumber; // how many are we looking for
  double  m_UserThresh; // lowest et jet
  double m_Stop; //seed tower threshold
  double m_Cone; //cone sixe
  int m_Gride; //how many cells in eta
  int m_Gridp; //how many cells in phi
  bool m_Type; // cone or grid to define jet

  // Internal parameters
  double m_EtaRange; //range over which search runs
  double m_emaxeta; // largest eta of bins
  double m_edeta; // size of eta bins
  double m_edphi; //size of phi bins
  double m_twopi; //twopi
  int m_nphicell; // number of phi cells inside half cone
  int m_netacell; // number of eta cells inside half cone
  int m_nphicell2; // number of phi cells inside full cone
  int m_netacell2; // number of eta cells inside full cone
  std::vector<McObj> m_Jets; //store jets

};

#endif
