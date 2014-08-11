// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -------------------------------------------------------------
//             Code produced by D3PDMakerReader
//
//  author: Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
// -------------------------------------------------------------

#include <TPRegexp.h>
#include <TList.h>
#include <TDataMember.h>

#include "../TrigCostD3PD/TrigDBKeysD3PDObject.h"

ClassImp( D3PDReader::TrigDBKeysD3PDObject )

namespace D3PDReader {

   /**
    * This constructor should be used when the object will be used to read
    * variables from an existing ntuple. The object will also be able to
    * output variables, but it will also need to read them from somewhere.
    *
    * @param master Reference to the variable holding the current event number
    * @param prefix Prefix of the variables in the D3PD
    */
   TrigDBKeysD3PDObject::TrigDBKeysD3PDObject( const ::Long64_t& master, const char* prefix )
      : UserD3PDObject( master, prefix ),
        fHandles(),
        fFromInput( kTRUE ),
        fPrefix( prefix ) {

      SetVarHandles( &master );
   }

   /**
    * This constructor can be used when the object will only have to output
    * (and temporarily store) new information into an output ntuple. For
    * instance when one wants to create a selected/modified list of information.
    *
    * @param prefix Prefix of the variables in the D3PD
    */
   TrigDBKeysD3PDObject::TrigDBKeysD3PDObject( const char* prefix )
      : UserD3PDObject( prefix ),
        fHandles(),
        fFromInput( kFALSE ),
        fPrefix( prefix ) {

      SetVarHandles( 0 );
   }

   /**
    * @returns The branch name prefix used by the object
    */
   const char* TrigDBKeysD3PDObject::GetPrefix() const {

      return fPrefix;
   }

   /**
    * @param prefix The prefix that should be used for the variables
    */
   void TrigDBKeysD3PDObject::SetPrefix( const char* prefix ) {

      // Call the base class's function:
      UserD3PDObject::SetPrefix( prefix );

      // Remember the prefix:
      fPrefix = prefix;

      // Set all the variable names:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->SetName( ::TString( prefix ) + itr->first );
      }

      return;
   }

   /**
    * This function should be called every time a new TFile is opened
    * by your analysis code.
    *
    * @param tree Pointer to the TTree with the variables
    */
   void TrigDBKeysD3PDObject::ReadFrom( TTree* tree ) {

      // Check if the object will be able to read from the TTree:
      if( ! fFromInput ) {
         Error( "ReadFrom", "The object was not created with the correct" );
         Error( "ReadFrom", "constructor to read data from a D3PD!" );
         return;
      }

      // Call the base class's function:
      UserD3PDObject::ReadFrom( tree );

      // Call ReadFrom(...) on all the variables:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->ReadFrom( tree );
      }

      return;
   }

   /**
    * This function can be called to connect the active variables of the object
    * to an output TTree. It can be called multiple times, then the variables
    * will be written to multiple TTrees.
    *
    * @param tree Pointer to the TTree where the variables should be written
    */
   void TrigDBKeysD3PDObject::WriteTo( TTree* tree ) {

      // Call the base class's function:
      UserD3PDObject::WriteTo( tree );

      // Call WriteTo(...) on all the variables:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->WriteTo( tree );
      }

      return;
   }

   /**
    * This is a convenience function for turning the branches active or
    * inactive conveniently. If the parameter is set to <code>kTRUE</code>
    * then the branches available from the input which match the given
    * pattern are turned active.
    * When it's set to <code>kFALSE</code> then all the variables matching
    * the pattern are turned inactive.
    *
    * @param active Flag behaving as explained above
    * @param pattern Regular expression specifying which branches to modify
    */
   void TrigDBKeysD3PDObject::SetActive( ::Bool_t active, const ::TString& pattern ) {

      // Call the base class's function:
      UserD3PDObject::SetActive( active, pattern );

      ::TPRegexp re( pattern );

      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         if( ! re.Match( fPrefix + itr->first ) ) continue;
         if( active ) {
            if( itr->second->IsAvailable() ) itr->second->SetActive( active );
         } else {
            itr->second->SetActive( active );
         }
      }

      return;
   }

   /**
    * This function can be used to read in all the branches from the input
    * TTree which are set active for writing out. This can simplify writing
    * event selector codes immensely. Remember to set the desired variable
    * active before calling this function.
    */
   void TrigDBKeysD3PDObject::ReadAllActive() {

      // Check if it makes sense to call this function:
      if( ! fFromInput ) {
         static ::Bool_t wPrinted = kFALSE;
         if( ! wPrinted ) {
            Warning( "ReadAllActive", "Function only meaningful when used on objects" );
            Warning( "ReadAllActive", "which are used to read information from a D3PD" );
            wPrinted = kTRUE;
         }
      }

      // Call the base class's function:
      UserD3PDObject::ReadAllActive();

      // Read in the current entry for each active variable:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         if( ! itr->second->IsActive() ) continue;
         itr->second->ReadCurrentEntry();
      }

      return;
   }

   /**
    * This function can be used to get information about the access
    * pattern/statistics of the job. It should be called at the end of
    * an analysis job to get the information about the performance of the
    * analysis.
    *
    * @returns An object describing the D3PD access statistics
    */
   // D3PDReadStats TrigDBKeysD3PDObject::GetStatistics() const {

   //    // The result object:
   //    D3PDReadStats result = UserD3PDObject::GetStatistics();

   //    // Add the statistics from each variable to the result:
   //    std::map< ::TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
   //    std::map< ::TString, VarHandleBase* >::const_iterator end = fHandles.end();
   //    for( ; itr != end; ++itr ) {
   //       result.AddVariable( itr->second->GetStatistics() );
   //    }

   //    return result;
   // }

   /**
    * This function can be used to copy the contents of the entire object
    * for a given event. This can be useful for instance when the user
    * wants to copy all information to an output file, and modify it a bit,
    * and only then write it out.
    *
    * @param parent The object to copy the information from
    * @returns This same object, for convenience reasons
    */
   TrigDBKeysD3PDObject& TrigDBKeysD3PDObject::Set( const TrigDBKeysD3PDObject& parent ) {

      // Check if this function can be used on the object:
      if( fFromInput ) {
         Error( "Set", "Objects used for reading a D3PD can't be modified!" );
         return *this;
      }

      // Call the base class's function:
      UserD3PDObject::Set( parent );

      if( parent.HLTPSK.IsAvailable() && HLTPSK.IsActive() ) {
         HLTPSK() = parent.HLTPSK();
      } else {
         HLTPSK() = 0;
      }
      if( parent.L1PSK.IsAvailable() && L1PSK.IsActive() ) {
         L1PSK() = parent.L1PSK();
      } else {
         L1PSK() = 0;
      }
      if( parent.SMK.IsAvailable() && SMK.IsActive() ) {
         SMK() = parent.SMK();
      } else {
         SMK() = 0;
      }

      return *this;
   }

   /**
    * This function is used internally to access VarHandle members
    * by name. This is necessary to push some setup from compile time
    * to run time. It may sound weird, but it makes a lot of sense for large
    * classes.
    *
    * @param name The name of the C++ variable (not of the branch)
    * @returns A pointer to the VarHandle object
    */
   VarHandleBase* TrigDBKeysD3PDObject::GetVarHandle( const char* name ) {

      if( ! ::strcmp( name, "HLTPSK" ) ) {
         return &HLTPSK;
      }
      else if( ! ::strcmp( name, "L1PSK" ) ) {
         return &L1PSK;
      }
      else if( ! ::strcmp( name, "SMK" ) ) {
         return &SMK;
      }

      Error( "GetVarHandle", "Variable \"%s\" unknown", name );
      return 0;
   }

   /**
    * This function is used internally to set up all the VarHandle members
    * of the class. It speeds up compilation *a lot* to do this at run-time
    * like this, instead of putting a lot of lines of code operating on
    * the std::map member.
    *
    * @param master Pointer to the master index, or a null pointer
    */
   void TrigDBKeysD3PDObject::SetVarHandles( const ::Long64_t* master ) {

      // Create a list of variable-branch name pairs:
      static const Int_t NVARNAMES = 3;
      static const char* VARNAMES[ NVARNAMES ][ 2 ] = {
         { "HLTPSK", "HLTPSK" },
         { "L1PSK", "L1PSK" },
         { "SMK", "SMK" }
      };

      // Set up the fHandles map using this list:
      for( Int_t i = 0; i < NVARNAMES; ++i ) {
         VarHandleBase* vh = GetVarHandle( VARNAMES[ i ][ 0 ] );
         vh->SetName( fPrefix + VARNAMES[ i ][ 1 ] );
         vh->SetMaster( master );
         fHandles[ VARNAMES[ i ][ 0 ] ] = vh;
      }

      return;
   }

} // namespace D3PDReader
