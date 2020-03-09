/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MISSINGETEVENT_MISSINGETCALO_H
#define MISSINGETEVENT_MISSINGETCALO_H
/********************************************************************

NAME:     MissingEtCalo.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE:  
********************************************************************/
#include "AthenaKernel/CLASS_DEF.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"
#include "CaloEvent/CaloCell.h"
#include "CaloGeoHelpers/CaloSampling.h"


#include <vector>

class MissingEtCalo : public MissingET
{
 public:

  enum CaloIndex  
   { 
     PEMB  = 0, 
     EMB   = 1,
     PEMEC = 2,          	      
     EME   = 3,
     TILE  = 4,
     HEC   = 5,	        	      
     FCAL  = 6,          	      
     Size  = 7,          	      
     NotValid = 999 
   };  	      
  
  //contructors
  MissingEtCalo();
////  MissingEtCalo(MissingET::Source aSource);
  MissingEtCalo(MissingEtRegions* aRegion);
  MissingEtCalo(MissingET::Source aSource, MissingEtRegions* aRegion);

  virtual ~MissingEtCalo();

  // add a cell
  void addCalo(const CaloCell* aCell, double weight);

  // get calorimeter index
  CaloIndex getCaloIndex(CaloSampling::CaloSample aSample);

  // set calorimeter contributions
  void setExCalo(CaloIndex aCalo,  double theEx);
  void setEyCalo(CaloIndex aCalo,  double theEy);
  void setEtSumCalo(CaloIndex aCalo, double theEtSum);
  void setNCellCalo(CaloIndex aCalo, unsigned int theNCell);
  void setCalibType(const std::string& theCalibType);
  
  void setExCaloVec(std::vector<double>&& exCaloVec);
  void setEyCaloVec(std::vector<double>&& exCaloVec);
  void setEtSumCaloVec(std::vector<double>&& etSumCaloVec);
  void setNCellCaloVec(std::vector<unsigned int>&& ncellCaloVec);

  // retrieve calorimeter contributions 
  double exCalo(CaloIndex aCalo) const;
  double eyCalo(CaloIndex aCalo) const;
  double etSumCalo(CaloIndex aCalo) const;
  unsigned int ncellCalo(CaloIndex aCalo) const;
  std::string calibType() const;

  const std::vector<double>& exCaloVec() const;
  const std::vector<double>& eyCaloVec() const;
  const std::vector<double>& etSumCaloVec() const;
  const std::vector<unsigned int>& ncellCaloVec() const;

protected:

  std::vector<double> m_exCalo;
  std::vector<double> m_eyCalo;
  std::vector<double> m_etSumCalo;
  std::vector<unsigned int> m_nCellsCalo;
  
  std::string m_calibType;
  
  void setup(); 
  void Calc_Ecellxy(const CaloCell* cObj, double &ex, double &ey);

};

CLASS_DEF(MissingEtCalo, 73891892, 1)

#endif
