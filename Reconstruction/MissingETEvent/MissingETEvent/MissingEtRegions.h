/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETEVENT_MissingEtRegions_H
#define MissingETEVENT_MissingEtRegions_H

/********************************************************************

NAME:     MissingEtRegions.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  S. Resconi
CREATED:  Oct 2004

PURPOSE: Calculate ExMiss,EyMiss and Sumet in Eta Regions
********************************************************************/
#include <vector>
#include "CaloEvent/CaloCell.h"

class MissingEtRegions
{
 public:

  // The following enum lists the various eta regions
  // fixed regions or set by JobOpt ?
  // not final, just an example
  enum RegionIndex
    { 
      Central   = 0,
      EndCap    = 1,
      Forward   = 2,  
      Size      = 3,
      Unknown   = 999
    };

  // constructor 
  MissingEtRegions();

  // destructor
  virtual ~MissingEtRegions();

  // add kinematics
  virtual void addReg(double theEx, double theEy, double theEt, 
		      double theEta, double weight );

  
  virtual RegionIndex getRegions();
  
  // set region contributions
  void setExReg(RegionIndex aRegion, double theEx);
  void setEyReg(RegionIndex aRegion, double theEy);
  void setEtSumReg(RegionIndex aRegion, double theEtSum);

  void setExRegVec(std::vector<double>&& exVec);
  void setEyRegVec(std::vector<double>&& eyVec);
  void setEtSumRegVec(std::vector<double>&& etSumVec);
  
  // get region contributions 
  double exReg(RegionIndex aRegion) const;
  double eyReg(RegionIndex aRegion) const;
  double etSumReg(RegionIndex aRegion) const;

  const std::vector<double>& exRegVec() const;
  const std::vector<double>& eyRegVec() const;
  const std::vector<double>& etSumRegVec() const;
  
 protected:

  RegionIndex m_reg;
  
  std::vector<double> m_exReg;
  std::vector<double> m_eyReg;
  std::vector<double> m_etReg;
  
  void setup();
  void Calc_Ecellxy(const CaloCell* cObj, double &ex, double &ey);
};


#endif
