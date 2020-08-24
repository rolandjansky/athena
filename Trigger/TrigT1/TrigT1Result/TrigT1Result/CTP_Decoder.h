/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1_CTP_DECODER_H
#define TRIGT1_CTP_DECODER_H

#include <cstdint>
#include <bitset>
#include <vector>

#include "AthenaBaseComps/AthMessaging.h"
#include "TrigT1Result/CTP_RDO.h"

class MsgStream;

class CTP_BC {
  
  /**
   *   $Date: 2007-07-10 14:58:38 $
   * 
   *   @short Helper class to decode the CTP data for one
   *   bunch-crossing.
   *
   *   @see CTP_Decoder
   *   @see CTP_RDO
   *
   *   @author David Berge <berge@cern.ch>
   *   @author Eleanor Dobson <eleanor.dobson@cern.ch>
   *
   *   @version \$ $
   *
   */
  
public:
  //! Helper to dump data for debugging
  void dumpData(MsgStream&) const;
  
  //!Set the CTP data format version (needed before extracting any information)
  void setCTPVersion(unsigned int ctpVersionNumber) {
    m_ctpVersion = CTPdataformatVersion(ctpVersionNumber);
  }
  
  
  //! Accessor to obtain std::bitset containing the bunch-crossing
  //  identifier (BCID)
  std::bitset<32> getBCIDBitSet() const;
  //! Get BCID as unsigned integer
  uint32_t getBCID() const;
  //! Return string with BCID in binary format
  std::string printBCID() const;
  
  //! Accessor to obtain random-trigger input bits
  std::bitset<32> getRandomTrigBitSet() const;
  //! Random-trigger inputs as uint32_t
  uint32_t getRandomTrig() const;
  //! Return string with random trigger in binary format
  std::string printRandomTrig() const;
  
  //! Accessor to obtain prescaled-clock input bits
  std::bitset<32> getPrescaledClockBitSet() const;
  //! Prescaled-clock inputs as uint32_t
  uint32_t getPrescaledClock() const;
  //! Return string with prescaled clock in binary format
  std::string printPrescaledClock() const;
  
  //! Set auxiliary PIT word, which is the one containing the 12-bit BCID (bit
  //0..11), the random trigger (bit 12,13) and prescaled clock (bit
  //14,15) 
  void setPITWordAux( uint32_t word ) { m_pitAux = word; }
  //! Get PIT word 5 bitset
  const std::bitset<32> & getPITWordAux() const {
    return m_pitAux;
  }
  //! Get auxiliary PIT word as string in binary format
  std::string printPITWordAux() const;
	
  //! Set TIP words
  void setTIP(std::vector<uint32_t> words);
  //! get bitset of TIP words
  const std::bitset<512> & getTIP() const {
    return m_tip;
  }
  //! Set TIP word number 'pos'
  void setTIPWord( uint32_t word, uint32_t pos = 0);
  //! Obtain TIP bitpattern string (binary format)
  std::string printTIP() const;
	
  
  //! Set TBP (Trigger Before Prescales) words
  void setTBP(std::vector<uint32_t> words);
  //! get bitset of TBP words
  const std::bitset<512> & getTBP() const {
    return m_tbp;
  }
  //! Set TBP word number 'pos'
  void setTBPWord( uint32_t word, uint32_t pos = 0);
  //! Obtain TBP bitpattern string (binary format)
  std::string printTBP() const;
  
  //! Set TAP (Trigger After Prescales) words
  void setTAP(std::vector<uint32_t> words);
  //! Set TAP word number 'pos'
  void setTAPWord( uint32_t word, uint32_t pos = 0);
  //! get bitset of TAP words
  const std::bitset<512> & getTAP() const {
    return m_tap;
  }
  //! Obtain TAP bitpattern string (binary format)
  std::string printTAP() const;
  
  //! Set TAV (Trigger After Veto) words
  void setTAV(std::vector<uint32_t> words);
  //! get bitset of TAV words
  const std::bitset<512> & getTAV() const {
    return m_tav;
  }
  //! Set TAV word number 'pos'
  void setTAVWord( uint32_t word, uint32_t pos = 0);
  //! Obtain TAV bitpattern string (binary format)
  std::string printTAV() const;
  
private:
  //! Bitsets containing bit-patterns of trigger inputs and trigger
  //items
  //std::bitset<320> m_pit;
  //std::bitset<192> m_fpi;
  std::bitset<512> m_tip;
  std::bitset<512> m_tbp;
  std::bitset<512> m_tap;
  std::bitset<512> m_tav;
  //! Contains BCID, random trigger and prescaled clock
  std::bitset<32>  m_pitAux;
  
  CTPdataformatVersion m_ctpVersion{0};
};





class CTP_Decoder : public AthMessaging {
  
  /**
   *   $Date: 2007-07-10 14:58:38 $
   * 
   *   @short Helper class to decode the CTP data fragment.
   *
   *          The CTP ROD data fragment contains the LVL1 trigger
   *          inputs (160 PITs), and the final trigger result in three
   *          stages, before pre-scale factors, after pre-scale
   *          factors, and after veto (dead-time, busy). All of these
   *          trigger bits can be stored for a number of
   *          bunch-crossings (BCs), up to 63, where the L1A can be
   *          almost anywhere within this window. 
   *          
   *          In addition, the GPS time-stamp of the LVL1 trigger
   *          decision is stored as part of the data fragment.
   *
   *
   *     @see CTP_RDO
   *
   *  @author David Berge <berge@cern.ch>
   *  @author Eleanor Dobson <eleanor.dobson@cern.ch>
   *
   *  @version \$ $
   *
   */
  
public:
  //! Default constructor
  CTP_Decoder();

  //! Set RDO and fill internal variables from the data object
  void setRDO(const CTP_RDO* rdo);
  //! Accessor function to retrieve pointer to RDO object
  const CTP_RDO* getRDO() const {
    return m_rdo;
  }
  //! Get vector reference of bunch-crossing data of the current event
  const std::vector<CTP_BC>& getBunchCrossings() {
    return m_BCs;
  }
  //! Get data of a single bunch-crossing
  const CTP_BC& getBunchCrossing(unsigned int pos) {
    return m_BCs[pos];
  }
  
  /*! Test the LVL1 trigger result for a certain trigger item
   @param itemNo The trigger item that shall be tested
   @param bcPos The bunch-crossing in the readout window that is to
   be checked
   @return A boolean value is returned indicating whether or not the
   trigger item has fired for a given event
   */
  bool checkTrigger(unsigned int itemNo,unsigned int bcPos);
  
  /*! Test the LVL1 trigger result after pre-scale factors are
   applied, before the final trigger decision, which includes the
   trigger masks, is taken. Here one sees all triggers of an event,
   irrespective of the trigger mask.
   @param itemNo The trigger item that shall be tested
   @param bcPos The bunch-crossing in the readout window that is to
   be checked
   @return A boolean value is returned indicating whether or not the
   trigger item is on before the veto step for a given event
   */
  bool checkTriggerAfterPrescale(unsigned int itemNo,unsigned int bcPos);
  
  /*! Function to obtain a vector with the numbers of the trigger
   items that fired for the current event.
   @param bcPos The bunch-crossing in the readout window that is to
   be used
   @return A vector<unsigned int> with the list of trigger items that
   fired in the event (trigger-item numbers run from 1 to 256 for run-I and from 1 to 512 for run-II).
   */
  std::vector<unsigned int> getAllTriggers(unsigned int bcPos);
  
  //! Helper to dump data for debugging
  void dumpData() const;

private:
  //! The RDO member
  const CTP_RDO* m_rdo;
  //! Vector of BCs for the current event
  std::vector<CTP_BC> m_BCs;
};

#endif
