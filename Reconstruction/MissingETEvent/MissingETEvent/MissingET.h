/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGET_H
#define MISSINGETEVENT_MISSINGET_H

/********************************************************************

NAME:     MissingEt.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE:  Base Class for MissingETEvent package, it owns the region object
********************************************************************/
#include "CLIDSvc/CLASS_DEF.h"
#include "MissingETEvent/MissingEtRegions.h"
#include "CaloEvent/CaloCell.h"

class MissingET
{
 public:

  // The following enum lists the various sources of MissingEt 
  // the source names are not final, can be changed 
  enum Source 
    { 
      Calo        = 0,
      Calib       = 1,
      Truth       = 2,
      Muon        = 3, 
      CaloRaw     = 4,
      Final       = 5,
      Cryo        = 6,
      Topo        = 7,
      Ref         = 8,
      DeadMaterial= 9,
      Track       = 10,
      ObjElectron = 100,
      ObjMuon	  = 101,
      ObjJet	  = 102,
      ObjIdTrk    = 103,
      ObjMiniJet  = 104,
      ObjRest	  = 105,
      ObjFinal    = 106,
      Unknown     = 999
    };

  // default constructor 
  MissingET();

  // construct with a specifier
  MissingET(MissingET::Source aSource);

  // construct with region sums
  MissingET(MissingEtRegions* theRegions);

  // construct with a specifier and region sums
  MissingET(MissingET::Source aSource, MissingEtRegions* theRegions);

  //fix by Andrei Gaponenko
  MissingET(const MissingET& b); // copy constructor

  //fix by Andrei Gaponenko
  MissingET& operator=(const MissingET& b); // assignment 
  
  // destructor
  virtual ~MissingET();

  // set methods for kinematics
  virtual void setEx(double theEx);
  virtual void setEy(double theEy);
  virtual void setEtSum(double theSum);

  // add kinematics
  //virtual void add(const CaloCell* aCell, double &weight);
  virtual void add(double theEx, double theEy, double theEt, 
		   double theEta,double weight );

  
  // retrieve kinematics
  virtual double etx()    const;
  virtual double ety()    const;
  virtual double sumet()  const;
  virtual double et()     const;
  virtual double phi()    const;


  virtual MissingEtRegions* getRegions() const;

  virtual Source getSource() const;

 protected:

  MissingEtRegions* m_regions;
  
  Source m_source;
  
  // basic components of MissingET 
  double m_ex;
  double m_ey;
  double m_etSum;
  
  //void Calc_Ecellxy(const CaloCell* cObj, double &ex, double &ey);
};

CLASS_DEF(MissingET, 75817330, 1)

#endif
