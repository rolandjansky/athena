/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 539451 2013-03-01 08:33:45Z krasznaa $

// ROOT include(s):
#include <TError.h>

// Local include(s):
#include "../TrigCostD3PD/VarHandle.h"
//#include "../TrigCostD3PD/D3PDPerfStats.h"

namespace D3PDReader {

   VarHandleBase::VarHandleBase( const char* name,
                                 const ::Long64_t* master )
      : m_master( master ), m_fromInput( kFALSE ),
        m_inTree( 0 ), m_inBranch( 0 ), m_available( UNKNOWN ), m_name( name ),
        m_active( kFALSE ), m_type( "" ),
        m_entriesRead(), m_branchSize(), m_zippedSize() {

#ifdef COLLECT_D3PD_READING_STATISTICS
      //D3PDPerfStats::Instance();
#endif // COLLECT_D3PD_READING_STATISTICS
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

   // VariableStats VarHandleBase::GetStatistics() const {

   //    // Calculate the statistics:
   //    ::Long64_t readEntries = 0;
   //    ::Long64_t unzippedBytes = 0;
   //    ::Long64_t zippedBytes = 0;
   //    for( size_t i = 0; i < m_entriesRead.size(); ++i ) { 
   //       readEntries += m_entriesRead[ i ];
   //       unzippedBytes += static_cast< ::Long64_t >( m_branchSize[ i ] *
   //                                                   m_entriesRead[ i ] );
   //       zippedBytes += static_cast< ::Long64_t >( m_zippedSize[ i ] *
   //                                                 m_entriesRead[ i ] );
   //    }

   //    // Now return the "smart" object:
   //    return VariableStats( GetName(), GetType(),
   //                          m_entriesRead.size(), readEntries,
   //                          unzippedBytes, zippedBytes );
   // }

   ::Bool_t VarHandleBase::ConnectVariable( void* var, ::TClass* realClass,
                                            EDataType dtype, Bool_t isptr ) const {

      if( ! m_inTree ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "Object not connected yet!" );
         return kFALSE;
      }
      if( ! m_inTree->GetBranch( GetName() ) ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "The following variable doesn't exist: %s",
                  GetName() );
         return kFALSE;
      }
#ifdef ACTIVATE_BRANCHES
      // Only call this function when the user asks for it. It's quite expensive...
      m_inTree->SetBranchStatus( ::TString( GetName() ) + "*", 1 );
#endif // ACTIVATE_BRANCHES
      if( m_inTree->SetBranchAddress( GetName(), var, &m_inBranch,
                                     realClass, dtype, isptr ) ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "Couldn't connect variable to branch: %s", GetName() );
         return kFALSE;
      }

#ifdef COLLECT_D3PD_READING_STATISTICS
      //UpdateStat( m_inBranch );
#endif // COLLECT_D3PD_READING_STATISTICS

      return kTRUE;
   }

   void VarHandleBase::UpdateBranch() const {
      //Cov warning
      if (!m_inBranch) return;

      if( *m_master != m_inBranch->GetReadEntry() ) {
         m_inBranch->GetEntry( *m_master );
#ifdef COLLECT_D3PD_READING_STATISTICS
        // ++( m_entriesRead.back() );
#endif // COLLECT_D3PD_READING_STATISTICS
      }

      return;
   }

   // void VarHandleBase::UpdateStat( ::TBranch* br ) const {

   //    m_entriesRead.push_back( 0 );
   //    m_branchSize.push_back( ( ::Float_t ) br->GetTotalSize( "*" ) /
   //                           ( ::Float_t ) br->GetEntries() );
   //    m_zippedSize.push_back( ( ::Float_t ) br->GetZipBytes( "*" ) /
   //                           ( ::Float_t ) br->GetEntries() );

   //    D3PDPerfStats::Instance()->NewTreeAccessed( m_inTree );

   //    return;
   // }

   const char* VarHandleBase::RootType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         ::Error( "D3PDReader::VarHandleBase::RootType",
                  "Received complex object description" );
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

      ::Error( "D3PDReader::VarHandleBase::RootType",
               "Unknown primitive type encountered: %s",
               typeid_type );
      return "";
   }

   const char* VarHandleBase::RootCppType( const char* typeid_type ) const {

      if( strlen( typeid_type ) != 1 ) {
         ::Error( "D3PDReader::VarHandleBase::RootCppType",
                  "Received complex object description" );
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

      ::Error( "D3PDReader::VarHandleBase::RootCppType",
               "Unknown primitive type encountered: %s",
               typeid_type );
      return "";
   }

} // namespace D3PDReader
