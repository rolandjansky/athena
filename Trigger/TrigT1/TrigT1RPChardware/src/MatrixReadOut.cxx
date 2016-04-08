/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<fstream>
#include<iomanip>
#include<cstring>
#include "TrigT1RPChardware/MatrixReadOut.h"
#include "TrigT1RPChardware/NOBXS.h"

using namespace std;

//----------------------------------------------------------------------------//
MatrixReadOut::MatrixReadOut(Matrix *m, ubit16 FEevent, DataVersion ver) 
                             : BaseObject(Hardware,"MatrixReadOut"){
  //
  // Constructor used by the simulation program
  //
  //           m        the pointer to the object "Matrix"
  //     FEevent        FrontEnd Event identifier
  //
  m_data_version = ver;
  initialize(); 
  FEevent = FEevent%512;
  FEL1ID  = FEevent;
  CM = m;
  makeFragment();
  BS = 0;
}//end-of-MatrixReadOut::MatrixReadOut
//----------------------------------------------------------------------------//
MatrixReadOut::MatrixReadOut(ubit16 FEevent, DataVersion ver) 
                             : BaseObject(Hardware,"MatrixReadOut") {
//
// Constructor used by an external user
// the readout words are supplied by the methods:
// "writeHeader" , "writeSubHeader" , "writeCMABody" and "writ".
//
//     FEevent        FrontEnd Event identifier
//
m_data_version = ver;
initialize();
MROS.setInit(); 
FEevent = FEevent%512;
FEL1ID  = FEevent;
CM = 0;
BS = 0;
}//end-of-MatrixReadOut::MatrixReadOut
//----------------------------------------------------------------------------//
MatrixReadOut::MatrixReadOut(ubit16 *v, ubit16 numWords, DataVersion ver)
                             : BaseObject(Hardware,"MatrixReadOut") {
  //
  // Constructor used by an external user
  //
  //           v        pointer to the CM readout fragment (16-bit words) 
  //    numWords        number of words in the readout fragment
  //
  m_data_version = ver;
  initialize();
  char field;
  CM = 0;
  BS = v;
  //
  numberOfWordsInFrag = numWords;
  ubit16 nWordsMax=100;
  //
  ubit16 nWord=0;
  while(nWord<nWordsMax&&!checkFooterNum) {
    MROS.decodeFragment(*(BS+nWord),field);
    //
    // Fragment Scanning ...
    //
    //    DISP<<" field= "<<field<<endl;
    //    DISP_DEBUG;
    if(field=='B') {
      //
      // this is a Body word
      //
      makeNewHit(*(BS+nWord));
    } else {
      //
      // this is a control word 
      //
      if(field=='H') {
	//
	// Header
	//
	m_Header = *(BS+nWord);
	checkHeaderNum++;
	checkHeaderPos=nWord+1;
      } else
	if(field=='S') {
	  //
	  // Subheader
	  //
	  m_SubHeader = *(BS+nWord);
	  checkSubHeaderNum++;
	  checkSubHeaderPos=nWord+1;
	} else
	  if(field=='F') {
	    //
	    // Footer
	    //
	    m_Footer = *(BS+nWord);
	    checkFooterNum++;
	    checkFooterPos=nWord+1;
	    checkCR=checkCRC8(*(BS+nWord));
	  } else {
	    //
	    // Word code unknown
	    //
	    checkUnkown++;
	  }//end-if
    }//end-if
    nWord++;
  }//end-of-while
}//end-of-MatrixReadOut::MatrixReadOut
//----------------------------------------------------------------------------//
MatrixReadOut::MatrixReadOut(const MatrixReadOut &MROOrig) 
                             : BaseObject(Hardware,"MatrixReadOut") {
//
// Copy Constructor
//
m_data_version = MROOrig.m_data_version;

BunchFrom   = MROOrig.BunchFrom;
BunchTo     = MROOrig.BunchTo;
FEL1ID      = MROOrig.FEL1ID;
m_Header    = MROOrig.m_Header;
m_Footer    = MROOrig.m_Footer;
m_SubHeader = MROOrig.m_SubHeader;
m_Body      = 0;
addressOfWordScanned = 0;
first8bitsON = MROOrig.first8bitsON;
m_BodyLast  = 0;
m_BodyCurr  = 0;
numberOfWordsInFrag = MROOrig.numberOfWordsInFrag;
numberOfWordsInBody = MROOrig.numberOfWordsInBody;
checkHeaderPos      = MROOrig.checkHeaderPos;
checkHeaderNum      = MROOrig.checkHeaderNum;
checkSubHeaderPos   = MROOrig.checkSubHeaderPos;
checkSubHeaderNum   = MROOrig.checkSubHeaderNum;
checkFooterPos      = MROOrig.checkFooterPos;
checkFooterNum      = MROOrig.checkFooterNum;
checkCR             = MROOrig.checkCR;
checkUnkown         = MROOrig.checkUnkown;
//
// copy CM hit structure
//
CMROData *q = 0;
CMROData *r = 0;
CMROData *p = MROOrig.m_Body;
ubit16 cnter=0;
while(p) {
 q = new CMROData;
 q->hit = p->hit;
 q->next = 0;
 if(!cnter) m_Body = q;  else r->next=q;
 cnter++;
 r = q;
 p=p->next;
}//end-of-while
m_BodyLast=r;
}//end-of-MatrixReadOut::MatrixReadOut(const MatrixReadOut &MRCopy)
//----------------------------------------------------------------------------//
MatrixReadOut::~MatrixReadOut() {
//
// delete the CMROData dynamic structure used to describe Body part
// of the event fragment.
//
deleteCMABody();
}//end-of-MatrixReadOut::~MatrixReadOut
//----------------------------------------------------------------------------//
void MatrixReadOut::writeRecord(ubit16 thisRecord, bool last) {
 if(numberOfWordsInFrag==0) {
  m_Header = thisRecord;
  checkHeaderNum++;
  checkHeaderPos=numberOfWordsInFrag+1;
 } else 
 if(numberOfWordsInFrag==1) {
  m_SubHeader = thisRecord;
  checkSubHeaderNum++;
  checkSubHeaderPos=numberOfWordsInFrag+1;
 } else
 if(numberOfWordsInFrag> 1 && !last) {
  makeNewHit(thisRecord);
 } else {
  m_Footer = thisRecord;
  checkFooterNum++;
  checkFooterPos=numberOfWordsInFrag+1;
  checkCR=checkCRC8(thisRecord);
 }//
 numberOfWordsInFrag++;
}//end-of-MatrixReadOut::writeRecord
//----------------------------------------------------------------------------//
void MatrixReadOut::initialize(){
  BunchFrom   =  0;
  BunchTo     =  7;
  m_Header    =  0;
  m_Footer    =  0;
  m_SubHeader =  0;
  m_Body      =  0;
  m_BodyLast  =  0;
  m_BodyCurr  =  0;
  //
  FEL1ID   = 0;
//  ROOffset = 1; // use this for comparison with hardware (VHDL) output
//  ROOffset = 0;  // use this for MC
  ROOffset = 2;
  NBunch = NOBXS;
  NDLLCYC  = 8;
  nchan[0] = 32;
  nchan[1] = 64;
  nclock = NBunch*NDLLCYC;
  timeSeparation = 8;
  //
  first8bitsON=0x00ff;
  //
  // initialize check flags
  //
  numberOfWordsInFrag   =0;
  numberOfWordsInBody   =0;
  checkHeaderPos        =0;
  checkHeaderNum        =0;
  checkSubHeaderPos     =0;
  checkSubHeaderNum     =0;
  checkFooterPos        =0;
  checkFooterNum        =0;
  checkCR               =0;
  checkUnkown           =0;
}//end-of-initialize
//----------------------------------------------------------------------------//
void MatrixReadOut::reset() {
deleteCMABody();
initialize();
MROS.setInit();
}//end-of-reset
//----------------------------------------------------------------------------//
void MatrixReadOut::deleteCMABody() {
  CMROData *p, *q;
  p=m_Body;
  while(p) {
    q=p;
    p=p->next;
    delete q;
  }//end-of-while
  p= 0;
  q= 0;
  m_Body     = 0;
  m_BodyLast = 0;
  m_BodyCurr = 0;
  numberOfWordsInBody=0;
}//end-of-deleteCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::makeFragment() {
  //
  // method executed in conjunction with the simulation program
  //
  makeHeader();
  makeSubHeader();
  makeCMABody();
  makeFooter();
  checkUnkown=0;
  numberOfWordsInFrag=
  numberOfWordsInBody+checkSubHeaderNum+checkHeaderNum+checkFooterNum;
}//end-of-MatrixReadOut::makeFragment
//----------------------------------------------------------------------------//
void MatrixReadOut::makeHeader() {

  //  ubit16 headerval[3];
  if(CM) {
    ubit16 CMcode;
    if (m_data_version == MatrixReadOut::Simulation) {
      // storical CMcode defintion (till 30/July/2007)
      CMcode= 4*CM->getLowHigh()+2*CM->getProjection()+CM->getLocalAdd();
    } else {
      // new CMcode defintion, used in the hardware
      CMcode= 4*CM->getLowHigh()+2*abs(CM->getProjection()-1)+CM->getLocalAdd();
    }  
    writeHeader(CMcode);
  } else {
    DISP<<" MatrixReadOut::makeHeader: CM object does not exist"<<endl;
    DISP_ERROR;
  }//end-of-if
}//end-of-MatrixReadOut::makeHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::makeSubHeader() {
  if(CM) {
    writeSubHeader();
  } else {
    DISP<<" MatrixReadOut::makeSubHeader: CM object does not exist"<<endl;
    DISP_ERROR;
  }//end-of-if
}//end-of-MatrixReadOut::makeSubHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::makeCMABody() {
  if(CM) {
    //    DISP<<"number of hits in matrixReadOut:"<<makeCMABodyHit()<<endl;
    //    DISP_DEBUG;
    makeCMABodyHit();
    makeCMABodyTrg();
  } else {
    DISP<<" MatrixReadOut::makeCMABody: CM object does not exist"<<endl;
    DISP_ERROR;
  }//end-of-if
}//end-of-MatrixReadOut::makeCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::makeFooter() {
  if(CM) {
    writeFooter();
  } else {
    DISP<<" MatrixReadOut::makeFooter: CM object does not exist"<<endl;
    DISP_ERROR;
  }//end-of-if(CM
}//end-of-makeFooter
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::computeCR() {
  //
  // compute the CR value of the event fragment stored in
  // the words "m_Header" , "m_SubHeader" , "m_Body" .
  //
  // The value computed is then returned.
  //
  crc8 CR;
  CMROData *p;
  CR.calc(m_Header);
  CR.calc(m_SubHeader);
  p=m_Body;
  while(p) {
    CR.calc(p->hit);
    p=p->next;
  }//end-of-while
  return CR.current();
}//end-of-MatrixReadOut::computeCR()
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::makeCMABodyHit() {
  //
  // set up the Body section of the event fragment.
  //
  // CMA_BODY --> |0|0| BCID | TIME | IJK | STRIP |
  //
  // This section is organized storing following the increasing value
  // of IJK, BCID, TIME, STRIP.
  // 

  ubit16 i, j, k, l, ijk, lijk;
  ubit16 CMABodyval[5];
  ubit16 numberOfHits=0;
  if(CM) {

      for(ijk=0; ijk<6; ijk++) {           // ijk values
//
// define the CMA side "i" as a fucntion of ijk value
//
       if(ijk<=1) i=0;
        else      i=1;
//
// define the layer address as a function of the ijk value
//
       if(ijk==0 || ijk==2 || ijk==3) j=0;
        else                          j=1;
//              
        for(k=0; k<nclock; k++) {          // clock
//
// loop up to 32 "channels" (lijk)
//
	  for(lijk=0; lijk<32; lijk++) {   // channel
//
// define the absolute channel address as a function of lijk
//
	    l=lijk;
	    if(ijk==3 || ijk==5) l=lijk+32;   
	    //        
	    //       
	    // Decoding map for CMA_BODY:
	    //     #1 BCID  filled with  _BC 
	    //     #2 TIME  filled with  _TIME in DLL steps
	    //     #3 IJK   filled with:
	    //
	    //        Side  Layer  #Channel   IJK
	    //         x      0      //        0
	    //         x      1      //        1
	    //         y      0     < 32       2
	    //         y      0     > 32       3
	    //         y      1     < 32       4
	    //         y      1     > 32       5
	    //               TRIGGER           6
	    //
	    //        more info (Trigger case) 
	    //               5 bit-word for IJK = 7
	    //              |__|__|__|__|__|    _____>Threshold(1="0" threshold
	    //               |  |***| |***|     |               2="1" threshold
	    //               |    |      |______|               3="2" threshold)
	    //               |    |   
	    //          0  <-|    |___overlap(2 bit)(0=no overlap,
	    //                                       1=overlap low chan
	    //                                       2=overlap high chann,
	    //                                       3 overlap low+high)       
	    //   
	    //       #4 STRIP filled with: _CHANNEL  or k-readout 
	    //    
	    //	DISP<<"rodat="<<CM->rodat[i][j][k][l/32]<<" boh="<<(1<<(l%32))<<endl;
	    //  DISP_DEBUG;
	    if( (CM->rodat[i][j][k][l/32]&(1<<(l%32))) ) {
	      ubit16 _SIDE=i;
	      ubit16 _TIME =(k+ROOffset)%NDLLCYC;     // from struct rpcdata in Matrix
	      ubit16 _CHANNEL =l%32;                  // from struct rpcdata
	      ubit16 _IJK=0;
	      ubit16 _BC=(k+ROOffset)/NDLLCYC;        // fill BCID word
	      //
	      if (_SIDE==0) {                
		_IJK=j;         // Fill the IJK-word(3-bit word for CMA_BODY)
	      } else {
		_IJK=2*j+l/32+2;
	      }//end-of-if
	      //
	      // WARNING !!! BC>=0 ?? TIME >=0??
	      // Per riprodurre l'output di stefano si deve imporre
	      // BC>0 and BC<=7 and time>0
	      //
              if (_BC>=BunchFrom && _BC<=BunchTo){
//  		DISP<<"Now store CM_hit with: BCID= "<<_BC<<" TIME = "<<_TIME
//  		    <<" SIDE = "<<_SIDE<<" layer = "<<j
//  		    <<" IJK = " <<_IJK<< " CHANNEL = "<<_CHANNEL<<endl; //print
//              DISP_DEBUG;
		CMABodyval[0] = 0;
		CMABodyval[1] = _BC-BunchFrom;
		CMABodyval[2] = _TIME;
		CMABodyval[3] = _IJK;
		CMABodyval[4] = _CHANNEL;
		makeNewHit(MROS.makeBody(CMABodyval));
		numberOfHits++;
	      } //end of if(_BC
	    }//end-of-if(input[....
	  }//end-of-for(lijk
	}//end-of-for(k
      }//end-of-for(ijk

    //
  } else {
    DISP<<" MatrixReadOut::makeCMABodyHit: CM class empty"<<endl;
    DISP_DEBUG;
  }//end-of-if
  return numberOfHits;
}//end-of-MatrixReadOut::makeCMABodyHit
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::makeCMABodyTrgObsolete() {
  ubit16 CMABodyval[5];
  ubit16 channel=0;
  ubit16 _BC, _TIME, _IJK, _CHANNEL;
  int set_latenza;
  int h_last[32]; // only pivot plane
//
// initialize h_last to value large value
//
  for(ubit16 i=0; i<nchan[0]; i++) {h_last[i]=0xffff;}
//
  ubit16 numberOfHits=0;
  if(CM) {
    CMAword one=1;
    //
    for (int h=0; h<nclock;h++){               // loop on time clock
      for (channel=0; channel<nchan[0]; channel++){
	if(CM->k_readout[h]&(one<<channel)){ //check K-readout register for 
	  set_latenza=abs(h-h_last[channel]);
	  if (set_latenza >timeSeparation) {
	    // a real-trigger data!
	    _BC=(h+ROOffset)/NDLLCYC;       // fill BCID word
	    _TIME=(h+ROOffset)%NDLLCYC;    // from struct rpcdata in Matrix
	    _IJK=6;              //Trigger flag for IJK
	    _CHANNEL=channel;
	    h_last[channel]=h;

	    DISP<<"MatrixReadOut: Now store Trigger_hit with: BCID= "<<_BC
		<<" TIME = "<<_TIME<<" IJK = "<<_IJK<<" CHANNEL = "<<_CHANNEL<<endl;
            DISP_DEBUG;
	    //       
	    CMABodyval[0] = 0;
	    CMABodyval[1] = _BC;
	    CMABodyval[2] = _TIME;
	    CMABodyval[3] = _IJK;
	    CMABodyval[4] = _CHANNEL;
	    makeNewHit(MROS.makeBody(CMABodyval));
	    numberOfHits++;
	  }//end of if set_latenza   
	}//end of if k_readout
      }//end of for(channel
    }//end of for(int h 
//
// Thershold and overlap word
//
    ubit16 lastTime=0xffff;
    for (int n=0;n<nclock;n++){
//**      if(!CM->highestthRO[n] || CM->highestthRO[n]!=highestthprec)????chefamo
       if(CM->highestthRO[n]){
        set_latenza=abs(n-lastTime);
	if(set_latenza>timeSeparation) {
         ubit16 thresh_n= CM->highestthRO[n];
	 ubit16 over_n=CM->overlapRO[n];
	 over_n=over_n<<2;
	 _BC=(n+ROOffset)/NDLLCYC;       // fill BCID word
	 _TIME=(n+ROOffset)%NDLLCYC;
	 _CHANNEL =(over_n|thresh_n);
	 _IJK=7;
	 lastTime=n;
//  	 DISP<<"MatrixReadOut: Now store Trigger_hit with: BCID= "
//  	     <<_BC<<" TIME = "<<_TIME
//  	     <<" IJK = "<<_IJK<<" CHANNEL = "<<_CHANNEL<<endl;
//       DISP_DEBUG;
	 //
	 CMABodyval[0] = 0;
	 CMABodyval[1] = _BC;
	 CMABodyval[2] = _TIME;
	 CMABodyval[3] = _IJK;
	 CMABodyval[4] = _CHANNEL;
	 makeNewHit(MROS.makeBody(CMABodyval));
	 numberOfHits++;
	}//end-of-if(set_latenza
      }//end-of-if(highestthRO  
    }//end-of-for(n=0
    //
    //    DISP<<"     ** Exit from makeCMABodyTrg method **"<<endl<<endl;
    //    DISP_DEBUG;
  } else {
    DISP<<" MatrixReadOut::makeCMABodyTrg: CM class empty"<<endl;
    DISP_ERROR;
  }//end-of-if(CM
  return numberOfHits;
}//end-of-MatrixReadOut::makeCMABodyTrgObsolete
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::makeCMABodyTrg() {
  ubit16 CMABodyval[5];
  ubit16 channel=0;
  ubit16 _BC, _TIME, _IJK, _CHANNEL;
  //int set_latenza;
  //  Removed h_last because it is not used
  //  int h_last[32]; // only pivot plane
//
// initialize h_last with large values
//
  //  for(ubit16 i=0; i<nchan[0]; i++) {h_last[i]=0xffff;}
//
  ubit16 numberOfHits=0;
  bool triggerRO; // flag that indicates a trigger hit recorded in the ReadOut
  if(CM) {
    CMAword one=1;
    //
    for (int h=0; h<nclock;h++){               // loop on time clock
      triggerRO=false; //initialize triggeRO
      for (channel=0; channel<nchan[0]; channel++){
	if(CM->k_readout[h]&(one<<channel)){ //check K-readout register for 
	  //set_latenza=abs(h-h_last[channel]);
	  //if (set_latenza >= timeSeparation) {
	    // a real-trigger data!
	    _BC=(h+ROOffset)/NDLLCYC;       // fill BCID word
	    _TIME=(h+ROOffset)%NDLLCYC;    // from struct rpcdata in Matrix
	    _IJK=6;              //Trigger flag for IJK
	    _CHANNEL=channel;
	    // h_last[channel]=h;

	    DISP<<"MatrixReadOut: Now store Trigger_hit with: BCID= "<<_BC
		<<" TIME = "<<_TIME<<" IJK = "<<_IJK<<" CHANNEL = "<<_CHANNEL<<endl;
            DISP_DEBUG;
	    //
	    if (_BC>=BunchFrom && _BC<=BunchTo){   
	      triggerRO=true; // there is a trigger hit in the ReadOut
	      CMABodyval[0] = 0;
	      CMABodyval[1] = _BC-BunchFrom;
	      CMABodyval[2] = _TIME;
	      CMABodyval[3] = _IJK;
	      CMABodyval[4] = _CHANNEL;
	      makeNewHit(MROS.makeBody(CMABodyval));
	      numberOfHits++;
	    }//end-of-if(_BC
	  // }//end of if set_latenza   
	}//end of if k_readout
      }//end of for(channel
//
// insert here the Threshold and overlap word
//
       if(CM->highestthRO[h]){
        if(triggerRO) {  // if there is a trigger in the RO add IJK=7 record
         ubit16 thresh_h= CM->highestthRO[h];
	 ubit16 over_h=CM->overlapRO[h];
	 over_h=over_h<<2;
	 _BC=(h+ROOffset)/NDLLCYC;       // fill BCID word
	 _TIME=(h+ROOffset)%NDLLCYC;
	 _CHANNEL =(over_h|thresh_h);
	 _IJK=7;
//
//  	 DISP<<"MatrixReadOut: Now store Trigger_hit with: BCID= "
//  	     <<_BC<<" TIME = "<<_TIME
//  	     <<" IJK = "<<_IJK<<" CHANNEL = "<<_CHANNEL<<endl;
//       DISP_DEBUG;
	 //
	 CMABodyval[0] = 0;
	 CMABodyval[1] = _BC-BunchFrom;
	 CMABodyval[2] = _TIME;
	 CMABodyval[3] = _IJK;
	 CMABodyval[4] = _CHANNEL;
	 makeNewHit(MROS.makeBody(CMABodyval));
	 numberOfHits++;
	}//end-of-if(triggerRO
      }//end-of-if(highestthRO  
    }//end of for(int h 
//
//    DISP<<"     ** Exit from makeCMABodyTrg method **"<<endl<<endl;
//    DISP_DEBUG;
//
  } else {
    DISP<<" MatrixReadOut::makeCMABodyTrg: CM class empty"<<endl;
    DISP_ERROR;
  }//end-of-if(CM
  return numberOfHits;
}//end-of-MatrixReadOut::makeCMABodyTrg
//----------------------------------------------------------------------------//
void MatrixReadOut::writeHeader(ubit16 CMcode){
  ubit16 headerval[3];
  headerval[0] = 0;
  headerval[1] = CMcode;
  headerval[2] = FEL1ID;
  m_Header = MROS.makeHeader(headerval);
  checkHeaderNum++;//=1
  checkHeaderPos=1;//=1
}//end-of-writeHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::writeSubHeader(){
  m_SubHeader = MROS.makeSubHeader();
  checkSubHeaderNum++;//=1
  checkSubHeaderPos=checkHeaderPos+1;//=2

}//end-of-MatrixReadOut::writeSubHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::writeCMABody(ubit16 _BC, ubit16 _TIME, ubit16 _IJK,
                                 ubit16 _CHANNEL) {
  ubit16 CMABodyval[5];
  CMABodyval[0] = 0;
  CMABodyval[1] = _BC;
  CMABodyval[2] = _TIME;
  CMABodyval[3] = _IJK;
  CMABodyval[4] = _CHANNEL;
  sortAndMakeNewHit(MROS.makeBody(CMABodyval));
}//end-of-MatrixReadOut::writeCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::writeFooter(){
  ubit16 footerval;
  footerval = computeCR()&first8bitsON;
  m_Footer = MROS.makeFooter(footerval);
  checkCR=0;
  checkFooterPos=numberOfWordsInBody+3 ;
  checkFooterNum++;
}//end-of-MatrixReadOut::writeFooter
//----------------------------------------------------------------------------//
void MatrixReadOut::makeNewHit(ubit16 newHit) {
  CMROData *p;
  p = new CMROData;
  p->hit=newHit;
  p->next= 0;
  if(!m_Body) {
    m_Body = p;
  } else {
    m_BodyLast->next = p;
  }//end-of-if
  m_BodyLast=p;
  numberOfWordsInBody++;
}//end-of-MatrixReadOut::makeNewHit
//----------------------------------------------------------------------------//
void MatrixReadOut::makeNewHit(ubit16 newHit, 
                               CMROData *previous, CMROData *next) {
  CMROData *newElement;
  newElement = new CMROData;
  newElement->hit = newHit;
  newElement->next = next;
  if(next==m_Body) m_Body=newElement; else previous->next = newElement;
  numberOfWordsInBody++;
}//end-of-MatrixReadOut::makeNewHit
//----------------------------------------------------------------------------//
void MatrixReadOut::sortAndMakeNewHit(ubit16 newHit) {
  CMROData *p, *previous;//, *newElement;
  char field;
  DISP<<" new Hit is "<<hex<<newHit<<dec<<endl;
  DISP_DEBUG;
  p          = m_Body;
  previous   = m_Body;
  //newElement = 0;
  MROS.decodeFragment(newHit,field);
  DISP<<" IELD= "<<field<<endl;
  DISP_DEBUG;
  const ubit16 hitBCID      = MROS.bcid();
  const ubit16 hitTIME      = MROS.time();
  const ubit16 hitIJK       = MROS.ijk();
  const ubit16 hitCHANNEL   = MROS.channel();
  DISP<<" decode HIT "<<hitBCID<<" "<<hitTIME<<" "<<hitIJK<<" "<<hitCHANNEL<<endl;
  DISP_DEBUG;
  //
  do {

    if(p) { 
      MROS.decodeFragment(p->hit,field);

      if(hitIJK>MROS.ijk()) {
	previous=p;
	p=p->next;
      } else 
	if(hitIJK<MROS.ijk()) {
	  makeNewHit(newHit,previous,p);
	  break;
	} else
	  if(hitIJK==MROS.ijk()) {
	    if(hitBCID>MROS.bcid()) {
	      previous=p;
	      p=p->next;
	    } else 
	      if(hitBCID<MROS.bcid()) {
		makeNewHit(newHit,previous,p);
		break;
	      } else
		if(hitBCID==MROS.bcid()) {
		  if(hitTIME>MROS.time()) {
		    previous=p;
		    p=p->next;
		  } else 
		    if(hitTIME<MROS.time()) {
		      makeNewHit(newHit,previous,p);
		      break;
		    } else
		      if(hitTIME==MROS.time()) {  
			if(hitCHANNEL>MROS.channel()) {
			  previous=p;
			  p=p->next;
			} else 
			  if(hitCHANNEL<MROS.channel()) {
			    makeNewHit(newHit,previous,p);
			    break;
			  } else {
			    DISP<<" duplicazione di hit??? "<<endl;
			    DISP_ERROR;
			    break; 
			  }//end-of-if(hitCHANNEL 
		      }//end-of-if(hitTIME
		}//end-of-if(hitBCID
	  }//end-of-if(hitIJK
 
    }//if(p
 
    if(!p) {   
      if(!m_Body) {                    // structure empty
	makeNewHit(newHit);
      } else {                         // we are in the end of the structure
	makeNewHit(newHit,previous,p);
      } //end-of-if(!m_Body
    }//end-of-if(p
  }
  while(p);
}//end-of-MatrixReadOut::sortAndMakeNewHit
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readHeader() { return m_Header; } 
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readSubHeader() { return m_SubHeader; } 
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readFooter() { return m_Footer; } 
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readCMABodyCurrent() {
  if(m_BodyCurr) {
    ubit16 hit = m_BodyCurr->hit;
    m_BodyCurr=m_BodyCurr->next;
    return hit;
  } else {
    return 0xffff;
  }
}//end-of-MatrixReadout::readCMABodyCurrent
//----------------------------------------------------------------------------//
void MatrixReadOut::readCMABody(ubit16 *Body) {
  ubit16 i=0;
  CMROData *p;
  if(numberOfWordsInBody) {
    p=m_Body;
    for(i=0;i<numberOfWordsInBody; i++) {
      *(Body+i)=p->hit;
      p=p->next;
    }//end-of-for
  }//end-of-if 
}//end-of-MatrixReadout::readCMABody
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readCMAWord() {
ubit16 output = 0xffff;
     if(addressOfWordScanned==0) output = readHeader();
else if(addressOfWordScanned==1) output = readSubHeader();
else if(addressOfWordScanned==(numberOfFragmentWords()-1)) 
                                output = readFooter();
else output = readCMABodyCurrent();
addressOfWordScanned++;
return output;
}//end-of-MatrixReadout::readCMAWord
//----------------------------------------------------------------------------//
MatrixReadOutStructure MatrixReadOut::getCMAHit(int index) {

  MatrixReadOutStructure theStruct;
  ubit16 theHit;
  if((numberOfWordsInBody<=index)||(numberOfWordsInBody==0)){
    DISP<<" getCMAHit: Wrong index given;"
	<<" numberOfWordsInBody= "<<numberOfWordsInBody
	<<" index = "<<index<<endl;
    DISP_DEBUG;
  } else {

    CMROData *p;
    p=m_Body;
    for(int i=0;i<numberOfWordsInBody; i++) {
      theHit=p->hit;
      if(index==i){theStruct=MatrixReadOutStructure(theHit);}
      p=p->next;
    }
  }
  return theStruct;
}//end-of-MatrixReadOut::getCMAHit(int index)
//----------------------------------------------------------------------------//
void MatrixReadOut::topCMABody() {
addressOfWordScanned=0;
m_BodyCurr = m_Body;
}//MatrixReadOut::end-of-topCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::display(ostream &stream) {
  displayHeader(stream);
  displaySubHeader(stream);
  displayBody(stream);
  displayFooter(stream);
}//end-of-MatrixReadOut::display
//----------------------------------------------------------------------------//
void MatrixReadOut::displayHeader(ostream &stream){
  char field;
  stream<<endl;
  stream<<hex<<m_Header<<dec;
  ubit16 errorDecode = MROS.decodeFragment(m_Header, field);
  if(!errorDecode&&field=='H') {
    stream<<" -- CMID   "<<MROS.cmid();
    stream<<" FEL1ID "   <<MROS.fel1id();
  } else {
    stream<<" ERROR IN HEADER DECODING ";
  }//end-of-if
  stream<<endl;
}//end-of-MatrixReadOut::displayHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::displaySubHeader(ostream &stream) {
  char field;
  stream<<hex<<m_SubHeader<<dec;
  ubit16 errorDecode = MROS.decodeFragment(m_SubHeader, field);
  if(!errorDecode&&field=='S') {
    stream<<" -- FEBCID   "<<MROS.febcid();
  } else {
    stream<<" ERROR IN SUBHEADER DECODING ";
  }//end-of-if
  stream<<endl;
}//end-of-MatrixReadOut::displaySubHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::displayBody(ostream &stream) {
  CMROData *p;
  char field;
  p=m_Body;
  //
  while(p) {

    stream<<hex<<p->hit<<dec;
    ubit16 errorDecode = 
      MROS.decodeFragment(p->hit, field);
    if(!errorDecode&&field=='B') {
      stream<<" -- BC     "<<MROS.bcid();
      stream<<" TIME   "<<MROS.time();
      if(MROS.ijk()<7) {
	stream<<" IJK    "<<      MROS.ijk();
	stream<<" STRIP  "<<      MROS.channel();
      } else {
	stream<<" OVL    "<<      MROS.overlap();
	stream<<" THR    "<<      MROS.threshold();
      }//end-of-if
    } else {
      stream<<" ERROR IN BODY DECODING ";
    }//end-of-if(!errorDecode
    stream<<endl;
    p=p->next;
  }//end-of-while
}//end-of-MatrixReadOut::displayBody
//----------------------------------------------------------------------------//
void MatrixReadOut::displayFooter(ostream &stream) {
  char field;
  stream<<hex<<m_Footer<<dec;
  stream<<" -- CODE   "<<1;
  ubit16 errorDecode = MROS.decodeFragment(m_Footer, field);
  if(!errorDecode&&field=='F') {
    stream<<" CRC "<<hex<<MROS.crc()<<dec;
  } else {
    stream<<" ERROR IN FOOTER DECODING ";
  }//end-of-if(!errorDecode
  stream<<endl;
}//end-of-MatrixReadOut::displayFooter
//----------------------------------------------------------------------------//
void MatrixReadOut::bytestream(ostream &stream) {
CMROData *p;
stream<<hex<<m_Header<<dec<<endl;      // header
stream<<hex<<m_SubHeader<<dec<<endl;   // subheader 
p=m_Body;
while(p) { 
 stream<<hex<<p->hit<<dec<<endl;       // body
 p=p->next;
}//end-of-while
stream<<hex<<m_Footer<<dec<<endl;      // footer
}//end-of-MatrixReadOut::bytestream
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkFragment() {
//DISP<<" MatrixReadOut(ubit16 *pointer, ubit16 num) ";
//    cout<<endl
//    cout<<" check dump "<<endl
//    std::cout<<" checkHeaderNum = "<<checkHeaderNum
//        <<" CheckHeaderPos = "<<checkHeaderPos<<std::endl;
//    cout<<" checkSubHeaderNum = "<<checkSubHeaderNum
//        <<" CheckSubHeaderPos = "<<checkSubHeaderPos<<endl
//    cout<<" checkFooterNum = "<<checkFooterNum
//        <<" CheckFooterPos = "<<checkFooterPos<<endl
//    cout<<" CheckUnkown = "<<checkUnkown<<endl
//    cout<<" Number of Words In Fragment = "<<numberOfWordsInFrag<<endl;
//DISP_DEBUG;
  ubit16 output = 0;
  if(checkHeaderNum!=1 &&                   !(output&0x00000001)) output+=1;//=1
  if(checkHeaderPos>1 &&                    !(output&0x00000002)) output+=2;//=1 
  if(checkSubHeaderNum!=1 &&                !(output&0x00000004)) output+=4;//=1
  if(checkSubHeaderPos!=2 &&                !(output&0x00000008)) output+=8;//=2
  if(checkFooterPos!=numberOfWordsInFrag && !(output&0x00000010)) output+=16;
  if(checkCR &&                             !(output&0x00000020)) output+=32;
  if(checkUnkown &&                         !(output&0x00000040)) output+=64;
  if(checkBodyOrder() &&                    !(output&0x00000080)) output+=128;
  return output;
}//end-of-MatrixReadOut::checkFragment
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkCRC8(ubit16 foot) {
  ubit16 output=0;
  if((computeCR()&first8bitsON)!=(foot&first8bitsON)) output=1;
  return output;
}//end-of-MatrixReadOut::checkCRC8()
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkBodyOrder() {
  CMROData *p, *pnext;
  char field;
  ubit16 currIJK, currBCID, currTIME, currCHANNEL;
  ubit16 nextIJK, nextBCID, nextTIME, nextCHANNEL;
  ubit16 prevIJK; //, prevBCID, prevTIME, prevCHANNEL;
  ubit16 output=0;
  ubit16 outTemp=0;
  p=m_Body;
  
  prevIJK=8;    //use a non-sense initializer
  //prevBCID=0;
  //prevTIME=0;
  //prevCHANNEL=0;
  
  while(p) {
    MROS.decodeFragment(p->hit,field);
    currIJK     = MROS.ijk();
    currBCID    = MROS.bcid();
    currTIME    = MROS.time();
    currCHANNEL = MROS.channel();
    pnext=p->next;
    if(pnext) {
      //      output=0;
      MROS.decodeFragment(pnext->hit,field);
      nextIJK     = MROS.ijk();
      nextBCID    = MROS.bcid();
      nextTIME    = MROS.time();
      nextCHANNEL = MROS.channel();
      //
      
      if(currIJK<6) {          // analyze first IJK=0,1,2,3,4,5 ...
      
      
      if(nextIJK<currIJK)          {output+=1; outTemp=1;} else if(nextIJK==currIJK) {
	if(nextBCID<currBCID)        {output+=2; outTemp=2;} else if(nextBCID==currBCID) {
	  if(nextTIME<currTIME)        {output+=4; outTemp=4;} else if(nextTIME==currTIME) {
	    if(nextCHANNEL<=currCHANNEL) {output+=8; outTemp=8;}
          }
        }
      }


      } else if(currIJK==6) {  // ... then first IJK=6 ...

        if(nextIJK==currIJK) { 
	 if(nextBCID!=currBCID || nextTIME!=currTIME) {
	  output+=16; 
	  outTemp=16;
	 } else  {
	  if(nextCHANNEL<=currCHANNEL) {output+=32; outTemp=32;}
         }//end-of-if(nextBCID!=currBCID || nextTIME!=currTIME)
	} else if(nextIJK==7) {
	}//end-of-if(nextIJK==currIJK)
      
      } else if(currIJK==7) {  // finally IJK=7.
      
        if(prevIJK!=6) {      // if this CM hit has IJK=7 then the previous on must have IJK=6
	 output+=64;
	 outTemp=64;
	}
      
      } else {
      
      }//end-of-if(currIJK
      
      if(outTemp>0){
       DISP<<"checkBodyOrder output= "<<output<<" with "<< hex<<pnext->hit<<dec
           <<endl;
       DISP_DEBUG;
      }//end-of-if(outTemp      
      outTemp=0;     
      
    } else {  // else of if(p->next
     
     if(currIJK==6) {
      output+=64;
      DISP<<" CheckBodyOrder; IJK 6 exists but the related IJK 7 has been found "<<endl;
      DISP_DEBUG;
     } else if(currIJK==7 && prevIJK!=6) {   // we are at the last CM hit; if this has IJK=7
      output+=128;                            // then the previous CM hit must have IJK=6
      DISP<<" CheckBodyOrder; IJK 7 exists but the related IJK 6 has been found "<<endl;
      DISP_DEBUG;
     }    
       
    }//end-of-if(p->next   
    
    prevIJK    =currIJK;
    //prevBCID   =currBCID;     // not used in this version
    //prevTIME   =currTIME;     // not used in this version
    //prevCHANNEL=currCHANNEL;  // not used in this version
    p=p->next;
    
  }//end-of-while(p)
  
  return output;
}//end-of-checkBodyOrder
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkBodyOrderObsolete() {
  CMROData *p, *pnext;
  char field;
  ubit16 currIJK, currBCID, currTIME, currCHANNEL;
  ubit16 nextIJK, nextBCID, nextTIME, nextCHANNEL;
  ubit16 output=0;
  ubit16 outTemp=0;
  p=m_Body;
  while(p) {
    pnext=p->next;
    if(pnext) {
      //      output=0;
      MROS.decodeFragment(p->hit,field);
      currIJK     = MROS.ijk();
      currBCID    = MROS.bcid();
      currTIME    = MROS.time();
      currCHANNEL = MROS.channel();
      MROS.decodeFragment(pnext->hit,field);
      nextIJK     = MROS.ijk();
      nextBCID    = MROS.bcid();
      nextTIME    = MROS.time();
      nextCHANNEL = MROS.channel();
      //
      if(nextIJK<currIJK)          {output+=1; outTemp=1;} else if(nextIJK==currIJK) {
	if(nextBCID<currBCID)        {output+=2; outTemp=2;} else if(nextBCID==currBCID) {
	  if(nextTIME<currTIME)        {output+=4; outTemp=4;} else if(nextTIME==currTIME) {
	    if(nextCHANNEL<=currCHANNEL) {output+=8; outTemp=8;}
          }
        }
      }

      if(outTemp>0){
       DISP<<"checkBodyOrder output= "<<output<<" with "<< hex<<pnext->hit<<dec
           <<endl;
       DISP_DEBUG;
      }//end-of-if
      outTemp=0;
    }//end-of-if(p->next
    p=p->next;
  }//end-of-while(p)
  return output;
}//end-of-checkBodyOrderObsolete
//----------------------------------------------------------------------------//
void MatrixReadOut::setManager( ReadOutManager* boss ) {
  myBoss= boss;
  return;
}//end-of-MatrixReadOut::setManager( ReadOutManager* boss )
//----------------------------------------------------------------------------//
MatrixReadOutStructure MatrixReadOut::getHeader() {
  MatrixReadOutStructure theStruct(m_Header);
  return theStruct;
}//end-of-MatrixReadOut::getHeader()
//----------------------------------------------------------------------------//
MatrixReadOutStructure MatrixReadOut::getSubHeader() {
  MatrixReadOutStructure theStruct(m_SubHeader);
  return theStruct;
}//end-of-MatrixReadOut::getSubHeader()
//----------------------------------------------------------------------------//
MatrixReadOutStructure MatrixReadOut::getFooter() {
  MatrixReadOutStructure theStruct(m_Footer);
  return theStruct;
}//end-of-MatrixReadOut::getFooter()
//----------------------------------------------------------------------------//
void MatrixReadOut::doMatrix(Matrix *CMpointer) {
//
//copy the pointer to CMA "CMpointer" in "CM" member
//
CM = CMpointer;
CM->reset();
ubit16 sidemat=0;
ubit16 layer  =0;
ubit16 stripaddress=0;
const ubit16 ROOffset=2;

for(ubit16 n=0; n<numberOfWordsInBody; n++) {
 MatrixReadOutStructure locMRO = getCMAHit(n);
 ubit16 IJK = locMRO.ijk();
 ubit16 CHANNEL = locMRO.global_channel();
 ubit16 BCID = locMRO.bcid();
 ubit16 TIME = locMRO.time();
 float  absTime=0.;

//
// select CMA strip data (IJK from 0 to 5; 6 and 7 are for trigger data
// 
 if(IJK<6) {
  switch (IJK) {
   case 0:
    sidemat=0; layer=0; stripaddress= CHANNEL;
    break;
   case 1:
    sidemat=0; layer=1; stripaddress= CHANNEL;
    break;
   case 2:
    sidemat=1; layer=0; stripaddress= CHANNEL;
    break;
   case 3:
    sidemat=1; layer=0; stripaddress= CHANNEL;
    break;
   case 4:
    sidemat=1; layer=1; stripaddress= CHANNEL;
    break;
   case 5:
    sidemat=1; layer=1; stripaddress= CHANNEL;
    break;
   default:
    DISP<<" IJK= "<<IJK<<" out of RANGE "<<endl;
    DISP_ERROR;
  }//end-of-switch
//
// estimate absolute time from CMA time
//
  absTime = 25.*( (float) BCID + ((float) (TIME-ROOffset))/8.);
  CM->putData(sidemat,layer,stripaddress,absTime);  
 }//end-of-if(IJK<6
}//end-of-for(ubit16 n
//
// use the Matrix data buffer from the the first available location and not
// at the center as it is done by default
//
CM->setBCzero(0);
//
// run the CMA logic
//
CM->execute();
}//end-of-MatrixReadOut::doMatrix()
//----------------------------------------------------------------------------//
void MatrixReadOut::makeTestPattern(ubit16 mode, ubit16 ktimes, int eventNum ) {
std::cout<<" makeTestPattern "<<std::endl;
MatrixReadOutStructure MRS=getHeader(); // get the CM Header
ubit16 cmid = MRS.cmid(); // CM Address (or identifier)
//
ofstream vhdlinput;
vhdlinput.open("vhdl.input",ios::app);
if(!vhdlinput){
 DISP<<" File for vhdl analysis not opened. "<<endl
     <<" =================================="<<endl<<endl;
 DISP_ERROR;
 };
ofstream h8output;
h8output.open("h8.output",ios::app);
if(!h8output){
 DISP<<" File of H8 data not opened. "<<endl
     <<" =================================="<<endl<<endl;
 DISP_ERROR;
 };
// h8output<<" RUN "<<0<<" EVENT "<<eventNum<<" CMID "<<cmid<<std::endl;
h8output<<this;
h8output.close();
//
const ubit16 maxchan = 100;
const ubit16 maxtimes= 200;
ubit16 IJ[maxtimes][4], channels[maxtimes][4][maxchan]; 
float times[maxtimes]; char plane[4][2];
strcpy(plane[0],"I0");
strcpy(plane[1],"I1");
strcpy(plane[2],"J0");
strcpy(plane[3],"J1");
ubit16 ntimes=0;
for(ubit16 l=0; l<maxtimes; l++) {for(ubit16 i=0; i<4; i++) {IJ[l][i]=0;}}

float timeover = 999999.;
bool completed = false;
float timelast= -timeover;

std::cout<<this<<std::endl;

while(!completed) {
  float timemin =  timeover;
  for(int iCMhit=0; iCMhit<numberOfWordsInBody; iCMhit++) {
    MatrixReadOutStructure MRS=getCMAHit(iCMhit);
    int ijk = MRS.ijk();  
    if(ijk<6) {
      float time=((float) (MRS.bcid()*8+MRS.time())-0.5)*3.125;
      //int channel=MRS.global_channel(); // CM channel
      //std::cout<<"time "<<time<<" IJK= "<<ijk<<" channel "<<channel<<std::endl;
      if(time>timelast && time<timemin ) timemin=time; 
    }//end-of-if(
  }//end-of-for(

  if(timemin==timeover) {
   completed=true;
  } else { 
    ubit16 this_time_counter=0;
    for(int iCMhit=0; iCMhit<numberOfWordsInBody; iCMhit++) {
      MatrixReadOutStructure MRS=getCMAHit(iCMhit);
      int ijk = MRS.ijk();  
      if(ijk<6) {
        int channel=MRS.global_channel(); // CM channel
        float time=((float) (MRS.bcid()*8+MRS.time())-0.5)*3.125;
        if(time==timemin && ntimes<maxtimes) {
	  if(!this_time_counter) ntimes++;
	  this_time_counter++;
	  ubit16 ijk_index=0;
	       if(ijk==0) ijk_index=0;
	  else if(ijk==1) ijk_index=1;
	  else if(ijk==2 || ijk==3) ijk_index=2;
	  else if(ijk==4 || ijk==5) ijk_index=3;
	  times[ntimes-1]=time;
	  if(IJ[ntimes-1][ijk_index] < maxchan) {
	   IJ[ntimes-1][ijk_index]++;
	   channels[ntimes-1][ijk_index][IJ[ntimes-1][ijk_index]-1]=channel;
	  }//end-of-if
	}//end-of-if(time==timemin 
      }//end-of-if(
     }//end-of-for(
   timelast=timemin;
   //std::cout<<" time min found: "<<timelast<<std::endl;
  }
}//end-of-while

if(mode) {
 int NBunch=0;
 int run=0;
 vhdlinput<<" RUN "<<run<<" EVENT "<<eventNum<<" CMID "<<cmid<<" WINDOW "<<NBunch;
 vhdlinput<<" LINES "<<(ntimes+ktimes)<<std::endl;
}//end-of-if(mode
 for(ubit16 l=0;l<ntimes; l++) {
  vhdlinput<<" TIME "<<times[l]<<" ";
  for(ubit16 i=0; i<4; i++) {
   vhdlinput<<plane[i][0]<<plane[i][1]<<" "<<IJ[l][i]<<" ";
   for(ubit16 j=0; j<IJ[l][i]; j++) {
    vhdlinput<<channels[l][i][j]<<" ";
   }//end-of-for(j
  }//end-of-for(i
  vhdlinput<<std::endl;
 }//end-of-for(l
//
vhdlinput.close();
}//end-of-MatrixReadOut::makeTestPattern
