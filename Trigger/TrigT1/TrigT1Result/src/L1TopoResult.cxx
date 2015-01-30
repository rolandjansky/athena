/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Result/L1TopoResult.h"
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/Helpers.h"
#include <sstream>
#include <iomanip>


namespace ROIB {

  L1TopoResult::L1TopoResult( const Header& header, const Trailer& trailer, const L1TopoRDO& rdo)
    : m_header(header), m_trailer(trailer), m_RDO(rdo)
  {}


  L1TopoResult::L1TopoResult()
    : m_header(), m_trailer(), m_RDO()
  {}

  L1TopoResult::~L1TopoResult()
  {}

  const Header& L1TopoResult::header() const {
    return m_header;
  }

  const Trailer& L1TopoResult::trailer() const {
    return m_trailer;
  }

  const L1TopoRDO& L1TopoResult::rdo() const {
    return m_RDO;
  }

  const std::string L1TopoResult::dump() const {
    std::ostringstream s;
    s << " [" << this->header().dump() << "] ";
    s << " [" << m_RDO.dump() << "] ";
    s << " [" << this->trailer().dump() << "] ";
    return s.str();
  }

} // namespace ROIB
