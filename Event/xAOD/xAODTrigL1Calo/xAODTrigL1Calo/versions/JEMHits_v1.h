// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMHits_v1.h 576279 2013-12-19 14:35:54Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of JEMHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576279 $
  /// $Date: 2013-12-19 15:35:54 +0100 (Thu, 19 Dec 2013) $  

  class JEMHits_v1 : public SG::AuxElement{
    public:
      // Default constructor
      JEMHits_v1();
      
      /// get crate number
      int crate() const;
      /// set crate number
      void setCrate(int);
      
      /// get module
      int module() const;
      /// set module
      void setModule(int);      
      
      /// get peak
      int peak() const;
      /// set peak
      void setPeak(int);       
             
      /// get jetHitsVec - jetHits for all time slices
      const std::vector<unsigned int>& jetHitsVec() const; 
      /// set jetHitsVec - jetHits for all time slices
      void setJetHitsVec(const std::vector<unsigned int>&);
      
      /// Is this JEMEtSums forward?
      unsigned int forward() const;

      /// get jetHits for jetHitsVec[peak] - time slice that (hopefully) contains the collision
      unsigned int jetHits() const;
      
      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation 
      /// See above for description
      inline void addJetHits(const std::vector<unsigned int>&  hits) {this->setJetHitsVec(hits);}
      inline unsigned int JetHits() const {return this->jetHits();}
  };
} // namespace xAOD

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::JEMHits_v1 , 106316946 , 1 )
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H
