// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: L1TopoRawData_v1.h 652824 2015-03-09 23:24:32Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATA_V1_H
#define XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATA_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD {

  /// Description of L1TopoRawData_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  /// @author Simon George <s.george@rhul.ac.uk>
  ///
  /// $Revision: 652824 $
  /// $Date: 2015-03-10 00:24:32 +0100 (Tue, 10 Mar 2015) $

  class L1TopoRawData_v1 : public SG::AuxElement{
    public:
      /// Default constructor
      L1TopoRawData_v1();
      
      /// Default desturctor
      virtual ~L1TopoRawData_v1(){}       
      
      /// initialize      
      virtual void initialize(const std::vector<uint32_t>& dataWords,
                              const std::vector<uint32_t>& statusWords,
                              const uint32_t errorWord,
                              const uint32_t sourceID);
      
      /// get dataWords
      const std::vector<uint32_t>& dataWords() const;
      /// set dataWords
      void setDataWords(const std::vector<uint32_t>&);
      
      
      /// get statusWords
      const std::vector<uint32_t>& statusWords() const;
      /// set statusWords
      void setStatusWords(const std::vector<uint32_t>&);
      
      /// get error
      uint32_t errorWord() const;
      /// set error
      void setErrorWord(const uint32_t);
      
      /// get sourceID
      uint32_t sourceID() const;
      /// set sourceID
      void setSourceID(const uint32_t);      
      
      
  }; // class L1TopoRawData_v1
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_L1TOPORAWDATA_V1_H
