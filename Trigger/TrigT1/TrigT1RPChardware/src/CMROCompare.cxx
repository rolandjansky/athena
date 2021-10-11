/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>
#include "TrigT1RPChardware/CMROCompare.h"

using namespace std;

//----------------------------------------------------------------------------//
CMROCompare::CMROCompare(MatrixReadOut *h, MatrixReadOut *s)
                         : BaseObject(Hardware,"CMROCompare") {
m_hardware=h;
m_simulation=s;
m_diffOutput=0; 
//
// m_diffOutput= 0             no differences
// 1 1 1 1 1 1 1 1 1 1 
// ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
// | | | | | * * * * * --- number of record in Body that shows the first discrepancy
// | | | | --------------- at least one Body record is different
// | | | ----------------- number of Body records is different
// | | ------------------- Footer record is different
// | --------------------- SubHeader record is different
// ----------------------- Header record is different
//
compare();
}//end-of-CMROCompare
//----------------------------------------------------------------------------//
CMROCompare::~CMROCompare() {

}//distructor
//----------------------------------------------------------------------------//
void CMROCompare::compare() {
compareHead();
compareSubh();
compareBody();
compareFoot();
}//end-of-CMROCompare::compare()
//----------------------------------------------------------------------------//
void CMROCompare::compareHead() {
ubit16 hardHeader=m_hardware->readHeader();
ubit16 simuHeader=m_simulation->readHeader();
if(hardHeader!=simuHeader) m_diffOutput+=1000000000;
}//end-of-compareHead
//----------------------------------------------------------------------------//
void CMROCompare::compareSubh() {
ubit16 hardSubHeader=m_hardware->readSubHeader();
ubit16 simuSubHeader=m_simulation->readSubHeader();
if(hardSubHeader!=simuSubHeader) m_diffOutput+=100000000;
}//end-of-compareSubh
//----------------------------------------------------------------------------//
void CMROCompare::compareBody() {
bool FragmentOK=true;
bool firstDiff=true;

cout<<" ora stampo la matrice hardware"<<endl
    <<m_hardware<<endl
    <<" ora stampo la matrice simulata"<<endl
    <<m_simulation<<endl;

ubit16 numberOfBodyRecHard = m_hardware->numberOfBodyWords();
ubit16 numberOfBodyRecSimu = m_simulation->numberOfBodyWords();
if(numberOfBodyRecHard!=numberOfBodyRecSimu) {
 FragmentOK=false;
 m_diffOutput+=1000000;
 cout<<" CMROCompare: different number of body words "<<endl
     <<"   hardware: "<<numberOfBodyRecHard
     <<"   simulation: "<<numberOfBodyRecSimu<<endl;
} else {
  m_hardware->topCMABody(); m_simulation->topCMABody();
 for(ubit16 i=0; i<numberOfBodyRecHard; i++) {
  ubit16 hardRec=m_hardware->readCMABodyCurrent();
  ubit16 simuRec=m_simulation->readCMABodyCurrent();
  if(hardRec!=simuRec){
   FragmentOK=false;
   cout<<" CMROCompare: different body words "
       <<"   hardware: "<<std::hex<<hardRec<<std::dec
       <<"   simulation: "<<std::hex<<simuRec<<std::dec<<std::endl;
   if(firstDiff) {
    if(i<50000) 
     m_diffOutput+=100000+(i+1);
    else
     m_diffOutput+=150000;
    firstDiff=false;
   }
  }//end-of-if
 }//end-of-for
}//end-of-if(number...
if(FragmentOK) {
 cout<<" CMROCompare: Fragment OK "<<endl;
} else {
 cout<<" CMROCompare: Fragment NOT OK "<<endl;
}//end-of-if(FragmentOK)
}//end-of-CMROCompare::compareBody()
//----------------------------------------------------------------------------//
void CMROCompare::compareFoot() {
ubit16 hardFooter=m_hardware->readFooter();
ubit16 simuFooter=m_simulation->readFooter();
if(hardFooter!=simuFooter) m_diffOutput+=10000000;
}//end-of-compareFoot
//----------------------------------------------------------------------------//
