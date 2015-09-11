// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RODHeader_v2.h 694350 2015-09-11 12:02:19Z krasznaa $
#ifndef XAODTRIGL1CALO_VERSIONS_RODHEADER_V2_H
#define XAODTRIGL1CALO_VERSIONS_RODHEADER_V2_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

/// Description of RODHeader_v2
///
/// @author alexander.mazurov@cern.ch
///
/// $Revision: 694350 $
/// $Date: 2015-09-11 14:02:19 +0200 (Fri, 11 Sep 2015) $

class RODHeader_v2 : public SG::AuxElement {
public:
  // Default constructor
  RODHeader_v2();
  /// Default desturctor
  virtual ~RODHeader_v2(){} 

  virtual void initialize(uint32_t version, uint32_t sourceId, uint32_t run,
                          uint32_t lvl1Id,  uint32_t bcId,     uint32_t trigType,
                          uint32_t lvl1DetType, const std::vector<uint32_t>& statusWords,
                          uint32_t nData);

  /// get version
  unsigned int version() const;
  /// set version
  void setVersion(unsigned int);

  /// get sourceId
  unsigned int sourceId() const;
  /// set sourceId
  void setSourceId(unsigned int);

  /// get run
  unsigned int run() const;
  /// set run
  void setRun(unsigned int);

  /// get lvl1Id
  unsigned int lvl1Id() const;
  /// set lvl1Id
  void setLvl1Id(unsigned int);

  /// get bcid
  unsigned int bcid() const;
  /// set bcid
  void setBcid(unsigned int);

  /// get trigType
  unsigned int trigType() const;
  /// set trigType
  void setTrigType(unsigned int);

  /// get detType
  unsigned int lvl1DetType() const;
  /// set detType
  void setLvl1DetType(unsigned int);

  /// get statusWords
  const std::vector<unsigned int>& statusWords() const;
  /// set statusWords
  void setStatusWords(const std::vector<unsigned int>&);

  /// get payloadSize
  unsigned int payloadSize() const;
  /// set payloadSize
  void setPayloadSize(unsigned int);

  // Header data
  int majorVersion()  const;
  int minorVersion()  const;
  int sourceID()      const;
  int subDetectorID() const;
  int moduleID()      const;
  int crate()         const;
  int sLink()         const;
  int dataType()      const;
  int runType()       const;
  int runNumber()     const;
  int extendedL1ID()  const;
  int ecrID()         const;
  int l1ID()          const;
  int bunchCrossing() const;
  int l1TriggerType() const;
  int detEventType()  const;
  int orbitCount()    const;
  int stepNumber()    const;
  int stepType()      const;

  // Status bits - word 1
  bool bcnMismatch()        const;
  bool gLinkTimeout()       const;
  bool dataTransportError() const;
  bool rodFifoOverflow()    const;
  bool lvdsLinkError()      const;
  bool cmmParityError()     const;
  bool gLinkError()         const;
  // Status bits - word 2
  bool limitedRoISet()      const;
  bool triggerTypeTimeout() const;

};
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_RODHEADER_V2_H
