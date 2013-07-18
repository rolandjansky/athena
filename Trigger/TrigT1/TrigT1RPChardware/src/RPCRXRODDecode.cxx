/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <math.h>
#include "TrigT1RPChardware/RPCRXRODDecode.h"

//----------------------------------------------------------------------------//
RPCRXRODDecode::RPCRXRODDecode() : BaseObject(Hardware,"RPCRXRODDecode") {
//std::cout<<" This is the constructor of RPCRXRODDecode "<<std::endl;
//
// define RPC data word indentifiers
//
field      = 0xf000; // field map of word identifier
//reserved3  = 0xd000; // reserved
reserved4  = 0xe000; // reserved
//reserved5  = 0xf000; // reserved
//
// ROD Data Address
//
RODDataAddress=noRecord32;
//
noRecord16=9999;     // no record content for 16bit words
noRecord32=0xdeadcafe; // no record content for 32bit words
//
// the ROD header
//
headerMarker=         noRecord32;
headerSize=           noRecord32;
formatVersion=        noRecord32;
sourceIdentifier=     noRecord32;
Level1ID=             noRecord32;
BunchXingID=          noRecord32;
Level1Type=           noRecord32;
DetectorEventType=    noRecord32;
SourceReserved=       noRecord16;
SourceModuleType=     noRecord16;
SourceSubDetectorID=  noRecord16;
SourceModuleID=       noRecord16;
//
// the RPC identifiers
//
SectorID=noRecord16;
PadID   =noRecord16;
CMID    =noRecord16;
//
// previousRecord initialization
//
previousRecord=Empty;
//
// the structure control flags
//
CMFragCheck=noRecord16;
CMFlag=0;
PADFlag=0;
RXFlag=0;
isSLBody=false;
m_enablePrintOut = false;
m_slFound=false;
}//end-of-RPCRXRODDecode::RPCRXRODDecode
//----------------------------------------------------------------------------//
RPCRXRODDecode::~RPCRXRODDecode() {
}//distructor
//----------------------------------------------------------------------------//
void RPCRXRODDecode::enablePrintOut() {
 m_enablePrintOut = true;
}//RPCRXRODDecode::enablePrintOut
//----------------------------------------------------------------------------//
void RPCRXRODDecode::disablePrintOut() {
 m_enablePrintOut = false;
}//dRPCRXRODDecode::disablePrintOut
//----------------------------------------------------------------------------//
int RPCRXRODDecode::pushWord(const RODword inword, ubit16 j) {
//
// select the less significant 16 bits (j=0) or the
// highest significant 16 bits (j=1) of the 32 bits word inword
// and push this 16 bit word selected to the method pushWord
// for decoding
//
RODword mask1[2];
RODword shift[2];
mask1[0]=0x0000ffff;
mask1[1]=0xffff0000;
shift[0]=0;
shift[1]=16;
return pushWord((inword & mask1[j])>>shift[j]);
}
//----------------------------------------------------------------------------//
int RPCRXRODDecode::pushWord(const ubit16 inword) {
//
// analyze the current inword ReadOutDriver (ROD) data word:
// 1st) identify the type of word and 
// 2nd) fill the MatrixReadOut object when CM hits are present;
// 3rd) close the MatrixReadOut fragment when the CM footer is ecountered;
// 4th) at this point the MatrixReadOut object can deliver the RPC hits
//
//
//
// analysis of patological cases and related actions:
//
// store in a flag, typePrevRec, the type of record of previous dataword:
//
// 0 == no CMA Record;   "0"
// 1 == CMA Header;      "H"
// 2 == CMA SubHeader;   "S"
// 3 == CMA Body;        "B"
// 3 == CMA Footer;      "F"
//
//
// all cases should be accompained by error messages, except cases reported as
// "regular sequence: no problem at all"
//
// -Current record-|-Previous record-----------------Action---------------------
//    type                 type    
//           
//     0                    H       close "CMRO": CMRO.writeRecord(inword,true);
//     0                    S       close "CMRO": CMRO.writeRecord(inword,true);
//     0                    B       close "CMRO": CMRO.writeRecord(inword,true);
//     0                    F       no action needed
//
//     H                    0       no action; no error message
//     H                    H       header overwriting;
//     H                    S       no action
//     H                    B       close "CMRO": CMRO.writeRecord(inword,true);
//     H                    F       no action needed
//
//     S                    0       no action
//     S                    H       regular sequence: no problem at all
//     S                    S       subheader overwriting;
//     S                    B       close "CMRO": CMRO.writeRecord(inword,true);
//     S                    F       no action needed
//
//     B                    0       no action
//     B                    H       no action needed;
//     B                    S       regular sequence: no problem at all
//     B                    B       regular sequence: no problem at all
//     B                    F       no action needed
//
//     F                    0       no action
//     F                    H       no action needed
//     F                    S       no action needed
//     F                    B       regular sequence: no problem at all
//     F                    F       no action needed
//
//
bool thereIsACM=false; // true if the data word is a CM footer 
                       // that closes the CM fragment.
bool thereIsASL=false; // true if the data word is a SL footer 
                       // that closes the SL fragment.
char recField;         // record field
if(m_enablePrintOut) {
  DISP<<" RPCRXRODDecode; inword= "<<std::hex<<inword<<std::dec<<std::endl;
  DISP_DEBUG;
}
//
// recognize first the current word
//
       CMROS.decodeFragment(inword,recField);
       PDROS.decodeFragment(inword,recField);
       RXROS.decodeFragment(inword,recField);
       SLROS.decodeFragment(inword,recField);
//
// Identify the current 16bit word and check it...
//

if(SLROS.isHeader()) {
//
// SL Header
//
        if(m_enablePrintOut) {
          DISP<<" RPCRXRODDecode: SectorLogic Header "<<std::endl;
	  DISP_DEBUG;
	}
        isSLBody=false;
	SLRO.reset();
	SLRO.writeRecord(inword,false);
	thereIsASL=false;
	m_slFound=true;

} else if(SLROS.isSubHeader() && m_slFound) {
//
// SL Header
//
        if(m_enablePrintOut) {
          DISP<<" RPCRXRODDecode: SectorLogic SubHeader "<<std::endl;
	  DISP_DEBUG;
	}
        isSLBody=true;
	SLRO.writeRecord(inword,false);

} else if(SLROS.isFooter() && m_slFound) {
//
// SL Footer
//
        if(m_enablePrintOut) {
          DISP<<" RPCRXRODDecode: SectorLogic Footer "<<std::endl;
	  DISP_DEBUG;
        }
        isSLBody=false;
	thereIsASL=true;
	SLRO.writeRecord(inword,true);

	m_slFound=false;

} else if(isSLBody && m_slFound)         {
//
// SL Body
//
        if(m_enablePrintOut) {
          DISP<<" RPCRXRODDecode: SectorLogic Body "<<std::endl;
	  DISP_DEBUG;	
        }
	SLRO.writeRecord(inword,false);
	thereIsASL=false;
	
} else if(RXROS.isHeader()) {

//
// RX header
//
        if(m_enablePrintOut) {
          DISP<<" RPCRXRODDecode: RX Header "<<std::endl;
	  DISP_DEBUG;
        }	  
	RXFlag++;
	RXROS.decodeFragment(inword,recField);
	if(recField=='H') {
	 SectorID=RXROS.RXid();
	}
	thereIsACM=false;
	if(m_enablePrintOut) {
	  DISP<<" Sector ID = "<<SectorID<<std::endl;
	  DISP_DEBUG;
	}
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and RX Header found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and and RX Header found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	   DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and RX Header found"<<std::endl;
	   DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Header or Pad Footer expected and RX Header found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }  
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;	
	
} else if(RXROS.isFooter()) {

//
// RX footer
//
        if(m_enablePrintOut) {
          DISP<<" RX footer "<<std::endl;
	  DISP_DEBUG;
	}
	RXFlag--;
        RXROS.decodeFragment(inword,recField);
	if(recField=='F') {
	}
	thereIsACM=false;
	if(m_enablePrintOut) {
	  DISP<<" RPCRXRODDecode: Footer of Sector with ID = "<<SectorID<<std::endl;
	  DISP_DEBUG;
	}
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and RX Footer found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and RX Footer found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and and RX Footer found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Header or Pad Footer expected and RX Footer found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;



} else if(PDROS.isHeader()) {

//
// Pad header
//
        if(m_enablePrintOut) {
          DISP<<" PAD Header "<<std::hex<<inword<<std::dec<<std::endl;
	  DISP_DEBUG;
	}
	PADFlag++;
	PDROS.decodeFragment(inword,recField);
	if(recField=='H') {
	 PadID=PDROS.padid();
	}
	thereIsACM=false;
	if(m_enablePrintOut) {
	  DISP<<" PAD "<<PadID<<" LVL1 "<<std::hex<<((inword & 0x0e00)>>8)<<std::dec<<std::endl;
	  DISP_DEBUG;
	}
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	   DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	   DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;


} else if(PDROS.isSubHeader()) {

//
// Pad Subheader
//
        if(m_enablePrintOut) {
	  DISP<<"   BCID "<<std::hex<<(inword & 0x0fff)<<std::dec<<std::endl;
	  DISP_DEBUG;
	}
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
            DISP<<" RPCRXRODDecode Fragment problem: Pad Header expected and not found"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }  
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }  
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;

} else if(PDROS.isPreFooter()) {

//
// Pad PreFooter
//
        if(m_enablePrintOut) {
	  DISP<<"   STATUS ERROR "<<std::hex<<(inword & 0x000f)<<std::dec<<std::endl;
	  DISP_DEBUG;
	}
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	   DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and not found"<<std::endl;
	   DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=PadPre;

} else if(PDROS.isFooter()) {

//
// Pad footer
//
        if(m_enablePrintOut) {
          DISP<<" PAD Footer "<<std::hex<<inword<<std::dec
	      <<"  ERROR FLAG "<<std::hex<<(inword & 0x0fff)<<std::dec<<std::endl;
	  DISP_DEBUG;
	}
	PADFlag--;
	PDROS.decodeFragment(inword,recField);
	if(recField=='F') {
	}
	thereIsACM=false;
	if(m_enablePrintOut) {
	  DISP<<" Footer of Pad with ID = "<<PadID<<std::endl;
	  DISP_DEBUG;
	}
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected and not found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;

} else if(CMROS.isHeader()) {

//
// CM Header: reset MatrixReadOut and load the word
//
	CMFlag++;
        CMRO.reset();
	CMRO.writeRecord(inword,false);
        thereIsACM=false;
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: CM SubHeader expected and CM Header found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMSub:
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: CM Body or CM Footer expected and CM Header found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMFoot:
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  } 
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=CMHead;
	

} else if(CMROS.isSubHeader()) {

//
// CM Subheader
//
	CMFlag++;
	CMRO.writeRecord(inword,false);
	thereIsACM=false;
	
	switch(previousRecord){
	 case Empty:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: previous record was not a CM Header"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMHead:
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: CM Body expected and CM SubHeader found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMBod:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: CM Body or CM Footer expected; CM SubHeader found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: CM Header or Pad Footer expected; CM SubHeader found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=CMSub;
	
} else if( CMROS.isBody()) {	  
//
// CM hit
//	  
	if(CMFlag) CMRO.writeRecord(inword,false);
	thereIsACM=false;
	
	switch(previousRecord){
	 case Empty:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMBody1) warning: previous record was not a CM subHeader"
	        <<" or CM Body Record "<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMBody2) warning: previous record was not a CM subHeader"
	        <<" or CM Body Record "<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMSub:
	  break;
	 case CMBod:
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMBody3) warning: previous record was not a CM subHeader"
	             <<" or CM Body Record "<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord="<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=CMBod;

	
} else if(CMROS.isFooter()) {

//
// CM footer
//  
	thereIsACM=true;
	CMFlag--;CMFlag--;
	CMRO.writeRecord(inword,true);
	CMFragCheck = CMRO.checkFragment();
	
	switch(previousRecord){
	case Empty:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMFooter1) warning: previous record was not a Body record "<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMFooter2) warning: previous record was not a Body record"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode warning: previous record was not a Body record"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case CMBod:
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode (CMFooter3) warning: previous record was not a Body record"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord"<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=CMFoot;
	
         //gimeCMROData();
	
//} else if(((inword & field)==reserved3) || ((inword & field)==reserved4)) {
} else if((inword & field)==reserved4) {

//
// reserved word
//	  
	thereIsACM=false; 
	if(m_enablePrintOut) { 
          DISP<<" (reserved) "<<std::endl;
	  DISP_DEBUG;
	}
	
	switch(previousRecord){
	 case Empty:
	  break;
	 case CMHead:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: CM SubHeader expected and (reserved) found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMSub:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body expected and (reserved) found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMBod:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: CM Body or Footer expected and (reserved) found"<<std::endl;
	    DISP_DEBUG;
	  }
	  thereIsACM=true; // close this matrix fragment
	  CMRO.writeRecord(inword,true);
	  // gimeCMROData();
	  break;
	 case CMFoot:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: CM Header or Pad Footer expected and (reserved) found"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadHead:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: PadSubHeader expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadPre:
	  if(m_enablePrintOut) { 
	    DISP<<" RPCRXRODDecode Fragment problem: CMA fragment expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadSub:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: CMA Footer expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 case PadFoot:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode Fragment problem: Pad PreFooter expected"<<std::endl;
	    DISP_DEBUG;
	  }
	  break;
	 default:
	  if(m_enablePrintOut) {
	    DISP<<" RPCRXRODDecode ERROR in previousRecord "<<std::endl;
	    DISP_DEBUG;
	  }
	}//end-of-switch 
	previousRecord=Empty;
	
}//end-of-if
int answer=0;
if(thereIsACM) answer=1;
if(thereIsASL) answer=2;
return answer;
}
//----------------------------------------------------------------------------//
void RPCRXRODDecode::gimeCMROData() {
MatrixReadOutStructure CMROS;
ubit16 nCMROData=CMRO.numberOfBodyWords();

if(m_enablePrintOut) {
  DISP<<" number of Body Words in this CM Fragment "<<nCMROData<<std::endl;
  DISP_DEBUG;
  
//
// get header data
//
  CMROS = CMRO.getHeader();

  DISP<<" gimeCMROData,   CMID: "<<CMROS.cmid()<<std::endl
      <<" gimeCMROData, FEL1ID: "<<CMROS.fel1id()<<std::endl
      <<" isLowPt  = "<<CMROS.isLowPt()<<std::endl
      <<" isEtaProj= "<<CMROS.isEtaProj()<<std::endl
      <<" is0LocAdd= "<<CMROS.is0LocAdd()<<std::endl;
  DISP_DEBUG;

//
// get subheader data
//
  CMROS = CMRO.getSubHeader();

  DISP<<" gimeCMROData, FEBCID: "<<CMROS.febcid()<<std::endl;
  DISP_DEBUG;

//
// get Body data
//
  for(ubit16 i=0; i<nCMROData; i++) {
   CMROS = CMRO.getCMAHit(i);
   ubit16 ijk = CMROS.ijk();
          if(ijk<6)  {
     DISP<<" gimeCMROData, hit chan: "<<CMROS.channel()
         <<" gimeCMROData, hit stri: "<<CMROS.global_channel()
         <<" hit BCID: "<<CMROS.bcid();
     DISP_DEBUG;
   } else if(ijk==6) {
     DISP<<" gimeCMROData, trg chan: "<<CMROS.channel()
         <<" hit BCID: "<<CMROS.bcid();
     DISP_DEBUG;
   } else if(ijk==7) {
    DISP<<" gimeCMROData, trg thr: "<<CMROS.threshold()
        <<" trg ovl: "<<CMROS.overlap();
    DISP_DEBUG;
   } else {
    DISP<<" this value of ijk="<<ijk<<" is NOT ALLOWED "<<std::endl;
    DISP_DEBUG;
   }//end-of-if
   DISP<<std::endl;
   DISP_DEBUG;
  }//end-of-for
}//end-of-if(m_enablePrintOut
}//end-of-gimeCMROData
//---------------------------------------------------------------------------//
void RPCRXRODDecode::RODHeader(const RODword *RODData) {
//
// decode the ROD header
//
headerSize = *(RODData+1);
for(ubit16 i=0; i<headerSize; i++) {

       if(i==0) {
  headerMarker=*(RODData+i);
} else if(i==2) {
  formatVersion=*(RODData+i);
} else if(i==3) {
  sourceIdentifier=*(RODData+i);
  RPCRODStructure RPCRODS;
  RPCRODS.decodeSourceID(sourceIdentifier);
  SourceModuleID = RPCRODS.getSourceIDRODID();
  SourceSubDetectorID = RPCRODS.getSourceIDSubdetectorID();
  SourceModuleType = RPCRODS.getSourceIDModuleType();
  SourceReserved = RPCRODS.getSourceIDReserved();
} else if(i==4) {
  Level1ID=*(RODData+i);
} else if(i==5) {
  BunchXingID=*(RODData+i);
} else if(i==6) {
  Level1Type=*(RODData+i);
} else if(i==7) {
  DetectorEventType=*(RODData+i);
}//end-of-if
}//end-of-for
}//end-of-RODHeader
//----------------------------------------------------------------------------//
//void RPCRXRODDecode::ROBAddresses(const RODword *ROBData) {
////
//// calculate ROB-ROD Addresses and ROB-ROD counters
////
//ROBTotalSize      = *(ROBData + locROBTotalFragSize-1);
//RODDataSize       = *(ROBData + ROBTotalSize-1 + locRODNumberOfDataElements);
//RODStatusSize     = *(ROBData + ROBTotalSize-1 + locRODNumberOfStatusElements);
//RODStatusPosition = *(ROBData + ROBTotalSize-1 + locRODStatusBlockPosition);
//RODBlockSize =   RODDataSize + RODStatusSize;
//if(RODStatusPosition) {
// RODDataAddress = ROBTotalSize-(RODBlockSize+RODTrailerSize-1);
//} else {
// RODDataAddress = ROBTotalSize-(RODDataSize+RODTrailerSize-1);
//}
//
//cout<<" Object ROB Total Size  "<<ROBTotalSize<<endl;
//cout<<" Object ROD Data Size   "<<RODDataSize<<endl;
//cout<<" Object ROD Status Size "<<RODStatusSize<<endl;
//cout<<" Object ROD Data Address "<<RODDataAddress<<endl;
//}//end-of-ROBAddresses
//----------------------------------------------------------------------------//
void RPCRXRODDecode::RODAddresses(const RODword *RODData,
                                const sbit32 numberOfStatusElements,
				const sbit32 statusBlockPosition) {
RODHeader(RODData);			
std::cout<<" status block position "<<statusBlockPosition<<std::endl;
std::cout<<" number of Status Elements "<<numberOfStatusElements<<std::endl;
if(statusBlockPosition) {
 RODDataAddress = headerSize;
} else {
 RODDataAddress = headerSize+numberOfStatusElements;
}
}//end-of-RPCRXRODDecode::RODAddresses
//----------------------------------------------------------------------------//
void RPCRXRODDecode::RODHeaderDisplay() {
if(m_enablePrintOut) {
  DISP<<"*****************************************************"<<std::endl
      <<"******         R O D    H E A D E R           *******"<<std::endl
      <<"*****************************************************"<<std::endl
      <<" ROD Header marker:               "<<std::hex<<headerMarker<<std::dec<<std::endl
      <<" ROD Header size:                 "<<headerSize<<std::endl
      <<" ROD Format version number:       "<<formatVersion<<std::endl
      <<" ROD Source Identifier:           "<<sourceIdentifier<<std::endl
      <<"      ROD Source Module ID:       "<<SourceModuleID<<std::endl
      <<"      ROD Source SubDectID:       "<<SourceSubDetectorID<<std::endl
      <<"      ROD Source Module Ty:       "<<SourceModuleType<<std::endl
      <<"      ROD Source Reserved :       "<<SourceReserved<<std::endl
      <<" ROD Level1 ID:                   "<<Level1ID<<std::endl
      <<" ROD BuncgXingID:                 "<<BunchXingID<<std::endl
      <<" ROD Level1 Type:                 "<<Level1Type<<std::endl
      <<" ROD Detector Ev. Type:           "<<DetectorEventType<<std::endl
      <<"*****************************************************"<<std::endl;
  DISP_DEBUG;
}//end-of-if(m_enablePrintOut
}//end-of-RODHeaderDisplay
