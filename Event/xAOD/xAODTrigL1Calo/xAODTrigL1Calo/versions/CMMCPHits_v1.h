// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMCPHits_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMCPHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMCPHITS_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of CMMCPHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $

  class CMMCPHits_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CMMCPHits_v1();

      /// get crate number
      int crate() const;
      /// set crate number
      void setCrate(int);

      /// get dataID
      int dataID() const;
      /// set dataID
      void setDataID(int);

      /// get peak 
      int peak() const;
      /// set peak
      void setPeak(int);

      /// get hitsVec0 - hits for all time slices
      const std::vector<unsigned int>& hitsVec0() const;
      /// set hitsVec0 - hits for all time slices
      void setHitsVec0(const std::vector<unsigned int>&);

      /// get hitsVec1 - hits for all time slices
      const std::vector<unsigned int>& hitsVec1() const;
      /// set hitsVec1 - hits for all time slices
      void setHitsVec1(const std::vector<unsigned int>&);  

      /// get errorVec0 - errors for all time slices
      const std::vector<int>& errorVec0() const;
      /// set errorVec0 - errors for all time slices
      void setErrorVec0(const std::vector<int>&);

      /// get errorVec1 - errors for all time slices
      const std::vector<int>& errorVec1() const;
      /// set errorVec1 - errors for all time slices
      void setErrorVec1(const std::vector<int>&);        

      /// add hits. Internally calls setHitsVecX(hitsX) and setErrorVecX(errorX)
      void addHits(const std::vector<unsigned int>& hits0,
                  const std::vector<unsigned int>& hits1,
                  const std::vector<int>& error0,
                  const std::vector<int>& error1); 
      
      /// get hit word0 for hitsVec0[peak] - time slice that (hopefully) contains the collision
      unsigned int hitWord0() const;
      /// get hit word1 for hitsVec1[peak] - time slice that (hopefully) contains the collision
      unsigned int hitWord1() const;
      /// get error for errorVec0[peak] - time slice that (hopefully) contains the collision
      int error0() const;
      /// get error for errorVec1[peak] - time slice that (hopefully) contains the collision
      int error1() const;
      
      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation 
      /// See above for description
      inline unsigned int HitWord0() const {return this->hitWord0();}
      inline unsigned int HitWord1() const {return this->hitWord1();}
      inline int Error0() const {return this->error0();}
      inline int Error1() const {return this->error1();}
      const std::vector<unsigned int>& HitsVec0() const {return this->hitsVec0();}
      const std::vector<unsigned int>& HitsVec1() const {return this->hitsVec1();}
      const std::vector<int>& ErrorVec0() const {return this->errorVec0();}
      const std::vector<int>& ErrorVec1() const {return this->errorVec1();}
      
  }; // class CMMCPHits_v1
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CMMCPHITS_V1_H
