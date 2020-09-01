/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// TO DO: encapsulate logic from raw data words! And fill the data

#ifndef TRIGT1RESULT_CTP_RDO_H
#define TRIGT1RESULT_CTP_RDO_H

// std includes
#include <cstdint>

// STL includes
#include <vector>
#include <string>


#include "CTPfragment/CTPdataformatVersion.h"

class CTP_RDO {
  
  enum WordType {/*PIT, FPI,*/ TIP, TBP, TAP, TAV, EXTRA};
  typedef enum WordType WordType;
  
 public:

   /**
    * Default constructor needed for pool converters
    * requires that @c setCTPVersionNumber is called before anything else
    */
  CTP_RDO();


  /// Constructor takes the number of BCs of the readout window as argument
  CTP_RDO(unsigned int ctpVersionNumber, const uint32_t nBCs = 1, uint32_t nExtraWords=0);

  /// Normal constructor taking the data words of one or several BCs as argument
  CTP_RDO(unsigned int ctpVersionNumber, std::vector<uint32_t>&& data, uint32_t nExtraWords=0);

  /// empty default destructor
  ~CTP_RDO();

  const std::vector<uint32_t> & getDataWords () const {
    return m_dataWords;
  }

  void setTimeSec(const uint32_t sec);
  void setTimeNanoSec(const uint32_t nano);

  uint32_t getTimeSec() const;
  uint32_t getTimeNanoSec() const;
  uint32_t getNumberOfBunches() const;
  uint32_t getL1AcceptBunchPosition() const;
  uint32_t getNumberOfAdditionalWords() const;
  uint32_t getTimeSinceLastL1A() const;
  uint32_t getTurnCounter() const;

  //std::vector<uint32_t> getPITWords() const; 
  //std::vector<uint32_t> getFPIWords() const;
  std::vector<uint32_t> getTIPWords() const;
  std::vector<uint32_t> getTBPWords() const;
  std::vector<uint32_t> getTAPWords() const;
  std::vector<uint32_t> getTAVWords() const;
  std::vector<uint32_t> getEXTRAWords() const;

  const CTPdataformatVersion & getCTPVersion() const;
  void setCTPVersionNumber( unsigned int ctpVersion );
  unsigned int getCTPVersionNumber() const { return m_ctpVersionNumber; }
    
  //void setPITWord(const unsigned int i, const uint32_t word);
  //void setFPIWord(const unsigned int i, const uint32_t word);
  void setTIPWord(const unsigned int i, const uint32_t word);
  void setTBPWord(const unsigned int i, const uint32_t word);
  void setTAPWord(const unsigned int i, const uint32_t word);
  void setTAVWord(const unsigned int i, const uint32_t word);

  void selectBunch(const uint32_t iBC);
  void selectNextBunch();
  void setNumberOfBunches(const uint32_t nBCs);
  void setL1AcceptBunchPosition(const uint8_t);
  void setTurnCounter(const uint32_t);
  void setNumberOfAdditionalWords(const uint32_t nExtraWords);


  // helper functions

  //! dump raw object content to string
  const std::string dump() const;
  //! print object content in a human readable format to string
  const std::string print(const bool longFormat = false) const;

  // constants

  static constexpr unsigned int SIZEOF_WORDS = 32;  //!< number of bits in one data word (32)

 private:

  // data members 
    
  unsigned int m_ctpVersionNumber {0};        //!< number of the CTP version to be used
  CTPdataformatVersion m_ctpDataFormat;  //!< CTP data format for a specified version
  std::vector<uint32_t> m_dataWords;      //!< raw data words
  uint8_t  m_l1AcceptPosition {0};            //!< bunch position, from which the level1 accept was calculated
  uint32_t m_turnCounter {0};                //!< turn counter
  uint32_t m_numberOfBunches {0};             //!< number of bunches in raw data (transient)
  uint32_t m_activeBunch {0};                 //!< active bunch, for book keepting (transient)
  uint32_t m_numberOfAdditionalWords {0};     //!< number of configurable extra words in the fragment

  // helper functions
  
  void setWord(const unsigned int i, const uint32_t word);
  std::vector<uint32_t> getWords(WordType type) const;

}; // class CTP_RDO

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( CTP_RDO, 6014, 0 )

#endif // TRIGT1RESULT_CTP_RDO_H
