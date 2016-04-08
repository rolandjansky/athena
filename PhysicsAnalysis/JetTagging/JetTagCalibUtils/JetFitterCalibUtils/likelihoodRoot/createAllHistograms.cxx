/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "createTheHistograms.h"
#include "../preparedatasets/collectionsToProcess.h"
#include <TString.h>
#include <vector>
#include <iostream>

using namespace std;

void createAllHistograms()
{

  vector<TString> collectionsToProcess=getCollectionsToProcess();

  
  vector<TString>::const_iterator collectionsToProcessBegin=collectionsToProcess.begin();
  vector<TString>::const_iterator collectionsToProcessEnd=collectionsToProcess.end();
  
  for (vector<TString>::const_iterator collectionsToProcessIter=++(collectionsToProcessBegin++);
       collectionsToProcessIter!=collectionsToProcessEnd;
       ++collectionsToProcessIter) 
  {  

    cout << " Processing : " << *collectionsToProcessIter << endl;

    createTheHistograms(5,*collectionsToProcessIter);    
    createTheHistograms(1,*collectionsToProcessIter);
    createTheHistograms(4,*collectionsToProcessIter);

    
  }
  
  
}

void createAllHistograms(int number, int flavour)
{



  vector<TString> collectionsToProcess=getCollectionsToProcess();
  if (number>collectionsToProcess.size()-1)
  {
    cout << " number to process: " << number << " not existent " << endl;
    return;
  }
  
  
    cout << " Processing : " << collectionsToProcess[number] << endl;
    
    createTheHistograms(flavour,collectionsToProcess[number]);
    
}
  

    
