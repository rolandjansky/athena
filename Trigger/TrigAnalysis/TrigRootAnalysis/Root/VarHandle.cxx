/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandle.cxx 514776 2012-08-24 08:17:59Z krasznaa $

// Local include(s):
#include "../TrigRootAnalysis/VarHandle.h"

namespace D3PDReaderPriv {

   VarHandleBase::VarHandleBase( ::TObject* parent, const char* name,
                                 const ::Long64_t* master )
      : fMaster( master ), fParent( parent ), fFromInput( kFALSE ),
        fInTree( 0 ), fInBranch( 0 ), fAvailable( UNKNOWN ), fName( name ),
        fActive( kFALSE ), fType( "" ),
        fEntriesRead(), fBranchSize(), fZippedSize() {

   }

   ::TObject* VarHandleBase::GetParent() const {

      return fParent;
   }

   void VarHandleBase::SetParent( ::TObject* parent ) {

      fParent = parent;
      return;
   }

   const char* VarHandleBase::GetName() const {

      return fName;
   }

   void VarHandleBase::SetName( const char* name ) {

      fName = name;
      return;
   }

   const char* VarHandleBase::GetType() const {

      return fType;
   }

   void VarHandleBase::SetType( const char* type ) {

      fType = type;
      return;
   }

   const ::Long64_t* VarHandleBase::GetMaster() const {

      return fMaster;
   }

   void VarHandleBase::SetMaster( const ::Long64_t* master ) {

      fMaster = master;
      return;
   }

   ::Bool_t VarHandleBase::IsActive() const {

      return fActive;
   }

   void VarHandleBase::SetActive( ::Bool_t active ) {

      fActive = active;
      return;
   }

   ::Bool_t VarHandleBase::IsAvailable() const {

      if( ! fFromInput ) return kTRUE;
      switch( fAvailable ) {

      case AVAILABLE:
         return kTRUE;
         break;
      case UNAVAILABLE:
         return kFALSE;
         break;
      case UNKNOWN:
         {
            if( ! fInTree ) return kTRUE;
            ::Bool_t temp = kFALSE;
            fAvailable = ( temp = fInTree->GetBranch( GetName() ) ) ? AVAILABLE :
                         UNAVAILABLE;
            return temp;
         }
         break;
      default:
         // This should really never be reached...
         break;
      }

      // It's just here to make the compiler happy:
      return kFALSE;
   }

   ::Bool_t VarHandleBase::ConnectVariable( void* var, ::TClass* realClass,
                                            EDataType dtype, Bool_t isptr ) const {

      if( ! fInTree ) {
         fParent->Error( ::TString( GetName() ) + "()", "Object not connected yet!" );
         return kFALSE;
      }
      if( ! fInTree->GetBranch( GetName() ) ) {
         fParent->Error( ::TString( GetName() ) + "()",
                         "The following variable doesn't exist: %s",
                         GetName() );
         return kFALSE;
      }
      // Make sure that the branch/tree is not in MakeClass mode:
      fInTree->SetMakeClass( 0 );
      // Only call this function when the user asks for it. It's quite expensive...
      fInTree->SetBranchStatus( ::TString( GetName() ) + "*", 1 );
      if( fInTree->SetBranchAddress( GetName(), var, &fInBranch,
                                     realClass, dtype, isptr ) ) {
         fParent->Error( ::TString( GetName() ) + "()",
                         "Couldn't connect variable to branch: %s", GetName() );
         return kFALSE;
      }

      return kTRUE;
   }

   void VarHandleBase::UpdateBranch() const {

      if( *fMaster != fInBranch->GetReadEntry() ) {
         fInBranch->GetEntry( *fMaster );
      }

      return;
   }

   void VarHandleBase::UpdateStat( ::TBranch* br ) const {

      fEntriesRead.push_back( 0 );
      fBranchSize.push_back( ( ::Float_t ) br->GetTotalSize( "*" ) /
                             ( ::Float_t ) br->GetEntries() );
      fZippedSize.push_back( ( ::Float_t ) br->GetZipBytes( "*" ) /
                             ( ::Float_t ) br->GetEntries() );

      return;
   }

   const char* VarHandleBase::RootType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         fParent->Error( "RootType", "Received complex object description" );
         return "";
      }

      switch( typeid_type[ 0 ] ) {

      case 'c':
         return "B";
         break;
      case 'h':
         return "b";
         break;
      case 's':
         return "S";
         break;
      case 't':
         return "s";
         break;
      case 'i':
         return "I";
         break;
      case 'j':
         return "i";
         break;
      case 'f':
         return "F";
         break;
      case 'd':
         return "D";
         break;
      case 'x':
         return "L";
         break;
      case 'y':
         return "l";
         break;
      case 'b':
         return "O";
         break;

      }

      fParent->Error( "RootType",
                      "Unknown primitive type encountered: %s",
                      typeid_type );
      return "";
   }

   const char* VarHandleBase::RootCppType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         fParent->Error( "RootCppType", "Received complex object description" );
         return "";
      }

      switch( typeid_type[ 0 ] ) {

      case 'c':
         return "Char_t";
         break;
      case 'h':
         return "UChar_t";
         break;
      case 's':
         return "Short_t";
         break;
      case 't':
         return "UShort_t";
         break;
      case 'i':
         return "Int_t";
         break;
      case 'j':
         return "UInt_t";
         break;
      case 'f':
         return "Float_t";
         break;
      case 'd':
         return "Double_t";
         break;
      case 'x':
         return "Long64_t";
         break;
      case 'y':
         return "ULong64_t";
         break;
      case 'b':
         return "Bool_t";
         break;

      }

      fParent->Error( "RootCppType",
                      "Unknown primitive type encountered: %s",
                      typeid_type );
      return "";
   }

} // namespace D3PDReaderPriv
