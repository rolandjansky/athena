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
      : fMaster( master ), fFromInput( kFALSE ),
        fInTree( 0 ), fInBranch( 0 ), fAvailable( UNKNOWN ), fName( name ),
        fActive( kFALSE ), fType( "" ),
        fEntriesRead(), fBranchSize(), fZippedSize() {

#ifdef COLLECT_D3PD_READING_STATISTICS
      //D3PDPerfStats::Instance();
#endif // COLLECT_D3PD_READING_STATISTICS
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

   // VariableStats VarHandleBase::GetStatistics() const {

   //    // Calculate the statistics:
   //    ::Long64_t readEntries = 0;
   //    ::Long64_t unzippedBytes = 0;
   //    ::Long64_t zippedBytes = 0;
   //    for( size_t i = 0; i < fEntriesRead.size(); ++i ) { 
   //       readEntries += fEntriesRead[ i ];
   //       unzippedBytes += static_cast< ::Long64_t >( fBranchSize[ i ] *
   //                                                   fEntriesRead[ i ] );
   //       zippedBytes += static_cast< ::Long64_t >( fZippedSize[ i ] *
   //                                                 fEntriesRead[ i ] );
   //    }

   //    // Now return the "smart" object:
   //    return VariableStats( GetName(), GetType(),
   //                          fEntriesRead.size(), readEntries,
   //                          unzippedBytes, zippedBytes );
   // }

   ::Bool_t VarHandleBase::ConnectVariable( void* var, ::TClass* realClass,
                                            EDataType dtype, Bool_t isptr ) const {

      if( ! fInTree ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "Object not connected yet!" );
         return kFALSE;
      }
      if( ! fInTree->GetBranch( GetName() ) ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "The following variable doesn't exist: %s",
                  GetName() );
         return kFALSE;
      }
#ifdef ACTIVATE_BRANCHES
      // Only call this function when the user asks for it. It's quite expensive...
      fInTree->SetBranchStatus( ::TString( GetName() ) + "*", 1 );
#endif // ACTIVATE_BRANCHES
      if( fInTree->SetBranchAddress( GetName(), var, &fInBranch,
                                     realClass, dtype, isptr ) ) {
         ::Error( "D3PDReader::VarHandleBase::ConnectVariable",
                  "Couldn't connect variable to branch: %s", GetName() );
         return kFALSE;
      }

#ifdef COLLECT_D3PD_READING_STATISTICS
      //UpdateStat( fInBranch );
#endif // COLLECT_D3PD_READING_STATISTICS

      return kTRUE;
   }

   void VarHandleBase::UpdateBranch() const {
      //Cov warning
      if (!fInBranch) return;

      if( *fMaster != fInBranch->GetReadEntry() ) {
         fInBranch->GetEntry( *fMaster );
#ifdef COLLECT_D3PD_READING_STATISTICS
        // ++( fEntriesRead.back() );
#endif // COLLECT_D3PD_READING_STATISTICS
      }

      return;
   }

   // void VarHandleBase::UpdateStat( ::TBranch* br ) const {

   //    fEntriesRead.push_back( 0 );
   //    fBranchSize.push_back( ( ::Float_t ) br->GetTotalSize( "*" ) /
   //                           ( ::Float_t ) br->GetEntries() );
   //    fZippedSize.push_back( ( ::Float_t ) br->GetZipBytes( "*" ) /
   //                           ( ::Float_t ) br->GetEntries() );

   //    D3PDPerfStats::Instance()->NewTreeAccessed( fInTree );

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
