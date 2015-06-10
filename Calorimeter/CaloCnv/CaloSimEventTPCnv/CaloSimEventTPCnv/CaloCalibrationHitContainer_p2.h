/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P2_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P2_H
/**
@class CaloCalibrationHitContainer_p2
@brief Persistent represenation of a CaloCalibrationContainer,
@author: Ilija Vukotic
*/
#include <vector>
#include <string>

class CaloCalibrationHitContainer_p2   
{
 public:
		 
  /// Default constructor
  CaloCalibrationHitContainer_p2();
  
  // Accessors
  const std::string&  name() const;
  
 private:
	std::vector<unsigned int>   m_channelHash; 
	std::vector<unsigned int>       m_energy; // 18 bits compressed - all four of them in the same array 
//	std::vector<unsigned int>       m_energy1; // 18 bits compressed
//	std::vector<unsigned int>       m_energy2; // 18 bits compressed
//	std::vector<unsigned int>       m_energy3; // 18 bits compressed
	std::string                          m_name;
 
};

// inlines

inline CaloCalibrationHitContainer_p2::CaloCalibrationHitContainer_p2() {}

inline const std::string& CaloCalibrationHitContainer_p2::name() const {return m_name;}

#endif
