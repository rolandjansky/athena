// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataAccess.h 726661 2016-02-28 01:18:32Z ssnyder $
#ifndef TRIGROOTANALYSIS_DATAACCESS_H
#define TRIGROOTANALYSIS_DATAACCESS_H

// STL include(s):
#include <vector>

// ROOT include(s):
#include <TNamed.h>
#include <TString.h>

// Local include(s):
#include "IDataAccess.h"
#include "VarHandle.h"

// Forward declaration(s):
class TTree;
class TBranch;

/**
 * @short Common namespace for all D3PD-related code
 *
 *        Maybe using "Root" as namespace would've been nicer, but now we're
 *        stuck with using "D3PD". By idea was that this would make it clear
 *        that this code is meant for analyzing D3PD files.
 *
 * $Revision: 726661 $
 * $Date: 2016-02-28 02:18:32 +0100 (Sun, 28 Feb 2016) $
 */
namespace D3PD {

   /**
    *  @short Namespace for non-public classes
    *
    *         To separate the classes that one is not supposed to use directly,
    *         they are put into this namespace.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 726661 $
    * $Date: 2016-02-28 02:18:32 +0100 (Sun, 28 Feb 2016) $
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
       * $Revision: 726661 $
       * $Date: 2016-02-28 02:18:32 +0100 (Sun, 28 Feb 2016) $
       */
      class DataAccess : public virtual ::TNamed,
                         public virtual D3PD::Trig::IDataAccess {

      public:
         /// Constructor specifying the event-wise tree
         DataAccess( ::TTree* eventTree = 0, const ::TString& prefix = "trig_" );
         /// Destructor
         ~DataAccess();

         /// Get the pointer to the event-wise tree currently being used
         ::TTree* GetEventTree() const;
         /// Set the pointer to the event-wise tree
         ::Bool_t SetEventTree( ::TTree* eventTree );
         /// Write the branches to an output tree
         ::Bool_t WriteToTree( ::TTree* outputTree, ::Int_t detailLevel = 10 );

         /// Load a new entry (new event)
         ::Int_t GetEntry( ::Long64_t entry, ::Int_t getall = 0 );

         /// Get the detail level that the D3PD was produced with
         virtual ::Int_t GetDetailLevel();

         /// Get the Super Master Key of the current event
         virtual ::Int_t GetSMK();
         /// Get the LVL1 prescale key of the current event
         virtual ::Int_t GetL1PSK();
         /// Get the HLT prescale key of the current event
         virtual ::Int_t GetHLTPSK() ;

         /// Function for retrieving the encoded LVL1 result
         virtual const std::vector< unsigned int >* GetL1Result( L1ResultType type );
         /// Function for retrieving the encoded LVL2 result
         virtual const std::vector< short >*        GetL2Result( HLTResultType type );
         /// Function for retrieving the encoded EF result
         virtual const std::vector< short >*        GetEFResult( HLTResultType type );

      private:
         /// Initialize the object from the specified TTree
         ::Bool_t Initialize();
         /// Reset the object
         void Reset();

         ::TTree* m_tree; ///< The event-wise TTree
         ::TString m_prefix; ///< Prefix for the trigger branch names
         ::Bool_t m_initialized; ///< Flag for knowing if the branches have been connected
         ::Int_t m_detailLevel; ///< The detail level guessed from the available branches
         ::TString m_trigCostTreeName; ///< The tree name used by the TrigCost code. Used to identify when dealing with a TrigCost ntuple.

         //
         // Variables for accessing the event-wise information:
         //
         ::Long64_t m_entry; ///< The current entry in the event-wise TTree
#ifndef __MAKECINT__
         D3PDReaderPriv::VarHandle< ::UInt_t > m_smk;
         D3PDReaderPriv::VarHandle< ::UInt_t > m_l1psk;
         D3PDReaderPriv::VarHandle< ::UInt_t > m_hltpsk;

         D3PDReaderPriv::VarHandle< std::vector< unsigned int >* > m_l1_tav;
         D3PDReaderPriv::VarHandle< std::vector< short >* >        m_l2_physics;
         D3PDReaderPriv::VarHandle< std::vector< short >* >        m_ef_physics;

         D3PDReaderPriv::VarHandle< std::vector< unsigned int >* > m_l1_tbp;
         D3PDReaderPriv::VarHandle< std::vector< unsigned int >* > m_l1_tap;
         D3PDReaderPriv::VarHandle< std::vector< short >* >        m_l2_raw;
         D3PDReaderPriv::VarHandle< std::vector< short >* >        m_ef_raw;

         D3PDReaderPriv::VarHandle< std::vector< short >* > m_l2_resurrected;
         D3PDReaderPriv::VarHandle< std::vector< short >* > m_ef_resurrected;

         D3PDReaderPriv::VarHandle< std::vector< short >* > m_l2_passedThrough;
         D3PDReaderPriv::VarHandle< std::vector< short >* > m_ef_passedThrough;
#endif // __MAKECINT__

         ClassDef( D3PD::Trig::DataAccess, 0 )

      }; // class DataAccess

   } // namespace Trig

} // namespace D3PD

#endif // TRIGROOTANALYSIS_DATAACCESS_H
