/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACOLLECTION_H
#define AFP_RAWDATACOLLECTION_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "AFP_RawEv/AFP_RawData.h"

#include <stdint.h>

//apparently changing strings with sed doesn't count as a change for svn.

class AFP_RawDataCollection : public DataVector <AFP_RawData>

{
  private:

 uint16_t m_L1Id;
 uint16_t m_link_number_POT;
 uint32_t m_frontend_flag;
 std::vector<AFP_RawData> m_POT_DATA;

 std::vector<bool> m_patternId;
 uint16_t m_BCIdId; 
 uint16_t m_header_numberId;
 uint16_t m_ADC2Id;
 bool m_TrigSyncErrId;
 uint32_t m_robID;		///< ID of ROB from which data come from. Destinguish A and C side

 public:

typedef AFP_RawData DIGIT;

AFP_RawDataCollection();
AFP_RawDataCollection(uint16_t MB_number);
AFP_RawDataCollection(uint16_t Mrod_number, uint16_t MB_number);
virtual ~AFP_RawDataCollection() { };

void Set_lvl1Id(uint16_t MB_number);
  void Set_link_header(uint16_t Mrod_number);
  void Set_flag(uint32_t Event_number);
  void PushBack_POT(const AFP_RawData& PMF_HIT);
 
  void Set_pattern_POT(const std::vector<bool>& pattern_number);
  void Set_bcid(uint16_t BCId_number);
  void Set_header(uint16_t header_number_number);
  void Set_ADC2_POT(uint16_t ADC2_number);
  void SetTrigSyncErr(bool bit); 
  void Set_robID(const uint32_t robID) {m_robID = robID;}

  void SetZero_POT();
	  

uint16_t Get_lvl1Id() const {return m_L1Id;};
  uint16_t Get_link_header() const {return m_link_number_POT;};
  uint32_t Get_flag()  const {return m_frontend_flag;};
  const std::vector<bool>& Get_pattern_POT()  const {return m_patternId;};
  uint16_t Get_bcid()  const {return m_BCIdId;};
  uint16_t Get_header_number_POT()  const {return m_header_numberId;};
  uint16_t Get_ADC2_POT()  const {return m_ADC2Id;};
  bool GetTrigSyncErr()  const {return m_TrigSyncErrId;};
  uint32_t Get_robID() const {return m_robID;}

  const std::vector<AFP_RawData>& Get_POT_DATA()  const {return m_POT_DATA;};
  AFP_RawData Get_POT_PMF_DATA(uint32_t Numpmf)  const {return m_POT_DATA[Numpmf];};

};

CLASS_DEF( AFP_RawDataCollection , 1078031561 , 1 ) 

#endif 

