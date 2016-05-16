/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: VarHandle.cxx 726663 2016-02-28 01:22:15Z ssnyder $

// Local include(s):
#include "../TrigRootAnalysis/VarHandle.h"

namespace D3PDReaderPriv {

   VarHandleBase::VarHandleBase( ::TObject* parent, const char* name,
                                 const ::Long64_t* master )
      : m_master( master ), m_parent( parent ), m_fromInput( kFALSE ),
        m_inTree( 0 ), m_inBranch( 0 ), m_available( UNKNOWN ), m_name( name ),
        m_active( kFALSE ), m_type( "" ),
        m_entriesRead(), m_branchSize(), m_zippedSize() {

   }

   ::TObject* VarHandleBase::GetParent() const {

      return m_parent;
   }

   void VarHandleBase::SetParent( ::TObject* parent ) {

      m_parent = parent;
      return;
   }

   const char* VarHandleBase::GetName() const {

      return m_name;
   }

   void VarHandleBase::SetName( const char* name ) {

      m_name = name;
      return;
   }

   const char* VarHandleBase::GetType() const {

      return m_type;
   }

   void VarHandleBase::SetType( const char* type ) {

      m_type = type;
      return;
   }

   const ::Long64_t* VarHandleBase::GetMaster() const {

      return m_master;
   }

   void VarHandleBase::SetMaster( const ::Long64_t* master ) {

      m_master = master;
      return;
   }

   ::Bool_t VarHandleBase::IsActive() const {

      return m_active;
   }

   void VarHandleBase::SetActive( ::Bool_t active ) {

      m_active = active;
      return;
   }

   ::Bool_t VarHandleBase::IsAvailable() const {

      if( ! m_fromInput ) return kTRUE;
      switch( m_available ) {

      case AVAILABLE:
         return kTRUE;
         break;
      case UNAVAILABLE:
         return kFALSE;
         break;
      case UNKNOWN:
         {
            if( ! m_inTree ) return kTRUE;
            ::Bool_t temp = kFALSE;
            m_available = ( temp = m_inTree->GetBranch( GetName() ) ) ? AVAILABLE :
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

      if( ! m_inTree ) {
         m_parent->Error( ::TString( GetName() ) + "()", "Object not connected yet!" );
         return kFALSE;
      }
      if( ! m_inTree->GetBranch( GetName() ) ) {
         m_parent->Error( ::TString( GetName() ) + "()",
                         "The following variable doesn't exist: %s",
                         GetName() );
         return kFALSE;
      }
      // Make sure that the branch/tree is not in MakeClass mode:
      m_inTree->SetMakeClass( 0 );
      // Only call this function when the user asks for it. It's quite expensive...
      m_inTree->SetBranchStatus( ::TString( GetName() ) + "*", 1 );
      if( m_inTree->SetBranchAddress( GetName(), var, &m_inBranch,
                                     realClass, dtype, isptr ) ) {
         m_parent->Error( ::TString( GetName() ) + "()",
                         "Couldn't connect variable to branch: %s", GetName() );
         return kFALSE;
      }

      return kTRUE;
   }

   void VarHandleBase::UpdateBranch() const {

      if( *m_master != m_inBranch->GetReadEntry() ) {
         m_inBranch->GetEntry( *m_master );
      }

      return;
   }

   void VarHandleBase::UpdateStat( ::TBranch* br ) const {

      m_entriesRead.push_back( 0 );
      m_branchSize.push_back( ( ::Float_t ) br->GetTotalSize( "*" ) /
                             ( ::Float_t ) br->GetEntries() );
      m_zippedSize.push_back( ( ::Float_t ) br->GetZipBytes( "*" ) /
                             ( ::Float_t ) br->GetEntries() );

      return;
   }

   const char* VarHandleBase::RootType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         m_parent->Error( "RootType", "Received complex object description" );
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

      m_parent->Error( "RootType",
                      "Unknown primitive type encountered: %s",
                      typeid_type );
      return "";
   }

   const char* VarHandleBase::RootCppType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         m_parent->Error( "RootCppType", "Received complex object description" );
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

      m_parent->Error( "RootCppType",
                      "Unknown primitive type encountered: %s",
                      typeid_type );
      return "";
   }

} // namespace D3PDReaderPriv
