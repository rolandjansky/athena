/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     Trig3Momentum.h
 PACKAGE:  Trigger/TrigEvent/TrigCaloEvent
 
 AUTHOR:   Kyle Cranmer
 PURPOSE:  
 MODIFIED: Tobias Kruker - Added cell quality variables for jet cleaning
 
 *******************************************************************/

#ifndef TRIGCALOEVENT_TRIG3MOMENTUM_H 
#define TRIGCALOEVENT_TRIG3MOMENTUM_H

#include "AthenaKernel/CLASS_DEF.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include <stdint.h>

/** 3 Momentum Class for Jet/MET Calculations */
class Trig3Momentum{
  
 public:
  /** Default Constructor */
  Trig3Momentum() : m_e(0.), m_eta(0.), m_phi(0.), m_inCone(false)
    , m_caloSample(CaloSampling::Unknown), m_time(0.){};
  /** Constructor with input parameters */
  Trig3Momentum( double Energy, double Eta, double Phi)
              : m_e(Energy), m_eta(Eta), m_phi(Phi)
	, m_caloSample(CaloSampling::Unknown), m_time(0.){};
  /** Constructor with input parameters */
  Trig3Momentum( double Energy, double Eta, double Phi
	, CaloSampling::CaloSample s)
              : m_e(Energy), m_eta(Eta), m_phi(Phi), m_inCone(false)
	, m_caloSample(s), m_time(0.){};
  
  // Get methods
  /** Get the energy */
  double e()      const {return m_e;  }
  /** Get the eta position */
  double eta()    const {return m_eta;  }
  /** Get the phi position */
  double phi()    const {return m_phi;  }
  /** Return if element is inside or outside of jet cone */
  bool   inCone() const {return m_inCone;  }
  /** Calorimeter Sample */
  CaloSampling::CaloSample caloSample() const {return m_caloSample;  }
  // Cleaning +++++++++++++++++++++:
  /** Get the quality from cell details*/
  uint16_t quality() const {return m_cellDetails[0]; }
  /** Get the provenance from cell details */
  uint16_t provenance() const {return m_cellDetails[1]; }
  /** Get the time */
  float time() 		const {return m_time;	 }


  // Set methods
  /** Set energy */
  void setE(double e)            {m_e = e;  }
  /** Set eta */
  void setEta(double eta)        {m_eta = eta;  }
  /** Set phi */
  void setPhi(double phi)        {m_phi = phi;  }
  /** Flag element inside or outside of jet cone */
  void setInCone(bool inCone)    {m_inCone = inCone;  }
  /** Set Calorimeter Sample */
  void setCaloSample(CaloSampling::CaloSample s)  {m_caloSample = s;  }

  /** Set RoI Word */
  void set_RoIword( long /*word*/ ) {};
  
  // Cleaning +++++++++++++++++++++:
  /** Set quality in cell details */
  void setQuality(uint16_t quality) {m_cellDetails[0] = quality;}
  /** Set provenance in cell details */
  void setProvenance(uint16_t provenance) {m_cellDetails[1] = provenance;}
  /** Set time */
	void setTime(float time)			{m_time = time;}
  
  

 private:
  /** Energy */
  double m_e;
  /** Eta */
  double m_eta;
  /** Phi */
  double m_phi;
  /** Set in cone flag */
  bool m_inCone;
  /** Calorimeter Sample */
  CaloSampling::CaloSample m_caloSample; 
	// Cleaning +++++++++++++++++++++:
	/** Cell details (to contain [0] = quality and [1] = provenance) */
  uint16_t m_cellDetails[2];
  /** Cell time */
  float m_time;
  
};

CLASS_DEF( Trig3Momentum , 87084890 , 1 )
#endif
