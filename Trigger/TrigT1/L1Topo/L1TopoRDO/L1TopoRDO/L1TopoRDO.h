/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_L1TOPORDO_H
#define L1TOPORDO_L1TOPORDO_H

#include <cstdint>
#include <vector>
#include <string>
#include "L1TopoRDO/Error.h"

/**
 * @short The class that represents the raw data received from an L1Topo board.
 *
 * This class encapsulated the status and data words received from an L1Topo
 *board via its DAQ or ROI readout.
 *
 * It is also used in the RoIBResult:
 * @see L1TopoResult
 *
 * For help decoding the data words,
 * @see L1Topo::Header
 * @see L1Topo::L1TopoTOB
 *
 * @author Simon George
 */

class L1TopoRDO {
 public:
  //! Constructor
  L1TopoRDO() :
  m_error(0),
  m_sourceID(0){ }

  //! get the data words
  const std::vector<uint32_t>& getDataWords() const;
  //! set the data words
  void setDataWords(std::vector<uint32_t>&&) noexcept;

  //! get the status words
  const std::vector<uint32_t>& getStatusWords() const;
  //! set the status words
  void setStatusWords(std::vector<uint32_t>&&) noexcept;

  //! get a vector of Errors found during RAW to RDO conversion; see enum and
  // print helpers
  std::vector<L1Topo::Error> getErrors() const;
  //! check if a certain error flag has been set
  bool checkError(L1Topo::Error) const;
  //! set a flag to record an error
  void setError(L1Topo::Error);

  //! Return error bit vector
  uint32_t getError() const { return m_error; }

  //! get the source identifier, i.e. the word representing the subdet type and
  // module ID, of the ROD from which this RDO was filled
  uint32_t getSourceID() const;
  //! set the source identifier
  void setSourceID(const uint32_t);

  //! check the module type, derived from the source ID and the L1Topo module
  // numbering convention; DAQ modules contain the full output
  bool isDAQModule() const;
  //! check the module type, derived from the source ID and the L1Topo module
  // numbering convention; ROI modules contain only the "L1Topo" TOB words and
  // are intended for the RoIB. They are also unpacked into the RoIBResult
  bool isROIModule() const;

  //! terse printout method used by RoIBResult
  const std::string dump() const;

 private:
  std::vector<uint32_t> m_dataWords;
  std::vector<uint32_t> m_statusWords;
  // unsigned int m_versionNumber;
  uint32_t m_error;
  uint32_t m_sourceID;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(L1TopoRDO, 138978439, 0)

#endif  // L1TOPORDO_L1TOPORDO_H
