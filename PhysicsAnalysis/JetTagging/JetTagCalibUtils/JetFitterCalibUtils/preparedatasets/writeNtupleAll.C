/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "writeNtuple_Official.h"
#include "collectionsToProcess.h"
#include <TString.h>
#include <iostream>

using namespace std;


void writeAllNtuples (TString inputfilename,
                      bool forNN=false) 
{
  vector<TString> collectionsToProcess=getCollectionsToProcess();

  vector<TString>::const_iterator collectionsToProcessBegin=collectionsToProcess.begin();
  vector<TString>::const_iterator collectionsToProcessEnd=collectionsToProcess.end();
  
  for (vector<TString>::const_iterator collectionsToProcessIter=collectionsToProcessBegin;
       collectionsToProcessIter!=collectionsToProcessEnd;
       ++collectionsToProcessIter) 
  {
    
    cout << " Writing ALL Ntuples: processing " << *collectionsToProcessIter << endl;
    writeNtuple_Official(inputfilename,
                         *collectionsToProcessIter,
                         "AOD",
                         forNN,
                         false);
  }

  
}



        
        
