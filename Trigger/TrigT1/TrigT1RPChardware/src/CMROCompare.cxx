/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <cmath>
#include "TrigT1RPChardware/CMROCompare.h"

using namespace std;

//----------------------------------------------------------------------------//
CMROCompare::CMROCompare(MatrixReadOut *h, MatrixReadOut *s)
                         : BaseObject(Hardware,"CMROCompare") {
hardware=h;
simulation=s;
diffOutput=0; 
//
// diffOutput= 0             no differences
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
ubit16 hardHeader=hardware->readHeader();
ubit16 simuHeader=simulation->readHeader();
if(hardHeader!=simuHeader) diffOutput+=1000000000;
}//end-of-compareHead
//----------------------------------------------------------------------------//
void CMROCompare::compareSubh() {
ubit16 hardSubHeader=hardware->readSubHeader();
ubit16 simuSubHeader=simulation->readSubHeader();
if(hardSubHeader!=simuSubHeader) diffOutput+=100000000;
}//end-of-compareSubh
//----------------------------------------------------------------------------//
void CMROCompare::compareBody() {
bool FragmentOK=true;
bool firstDiff=true;

DISP<<" ora stampo la matrice hardware"<<endl
    <<hardware<<endl;
DISP_DEBUG;
DISP<<" ora stampo la matrice simulata"<<endl
    <<simulation<<endl;
DISP_DEBUG;

ubit16 numberOfBodyRecHard = hardware->numberOfBodyWords();
ubit16 numberOfBodyRecSimu = simulation->numberOfBodyWords();
if(numberOfBodyRecHard!=numberOfBodyRecSimu) {
 FragmentOK=false;
 diffOutput+=1000000;
 DISP<<" CMROCompare: different number of body words "<<endl
     <<"   hardware: "<<numberOfBodyRecHard
     <<"   simulation: "<<numberOfBodyRecSimu<<endl;
 DISP_DEBUG;
} else {
  hardware->topCMABody(); simulation->topCMABody();
 for(ubit16 i=0; i<numberOfBodyRecHard; i++) {
  ubit16 hardRec=hardware->readCMABodyCurrent();
  ubit16 simuRec=simulation->readCMABodyCurrent();
  if(hardRec!=simuRec){
   FragmentOK=false;
   DISP<<" CMROCompare: different body words "
       <<"   hardware: "<<std::hex<<hardRec<<std::dec
       <<"   simulation: "<<std::hex<<simuRec<<std::dec<<std::endl;
   DISP_DEBUG;
   if(firstDiff) {
    if(i<50000) 
     diffOutput+=100000+(i+1);
    else
     diffOutput+=150000;
    firstDiff=false;
   }
  }//end-of-if
 }//end-of-for
}//end-of-if(number...
if(FragmentOK) {
 DISP<<" CMROCompare: Fragment OK "<<endl;
 DISP_DEBUG;
} else {
 DISP<<" CMROCompare: Fragment NOT OK "<<endl;
 DISP_DEBUG;
}//end-of-if(FragmentOK)
}//end-of-CMROCompare::compareBody()
//----------------------------------------------------------------------------//
void CMROCompare::compareFoot() {
ubit16 hardFooter=hardware->readFooter();
ubit16 simuFooter=simulation->readFooter();
if(hardFooter!=simuFooter) diffOutput+=10000000;
}//end-of-compareFoot
//----------------------------------------------------------------------------//
