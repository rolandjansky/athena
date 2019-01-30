/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          EnergyTopoData.h  -  description
                             -------------------
    begin                : Tuesday September 02 2014
    email                : Alan.Watson@CERN.CH
 ***************************************************************************/


#ifndef EnergyTopoData_H
#define EnergyTopoData_H

// STL includes
#include <vector>

// Gaudi kernel stuff.
#include "GaudiKernel/DataObject.h"

namespace LVL1 {

/**This class defines the Ex/Ey/ET data transferred from the EnergySum CMX
 * to the L1Topo processors
*/
  class EnergyTopoData : public DataObject {
  public:
    
  enum SumTypes { Normal = 0, Restricted = 1 };

  /// constructor
  EnergyTopoData(unsigned int word0, unsigned int word1, unsigned int word2);
    
  EnergyTopoData() ;

  /// destructor
  ~EnergyTopoData(){};
  
  /** add data. Ex/Ey values should be in 15 bit twos-complement format */
  void addEx(unsigned int Ex, unsigned int overflow, int type);
  void addEy(unsigned int Ey, unsigned int overflow, int type);
  void addEt(unsigned int Et, unsigned int overflow, int type);
  
  /** add data using RoI word */
  void addRoI(uint32_t roiWord);
  void addRoIs(const std::vector<uint32_t>& roiWords);
  
  /** return  word 0*/
  unsigned int word0() const {return m_word0;}
  /** return word 1*/
  unsigned int word1() const {return m_word1;}
  /** return  word 2*/
  unsigned int word2() const {return m_word2;}
  
  /** return Ex, Ey, ET values */
  int Ex(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  int Ey(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  int Et(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  /** return twos-complement Ex, Ey values */
  int ExTC(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  int EyTC(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  /** return overflow flags */
  unsigned int ExOverflow(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  unsigned int EyOverflow(SumTypes type = LVL1::EnergyTopoData::Normal) const;
  unsigned int EtOverflow(SumTypes type = LVL1::EnergyTopoData::Normal) const; 
  
private:
  unsigned int m_word0;
  unsigned int m_word1;
  unsigned int m_word2;
  
  int decodeTC(unsigned int word) const;

};

}//end of LVL1 namespace defn

#include "TrigT1CaloEvent/EnergyTopoData_ClassDEF.h"

#endif
