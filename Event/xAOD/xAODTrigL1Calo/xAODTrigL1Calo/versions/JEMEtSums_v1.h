// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMEtSums_v1.h 576279 2013-12-19 14:35:54Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMETSUMS_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMETSUMS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of JEMEtSums_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 576279 $
  /// $Date: 2013-12-19 15:35:54 +0100 (Thu, 19 Dec 2013) $  
  
  class JEMEtSums_v1 : public SG::AuxElement{
    public:
      // Default constructor
      JEMEtSums_v1();
      
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
      
      /// Please note that in the context of JEMEtSums, et is total energy, not transverse energy
      /// For transverse energy, please use Pythagoras' theorem on ex and ey
      
      /// get etVec - et for all time slices
      const std::vector<unsigned int>& etVec() const; 
      /// set etVec - et for all time slices
      void setEtVec(const std::vector<unsigned int>&);      
          
      /// get exVec - ex for all time slices
      const std::vector<unsigned int>& exVec() const;  
      /// set exVec - ex for all time slices
      void setExVec(const std::vector<unsigned int>&); 
      
      /// get eyVec - ey for all time slices
      const std::vector<unsigned int>& eyVec() const;
      /// set eyVec - ey for all time slices
      void setEyVec(const std::vector<unsigned int>&);  
      
      /// Is this JEMEtSums forward?
      unsigned int forward() const;
      
      /// get et for etVec[peak] - time slice that (hopefully) contains the collision
      unsigned int et() const;
      /// get ex for exVec[peak] - time slice that (hopefully) contains the collision
      unsigned int ex() const;
      /// get ey for eyVec[peak] - time slice that (hopefully) contains the collision
      unsigned int ey() const;
      
      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation 
      /// See above for description
      inline void addEt(const std::vector<unsigned int>&  Et) {this->setEtVec( Et );}
      inline void addEx(const std::vector<unsigned int>&  Ex) {this->setExVec( Ex );}
      inline void addEy(const std::vector<unsigned int>&  Ey) {this->setEyVec( Ey );}
      inline unsigned int Et() const {return this->et();}
      inline unsigned int Ex() const {return this->ex();}
      inline unsigned int Ey() const {return this->ey();}
      inline const std::vector<unsigned int>& EtVec() const {return this->etVec();}
      inline const std::vector<unsigned int>& ExVec() const {return this->exVec();}
      inline const std::vector<unsigned int>& EyVec() const {return this->eyVec();}
      
  };
} // namespace xAOD

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
  CLASS_DEF( xAOD::JEMEtSums_v1 , 168396611 , 1 )
#endif // not XAOD_STANDALONE
#endif // XAODTRIGL1CALO_VERSIONS_JEMETSUMS_V1_H
