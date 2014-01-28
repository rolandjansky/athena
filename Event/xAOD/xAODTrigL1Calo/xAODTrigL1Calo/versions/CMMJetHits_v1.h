// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMJetHits_v1.h 576279 2013-12-19 14:35:54Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMJETHITS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMJETHITS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  /// Description of CMMJetHits_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576279 $
  /// $Date: 2013-12-19 15:35:54 +0100 (Thu, 19 Dec 2013) $  

  class CMMJetHits_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CMMJetHits_v1();
      
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
      
      /// get hitsVec - hits for all time slices
      const std::vector<unsigned int>& hitsVec() const;
      /// set hitsVec - hits for all time slices
      void setHitsVec(const std::vector<unsigned int>&);
          
      /// get errorVec - errors for all time slices
      const std::vector<int>& errorVec() const;
      /// set errorVec - errors for all time slices
      void setErrorVec(const std::vector<int>&);
            
      /// add hits. Internally calls setHitsVec(hits);setErrorVec(error);
      void addHits(const std::vector<unsigned int>& hits,const std::vector<int>& error); 
      
      /// get hits for hitsVec[peak] - time slice that (hopefully) contains the collision
      unsigned int hits() const;
      
      /// get error for errorVec[peak] - time slice that (hopefully) contains the collision
      int error() const;

      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation
      /// See above for description      
      inline unsigned int Hits() const {return this->hits();}
      inline int Error() const {return this->error();}
  };
} // namespace xAOD

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::CMMJetHits_v1 , 207472471 , 1 )
#endif // not XAOD_STANDALONE

#endif // XAODTRIGL1CALO_VERSIONS_CMMJETHITS_V1_H
