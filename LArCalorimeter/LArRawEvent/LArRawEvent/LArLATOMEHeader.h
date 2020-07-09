/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARLATOMEHEADER_H
#define LARLATOMEHEADER_H

#include "Identifier/HWIdentifier.h"
#include <vector>

/** 
@class LArLATOMEHeader
@brief Holds information from the LATOME Header

@author Pavol Strizenec
*/

class LArLATOMEHeader
{
 public: 
  
  /** @brief Constructor with Id*/
  LArLATOMEHeader(const uint32_t lid);

  /** @brief Constructor with all numbers*/
  LArLATOMEHeader(const uint32_t sourceid, const uint32_t latomeId, const uint16_t nchan, const uint16_t bcid, const uint32_t l1Id);

  /** @brief Destructor */
  ~LArLATOMEHeader(){ }

  /** @brief get the source Id */
  inline uint32_t SourceId() const {return m_sourceId;}

  /** @brief get the latome Id */
  inline uint32_t LatomeId() const {return m_latomeId;}

  /** @brief get the number of channels*/
  inline uint16_t NChans() const {return m_nChannels;}

  /** @brief get the Bunch Crossing IDs*/
  inline uint16_t BCId() const {return m_BCId;}

  /** @brief get the L1 Id */
  inline uint32_t L1Id() const {return m_l1Id;}

  /** @brief set the LATOME Bunch Crossing ID*/
  inline void SetBCId(const uint16_t bcid)
    { m_BCId=bcid; return; }

  /** @brief set the LATOME Channel*/
  inline void SetNChan(const uint16_t nchan)
    { m_nChannels=nchan; return; }

  /** @brief set the LATOME SourceId*/
  inline void SetSourceId(const uint32_t source)
    { m_sourceId=source; return; }

  /** @brief set the latome Id*/
  inline void SetLatomeId(const uint32_t source)
    { m_latomeId=source; return; }

  /** @brief set the latome Id*/
  inline void SetL1Id(const uint32_t source)
    { m_l1Id=source; return; }

 private:

  /** @brief LATOME sourceId */
  uint32_t m_sourceId; 

  /** @brief LATOME Id */
  uint32_t m_latomeId; 

  /** @brief LATOME channels */
  uint16_t m_nChannels; 

  /** @brief LATOME BCId from TTC*/
  uint16_t m_BCId;    

  /** @brief extended LVL1 Id */
  uint32_t m_l1Id; 

};

#endif
