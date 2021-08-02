/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  m_FEL1ID  = FEevent;
  m_CM = m;
  makeFragment();
  m_BS = 0;
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
m_MROS.setInit(); 
FEevent = FEevent%512;
m_FEL1ID  = FEevent;
m_CM = 0;
m_BS = 0;
m_myBoss= 0;
m_addressOfWordScanned = 0;
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
  m_CM = 0;
  m_BS = v;
  m_myBoss= 0;
  m_addressOfWordScanned = 0;

  //
  m_numberOfWordsInFrag = numWords;
  ubit16 nWordsMax=100;
  //
  ubit16 nWord=0;
  while(nWord<nWordsMax&&!m_checkFooterNum) {
    m_MROS.decodeFragment(*(m_BS+nWord),field);
    //
    // Fragment Scanning ...
    //
    //    cout<<" field= "<<field<<endl;
    if(field=='B') {
      //
      // this is a Body word
      //
      makeNewHit(*(m_BS+nWord));
    } else {
      //
      // this is a control word 
      //
      if(field=='H') {
	//
	// Header
	//
	m_Header = *(m_BS+nWord);
	m_checkHeaderNum++;
	m_checkHeaderPos=nWord+1;
      } else
	if(field=='S') {
	  //
	  // Subheader
	  //
	  m_SubHeader = *(m_BS+nWord);
	  m_checkSubHeaderNum++;
	  m_checkSubHeaderPos=nWord+1;
	} else
	  if(field=='F') {
	    //
	    // Footer
	    //
	    m_Footer = *(m_BS+nWord);
	    m_checkFooterNum++;
	    m_checkFooterPos=nWord+1;
	    m_checkCR=checkCRC8(*(m_BS+nWord));
	  } else {
	    //
	    // Word code unknown
	    //
	    m_checkUnkown++;
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

m_BunchFrom   = MROOrig.m_BunchFrom;
m_BunchTo     = MROOrig.m_BunchTo;
m_FEL1ID      = MROOrig.m_FEL1ID;
m_Header    = MROOrig.m_Header;
m_Footer    = MROOrig.m_Footer;
m_SubHeader = MROOrig.m_SubHeader;
m_Body      = 0;
m_addressOfWordScanned = 0;
m_first8bitsON = MROOrig.m_first8bitsON;
m_BodyLast  = 0;
m_BodyCurr  = 0;
m_numberOfWordsInFrag = MROOrig.m_numberOfWordsInFrag;
m_numberOfWordsInBody = MROOrig.m_numberOfWordsInBody;
m_checkHeaderPos      = MROOrig.m_checkHeaderPos;
m_checkHeaderNum      = MROOrig.m_checkHeaderNum;
m_checkSubHeaderPos   = MROOrig.m_checkSubHeaderPos;
m_checkSubHeaderNum   = MROOrig.m_checkSubHeaderNum;
m_checkFooterPos      = MROOrig.m_checkFooterPos;
m_checkFooterNum      = MROOrig.m_checkFooterNum;
m_checkCR             = MROOrig.m_checkCR;
m_checkUnkown         = MROOrig.m_checkUnkown;
m_myBoss= 0;
m_ROOffset = 2;
m_NDLLCYC  = 8;
m_NBunch = NOBXS;
m_nclock = m_NBunch*m_NDLLCYC;
m_timeSeparation = 8;
m_CM = 0;
m_BS = 0; 
//
// copy m_CM hit structure
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
 if(m_numberOfWordsInFrag==0) {
  m_Header = thisRecord;
  m_checkHeaderNum++;
  m_checkHeaderPos=m_numberOfWordsInFrag+1;
 } else 
 if(m_numberOfWordsInFrag==1) {
  m_SubHeader = thisRecord;
  m_checkSubHeaderNum++;
  m_checkSubHeaderPos=m_numberOfWordsInFrag+1;
 } else
 if(m_numberOfWordsInFrag> 1 && !last) {
  makeNewHit(thisRecord);
 } else {
  m_Footer = thisRecord;
  m_checkFooterNum++;
  m_checkFooterPos=m_numberOfWordsInFrag+1;
  m_checkCR=checkCRC8(thisRecord);
 }//
 m_numberOfWordsInFrag++;
}//end-of-MatrixReadOut::writeRecord
//----------------------------------------------------------------------------//
void MatrixReadOut::initialize(){
  m_BunchFrom   =  0;
  m_BunchTo     =  7;
  m_Header    =  0;
  m_Footer    =  0;
  m_SubHeader =  0;
  m_Body      =  0;
  m_BodyLast  =  0;
  m_BodyCurr  =  0;
  //
  m_FEL1ID   = 0;
//  m_ROOffset = 1; // use this for comparison with hardware (VHDL) output
//  m_ROOffset = 0;  // use this for MC
  m_ROOffset = 2;
  m_NBunch = NOBXS;
  m_NDLLCYC  = 8;
  m_nchan[0] = 32;
  m_nchan[1] = 64;
  m_nclock = m_NBunch*m_NDLLCYC;
  m_timeSeparation = 8;
  //
  m_first8bitsON=0x00ff;
  //
  // initialize check flags
  //
  m_numberOfWordsInFrag   =0;
  m_numberOfWordsInBody   =0;
  m_checkHeaderPos        =0;
  m_checkHeaderNum        =0;
  m_checkSubHeaderPos     =0;
  m_checkSubHeaderNum     =0;
  m_checkFooterPos        =0;
  m_checkFooterNum        =0;
  m_checkCR               =0;
  m_checkUnkown           =0;
}//end-of-initialize
//----------------------------------------------------------------------------//
void MatrixReadOut::reset() {
deleteCMABody();
initialize();
m_MROS.setInit();
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
  m_numberOfWordsInBody=0;
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
  m_checkUnkown=0;
  m_numberOfWordsInFrag=
  m_numberOfWordsInBody+m_checkSubHeaderNum+m_checkHeaderNum+m_checkFooterNum;
}//end-of-MatrixReadOut::makeFragment
//----------------------------------------------------------------------------//
void MatrixReadOut::makeHeader() {

  //  ubit16 headerval[3];
  if(m_CM) {
    ubit16 CMcode;
    if (m_data_version == MatrixReadOut::Simulation) {
      // storical CMcode defintion (till 30/July/2007)
      CMcode= 4*m_CM->getLowHigh()+2*m_CM->getProjection()+m_CM->getLocalAdd();
    } else {
      // new CMcode defintion, used in the hardware
      CMcode= 4*m_CM->getLowHigh()+2*abs(m_CM->getProjection()-1)+m_CM->getLocalAdd();
    }  
    writeHeader(CMcode);
  } else {
    throw std::runtime_error("MatrixReadOut::makeHeader: m_CM object does not exist");
  }//end-of-if
}//end-of-MatrixReadOut::makeHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::makeSubHeader() {
  if(m_CM) {
    writeSubHeader();
  } else {
    throw std::runtime_error("MatrixReadOut::makeSubHeader: m_CM object does not exist");
  }//end-of-if
}//end-of-MatrixReadOut::makeSubHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::makeCMABody() {
  if(m_CM) {
    //    cout<<"number of hits in matrixReadOut:"<<makeCMABodyHit()<<endl;
    makeCMABodyHit();
    makeCMABodyTrg();
  } else {
    throw std::runtime_error("MatrixReadOut::makeCMABody: m_CM object does not exist");
  }//end-of-if
}//end-of-MatrixReadOut::makeCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::makeFooter() {
  if(m_CM) {
    writeFooter();
  } else {
    throw std::runtime_error("MatrixReadOut::makeFooter: m_CM object does not exist");
  }//end-of-if(m_CM
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
  if(m_CM) {

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
        for(k=0; k<m_nclock; k++) {          // clock
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
	    //     #1 BCID  filled with  BC 
	    //     #2 TIME  filled with  TIME in DLL steps
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
	    //       #4 STRIP filled with: CHANNEL  or k-readout 
	    //    
	    //	cout<<"rodat="<<m_CM->rodat[i][j][k][l/32]<<" boh="<<(1<<(l%32))<<endl;
	    if( (m_CM->rodat[i][j][k][l/32]&(1<<(l%32))) ) {
	      ubit16 SIDE=i;
	      ubit16 TIME =(k+m_ROOffset)%m_NDLLCYC;     // from struct rpcdata in Matrix
	      ubit16 CHANNEL =l%32;                  // from struct rpcdata
	      ubit16 IJK=0;
	      ubit16 BC=(k+m_ROOffset)/m_NDLLCYC;        // fill BCID word
	      //
	      if (SIDE==0) {                
		IJK=j;         // Fill the IJK-word(3-bit word for CMA_BODY)
	      } else {
		IJK=2*j+l/32+2;
	      }//end-of-if
	      //
	      // WARNING !!! BC>=0 ?? TIME >=0??
	      // Per riprodurre l'output di stefano si deve imporre
	      // BC>0 and BC<=7 and time>0
	      //
              if (BC>=m_BunchFrom && BC<=m_BunchTo){
//  		cout<<"Now store CM_hit with: BCID= "<<BC<<" TIME = "<<TIME
//  		    <<" SIDE = "<<SIDE<<" layer = "<<j
//  		    <<" IJK = " <<IJK<< " CHANNEL = "<<CHANNEL<<endl;
		CMABodyval[0] = 0;
		CMABodyval[1] = BC-m_BunchFrom;
		CMABodyval[2] = TIME;
		CMABodyval[3] = IJK;
		CMABodyval[4] = CHANNEL;
		makeNewHit(m_MROS.makeBody(CMABodyval));
		numberOfHits++;
	      } //end of if(BC
	    }//end-of-if(input[....
	  }//end-of-for(lijk
	}//end-of-for(k
      }//end-of-for(ijk

    //
  } else {
    throw std::runtime_error("MatrixReadOut::makeCMABodyHit: m_CM object does not exist");
  }//end-of-if
  return numberOfHits;
}//end-of-MatrixReadOut::makeCMABodyHit
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::makeCMABodyTrg() {
  ubit16 CMABodyval[5];
  ubit16 channel=0;
  ubit16 BC, TIME, IJK, CHANNEL;
  //int set_latenza;
  //  Removed h_last because it is not used
  //  int h_last[32]; // only pivot plane
//
// initialize h_last with large values
//
  //  for(ubit16 i=0; i<m_nchan[0]; i++) {h_last[i]=0xffff;}
//
  ubit16 numberOfHits=0;
  bool triggerRO; // flag that indicates a trigger hit recorded in the ReadOut
  if(m_CM) {
    CMAword one=1;
    //
    for (int h=0; h<m_nclock;h++){               // loop on time clock
      triggerRO=false; //initialize triggeRO
      for (channel=0; channel<m_nchan[0]; channel++){
	if(m_CM->k_readout[h]&(one<<channel)){ //check K-readout register for 
	  //set_latenza=abs(h-h_last[channel]);
	  //if (set_latenza >= m_timeSeparation) {
	    // a real-trigger data!
	    BC=(h+m_ROOffset)/m_NDLLCYC;       // fill BCID word
	    TIME=(h+m_ROOffset)%m_NDLLCYC;    // from struct rpcdata in Matrix
	    IJK=6;              //Trigger flag for IJK
	    CHANNEL=channel;
	    // h_last[channel]=h;

	    //cout<<"MatrixReadOut: Now store Trigger_hit with: BCID= "<<BC
		//<<" TIME = "<<TIME<<" IJK = "<<IJK<<" CHANNEL = "<<CHANNEL<<endl;
	    //
	    if (BC>=m_BunchFrom && BC<=m_BunchTo){   
	      triggerRO=true; // there is a trigger hit in the ReadOut
	      CMABodyval[0] = 0;
	      CMABodyval[1] = BC-m_BunchFrom;
	      CMABodyval[2] = TIME;
	      CMABodyval[3] = IJK;
	      CMABodyval[4] = CHANNEL;
	      makeNewHit(m_MROS.makeBody(CMABodyval));
	      numberOfHits++;
	    }//end-of-if(BC
	  // }//end of if set_latenza   
	}//end of if k_readout
      }//end of for(channel
//
// insert here the Threshold and overlap word
//
       if(m_CM->highestthRO[h]){
        if(triggerRO) {  // if there is a trigger in the RO add IJK=7 record
         ubit16 thresh_h= m_CM->highestthRO[h];
	 ubit16 over_h=m_CM->overlapRO[h];
	 over_h=over_h<<2;
	 BC=(h+m_ROOffset)/m_NDLLCYC;       // fill BCID word
	 TIME=(h+m_ROOffset)%m_NDLLCYC;
	 CHANNEL =(over_h|thresh_h);
	 IJK=7;
//
//  	 cout<<"MatrixReadOut: Now store Trigger_hit with: BCID= "
//  	     <<BC<<" TIME = "<<TIME
//  	     <<" IJK = "<<IJK<<" CHANNEL = "<<CHANNEL<<endl;
	 //
	 CMABodyval[0] = 0;
	 CMABodyval[1] = BC-m_BunchFrom;
	 CMABodyval[2] = TIME;
	 CMABodyval[3] = IJK;
	 CMABodyval[4] = CHANNEL;
	 makeNewHit(m_MROS.makeBody(CMABodyval));
	 numberOfHits++;
	}//end-of-if(triggerRO
      }//end-of-if(highestthRO  
    }//end of for(int h 
//
//    cout<<"     ** Exit from makeCMABodyTrg method **"<<endl<<endl;
//
  } else {
    throw std::runtime_error("MatrixReadOut::makeHeader: m_CM object does not exist");
  }//end-of-if(m_CM
  return numberOfHits;
}//end-of-MatrixReadOut::makeCMABodyTrg
//----------------------------------------------------------------------------//
void MatrixReadOut::writeHeader(ubit16 CMcode){
  ubit16 headerval[3];
  headerval[0] = 0;
  headerval[1] = CMcode;
  headerval[2] = m_FEL1ID;
  m_Header = m_MROS.makeHeader(headerval);
  m_checkHeaderNum++;//=1
  m_checkHeaderPos=1;//=1
}//end-of-writeHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::writeSubHeader(){
  m_SubHeader = m_MROS.makeSubHeader();
  m_checkSubHeaderNum++;//=1
  m_checkSubHeaderPos=m_checkHeaderPos+1;//=2

}//end-of-MatrixReadOut::writeSubHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::writeCMABody(ubit16 BC, ubit16 TIME, ubit16 IJK,
                                 ubit16 CHANNEL) {
  ubit16 CMABodyval[5];
  CMABodyval[0] = 0;
  CMABodyval[1] = BC;
  CMABodyval[2] = TIME;
  CMABodyval[3] = IJK;
  CMABodyval[4] = CHANNEL;
  sortAndMakeNewHit(m_MROS.makeBody(CMABodyval));
}//end-of-MatrixReadOut::writeCMABody
//----------------------------------------------------------------------------//
void MatrixReadOut::writeFooter(){
  ubit16 footerval;
  footerval = computeCR()&m_first8bitsON;
  m_Footer = m_MROS.makeFooter(footerval);
  m_checkCR=0;
  m_checkFooterPos=m_numberOfWordsInBody+3 ;
  m_checkFooterNum++;
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
  m_numberOfWordsInBody++;
}//end-of-MatrixReadOut::makeNewHit
//----------------------------------------------------------------------------//
void MatrixReadOut::makeNewHit(ubit16 newHit, 
                               CMROData *previous, CMROData *next) {
  CMROData *newElement;
  newElement = new CMROData;
  newElement->hit = newHit;
  newElement->next = next;
  if(next==m_Body) m_Body=newElement; else previous->next = newElement;
  m_numberOfWordsInBody++;
}//end-of-MatrixReadOut::makeNewHit
//----------------------------------------------------------------------------//
void MatrixReadOut::sortAndMakeNewHit(ubit16 newHit) {
  CMROData *p, *previous;//, *newElement;
  char field;
  //cout<<" new Hit is "<<hex<<newHit<<dec<<endl;
  p          = m_Body;
  previous   = m_Body;
  //newElement = 0;
  m_MROS.decodeFragment(newHit,field);
  //cout<<" IELD= "<<field<<endl;
  const ubit16 hitBCID      = m_MROS.bcid();
  const ubit16 hitTIME      = m_MROS.time();
  const ubit16 hitIJK       = m_MROS.ijk();
  const ubit16 hitCHANNEL   = m_MROS.channel();
  //cout<<" decode HIT "<<hitBCID<<" "<<hitTIME<<" "<<hitIJK<<" "<<hitCHANNEL<<endl;
  //
  do {

    if(p) { 
      m_MROS.decodeFragment(p->hit,field);

      if(hitIJK>m_MROS.ijk()) {
	previous=p;
	p=p->next;
      } else 
	if(hitIJK<m_MROS.ijk()) {
	  makeNewHit(newHit,previous,p);
	  break;
	} else
	  if(hitIJK==m_MROS.ijk()) {
	    if(hitBCID>m_MROS.bcid()) {
	      previous=p;
	      p=p->next;
	    } else 
	      if(hitBCID<m_MROS.bcid()) {
		makeNewHit(newHit,previous,p);
		break;
	      } else
		if(hitBCID==m_MROS.bcid()) {
		  if(hitTIME>m_MROS.time()) {
		    previous=p;
		    p=p->next;
		  } else 
		    if(hitTIME<m_MROS.time()) {
		      makeNewHit(newHit,previous,p);
		      break;
		    } else
		      if(hitTIME==m_MROS.time()) {  
			if(hitCHANNEL>m_MROS.channel()) {
			  previous=p;
			  p=p->next;
			} else 
			  if(hitCHANNEL<m_MROS.channel()) {
			    makeNewHit(newHit,previous,p);
			    break;
			  } else {
			    throw std::runtime_error("duplicazione di hit???");
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
  if(m_numberOfWordsInBody) {
    p=m_Body;
    for(i=0;i<m_numberOfWordsInBody; i++) {
      *(Body+i)=p->hit;
      p=p->next;
    }//end-of-for
  }//end-of-if 
}//end-of-MatrixReadout::readCMABody
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::readCMAWord() {
ubit16 output = 0xffff;
     if(m_addressOfWordScanned==0) output = readHeader();
else if(m_addressOfWordScanned==1) output = readSubHeader();
else if(m_addressOfWordScanned==(numberOfFragmentWords()-1)) 
                                output = readFooter();
else output = readCMABodyCurrent();
m_addressOfWordScanned++;
return output;
}//end-of-MatrixReadout::readCMAWord
//----------------------------------------------------------------------------//
MatrixReadOutStructure MatrixReadOut::getCMAHit(int index) {

  MatrixReadOutStructure theStruct;
  ubit16 theHit;
  if((m_numberOfWordsInBody<=index)||(m_numberOfWordsInBody==0)){
    //cout<<" getCMAHit: Wrong index given;"
	//<<" numberOfWordsInBody= "<<m_numberOfWordsInBody
	//<<" index = "<<index<<endl;
  } else {

    CMROData *p;
    p=m_Body;
    for(int i=0;i<m_numberOfWordsInBody; i++) {
      theHit=p->hit;
      if(index==i){theStruct=MatrixReadOutStructure(theHit);}
      p=p->next;
    }
  }
  return theStruct;
}//end-of-MatrixReadOut::getCMAHit(int index)
//----------------------------------------------------------------------------//
void MatrixReadOut::topCMABody() {
m_addressOfWordScanned=0;
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
  ubit16 errorDecode = m_MROS.decodeFragment(m_Header, field);
  if(!errorDecode&&field=='H') {
    stream<<" -- CMID   "<<m_MROS.cmid();
    stream<<" FEL1ID "   <<m_MROS.fel1id();
  } else {
    stream<<" ERROR IN HEADER DECODING ";
  }//end-of-if
  stream<<endl;
}//end-of-MatrixReadOut::displayHeader
//----------------------------------------------------------------------------//
void MatrixReadOut::displaySubHeader(ostream &stream) {
  char field;
  stream<<hex<<m_SubHeader<<dec;
  ubit16 errorDecode = m_MROS.decodeFragment(m_SubHeader, field);
  if(!errorDecode&&field=='S') {
    stream<<" -- FEBCID   "<<m_MROS.febcid();
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
      m_MROS.decodeFragment(p->hit, field);
    if(!errorDecode&&field=='B') {
      stream<<" -- BC     "<<m_MROS.bcid();
      stream<<" TIME   "<<m_MROS.time();
      if(m_MROS.ijk()<7) {
	stream<<" IJK    "<<      m_MROS.ijk();
	stream<<" STRIP  "<<      m_MROS.channel();
      } else {
	stream<<" OVL    "<<      m_MROS.overlap();
	stream<<" THR    "<<      m_MROS.threshold();
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
  ubit16 errorDecode = m_MROS.decodeFragment(m_Footer, field);
  if(!errorDecode&&field=='F') {
    stream<<" CRC "<<hex<<m_MROS.crc()<<dec;
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
//cout<<" MatrixReadOut(ubit16 *pointer, ubit16 num) ";
//    cout<<endl
//    cout<<" check dump "<<endl
//    std::cout<<" m_checkHeaderNum = "<<m_checkHeaderNum
//        <<" CheckHeaderPos = "<<m_checkHeaderPos<<std::endl;
//    cout<<" m_checkSubHeaderNum = "<<m_checkSubHeaderNum
//        <<" CheckSubHeaderPos = "<<m_checkSubHeaderPos<<endl
//    cout<<" m_checkFooterNum = "<<m_checkFooterNum
//        <<" CheckFooterPos = "<<m_checkFooterPos<<endl
//    cout<<" CheckUnkown = "<<m_checkUnkown<<endl
//    cout<<" Number of Words In Fragment = "<<m_numberOfWordsInFrag<<endl;
  ubit16 output = 0;
  if(m_checkHeaderNum!=1 &&                   !(output&0x00000001)) output+=1;//=1
  if(m_checkHeaderPos>1 &&                    !(output&0x00000002)) output+=2;//=1 
  if(m_checkSubHeaderNum!=1 &&                !(output&0x00000004)) output+=4;//=1
  if(m_checkSubHeaderPos!=2 &&                !(output&0x00000008)) output+=8;//=2
  if(m_checkFooterPos!=m_numberOfWordsInFrag && !(output&0x00000010)) output+=16;
  if(m_checkCR &&                             !(output&0x00000020)) output+=32;
  if(m_checkUnkown &&                         !(output&0x00000040)) output+=64;
  if(checkBodyOrder() &&                    !(output&0x00000080)) output+=128;
  return output;
}//end-of-MatrixReadOut::checkFragment
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkCRC8(ubit16 foot) {
  ubit16 output=0;
  if((computeCR()&m_first8bitsON)!=(foot&m_first8bitsON)) output=1;
  return output;
}//end-of-MatrixReadOut::checkCRC8()
//----------------------------------------------------------------------------//
ubit16 MatrixReadOut::checkBodyOrder(bool debugPrint) {
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
    m_MROS.decodeFragment(p->hit,field);
    currIJK     = m_MROS.ijk();
    currBCID    = m_MROS.bcid();
    currTIME    = m_MROS.time();
    currCHANNEL = m_MROS.channel();
    pnext=p->next;
    if(pnext) {
      //      output=0;
      m_MROS.decodeFragment(pnext->hit,field);
      nextIJK     = m_MROS.ijk();
      nextBCID    = m_MROS.bcid();
      nextTIME    = m_MROS.time();
      nextCHANNEL = m_MROS.channel();
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
      
        if(prevIJK!=6) {      // if this m_CM hit has IJK=7 then the previous on must have IJK=6
	 output+=64;
	 outTemp=64;
	}
      
      } else {
      
      }//end-of-if(currIJK
      
      if(outTemp>0 && debugPrint){
        cout<<"checkBodyOrder output= "<<output<<" with "<< hex<<pnext->hit<<dec<<endl;
      }//end-of-if(outTemp
      outTemp=0;     
      
    } else {  // else of if(p->next
     
     if(currIJK==6) {
      output+=64;
      if (debugPrint) cout<<" CheckBodyOrder; IJK 6 exists but the related IJK 7 has been found "<<endl;
     } else if(currIJK==7 && prevIJK!=6) {   // we are at the last m_CM hit; if this has IJK=7
      output+=128;                            // then the previous m_CM hit must have IJK=6
      if (debugPrint) cout<<" CheckBodyOrder; IJK 7 exists but the related IJK 6 has been found "<<endl;
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
void MatrixReadOut::setManager( ReadOutManager* boss ) {
  m_myBoss= boss;
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
//copy the pointer to CMA "CMpointer" in "m_CM" member
//
m_CM = CMpointer;
m_CM->reset();
ubit16 sidemat=0;
ubit16 layer  =0;
ubit16 stripaddress=0;
const ubit16 ROOffset=2;

for(ubit16 n=0; n<m_numberOfWordsInBody; n++) {
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
    throw std::runtime_error("MatrixReadOut::doMatrix: IJK= "+std::to_string(IJK)+" out of RANGE");
  }//end-of-switch
//
// estimate absolute time from CMA time
//
  absTime = 25.*( (float) BCID + ((float) (TIME-ROOffset))/8.);
  m_CM->putData(sidemat,layer,stripaddress,absTime);  
 }//end-of-if(IJK<6
}//end-of-for(ubit16 n
//
// use the Matrix data buffer from the the first available location and not
// at the center as it is done by default
//
m_CM->setBCzero(0);
//
// run the CMA logic
//
m_CM->execute();
}//end-of-MatrixReadOut::doMatrix()
//----------------------------------------------------------------------------//
void MatrixReadOut::makeTestPattern(ubit16 mode, ubit16 ktimes, int eventNum ) {
std::cout<<" makeTestPattern "<<std::endl;
MatrixReadOutStructure MRS=getHeader(); // get the m_CM Header
ubit16 cmid = MRS.cmid(); // m_CM Address (or identifier)
//
ofstream vhdlinput;
vhdlinput.open("vhdl.input",ios::app);
if(!vhdlinput){
 cout<<" File for vhdl analysis not opened. "<<endl
     <<" =================================="<<endl<<endl;
 };
//
const ubit16 maxchan = 100;
const ubit16 maxtimes= 200;
ubit16 IJ[maxtimes][4], channels[maxtimes][4][maxchan]; 
float times[maxtimes]; char plane[4][3];
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
  for(int iCMhit=0; iCMhit<m_numberOfWordsInBody; iCMhit++) {
    MatrixReadOutStructure MRS=getCMAHit(iCMhit);
    int ijk = MRS.ijk();  
    if(ijk<6) {
      float time=((float) (MRS.bcid()*8+MRS.time())-0.5)*3.125;
      //int channel=MRS.global_channel(); // m_CM channel
      //std::cout<<"time "<<time<<" IJK= "<<ijk<<" channel "<<channel<<std::endl;
      if(time>timelast && time<timemin ) timemin=time; 
    }//end-of-if(
  }//end-of-for(

  if(timemin==timeover) {
   completed=true;
  } else { 
    ubit16 this_time_counter=0;
    for(int iCMhit=0; iCMhit<m_numberOfWordsInBody; iCMhit++) {
      MatrixReadOutStructure MRS=getCMAHit(iCMhit);
      int ijk = MRS.ijk();  
      if(ijk<6) {
        int channel=MRS.global_channel(); // m_CM channel
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
