/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TOPORDO_L1TOPORDO_H
#define L1TOPORDO_L1TOPORDO_H

#include <cstdint>
#include <vector>
#include <string>

/**
 * @short The class that represents the raw data received from an L1Topo board.
 *
 * This class encapsulated the status and data words received from an L1Topo board via its DAQ or ROI readout. 
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
  L1TopoRDO();
  //! Destructor
  ~L1TopoRDO();

  //! get the data words
  const std::vector<uint32_t>& getDataWords () const;
  //! set the data words 
  void setDataWords (const std::vector<uint32_t>);
  //! get the status words
  const std::vector<uint32_t>& getStatusWords () const;
  //! set the status words
  void setStatusWords (const std::vector<uint32_t>);
  //! get an integer used to report errors found during RAW to RDO conversion; to be documented...
  unsigned int getError() const;
  //! set the error value
  void setError(const unsigned int);
  //! get the source identifier, i.e. the module ID, of the ROD from which this RDO was filled
  unsigned int getSourceID() const;
  //! set the source identifier
  void setSourceID(const unsigned int);
  //! terse printout method used by RoIBResult
  const std::string dump() const;

 private:
  std::vector<uint32_t> m_dataWords;
  std::vector<uint32_t> m_statusWords;
  //unsigned int m_versionNumber;
  unsigned int m_error;
  unsigned int m_sourceID;
};

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( L1TopoRDO, 138978439, 0 )

#endif  // L1TOPORDO_L1TOPORDO_H
