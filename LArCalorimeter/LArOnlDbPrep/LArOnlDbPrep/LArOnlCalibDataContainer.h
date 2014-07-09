/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLDBPREP_LARONLCALIBDATACONTAINER_H
#define LARONLDBPREP_LARONLCALIBDATACONTAINER_H

#include <vector> 
#include "LArRawConditions/LArConditionsContainer.h" 
#include "LArIdentifier/LArOnlineID.h" 

#include "CLIDSvc/CLASS_DEF.h"

/** @class LArOnlCalibData
    @brief data class for online calibration 
*/


class LArOnlCalibData
{
public:
  LArOnlCalibData(): m_pedestal(-9999),m_fxofc_a(),m_fxofc_b(),m_shape(),m_shapeDer(),
    m_rampRatio(0),
    m_escale(-9999)
   { } 
  float m_pedestal; 
  std::vector<float> m_fxofc_a; 
  std::vector<float> m_fxofc_b; 
  std::vector<float> m_shape ;
  std::vector<float> m_shapeDer ;
  float m_rampRatio;
  float m_escale ; 

  bool isEmpty(){
	return (m_fxofc_a.size()==0 || m_fxofc_b.size()==0 
		|| m_shape.size()==0 || m_shapeDer.size()==0); 
  }
} ; 


/** @class LArOnlCalibDataContainer 
    @brief Container of LArOnlCalibData
*/

class LArOnlCalibDataContainer : public
LArConditionsContainer<LArOnlCalibData> 
{
 public:
  typedef LArConditionsContainer<LArOnlCalibData> CONTAINER ;

  LArOnlCalibDataContainer()
    : m_onlineID(0)
  { } 
  virtual ~LArOnlCalibDataContainer() { } 

  void setLArOnlineID(const LArOnlineID* helper ) 
  { 
	m_onlineID =helper;
	return;
  }
  const LArOnlineID*  onlineHelper()
	{ return m_onlineID ;}

 private: 
   const LArOnlineID* m_onlineID; 
};


CLASS_DEF( LArOnlCalibDataContainer, 1309714920 ,1)     

#endif 


