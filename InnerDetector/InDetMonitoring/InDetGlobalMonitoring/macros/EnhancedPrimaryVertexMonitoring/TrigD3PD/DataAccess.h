// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataAccess.h 339267 2011-01-07 16:30:10Z krasznaa $
#ifndef TRIGGERD3PDMAKER_DATAACCESS_H
#define TRIGGERD3PDMAKER_DATAACCESS_H

// STL include(s):
#include <vector>

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
#include "IDataAccess.h"

// Forward declaration(s):
class TTree;
class TBranch;

namespace D3PD {

   /**
    *  @short Namespace for non-public classes
    *
    *         To separate the classes that one is not supposed to use directly,
    *         they are put into this namespace.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 339277 $
    * $Date: 2011-01-07 17:30:10 +0100 (Fri, 07 Jan 2011) $
    */
   namespace Trig {

      /**
       *  @short Class handling the access to the trigger data in the events
       *
       *         The TDT implementation is broken into separate pieces. This piece
       *         takes care of accessing the trigger decision information.
       *
       *         Should not be used directly.
       *
       * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
       *
       * $Revision: 339277 $
       * $Date: 2011-01-07 17:30:10 +0100 (Fri, 07 Jan 2011) $
       */
      class DataAccess : public virtual TNamed,
                         public virtual IDataAccess {

      public:
         /// Constructor specifying the event-wise tree
         DataAccess( TTree* eventTree = 0, const TString& prefix = "trig_" );
         /// Destructor
         ~DataAccess();

         /// Get the pointer to the event-wise treee currently being used
         TTree* GetEventTree() const;
         /// Set the pointer to the event-wise treee
         Bool_t SetEventTree( TTree* eventTree );

         /// Load a new entry (new event)
         Int_t GetEntry( Long64_t entry, Int_t getall = 0 );

         /// Get the detail level that the D3PD was produced with
         virtual Int_t GetDetailLevel() const;

      protected:
         /// Get the Super Master Key of the current event
         virtual Int_t GetSMK() const;
         /// Get the LVL1 prescale key of the current event
         virtual Int_t GetL1PSK() const;
         /// Get the HLT prescale key of the current event
         virtual Int_t GetHLTPSK() const;

         /// Function for retrieving the encoded LVL1 result
         virtual const std::vector< unsigned int >* GetL1Result( L1ResultType type ) const;
         /// Function for retrieving the encoded LVL2 result
         virtual const std::vector< short >*        GetL2Result( HLTResultType type ) const;
         /// Function for retrieving the encoded EF result
         virtual const std::vector< short >*        GetEFResult( HLTResultType type ) const;

      private:
         /// Initialize the object from the specified TTree
         Bool_t Initialize();
         /// Reset the object
         void Reset();

         TTree* m_tree; ///< The event-wise TTree
         const TString m_prefix; ///< Prefix for the trigger branch names
         Bool_t m_initialized; ///< Flag for knowing if the branches have been connected
         Int_t m_detailLevel; ///< The detail level guessed from the available branches

         //
         // Variables for accessing the event-wise information:
         //
         UInt_t m_smk;
         UInt_t m_l1psk;
         UInt_t m_hltpsk;

         TBranch* m_b_smk;
         TBranch* m_b_l1psk;
         TBranch* m_b_hltpsk;

         std::vector< unsigned int >* m_l1_tav;
         std::vector< short >*        m_l2_physics;
         std::vector< short >*        m_ef_physics;

         TBranch* m_b_l1_tav;
         TBranch* m_b_l2_physics;
         TBranch* m_b_ef_physics;

         std::vector< unsigned int >* m_l1_tbp;
         std::vector< unsigned int >* m_l1_tap;
         std::vector< short >*        m_l2_raw;
         std::vector< short >*        m_ef_raw;

         TBranch* m_b_l1_tbp;
         TBranch* m_b_l1_tap;
         TBranch* m_b_l2_raw;
         TBranch* m_b_ef_raw;

         std::vector< short >* m_l2_resurrected;
         std::vector< short >* m_ef_resurrected;

         TBranch* m_b_l2_resurrected;
         TBranch* m_b_ef_resurrected;

         std::vector< short >* m_l2_passedThrough;
         std::vector< short >* m_ef_passedThrough;

         TBranch* m_b_l2_passedThrough;
         TBranch* m_b_ef_passedThrough;

         ClassDef( DataAccess, 0 )

      }; // class DataAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGGERD3PDMAKER_DATAACCESS_H
