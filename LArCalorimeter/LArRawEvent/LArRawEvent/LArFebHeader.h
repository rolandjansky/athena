/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARFEBHEADER_H
#define LARFEBHEADER_H

#include "Identifier/HWIdentifier.h"
#include <stdint.h>


/** 
@class LArFebHeader
@brief Holds information from the FEB Header

@author Walter Lampl
@author Remi Lafaye
*/

class LArFebHeader
{
 public: 
  
  /** @brief Constructor with FEB Id*/
  LArFebHeader(const HWIdentifier febid);

  /** @brief Destructor */
  ~LArFebHeader();

  /** @brief get the FEBId */
  inline HWIdentifier FEBId() const {return m_FEBId;}

  /** @brief get the format version */
  inline uint32_t FormatVersion() const {return m_RodHeader.FormatVersion;}

  /** @brief get the source Id */
  inline uint32_t SourceId() const {return m_RodHeader.SourceId;}

  /** @brief get the run number */
  inline uint32_t RunNumber() const {return m_RodHeader. RunNumber;}

  /** @brief get the EventID */
  inline uint16_t ELVL1Id() const {return m_RodHeader.ELVL1Id;}

  /** @brief get the Bunch Crossing ID */
  inline uint16_t BCId() const {return m_RodHeader.BCId;}

  /** @brief get the Level1 trigger type ID */
  inline uint32_t LVL1TigType() const {return m_RodHeader.LVL1TigType;}

  /** @brief get the Detector event type ID */
  inline uint32_t DetEventType() const {return m_RodHeader.DetEventType;}

  /** @brief get the version of the DSP code */
  inline uint32_t DspCodeVersion() const {return m_DspHeader.CodeVersion;}

  /** @brief get the Event number counted by the DSP code*/
  inline uint32_t DspEventCounter() const {return m_DspHeader.EventCounter;}

  /** @brief get the FEB Event ID*/
  inline uint16_t FebELVL1Id() const {return m_ELVL1Id;}

  /** @brief get the FEB Bunch Crossing ID*/
  inline uint16_t FebBCId() const {return m_BCId;}

  /** @brief get the ROD block sizes */
  inline uint16_t RodResults1Size() const {return m_Results1Size;}
  inline uint16_t RodResults2Size() const {return m_Results2Size;}
  inline uint16_t RodRawDataSize()  const {return m_RawDataSize;}

  /** @brief get the number of samples and cells above threshold */
  inline uint16_t NbSamples()     const {return m_NbSamples;}
  inline uint16_t NbSweetCells1() const {return m_NbSweetCells1;}
  inline uint16_t NbSweetCells2() const {return m_NbSweetCells2;}
  inline uint32_t OnlineChecksum()  const { return m_OnlineChecksum; }
  inline uint32_t OfflineChecksum() const { return m_OfflineChecksum; }
  inline bool     ChecksumVerification() const { return m_OnlineChecksum==m_OfflineChecksum; }

  /** @brief get the FEB Control Word #1*/
  inline const std::vector <uint16_t> & FebCtrl1()  const {return m_Ctrl1;}

  /** @brief get the FEB Control Word #2*/
  inline const std::vector <uint16_t> & FebCtrl2()  const {return m_Ctrl2;}

  /** @brief get the FEB Control Word #3*/
  inline const std::vector <uint16_t> & FebCtrl3()  const {return m_Ctrl3;}

  /** @brief get the ROD Status*/
  inline uint32_t RodStatus() const {return m_Status;}

  /** @brief get the SCA's*/
  inline const std::vector <uint16_t> & SCA() const { return m_SCA; }

  /** @brief Check functions: return true if mismatch in Event ID*/
  inline bool CheckErrorELVL1Id() const { return (m_RodHeader.ELVL1Id&0x1f)!=m_ELVL1Id; }

  /** @brief Check functions: return true if mismatch in Bunch Crossing ID*/
  inline bool CheckErrorBCId() const { return m_RodHeader.BCId!=((m_BCId+1)&0xfff); }

  /** @brief set the format version */
  inline void SetFormatVersion(const uint32_t formatVersion) 
    { m_RodHeader.FormatVersion=formatVersion; return; }

  /** @brief set the source Id */
  inline void SetSourceId(const uint32_t sourceID)
    { m_RodHeader.SourceId=sourceID; return; }

  /** @brief set the run number */
  inline void SetRunNumber(const uint32_t runNumber)
    { m_RodHeader.RunNumber=runNumber; return; }

  /** @brief set the EventID */
  inline void SetELVL1Id(const uint16_t elvl1Id)
    { m_RodHeader.ELVL1Id=elvl1Id; return; }

  /** @brief set the Bunch Crossing ID */
  inline void SetBCId(const uint16_t bcid)
    { m_RodHeader.BCId=bcid; return; }

  /** @brief set the ROD block sizes */
  inline void SetRodResults1Size(const uint16_t size)
    { m_Results1Size=size; return; }
  inline void SetRodResults2Size(const uint16_t size)
    { m_Results2Size=size; return; }
  inline void SetRodRawDataSize (const uint16_t size)
    { m_RawDataSize=size; return; }

  /** @brief set the number of samples and cells above thresholds */
  inline void SetNbSweetCells1(const uint16_t n)
    { m_NbSweetCells1=n; return; }
  inline void SetNbSweetCells2(const uint16_t n)
    { m_NbSweetCells2=n; return; }
  inline void SetNbSamples(const uint16_t n)
    { m_NbSamples=n; return; }
  inline void SetOnlineChecksum(const uint32_t checksum)
    { m_OnlineChecksum=checksum; return; }
  inline void SetOfflineChecksum(const uint32_t checksum)
    { m_OfflineChecksum=checksum; return; }

  /** @brief set the Level1 trigger type ID */
  inline void SetLVL1TigType(const uint32_t lvl1ttype)
    { m_RodHeader.LVL1TigType=lvl1ttype; return; }

  /** @brief set the Detector event type ID */
  inline void SetDetEventType(const uint32_t detEvType)
    { m_RodHeader.DetEventType=detEvType; return; }

  /** @brief set the version of the DSP code */
  inline void SetDspCodeVersion(const uint32_t codeVersion)
    { m_DspHeader.CodeVersion=codeVersion; return; }

  /** @brief set the Event number counted by the DSP code*/
  inline void SetDspEventCounter(const uint32_t eventCounter)
    { m_DspHeader.EventCounter=eventCounter; return; }

  /** @brief set the FEB Event ID*/
  inline void SetFebELVL1Id(const uint16_t elvl1Id)
    { m_ELVL1Id=elvl1Id; return; }

  /** @brief set the FEB Bunch Crossing ID*/
  inline void SetFebBCId(const uint16_t bcid)
    { m_BCId=bcid; return; }

  /** @brief set the FEB Control Word #1*/
  inline void SetFebCtrl1(const uint16_t ctrl1)
    { m_Ctrl1.push_back(ctrl1); return; }

  /** @brief set the FEB Control Word #2*/
  inline void SetFebCtrl2(const uint16_t ctrl2)
    { m_Ctrl2.push_back(ctrl2); return; }

  /** @brief set the FEB Control Word #3*/
  inline void SetFebCtrl3(const uint16_t ctrl3)
    { m_Ctrl3.push_back(ctrl3); return; }

  /** @brief set the ROD Status*/
  inline void SetRodStatus(const uint32_t status)
    { m_Status=status; return; }

  /** @brief set the SCA's*/
  inline void SetFebSCA(const uint16_t sca)
    { m_SCA.push_back(degray(sca)); return; }

 private:

  /** @brief like explained in: http://mathworld.wolfram.com/GrayCode.html */
  int degray(unsigned int x);

  /** @brief Feb Identifier */
  const HWIdentifier m_FEBId;  

  /** @brief  ROD-Header, always present (is part of the FEB-Header) */
  struct {
    uint32_t FormatVersion;
    uint32_t SourceId;
    uint32_t RunNumber;
    uint16_t ELVL1Id;
    uint16_t BCId;
    uint32_t LVL1TigType;
    uint32_t DetEventType;
  } m_RodHeader;

  /** @brief DSP-Header (most of it is actually in the DSP-Trailer) */
  struct {
    uint32_t CodeVersion;  // DSP code version
    uint32_t EventCounter; // DSP event counter
  } m_DspHeader;

  /** @brief FEB EventId */
  uint16_t m_ELVL1Id; 

  /** @brief FEB BCId */
  uint16_t m_BCId;    

  uint16_t m_Results1Size;
  uint16_t m_Results2Size;
  uint16_t m_RawDataSize;
  uint16_t m_NbSweetCells1;
  uint16_t m_NbSweetCells2;
  uint16_t m_NbSamples;
  uint32_t m_OnlineChecksum;
  uint32_t m_OfflineChecksum;

  /** @brief ROD Status word */
  uint32_t m_Status;  

  /** @brief SCA number for each samples */
  std::vector<uint16_t> m_SCA; 

  /** @brief FEB Control word 1 */
  std::vector<uint16_t> m_Ctrl1;    

  /** @Brief FEB Control word 2 */
  std::vector<uint16_t> m_Ctrl2;  

  /** @brief FEB Control word 3 */
  std::vector<uint16_t> m_Ctrl3;  
};

#endif
