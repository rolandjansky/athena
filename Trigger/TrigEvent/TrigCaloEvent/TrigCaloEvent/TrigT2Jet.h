/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TrigT2Jet.h
 PACKAGE:  Trigger/TrigEvent/TrigCaloEvent
 
 AUTHOR:   Kyle Cranmer
 MODIFIED: Tobias Kruker (added cleaning variables)
 
 *******************************************************************/

#ifndef TRIGCALOEVENT_TRIGT2JET_H 
#define TRIGCALOEVENT_TRIGT2JET_H


#include "AthenaKernel/CLASS_DEF.h"

#include "TrigCaloEvent/Trig3Momentum.h"
#include "FourMom/P4EEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"

#include <vector>

/** LVL2 Trigger Jet Class */
class TrigT2Jet : public P4EEtaPhiMBase,  public NavigableTerminalNode ,
	     virtual public INavigable4Momentum {
  
 public:
  /** Constructor */
  TrigT2Jet();
  /** Destructor */
  ~TrigT2Jet();

  /** Return jet energy */
  double e()           const {return m_e;  }
  /** Return the hadronic energy of the jet, calculated before calibration */
  double ehad0()        const {return m_ehad0;  }
  /** Return the electromagnetic energy of the jet, calculated before the calibration */
  double eem0()         const {return m_eem0;  }
  /** Return jet eta */
  double eta()         const {return m_eta;  }
  /** Return jet phi */
  double phi()         const {return m_phi;  }
  /** Return the jet mass - if we constrain, no need to store */
  double m()           const {return 0.;  }
  /** Return the cone Radius */
  //double coneRadius()  const {return m_coneRadius;  }
  /** Return the RoI Word */
  unsigned int RoIword()       const {return m_roiWord;}

  /** Trig3Momentum entities making part of Jet are
	stored in a grid */
        std::vector<Trig3Momentum>* grid()        {  return m_grid; }
  const std::vector<Trig3Momentum>* grid()  const {  return m_grid; }
  
  // Cleaning:
  /** Return nLeadingCells (e.g. n90) */
  int nLeadingCells()		const {return m_nLeadingCells;}
  /** Return fraction of energy in HEC */
  float hecf()					const {return m_hecf;}
  /** Return fraction of jet energy from bad cells */
  float jetQuality()		const {return m_jetQuality;}
  /** Return fraction of energy in EMC*/
  float emf()						const {return m_emf;}
  /** Return jet time calculated from cells */
  float jetTimeCells() 	const {return m_jetTimeCells;}

  /** set Jet energy */
  void setE(double e)            {m_e = e;  }
  /** Set Hadronic energy */
  void setEhad0(double ehad)     {m_ehad0=ehad;}
  /** Set Electromagnetic energy */
  void setEem0(double eem)       {m_eem0 = eem;}
  /** set Jet eta */
  void setEta(double eta)        {m_eta = eta;  }
  /** set Jet phi */
  void setPhi(double phi)        {m_phi = phi;  }
  /** set Jet cone Radius */
  //void setConeRadius(double R)   {m_coneRadius = R;  }

  /** set Jet grid vector */
  void setGrid(std::vector<Trig3Momentum>* grid)  { m_grid = grid; }

  /** set RoI Word */
  void set_RoIword( unsigned int word ) {m_roiWord=word;};

  /** Clear Trig3Momentum grid */
  void clearGrid();
  
  // Cleaning:
  /** Set nLeadingCells (e.g. n90) */
  void setNLeadingCells(int nLeadingCells)		{ m_nLeadingCells = nLeadingCells;}
  /** Set fraction of energy in HEC */
  void setHecf(float hecf)										{ m_hecf = hecf;}
  /** Set fraction of jet energy from bad cells */
  void setJetQuality(float jetQuality)				{ m_jetQuality = jetQuality;}
  /** Set fraction of energy in EMC*/
  void setEmf(float emf)											{ m_emf = emf;}
  /** Set jet time calculated from cells */
  void setJetTimeCells(float jetTimeCells) 		{ m_jetTimeCells = jetTimeCells;}

 private:

  /** Jet energy */
  double m_e;
  /** Hadronic Jet Energy before calibration */
  double m_ehad0;
  /** Electromagnetic Jet Energy before calibration */
  double m_eem0;
  /** Jet eta */
  double m_eta;
  /** Jet phi */
  double m_phi  ;
  /** Jet coneRadius */
  //double m_coneRadius  ;

  /** vector of Trig3Momentum elements that compose Jet */
  std::vector<Trig3Momentum>* m_grid;
  
  /** RoI Word */
  unsigned int m_roiWord;
  
  // Cleaning:
  /** Jet nLeadingCells (e.g. n90) */
  int m_nLeadingCells;
  /** Fraction of energy in HEC */
  float m_hecf;
  /** fraction of jet energy from bad cells */
  float m_jetQuality;
  /** Fraction of energy in EMC*/
  float m_emf;
  /** Jet time */
  float m_jetTimeCells;

};

/// Operator comparing two objects for equality
bool operator== ( const TrigT2Jet& a, const TrigT2Jet& b );

/// Operator comparing two objects for inequality
inline bool operator!= ( const TrigT2Jet& a, const TrigT2Jet& b ) {
   return !( a == b );
}

/// Comparison with feedback
void diff(const TrigT2Jet& a, const TrigT2Jet& b,
	  std::map<std::string, double>& variableChange);

/// conversion to formatted string: multi-line!
std::string str (const TrigT2Jet& a);

/// formatted output
MsgStream& operator<< (MsgStream& m, const TrigT2Jet& d);

CLASS_DEF(TrigT2Jet, 198767871, 1)

#include "TrigCaloEvent/TrigT2JetContainer.h"

#endif
