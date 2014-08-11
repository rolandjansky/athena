/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $

// ROOT include(s):
#include <TPRegexp.h>

// Local include(s):
#include "../TrigCostD3PD/UserD3PDObject.h"

namespace D3PDReader {

   UserD3PDObjectElement::UserD3PDObjectElement( size_t index,
                                                 const UserD3PDObject& parent )
      : ::TNamed(), fParent( &parent ), fIndex( index ), fProxies() {

   }

   UserD3PDObjectElement::
   UserD3PDObjectElement( const UserD3PDObjectElement& parent )
      : TNamed( parent ), fParent( parent.fParent ), fIndex( parent.fIndex ),
        fProxies() {

   }

   UserD3PDObjectElement::~UserD3PDObjectElement() {

      // Delete the allocated objects:
      std::map< ::TString, VarProxyBase* >::iterator itr = fProxies.begin();
      std::map< ::TString, VarProxyBase* >::iterator end = fProxies.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
   }

   size_t UserD3PDObjectElement::GetIndex() const {

      return fIndex;
   }

   UserD3PDObject::UserD3PDObject( const ::Long64_t& master,
                                   const char* prefix )
      : D3PDObjectBase(), fMaster( &master ), fPrefix( prefix ), fProxies(),
        fHandles(), fExtraHandles(), fFromInput( kTRUE ),
        fInTree( 0 ) {

   }

   UserD3PDObject::UserD3PDObject( const char* prefix )
      : D3PDObjectBase(), fMaster( 0 ), fPrefix( prefix ), fProxies(),
        fHandles(), fExtraHandles(), fFromInput( kFALSE ),
        fInTree( 0 ) {

   }

   UserD3PDObject::~UserD3PDObject() {

      // Delete the allocated objects:
      std::map< ::TString, VarHandleBase* >::iterator itr = fHandles.begin();
      std::map< ::TString, VarHandleBase* >::iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      // Although the code almost certainly crashes when such dummy objects
      // are created, there's still some chance that they need to be cleaned:
      std::vector< VarHandleBase* >::iterator eitr = fExtraHandles.begin();
      std::vector< VarHandleBase* >::iterator eend = fExtraHandles.end();
      for( ; eitr != eend; ++eitr ) {
         delete *eitr;
      }
   }

   /**
    * @returns The branch name prefix used by the object
    */
   const char* UserD3PDObject::GetPrefix() const {

      return fPrefix;
   }

   /**
    * @param prefix The prefix that should be used for the variables
    */
   void UserD3PDObject::SetPrefix( const char* prefix ) {

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
   void UserD3PDObject::ReadFrom( TTree* tree ) {

      // Check if the object will be able to read from the TTree:
      if( ! fFromInput ) {
         Error( "ReadFrom",
                "The object was not created with the correct" );
         Error( "ReadFrom",
                "constructor to read data from a D3PD!" );
         return;
      }

      // Call ReadFrom(...) on all the variables:
      std::map< TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         // Ignore the variables that are created only now:
         if( ! itr->second->GetMaster() ) continue;
         itr->second->ReadFrom( tree );
      }

      // Remember the pointer:
      fInTree = tree;

      return;
   }

   /**
    * This function can be called to connect the active variables of the object
    * to an output TTree. It can be called multiple times, then the variables
    * will be written to multiple TTrees.
    *
    * @param tree Pointer to the TTree where the variables should be written
    */
   void UserD3PDObject::WriteTo( TTree* tree ) {

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
   void UserD3PDObject::SetActive( ::Bool_t active, const ::TString& pattern ) {

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
   void UserD3PDObject::ReadAllActive() {

      // Check if it makes sense to call this function:
      if( ! fFromInput ) {
         static ::Bool_t wPrinted = kFALSE;
         if( ! wPrinted ) {
            Warning( "ReadAllActive",
                     "Function only meaningful when used on objects" );
            Warning( "ReadAllActive",
                     "which are used to read information from a D3PD" );
            wPrinted = kTRUE;
         }
      }

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
   // D3PDReadStats UserD3PDObject::GetStatistics() const {

   //    // The result object:
   //    D3PDReadStats result;

   //    // Add the statistics from each variable to the result:
   //    std::map< ::TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
   //    std::map< ::TString, VarHandleBase* >::const_iterator end = fHandles.end();
   //    for( ; itr != end; ++itr ) {
   //       result.AddVariable( itr->second->GetStatistics() );
   //    }

   //    return result;
   // }

   /**
    * User variables can't be copied like this at the moment.
    */
   UserD3PDObject& UserD3PDObject::Set( const UserD3PDObject& parent ) {

      if( parent.fHandles.size() ) {
         Error( "Set",
                "User variables can not be copied usig this function!" );
      }
      return *this;
   }

   /**
    * This function makes it easier to clear out the object completely.
    * It cleares all the vector variables, and sets the element number
    * variable to 0. Very useful when performing object selection.
    * The option argument is not used at the moment for anything.
    * It's only there because the <code>Clear</code> function defined in
    * TObject has this parameter as well.
    *
    * @param option Ignored at the moment
    */
   void UserD3PDObject::Clear( Option_t* ) {

      // Clear each variable:
      std::map< ::TString, VarHandleBase* >::const_iterator itr = fHandles.begin();
      std::map< ::TString, VarHandleBase* >::const_iterator end = fHandles.end();
      for( ; itr != end; ++itr ) {
         itr->second->Clear();
      }

      return;
   }

   /**
    * User variables can't be copied like this at the moment.
    */
   UserD3PDObject& UserD3PDObject::Add( const UserD3PDObjectElement& el ) {

      if( el.fProxies.size() ) {
         Error( "Add",
                "User variables can not be copied usig this function!" );
      }
      return *this;
   }

   /**
    * This operator can be used to get access to one element in the
    * collection. This element can then be passed around between parts
    * of the analysis code easily.
    *
    * This version is useful when modifying the variable contents through
    * the proxy objects.
    *
    * @param index Index of the element inside the collection
    */
   UserD3PDObjectElement& UserD3PDObject::operator[]( size_t index ) {

      while( fProxies.size() <= index ) {
         fProxies.push_back( new UserD3PDObjectElement( fProxies.size(), *this ) );
      }
      return *fProxies[ index ];
   }

   /**
    * This operator can be used to get access to one element in the
    * collection. This element can then be passed around between parts
    * of the analysis code easily.
    *
    * This version is useful when only reading the variables.
    *
    * @param index Index of the element inside the collection
    */
   const UserD3PDObjectElement& UserD3PDObject::operator[]( size_t index ) const {

      while( fProxies.size() <= index ) {
         fProxies.push_back( new UserD3PDObjectElement( fProxies.size(), *this ) );
      }
      return *fProxies[ index ];
   }

   /**
    * A convenience operator for adding an 'element' to this collection.
    *
    * @see Add
    * @param el The 'element' that should be added to the collection
    */
   UserD3PDObject& UserD3PDObject::operator+=( const UserD3PDObjectElement& el ) {

      return this->Add( el );
   }

} // namespace D3PDReader
