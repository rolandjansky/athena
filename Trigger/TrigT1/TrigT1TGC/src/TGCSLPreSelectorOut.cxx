/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCSLPreSelectorOut.hh"
#include <iostream>

namespace LVL1TGCTrigger {

TGCSLPreSelectorOut::TGCSLPreSelectorOut()
{
  for(int  i=1; i<=NumberOfPtLevel; i+=1)
    for( int j=0; j<NCandidateInSLPreSelector; j+=1){
      idSSC[i][j]=0;
      r[i][j]=0;
      phi[i][j]=0;
      dr[i][j]=99;
      dphi[i][j]=99;
      hit[i][j]=false;
      iVeto[i][j]=false;
    }      
}


void TGCSLPreSelectorOut::print() const
{
#ifdef TGCCOUT
  int i,j;
  std::cout<<std::endl<<"TGCSLPreSelectorOut::print() begin"<<std::endl;
  for( i=1; i<=NumberOfPtLevel; i+=1)
    for( j=0; j<NCandidateInSLPreSelector; j+=1)
      if(hit[i][j]) std::cout<<" No."<<j<<" Pt= "<<i<<" #SSC="<<idSSC[i][j]
			     <<" r= "<<r[i][j]<<" phi= "<<phi[i][j]
			     <<" veto= "<<iVeto[i][j] 
			     << std::endl;
  std::cout<<"TGCSLPreSelectorOut::print() end"<<std::endl;
#endif
}

} //end of namespace bracket
