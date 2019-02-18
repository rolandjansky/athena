// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMXEtSums_v1.h 687949 2015-08-06 15:48:49Z amazurov $
#ifndef XAODTRIGL1CALO_VERSIONS_CMXETSUMS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMXETSUMS_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>
#include <ostream>

namespace xAOD {

/// Description of CMXEtSums_v1
///
/// @author John Morris <john.morris@cern.ch>
///
/// $Revision: 687949 $
/// $Date: 2015-08-06 17:48:49 +0200 (Thu, 06 Aug 2015) $

class CMXEtSums_v1 : public SG::AuxElement {
public:
  enum Sources { REMOTE_STANDARD = 16,    REMOTE_RESTRICTED,
                 LOCAL_STANDARD,          LOCAL_RESTRICTED,
                 TOTAL_STANDARD,          TOTAL_RESTRICTED,
                 SUM_ET_STANDARD,         SUM_ET_RESTRICTED,
                 MISSING_ET_STANDARD,     MISSING_ET_RESTRICTED,
                 MISSING_ET_SIG_STANDARD, MAX_SOURCE
  };
public:
  /// Default constructor
  CMXEtSums_v1();
  /// Default desturctor
  virtual ~CMXEtSums_v1() {}

  /// initialize
  virtual void initialize(const uint8_t crate, const uint8_t source);

  /// initialize
  virtual void initialize(const uint8_t crate, const uint8_t source,
                          const std::vector<uint16_t>& etVec,
                          const std::vector<uint16_t>& exVec,
                          const std::vector<uint16_t>& eyVec,
                          const std::vector<uint32_t>& etErrorVec,
                          const std::vector<uint32_t>& exErrorVec,
                          const std::vector<uint32_t>& eyErrorVec,
                          const uint8_t peak);

  /// add data to existing object
  /// add Et
  void addEt(const std::vector<uint16_t>& etVec, const std::vector<uint32_t>& etErrorVec);
  /// add Ex
  void addEx(const std::vector<uint16_t>& exVec, const std::vector<uint32_t>& exErrorVec);
  /// add Ey
  void addEy(const std::vector<uint16_t>& eyVec, const std::vector<uint32_t>& eyErrorVec);

  /// get crate
  uint8_t crate() const;
  /// set crate
  void setCrate(uint8_t);

  /// get source
  uint8_t sourceComponent() const;
  /// set source
  void setSourceComponent(uint8_t);

  /// get peak
  uint8_t peak() const;
  /// set peak
  void setPeak(uint8_t);

  /// get etVec
  const std::vector<uint16_t>& etVec() const;
  /// set etVec
  void setEtVec(const std::vector<uint16_t>&);

  /// get exVec
  const std::vector<uint16_t>& exVec() const;
  /// set exVec
  void setExVec(const std::vector<uint16_t>&);

  /// get eyVec
  const std::vector<uint16_t>& eyVec() const;
  /// set eyVec
  void setEyVec(const std::vector<uint16_t>&);

  /// get etErrorVec
  const std::vector<uint32_t>& etErrorVec() const;
  /// set etErrorVec
  void setEtErrorVec(const std::vector<uint32_t>&);

  /// get exErrorVec
  const std::vector<uint32_t>& exErrorVec() const;
  /// set exErrorVec
  void setExErrorVec(const std::vector<uint32_t>&);

  /// get eyErrorVec
  const std::vector<uint32_t>& eyErrorVec() const;
  /// set eyErrorVec
  void setEyErrorVec(const std::vector<uint32_t>&);

  /// Peak functions - return value of peak bunch crossing
  /// Assuming we have got the timing calibration correct,
  /// This is the bunch crossing containing the collision

  /// get etVec at peak bunch crossing
  uint16_t et() const;

  /// get exVec at peak bunch crossing
  uint16_t ex() const;

  /// get eyVec at peak bunch crossing
  uint16_t ey() const;

  /// get etErrorVec at peak bunch crossing
  uint32_t etError() const;

  /// get exErrorVec at peak bunch crossing
  uint32_t exError() const;

  /// get eyErrorVec at peak bunch crossing
  uint32_t eyError() const;

}; // class CMXEtSums_v1


std::ostream &operator<<(std::ostream &os, const xAOD::CMXEtSums_v1 &el);

} // namespace xAOD
#endif

