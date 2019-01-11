/* emacs: this is -*- c++ -*- */
/**
 **     @file    TIDARoiParameters.h
 **
 **     @brief   Class to store the eta, phi, zed, parameters and ranges 
 **              for the TIDARoiDescriptor class
 **              
 **              This is needed, since the RoiDescriptor can contain a vector
 **              of RoiDescriptor* and I couldn;t work out how to have the root
 **              class persistify the members of the class, recursively
 **              within itself          
 **
 **     @author  mark sutton
 **     @date    Wed  4 Mar 2015 16:09:33 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef  TIDAROIPARAMETERS_H
#define  TIDAROIPARAMETERS_H

#include <iostream>

#include "TObject.h"



class TIDARoiParameters : public TObject {

public:

  TIDARoiParameters(); 


  TIDARoiParameters(double eta,  double etaMinus,   double etaPlus,
		    double phi,  double phiMinus,   double phiPlus,
		    double zed,  double zedMinus,   double zedPlus );


  TIDARoiParameters(const TIDARoiParameters& p );


  virtual ~TIDARoiParameters();

  float eta() const { return m_eta; }
  float phi() const { return m_phi; }
  float zed() const { return m_zed; }

  float etaMinus() const { return m_etaMinus; }
  float etaPlus()  const { return m_etaPlus; }

  float phiMinus() const { return m_phiMinus; }
  float phiPlus()  const { return m_phiPlus; }

  float zedMinus() const { return m_zedMinus; }
  float zedPlus()  const { return m_zedPlus; }

protected:

  float m_eta;
  float m_phi;
  float m_zed;
  
  float m_etaMinus;
  float m_etaPlus;

  float m_phiMinus;
  float m_phiPlus;

  float m_zedMinus;
  float m_zedPlus;

  ClassDef( TIDARoiParameters, 1 ) 

};

inline std::ostream& operator<<( std::ostream& s, const TIDARoiParameters& _t ) { 
  return s << "\tP: eta: " << _t.eta() << " " << _t.etaMinus() << " - " << _t.etaPlus()
	   <<    "\tphi: " << _t.phi() << " " << _t.phiMinus() << " - " << _t.phiPlus()
	   <<      "\tz: " << _t.zed() << " " << _t.zedMinus() << " - " << _t.zedPlus();
}


#endif  // TIDAROIPARAMETERS_H 










