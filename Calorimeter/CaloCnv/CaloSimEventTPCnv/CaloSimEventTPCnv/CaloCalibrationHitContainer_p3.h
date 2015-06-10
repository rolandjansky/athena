/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P3_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P3_H
/**
@class CaloCalibrationHitContainer_p3
@brief Persistent represenation of a CaloCalibrationContainer,
@author: Ilija Vukotic
*/
#include <vector>
#include <string>

class CaloCalibrationHitContainer_p3   
{
 public:
		 
  /// Default constructor
  CaloCalibrationHitContainer_p3();
  
  // Accessors
  const std::string&  name() const;
  
 private:
	std::vector<unsigned int>   m_channelHash; 
	std::vector<unsigned int>       m_energy; // 18 bits compressed - all four of them in the same array 
//	std::vector<unsigned int>       m_energy1; // 18 bits compressed
//	std::vector<unsigned int>       m_energy2; // 18 bits compressed
//	std::vector<unsigned int>       m_energy3; // 18 bits compressed
	std::string                          m_name;
        std::vector<unsigned int>            m_particleID;
 
};

// inlines

inline CaloCalibrationHitContainer_p3::CaloCalibrationHitContainer_p3() {}

inline const std::string& CaloCalibrationHitContainer_p3::name() const {return m_name;}

#endif
