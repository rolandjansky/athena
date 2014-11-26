// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CMMEtSums_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_CMMETSUMS_V1_H
#define XAODTRIGL1CALO_VERSIONS_CMMETSUMS_V1_H

#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  /// Description of CMMEtSums_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  

  class CMMEtSums_v1 : public SG::AuxElement{
    public:
      // Default constructor
      CMMEtSums_v1();

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
      
      /// Please note that in the context of CMMEtSums, et is total energy, not transverse energy
      /// For transverse energy, please use Pythagoras' theorem on ex and ey      

      /// get etVec - et for all time slices
      const std::vector<unsigned int>& etVec() const;
      /// set etVec  - et for all time slices
      void setEtVec(const std::vector<unsigned int>&);

      /// get exVec - ex for all time slices
      const std::vector<unsigned int>& exVec() const;
      /// set exVec - ex for all time slices
      void setExVec(const std::vector<unsigned int>&);

      /// get eyVec - ey for all time slices      
      const std::vector<unsigned int>& eyVec() const;
      /// set eyVec - ey for all time slices      
      void setEyVec(const std::vector<unsigned int>&);      

      /// get etErrorVec - et Error for all time slices
      const std::vector<int>& etErrorVec() const;
      /// set etErrorVec - et Error for all time slices
      void setEtErrorVec(const std::vector<int>&);

      /// get exErrorVec - ex Error for all time slices
      const std::vector<int>& exErrorVec() const;
      /// set exErrorVec - ex Error for all time slices
      void setExErrorVec(const std::vector<int>&);

      /// get eyErrorVec - ey Error for all time slices
      const std::vector<int>& eyErrorVec() const;
      /// set eyErrorVec - ey Error for all time slices
      void setEyErrorVec(const std::vector<int>&);  

      // add et. Internally calls setEtVec(Et); setEtErrorVec(EtError);
      void addEt(const std::vector<unsigned int>& Et,const std::vector<int>& EtError);
      // add ex. Internally calls setExVec(Ex); setExErrorVec(ExError);
      void addEx(const std::vector<unsigned int>& Ex,const std::vector<int>& ExError);
      // add ey. Internally calls setEyVec(Ey); setEyErrorVec(EyError);
      void addEy(const std::vector<unsigned int>& Ey,const std::vector<int>& EyError);

      // get et for etVec[peak] - time slice that (hopefully) contains the collision
      unsigned int et() const;
      // get ex for exVec[peak] - time slice that (hopefully) contains the collision
      unsigned int ex() const;
      // get ey for eyVec[peak] - time slice that (hopefully) contains the collision
      unsigned int ey() const;
      // get etError for etErrorVec[peak] - time slice that (hopefully) contains the collision
      int etError() const;
      // get exError for exErrorVec[peak] - time slice that (hopefully) contains the collision
      int exError() const;
      // get eyError for eyErrorVec[peak] - time slice that (hopefully) contains the collision
      int eyError() const; 

      /// Backwards Compatibility with Trigger/TrigT1/TrigT1CaloEvent
      /// Run1 EDM did not strictly follow camelCaseNotation
      /// See above for description
      inline const std::vector<unsigned int>& EtVec() const {return this->etVec();}
      inline const std::vector<unsigned int>& ExVec() const {return this->exVec();}
      inline const std::vector<unsigned int>& EyVec() const {return this->eyVec();}
      inline const std::vector<int>& EtErrorVec() const {return this->etErrorVec();}
      inline const std::vector<int>& ExErrorVec() const {return this->exErrorVec();}
      inline const std::vector<int>& EyErrorVec() const {return this->eyErrorVec();}
      inline unsigned int Et() const {return this->et();}
      inline unsigned int Ex() const {return this->ex();}
      inline unsigned int Ey() const {return this->ey();}
      inline int EtError() const {return this->etError();}
      inline int ExError() const {return this->exError();}
      inline int EyError() const {return this->eyError();}
      
  };
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_CMMEtSums_V1_H
