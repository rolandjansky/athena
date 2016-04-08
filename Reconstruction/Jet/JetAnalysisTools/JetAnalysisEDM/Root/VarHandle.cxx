/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 510583 2012-07-18 09:12:16Z krasznaa $

// Local include(s):
#include "../JetAnalysisEDM/VarHandle.h"

namespace JetAnalysisEDM {
  VarHandleLog VarHandleLog::vlog;

   VarHandleBase::VarHandleBase(  const string_t name,
                                 const ::Long64_t* master )
      : fMaster( master ),  
        m_inputTree( 0 ), fInBranch( 0 ), fName( name ),
        m_outputVar( kFALSE ), fOutBranch(NULL), fType( "" ),m_branchNotFoundFirstTime(true),
        m_currentEntry( (::Long64_t ) -1 ), m_currentTree(NULL), m_currentTreeIndex(0)
   {

   }


  string_t VarHandleBase::getName() const {
      return fName;
   }

   void VarHandleBase::setName( const string_t name ) {
      fName = name;
      return;
   }

  string_t VarHandleBase::getType() const {
      return fType;
   }

   void VarHandleBase::setType( const string_t type ) {
      fType = type;
      return;
   }

   const ::Long64_t* VarHandleBase::getMaster() const {
      return fMaster;
   }

   void VarHandleBase::setMaster( const ::Long64_t* master ) {
      fMaster = master;
      return;
   }

   ::Bool_t VarHandleBase::outputVar() const {
      return m_outputVar;
   }

   void VarHandleBase::setOutputVar( ::Bool_t active ) {
     m_outputVar = active;
     return;
   }


   ::Bool_t VarHandleBase::ConnectVariable( void* var, ::TClass* realClass,
                                            EDataType dtype, Bool_t isptr ) const {
     if( m_inputTree->SetBranchAddress( getName().c_str(), var, &fInBranch,
                                     realClass, dtype, isptr ) ) {
        ERROR(  (getName()  + "()").c_str(),
                        "Couldn't connect variable to branch: %s", getName().c_str() );
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
      

  string_t VarHandleBase::RootType( const string_t typeid_type ) const {
    if( typeid_type.size() != 1 ) {
      ERROR( "RootType", "Received complex object description" );
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

      ERROR( "RootType",
                      "Unknown primitive type encountered: %s",
                      typeid_type.c_str() );
      return "";
   }

    string_t VarHandleBase::RootCppType( const string_t typeid_type ) const {

      if( typeid_type.size() != 1 ) {
        ERROR( "RootCppType", "Received complex object description" );
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

      ERROR( "RootCppType",
             "Unknown primitive type encountered: %s",
             typeid_type.c_str() );
      return "";
   }

} // namespace JetAnalysisEDM
