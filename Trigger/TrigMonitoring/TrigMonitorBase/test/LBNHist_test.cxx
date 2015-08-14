/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include <TH1F.h>


TrigLBNHist<TH1F> usual(TH1F("SINGLE_LBN", "The H", 10, 0, 10));


TrigLBNHist<TH1F> grouped(TH1F("10_LBN_GROUPED", "The H", 10, 0, 10));


void fillForLBN(unsigned int l, bool force=false){

  if ( force ) {
    std::cout << "\n.................. FORCING filling for LBN: " <<  l << std::endl;
    ITrigLBNHist::force_lbn(l);

  } else { 
    std::cout << "\n.................. SETTING filling for LBN: " <<  l << std::endl;
    ITrigLBNHist::set_lbn(l);
    usual.Fill(0.1);
    grouped.Fill(0.1);
  }


}

int main() {
  usual.setDepth(3);
  usual.setGroup(1);
  usual.setPath("/expert/SINGLE"); 
  grouped.setDepth(3);
  grouped.setGroup(10);
  grouped.setPath("/expert/GROUPED");
  usual.print();
  grouped.print();  

  fillForLBN(1);
  usual.print();
  grouped.print();

  fillForLBN(1);
  usual.print();
  grouped.print();

  fillForLBN(1);
  usual.print();
  grouped.print();

  fillForLBN(2);
  fillForLBN(2);
  fillForLBN(2);
  usual.print();
  grouped.print();  

  fillForLBN(3);
  fillForLBN(3);
  fillForLBN(3);
  usual.print();
  grouped.print();  


  fillForLBN(4);
  fillForLBN(4);
  fillForLBN(4);
  usual.print();
  grouped.print();  


  fillForLBN(5);
  fillForLBN(5);
  fillForLBN(5);
  usual.print();
  grouped.print();  



  // imagine we fill it sparsely
  fillForLBN(10);
  fillForLBN(10);
  fillForLBN(10);
  usual.print();
  grouped.print();  

  fillForLBN(11);
  fillForLBN(11);
  fillForLBN(11);
  usual.print();
  grouped.print();  

  // at LBN 16 we switch to new PSK

  fillForLBN(16, true);
  fillForLBN(16);
  usual.print();
  grouped.print(); 
  
  // then we got old event
  fillForLBN(15);
  usual.print();
  grouped.print(); 

  fillForLBN(21);
  usual.print();
  grouped.print(); 


  

  //
  // now try some patology
  //
  fillForLBN(31);  
  usual.print();
  grouped.print(); 

  fillForLBN(30);  
  usual.print();
  grouped.print(); 

  fillForLBN(43, true);  
  // filling is so sparse that we go now back by many LBNs
  fillForLBN(35);
  usual.print();
  grouped.print(); 
}
