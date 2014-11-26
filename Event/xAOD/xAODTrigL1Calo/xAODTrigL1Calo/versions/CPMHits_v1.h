// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMHits_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMHITS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  /// Description of CPMHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  

  class CPMHits_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CPMHits_v1();
       
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
      
      /// get hitsVec0  - hits for all time slices
      const std::vector<unsigned int>& hitsVec0() const; 
      /// set hitsVec0  - hits for all time slices
      void setHitsVec0(const std::vector<unsigned int>&);

      /// get hitsVec1  - hits for all time slices      
      const std::vector<unsigned int>& hitsVec1() const; 
      /// set hitsVec1  - hits for all time slices      
      void setHitsVec1(const std::vector<unsigned int>&);     
      
      // add hits. Internally calls setHitsVec0(hits0);setHitsVec1(hits1);
      void addHits(const std::vector<unsigned int>&  hits0,const std::vector<unsigned int>&  hits1);
      
      /// get hitword for hitsVec0[peak] - time slice that (hopefully) contains the collision
      unsigned int hitWord0() const;
      /// get hitword for hitsVec1[peak] - time slice that (hopefully) contains the collision
      unsigned int hitWord1() const;      
      
      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation
      /// See above for description        
      inline unsigned int HitWord0() const {return this->hitWord0();}
      inline unsigned int HitWord1() const {return this->hitWord1();}

  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CPMHITS_V1_H
