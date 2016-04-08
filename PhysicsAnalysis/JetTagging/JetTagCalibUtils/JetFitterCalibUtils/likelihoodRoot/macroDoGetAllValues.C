/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../preparedatasets/collectionsToProcess.h"
#include "macroToGetAllValues.h"
#include <TString.h>

using namespace std;

void prepareCalibrationFileForCOOL()
{

  vector<TString> collectionsToProcess=getCollectionsToProcess();
  
  vector<TString>::const_iterator collectionsToProcessBegin=collectionsToProcess.begin();
  vector<TString>::const_iterator collectionsToProcessEnd=collectionsToProcess.end();
  
  for (vector<TString>::const_iterator collectionsToProcessIter=collectionsToProcessBegin;
       collectionsToProcessIter!=collectionsToProcessEnd;
       ++collectionsToProcessIter) 
  {

    TString filename("../reduceddatasets/reduceddataset_");
    filename+=*collectionsToProcessIter;
    filename+=".root";

    getAllValues(filename,
                 *collectionsToProcessIter,
                 5);
    
    getAllValues(filename,
                 *collectionsToProcessIter,
                 4);
    
    getAllValues(filename,
                 *collectionsToProcessIter,
                 1);

    mergeFiles(*collectionsToProcessIter);

  }
  
}

    

    
                  
                  
