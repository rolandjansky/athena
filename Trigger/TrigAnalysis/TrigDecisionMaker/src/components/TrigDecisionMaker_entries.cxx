/**************************************************************************
 **
 **   File:         TrigDecisionMaker_entries.cxx        
 **
 **   Description:    Load DLL (libTrigDecisionMaker.so).
 **                   
 **   Author:       R.Goncalo    
 **
 **   Created:      Tue May  09 19:55:56 GMT 2006
 **   Modified:     
 **                   
 **************************************************************************/ 

#include "GaudiKernel/DeclareFactoryEntries.h"

// derived algorithm classes
#include "../TrigDecisionMaker.h"
//#include "TrigDecisionMaker/TrigDecisionTest.h"

using namespace TrigDec;

DECLARE_ALGORITHM_FACTORY( TrigDecisionMaker )
//DECLARE_ALGORITHM_FACTORY( TrigDecisionTest )

DECLARE_FACTORY_ENTRIES( TrigDecisionMaker ) {
  DECLARE_ALGORITHM( TrigDecisionMaker )
    //  DECLARE_ALGORITHM( TrigDecisionTest )
}

