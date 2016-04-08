/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  ALFA_RAWDATACOLLECTION_H
#define  ALFA_RAWDATACOLLECTION_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_RawEv/ALFA_RawData.h"

#include<stdint.h>

/** RDOs : Chamber Service Module, collection of RawData
    @author sara diglio Sept 2009*/


class ALFA_RawDataCollection : public DataVector<ALFA_RawData>
{

 private:
 
  //Motherboard id
  uint16_t m_MBId_POT;
  //Mrod id
  uint16_t m_MrodId_POT;
  //Event counter
  uint32_t m_EventCount_POT;
  //Hit store for one PMF
  std::vector<ALFA_RawData> m_POT_DATA;

  //pattern id
  std::vector<bool> m_patternId;
  //scaler id
  uint16_t m_scalerId;
  //ADC1 id Chan0
  uint16_t m_ADC1Id;
  //ADC1 id Chan1
  uint16_t m_ADC2Id;
  //syncronization error of PMF 24 (trigger info)
  bool m_TrigSyncErrId;

 public:
  //typedef 
  typedef ALFA_RawData DIGIT; 
  
  
  /** Default constructor*/
  ALFA_RawDataCollection();


  /** Full constructor*/
  ALFA_RawDataCollection(uint16_t MB_number);

  /** Full constructor*/
  ALFA_RawDataCollection(uint16_t Mrod_number, uint16_t MB_number);

  virtual ~ALFA_RawDataCollection() { };

  // set data members

  void SetMBId_POT(uint16_t MB_number);
  void SetMrodId_POT(uint16_t Mrod_number);
  void SetEventCount_POT(uint32_t Event_number);
  void PushBack_POT(const ALFA_RawData& PMF_HIT);
  
  void Set_pattern_POT(const std::vector<bool>& pattern_number);
  void Set_scaler_POT(uint16_t scaler_number);
  void Set_ADC1_POT(uint16_t ADC1_number);
  void Set_ADC2_POT(uint16_t ADC2_number);
  void SetTrigSyncErr(bool bit);  

  void SetZero_POT();
  
 

  // Methods to access the decoded information
 

  uint16_t GetMBId_POT() const {return m_MBId_POT;};
  uint16_t GetMrodId_POT() const {return m_MrodId_POT;};
  uint32_t GetEventCount_POT()  const {return m_EventCount_POT;};
  const std::vector<bool>& Get_pattern_POT()  const {return m_patternId;};
  uint16_t Get_scaler_POT()  const {return m_scalerId;};
  uint16_t Get_ADC1_POT()  const {return m_ADC1Id;};
  uint16_t Get_ADC2_POT()  const {return m_ADC2Id;};
  bool GetTrigSyncErr()  const {return m_TrigSyncErrId;};

  const std::vector<ALFA_RawData>& Get_POT_DATA()  const {return m_POT_DATA;};
  ALFA_RawData Get_POT_PMF_DATA(uint32_t Numpmf)  const {return m_POT_DATA[Numpmf];};


};

CLASS_DEF( ALFA_RawDataCollection , 1127672441 , 1 )


#endif   //  ALFA_RAWDATACOLLECTION_H

