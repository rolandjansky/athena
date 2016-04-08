/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  ALFA_RAWDATACOLLECTION_CHARGE_H
#define  ALFA_RAWDATACOLLECTION_CHARGE_H

#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "ALFA_RawEv/ALFA_RawData_charge.h"

#include<stdint.h>

/** RDOs : Chamber Service Module, collection of RawData
    @author sara diglio Sept 2009*/


class ALFA_RawDataCollection_charge : public DataVector<ALFA_RawData_charge>
{

 private:
 
  //Motherboard id
  uint16_t m_MBId_POT;
  //Mrod id
  uint16_t m_MrodId_POT;
  //Event counter
  uint32_t m_EventCount_POT;
  //Hit store for one PMF
  std::vector<ALFA_RawData_charge> m_POT_DATA;
  //Time Stamp
  uint32_t m_TimeStamp_POT;
  //Bunch Crossing ID
  uint32_t m_BCId_POT;

 public:
  //typedef 
  typedef ALFA_RawData_charge DIGIT; 
  
  
  /** Default constructor*/
  ALFA_RawDataCollection_charge();


  /** Full constructor*/
  ALFA_RawDataCollection_charge(uint16_t MB_number);

  /** Full constructor*/
  ALFA_RawDataCollection_charge(uint16_t Mrod_number, uint16_t MB_number);

  virtual ~ALFA_RawDataCollection_charge() { };

  // set data members

  void SetMBId_POT(uint16_t MB_number) ;
  void SetMrodId_POT(uint16_t Mrod_number) ;
  void SetEventCount_POT(uint32_t Event_number);
  void PushBack_POT(const ALFA_RawData_charge& PMF_HIT);
  
  void SetTimeStamp_POT(uint32_t TimeStamp);
  void SetBCId_POT(uint32_t BCId);
  
  void SetZero_POT();
  
  // Methods to access the decoded information
 
  uint16_t GetMBId_POT() const {return m_MBId_POT;};
  uint16_t GetMrodId_POT() const {return m_MrodId_POT;};
  uint32_t GetEventCount_POT() const {return m_EventCount_POT;};
 
  uint32_t GetTimeStamp_POT() const {return m_TimeStamp_POT;};	
  uint32_t GetBCId_POT() const {return m_BCId_POT;};	

  const std::vector<ALFA_RawData_charge> Get_POT_DATA() const {return m_POT_DATA;};
  ALFA_RawData_charge Get_POT_PMF_DATA(uint32_t Numpmf) const {return m_POT_DATA[Numpmf];};


};

CLASS_DEF( ALFA_RawDataCollection_charge , 1210134863 , 1 )


#endif   //  ALFA_RAWDATACOLLECTION_CHARGE_H

