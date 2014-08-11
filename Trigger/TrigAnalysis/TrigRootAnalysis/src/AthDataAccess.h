// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthDataAccess.h 502443 2012-05-25 10:43:25Z krasznaa $
#ifndef TRIGROOTANALYSIS_ATHDATAACCESS_H
#define TRIGROOTANALYSIS_ATHDATAACCESS_H

// ROOT include(s):
#include <Rtypes.h>

// Gaudi/Athena include(s):
#include "StoreGate/RVar.h"

// Local include(s):
#include "../TrigRootAnalysis/IDataAccess.h"

namespace D3PD {

   /**
    *  @short Athena implementation of the IDataAccess interface
    *
    *         This implementation of the IDataAccess interface is used as
    *         a component of the Athena version of ITrigDecisionToolD3PD.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 502443 $
    * $Date: 2012-05-25 12:43:25 +0200 (Fri, 25 May 2012) $
    */
   class AthDataAccess : public virtual D3PD::Trig::IDataAccess {

   public:
      /// Default constructor
      AthDataAccess();

      /// Get the detail level that the D3PD was produced with
      virtual ::Int_t GetDetailLevel() const;

      /// Get the Super Master Key of the current event
      virtual ::Int_t GetSMK() const;
      /// Get the LVL1 prescale key of the current event
      virtual ::Int_t GetL1PSK() const;
      /// Get the HLT prescale key of the current event
      virtual ::Int_t GetHLTPSK() const;

      /// Function for retrieving the encoded LVL1 result
      virtual const std::vector< unsigned int >*
      GetL1Result( L1ResultType type ) const;
      /// Function for retrieving the encoded LVL2 result
      virtual const std::vector< short >*
      GetL2Result( HLTResultType type ) const;
      /// Function for retrieving the encoded EF result
      virtual const std::vector< short >*
      GetEFResult( HLTResultType type ) const;

   private:
      /// Set the prefix of the variable names
      void setPrefix( const std::string& prefix );

      //
      // Variables coming from TrigDBKeysD3PDObject:
      //
      SG::RVar< ::UInt_t > m_smk;
      SG::RVar< ::UInt_t > m_l1psk;
      SG::RVar< ::UInt_t > m_hltpsk;

      //
      // Variables coming from TrigDecisionD3PDObject:
      //
      SG::RVar< std::vector< unsigned int > > m_l1_tav;
      SG::RVar< std::vector< short > >        m_l2_physics;
      SG::RVar< std::vector< short > >        m_ef_physics;

      SG::RVar< std::vector< unsigned int > > m_l1_tbp;
      SG::RVar< std::vector< unsigned int > > m_l1_tap;
      SG::RVar< std::vector< short > >        m_l2_raw;
      SG::RVar< std::vector< short > >        m_ef_raw;

      SG::RVar< std::vector< short > > m_l2_resurrected;
      SG::RVar< std::vector< short > > m_ef_resurrected;

      SG::RVar< std::vector< short > > m_l2_passedThrough;
      SG::RVar< std::vector< short > > m_ef_passedThrough;

   }; // class AthDataAccess

} // namespace D3PD

#endif // TRIGROOTANALYSIS_ATHDATAACCESS_H
