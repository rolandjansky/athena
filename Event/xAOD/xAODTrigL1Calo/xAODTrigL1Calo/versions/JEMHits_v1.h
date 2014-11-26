// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMHits_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of JEMHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  

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

#endif // XAODTRIGL1CALO_VERSIONS_JEMHITS_V1_H
