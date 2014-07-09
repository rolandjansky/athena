/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARONLDBPREP_LARONLENERGYDATACONTAINER_H
#define LARONLDBPREP_LARONLENERGYDATACONTAINER_H

#include <vector> 
#include <string>
#include <math.h>
#include "LArRawConditions/LArConditionsContainer.h" 
#include "LArIdentifier/LArOnlineID.h" 

#include "CLIDSvc/CLASS_DEF.h"

/** @class LArOnlEnergyData
    @brief data class for online energy (Threshold, GeoWeight, TTid etc.)
*/


class LArOnlEnergyData
{
public:
  LArOnlEnergyData(): m_threshold(),m_geoweight(),m_ttid("")
   { } 
  std::vector<float> m_threshold; 
  std::vector<float> m_geoweight; 
  std::string m_ttid; 

  bool isEmpty(){
    return (m_threshold.size()==0 || m_geoweight.size()==0);
  }
} ; 


/** @class LArOnlEnergyDataContainer 
    @brief Container of LArOnlEnergyData
*/

class LArOnlEnergyDataContainer : public
LArConditionsContainer<LArOnlEnergyData> 
{
 public:
  typedef LArConditionsContainer<LArOnlEnergyData> CONTAINER ;

  LArOnlEnergyDataContainer()
    : m_onlineID(0)
  { } 
  virtual ~LArOnlEnergyDataContainer() { } 

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


CLASS_DEF( LArOnlEnergyDataContainer, 1288555282 ,1)

#endif 


