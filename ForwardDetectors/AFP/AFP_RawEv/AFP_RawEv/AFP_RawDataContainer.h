/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACONTAINER_H
#define AFP_RAWDATACONTAINER_H

#include <vector>
#include <string>
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "SGTools/CLASS_DEF.h"
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

//svn hook

class AFP_RawDataContainer : public DataVector <AFP_RawDataCollection>
{

private:

uint32_t m_wordMarker;
 uint16_t m_subdetId; 
uint16_t m_mrodId;
 uint32_t m_lvl1Id;
 uint16_t m_ecrId;     
 uint16_t m_bcId; 
  uint32_t m_runNum;
  uint16_t m_runType;    
 uint16_t m_triggerTypeId;
uint32_t m_DetEventType; 

  static const uint32_t s_FullEVmarker = 0xaa1234aa;
  static const uint32_t s_ROBmarker = 0xdd1234dd;
  static const uint32_t s_RODmarker = 0xee1234ee;
  static const uint32_t s_RODheadersize = 0x00000009; 
  static const uint32_t s_RODversion  = 0x03010000;     

uint32_t m_TimeStamp;
uint32_t m_TimeStampns;

uint32_t m_LumiBlock;
std::vector<bool> m_lvl1Pattern;
std::vector<bool> m_lvl2Pattern;
std::vector<bool> m_efPattern;

  public:

AFP_RawDataContainer();
~AFP_RawDataContainer();
typedef AFP_RawDataCollection::size_type size_type;

size_type digit_size() const;

bool is_FullEVmarker() const  {return m_wordMarker == s_FullEVmarker;};
 bool is_ROBmarker() const {return m_wordMarker == s_ROBmarker;};      
 bool is_RODmarker() const {return m_wordMarker == s_RODmarker;};      


 uint16_t subdetId()  const {return m_subdetId;}
  uint16_t mrodId()    const {return m_mrodId;}
  uint32_t runNum()    const {return m_runNum;}
  uint16_t runType()   const {return m_runType;}
  uint32_t lvl1Id()    const {return m_lvl1Id;}
  uint16_t ecrId()     const {return m_ecrId;}
  uint16_t bcId()      const {return m_bcId;}
  uint16_t trigtypeId() const {return m_triggerTypeId;}
  uint32_t DetEventType() const    {return m_DetEventType;}

  void SetLvl1Pattern(const std::vector<bool>& tmplvl1_pattern);
  void SetLvl2Pattern(const std::vector<bool>& tmplvl2_pattern);
  void SetEFPattern(const std::vector<bool>& tmpef_pattern);
  void SetTimeStamp(uint32_t tmpTimeStamp);
  void SetTimeStampns(uint32_t tmpTimeStampns);
  void SetLumiBlock(uint32_t tmpLumiBlock);
  void SetBCId(uint32_t tmpBCId);
  void SetLvl1Id(uint32_t tmpLvl1ID);

  const std::vector<bool>& GetLvl1Pattern_POT() const {return m_lvl1Pattern;};
  const std::vector<bool>& GetLvl2Pattern_POT() const {return m_lvl2Pattern;};
  const std::vector<bool>& GetEFPattern_POT() const {return m_efPattern;};
  uint32_t GetTimeStamp() const {return m_TimeStamp;}; 
  uint32_t GetTimeStampns() const {return m_TimeStampns;};     
  uint32_t GetLumiBlock() const {return m_LumiBlock;};
  uint32_t GetBCId() const {return m_bcId;};    


private:

void setZero();

};


CLASS_DEF( AFP_RawDataContainer , 1237702017 , 1 )



#endif


