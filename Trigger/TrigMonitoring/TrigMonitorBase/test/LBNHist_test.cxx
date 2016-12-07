/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMonitorBase/TrigMonitorToolBase.h"
#include "TrigMonitorBase/TrigLBNHist.h"
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include <iostream>
using namespace std;

TrigLBNHist<TH1F> usual(TH1F("SINGLE_LBN", "The H", 10, 0, 10));
TrigLBNHist<TH1F> grouped(TH1F("10_LBN_GROUPED", "The H", 10, 0, 10));

void fillForLBN(unsigned int l, bool force=false){

  if ( force ) {
    cout << "\n.................. FORCING filling for LBN: " <<  l << endl;
    ITrigLBNHist::force_lbn(l);

  } else { 
    cout << "\n.................. SETTING filling for LBN: " <<  l << endl;
    ITrigLBNHist::set_lbn(l);
    usual.Fill(0.1);
    grouped.Fill(0.1);
  }
}

// Check that all methods were overloaded correctly (should return 1 on Fill)
void checkFills()
{
  TrigLBNHist<TH1F> h1(TH1F("h1","",10,0,10));
  TrigLBNHist<TH2F> h2(TH2F("h2","",10,0,10,20,0,20));
  TrigLBNHist<TProfile> prof1(TProfile("PROFILE_LBN", "The H", 10, 0, 10));
  TrigLBNHist<TProfile2D> prof2(TProfile2D("PROFILE_LBN", "The H", 10, 0, 10, 20, 0, 20));
  ITrigLBNHist::set_lbn(1);
  
  // TH1
  cout << "Checking TH1::Fill (should return 1):" << endl;
  cout << h1.Fill(4.0) << endl;
  cout << h1.Fill(4.0,3.0) << endl;
  cout << h1.Fill("a",4.0) << endl;
  cout << "TH1 entries: " << h1.GetEntries() << endl;
  
  // TH2
  cout << "Checking TH2::Fill (should return 1):" << endl;
  cout << h2.Fill(4.0,15.0) << endl;
  cout << h2.Fill(4.0,15,3.0) << endl;
  cout << h2.Fill(4.0,"A",3.0) << endl;
  cout << h2.Fill("a",16.0,3.0) << endl;
  cout << h2.Fill("b","B",3.0) << endl;
  cout << "TH2 entries: " << h2.GetEntries() << endl;
  
  // TProfile
  cout << "Checking TProfile::Fill (should return 1):" << endl;
  cout << prof1.Fill(4.0,0.1) << endl;
  cout << prof1.Fill(4.0,0.1,3.0) << endl;
  cout << prof1.Fill("abc",0.1) << endl;
  cout << prof1.Fill("abc",0.1,3.0) << endl;
  cout << "TProfile entries: " << prof1.GetEntries() << endl;

  // TProfile2D
  cout << "Checking TProfile2D::Fill (should return 1):" << endl;
  cout << prof2.Fill(4,15,3) << endl;
  //cout << prof2.Fill(4,15,3,3.0) << endl;  // not supported yet
  cout << prof2.Fill("abc",15,3) << endl;
  cout << prof2.Fill(4,"ABC",15) << endl;
  cout << prof2.Fill("abc","ABC",15) << endl;
  cout << "TProfile2D entries: " << prof2.GetEntries() << endl;
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

  // Check Fill methods
  cout << endl;
  checkFills();
}
