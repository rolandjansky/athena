/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCStripHighPtBoard.hh"
#include <iostream>
#include <cstdlib>

namespace LVL1TGCTrigger {

TGCStripHighPtBoard::TGCStripHighPtBoard():
  TGCHighPtBoard()   
{
  m_maxDev = 15;
  m_maxDevOred = 20;
  m_nChOfTSBOut = 64;
  m_nChInTSBRegion = m_nChOfTSBOut/4;
  m_nChOfDSBOut = 64;
  m_nChOfDSBHit = 3*m_nChOfDSBOut;
  m_nChOfTSBHit = 3*m_nChOfTSBOut;
}
  

void TGCStripHighPtBoard::setDecoderIn()
{
  if(m_region == Endcap){
      m_decoderInDSB[ChipA][0] = m_DSBOut[ChipA][0];
      m_decoderInDSB[ChipA][1] = m_DSBOut[ChipA][1];
      m_decoderInDSB[ChipA][2] = m_DSBOut[ChipA][2];

      m_decoderInDSB[ChipB][0] = m_DSBOut[ChipB][0];
      m_decoderInDSB[ChipB][1] = m_DSBOut[ChipB][1];
      m_decoderInDSB[ChipB][2] = m_DSBOut[ChipB][2];

      m_decoderInTSB[ChipA][0] = divideTSBOut(1,m_TSBOut[ChipA][1]);
      m_decoderInTSB[ChipA][1] = divideTSBOut(0,m_TSBOut[ChipA][1]);
      m_decoderInTSB[ChipA][2] = divideTSBOut(1,m_TSBOut[ChipB][0]);
      m_decoderInTSB[ChipA][3] = 0;
      
      m_decoderInTSB[ChipB][0] = divideTSBOut(1,m_TSBOut[ChipB][0]);
      m_decoderInTSB[ChipB][1] = divideTSBOut(0,m_TSBOut[ChipB][0]);
      m_decoderInTSB[ChipB][2] = 0;
      m_decoderInTSB[ChipB][3] = 0;

  }
  
  if(m_region == Forward) {
      m_decoderInDSB[ChipA][0] = 0;
      m_decoderInDSB[ChipA][1] = m_DSBOut[ChipA][1];
      m_decoderInDSB[ChipA][2] = 0;
      
      m_decoderInDSB[ChipB][0] = 0;
      m_decoderInDSB[ChipB][1] = 0;
      m_decoderInDSB[ChipB][2] = 0;


      m_decoderInTSB[ChipA][0] = 0; 
      m_decoderInTSB[ChipA][1] = m_TSBOut[ChipA][1];
      m_decoderInTSB[ChipA][2] = 0;
      m_decoderInTSB[ChipA][3] = 0;
      
      m_decoderInTSB[ChipB][0] = 0;
      m_decoderInTSB[ChipB][1] = 0;
      m_decoderInTSB[ChipB][2] = 0;
      m_decoderInTSB[ChipB][3] = 0;
      
  }
}

void TGCStripHighPtBoard::clearDecoderIn()
{
  if(m_region == Endcap){
      if(m_decoderInTSB[ChipA][0]) delete m_decoderInTSB[ChipA][0];
      if(m_decoderInTSB[ChipA][1]) delete m_decoderInTSB[ChipA][1];
      if(m_decoderInTSB[ChipA][2]) delete m_decoderInTSB[ChipA][2];
      if(m_decoderInTSB[ChipB][0]) delete m_decoderInTSB[ChipB][0];
      if(m_decoderInTSB[ChipB][1]) delete m_decoderInTSB[ChipB][1];
  }
  int chip,j;
  for( chip=0; chip<NumberOfChip; chip+=1){
    for( j=0; j<NDecoderInTSB; j+=1) m_decoderInTSB[chip][j]=0;
    for( j=0; j<NDecoderInDSB; j+=1) m_decoderInDSB[chip][j]=0;
  }
}

TGCSlaveBoardOut* TGCStripHighPtBoard::divideTSBOut(int iSet, 
						    TGCSlaveBoardOut* SBOut)
{
  // divide output of TripletSlaveBoard to distribute to decoder channels.
  // SBOut     out
  //   a   ->   (iSet=0)
  //   b   ->   (iSet=0)
  //   c   ->   (iSet=0)
  //   d   ->   (iSet=0)
  //   a   ->   (iSet=1)
  //   b   ->   (iSet=1)
  //   c   ->   (iSet=1)
  //   d   ->   (iSet=1)

  if(SBOut!=0)
    {
      TGCSlaveBoardOut* out =
	new TGCSlaveBoardOut(SBOut->getOrigin(), SBOut->getBid());
      int nData = SBOut->getNumberOfData();// total number of block
      out->setNumberOfData(nData/2);
      
      int i;
      for( i=0; i< out->getNumberOfData(); i+=1)
	{
	  out->setDev(i,SBOut->getDev((nData/2)*iSet+i));
	  out->setPos(i,SBOut->getPos((nData/2)*iSet+i));
	  out->setHit(i,SBOut->getHit((nData/2)*iSet+i));
	  out->setbPos(i,SBOut->getPos((nData/2)*iSet+i));
	}
      return out;
    }
  else
    {
      return 0;
    }
}


void TGCStripHighPtBoard::decodeSlaveBoardOut(int chip, 
					      TGCHitPattern* tHit, 
					      TGCHitPattern* dHit)
{
  int block,j,ch;
  // input from TripletSlaveBoard
#ifdef TGCCOUT  
  std::cout << "#--Strip decodeSlaveBoardOut()" << std::endl;
  std::cout << "Triplet" << std::endl;
#endif

  for( j=0; j<NDecoderInSTSB; j+=1) {
    if(m_decoderInTSB[chip][j]!=0) {
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInSTSB-1 << " in a chip  ";
      std::cout<<"SHPB:decoderInTSB["<<chip<<"]["<<j<<"]:";
      m_decoderInTSB[chip][j]->print();
#endif
      for( block=0; block<m_decoderInTSB[chip][j]->getNumberOfData(); block+=1) {
	if(m_decoderInTSB[chip][j]->getHit(block)) {
	  // input to the 2nd Decoder.
	  if(j>0) {
	    ch=(j-1)*m_nChOfTSBOut+block*m_nChInTSBRegion
	      +m_decoderInTSB[chip][j]->getPos(block);
	    if((ch>=0)&&(ch<m_nChOfTSBHit))
	      tHit->onChannel(ch);
#ifdef TGCCOUT
	    else
	      std::cout<<"decodeSlaveBoardOut ch= "<<ch<<std::endl;
#endif
	  }
	  
	  // input to the 2nd Decoder.
	  ch=j*m_nChOfTSBOut+block*m_nChInTSBRegion
	    +m_decoderInTSB[chip][j]->getPos(block);
	  if((ch>=0)&&(ch<m_nChOfTSBHit))
	    tHit->onChannel(ch);
#ifdef TGCCOUT
	  else
	    std::cout<<"decodeSlaveBoardOut ch= "<<ch<<std::endl;
#endif		  
	  // input to the 1st Decoder.
	  if(j<NDecoderInSTSB){
	      ch=(j+1)*m_nChOfTSBOut+block*m_nChInTSBRegion
		+m_decoderInTSB[chip][j]->getPos(block);
	      if((ch>=0)&&(ch<m_nChOfTSBHit))
		tHit->onChannel(ch);
#ifdef TGCCOUT
	      else
		std::cout<<"decodeSlaveBoardOut ch= "<<ch<<std::endl;
#endif
	  }
	}
      }
    }
  }
  
  // input from DoubletSlaveBoard.
#ifdef TGCCOUT
  std::cout << "Doublet" << std::endl;
#endif
  for( j=0; j<NDecoderInDSB; j+=1){
    if(m_decoderInDSB[chip][j]!=0) {
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInDSB-1 << " in a chip  ";
      std::cout<<"SHPB:decoderInDSB["<<chip<<"]["<<j<<"]:";
      m_decoderInDSB[chip][j]->print();
#endif
      for( block=0; block<m_decoderInDSB[chip][j]->getNumberOfData(); block+=1) {
	if(m_decoderInDSB[chip][j]->getHit(block))
	  dHit->onChannel(j*m_nChOfDSBOut+m_nChOfDSBOut/2*block
			  +m_decoderInDSB[chip][j]->getPos(block));
      }
    }
  }
}

void TGCStripHighPtBoard::doCoincidenceMatrix(int chip, 
					      int block, 
					      const TGCHitPattern* tHit, 
					      const TGCHitPattern* dHit)
{
  int nChUnitOfDSBHit=m_nChOfDSBHit/NBlockOfDSBChannel;
  int maxTSBCh=(block+1)*nChUnitOfDSBHit;
  int minTSBCh=block*nChUnitOfDSBHit;
#ifdef TGCCOUT
  std::cout << "# Strip doCoincidenceMatrix() chip:" << chip << " block:" << block << std::endl;
#endif
  for(int dev=0; dev<=m_maxDev; dev+=1) {
    // convert Nonlinear dPhi
    int dp = getNonLinearDelta(dev);
    if (dp<0) continue;

    int Oring, DevOrg;
    int DSBCh, TSBCh;
    for (int isgn=1; isgn >-2; isgn-=2) {
      if ( (dev==0) && (isgn==-1) ) continue;
      for(int i=0; i<nChUnitOfDSBHit; i+=1) {
	//total ch from doublet in a block
	if(dev>2*m_maxDev-m_maxDevOred){
	  Oring = 1;
	  DevOrg = 2*m_maxDev-m_maxDevOred + (dev - (2*m_maxDev-m_maxDevOred))*2 - 1;
	} else {
	  Oring = 0;
	  DevOrg = dev;
	}
	for(int ior = 0; ior <= Oring; ior++) {
	  DSBCh = nChUnitOfDSBHit*block + i;
	  TSBCh = DSBCh + isgn*m_priorSign*(DevOrg + ior);
          if ( ((isgn>0) && ( (block%2==0)||((block%2==1)&&(TSBCh<maxTSBCh))  ) ) ||  // dphi>=0
               ((isgn<0) && ( (block%2==1)||((block%2==0)&&(TSBCh>=minTSBCh)) ) ) ){  // dphi<0
	    if(tHit->getChannel(TSBCh) && dHit->getChannel(DSBCh)) {
#ifdef TGCDEBUG
	      std::cout<< "HighPt" << std::endl 
		       << " th&&dh==1 priorSign*dev:" << m_priorSign 
		       << "*" << dev
		       << " DSBCh:" << DSBCh
		       << " TSBCh:" << TSBCh << std::endl;
#endif
	      m_highPtBoardOut->setPt(chip,block,PtHigh);
	      m_highPtBoardOut->setHit(chip,block,true);
	      m_highPtBoardOut->setDev(chip,block,isgn*m_priorSign*dp);
	      // assume that a unit of position is quarter of nch of DSB. 
	      m_highPtBoardOut->setPos(chip,block,i/(m_nChOfDSBOut/4)); // !
	      return;
	    }
	  }
	} // loop ior
      } // loop i
    } //loop isgn
    
  } //loop dev
  m_highPtBoardOut->setHit(chip,block,false);
}

void TGCStripHighPtBoard::highLowSelector(int chip)
{
  int iblock;
  int highPt1stBlock, highPt2ndBlock;
  int lowPt1stBlock, lowPt2ndBlock;
  int pos, dev;
  // Get 1st, 2nd candidate block for High-PT
  highPt1stBlock = -1;
  highPt2ndBlock = -1;
  for(iblock=0;iblock<NBlockOfDSBChannel;iblock+=1){
    switch (m_highPtBoardOut->getSel(chip,iblock)){
      case 0:	// No Hit
        break;
      case 1:   // 1st candidate
        highPt1stBlock = iblock;
        break;
      case 2:   // 2nd candidate
        highPt2ndBlock = iblock;
        break;
      default:
        break;
    }
  }
  // Get 1st, 2nd candidate block for Low-PT
  lowPt1stBlock = -1;
  lowPt2ndBlock = -1;
  for(iblock=0;iblock<NBlockOfDSBChannel;iblock+=1){
    switch (m_lowPtBoardOut->getSel(chip,iblock)){
      case 0:	// No Hit
        break;
      case 1:   // 1st candidate
        lowPt1stBlock = iblock;
        break;
      case 2:   // 2nd candidate
        lowPt2ndBlock = iblock;
        break;
      default:
        break;
    }
  }

  if (highPt1stBlock < 0){		//No HPT Matrix Output
    if (lowPt1stBlock >= 0){
      // LPT 1st
      pos=m_lowPtBoardOut->getPos(chip,lowPt1stBlock);
      dev=m_lowPtBoardOut->getDev(chip,lowPt1stBlock);
      m_highPtChipOut->setPt(chip,FirstCandidate,PtLow);
      m_highPtChipOut->setSel(chip,FirstCandidate,1);
      m_highPtChipOut->setDev(chip,FirstCandidate,dev);
      m_highPtChipOut->setPos(chip,FirstCandidate,pos);
      m_highPtChipOut->setHitID(chip,FirstCandidate,lowPt1stBlock);
    }
    if (lowPt2ndBlock >= 0){
      // LPT 2nd
      pos=m_lowPtBoardOut->getPos(chip,lowPt2ndBlock);
      dev=m_lowPtBoardOut->getDev(chip,lowPt2ndBlock);
      m_highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      m_highPtChipOut->setSel(chip,SecondCandidate,2);
      m_highPtChipOut->setDev(chip,SecondCandidate,dev);
      m_highPtChipOut->setPos(chip,SecondCandidate,pos);
      m_highPtChipOut->setHitID(chip,SecondCandidate,lowPt2ndBlock);
    }
    return;
  } else if (highPt2ndBlock < 0){	// 1 HPT Matrix Output
    // HPT 1st
    pos=m_highPtBoardOut->getPos(chip,highPt1stBlock);
    dev=m_highPtBoardOut->getDev(chip,highPt1stBlock);
    if ((dev>-8) && (dev<8)) { // check if |dev|<8
      m_highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
      m_highPtChipOut->setSel(chip,FirstCandidate,1);
      m_highPtChipOut->setDev(chip,FirstCandidate,dev);
      m_highPtChipOut->setPos(chip,FirstCandidate,pos);
      m_highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);
    }

    if ((lowPt1stBlock >= 0) &&
	!((lowPt1stBlock == highPt1stBlock) && ((m_lowPtBoardOut->getPos(chip,lowPt1stBlock)) == (m_highPtBoardOut->getPos(chip,highPt1stBlock))))
	){			//for vi }
      // LPT 1st (as 2nd candidate)
      pos=m_lowPtBoardOut->getPos(chip,lowPt1stBlock);
      dev=m_lowPtBoardOut->getDev(chip,lowPt1stBlock);
      m_highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      m_highPtChipOut->setSel(chip,SecondCandidate,2);
      m_highPtChipOut->setDev(chip,SecondCandidate,dev);
      m_highPtChipOut->setPos(chip,SecondCandidate,pos);
      m_highPtChipOut->setHitID(chip,SecondCandidate,lowPt1stBlock);
      return;
    } else if ((lowPt2ndBlock >= 0) &&
	       !((lowPt2ndBlock == highPt1stBlock) && ((m_lowPtBoardOut->getPos(chip,lowPt2ndBlock)) == (m_highPtBoardOut->getPos(chip,highPt1stBlock))))
	       ){
      
      // LPT 2nd (as 2nd candidate)
      pos=m_lowPtBoardOut->getPos(chip,lowPt2ndBlock);
      dev=m_lowPtBoardOut->getDev(chip,lowPt2ndBlock);
      m_highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      m_highPtChipOut->setSel(chip,SecondCandidate,2);
      m_highPtChipOut->setDev(chip,SecondCandidate,dev);
      m_highPtChipOut->setPos(chip,SecondCandidate,pos);
      m_highPtChipOut->setHitID(chip,SecondCandidate,lowPt2ndBlock);
      return;
    }
  } else {				// 2 HPT Matrix Outputs
    // HPT 1st
    pos=m_highPtBoardOut->getPos(chip,highPt1stBlock);
    dev=m_highPtBoardOut->getDev(chip,highPt1stBlock);
    if ((dev>-8) && (dev<8)) { // check if |dev|<8
      m_highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
      m_highPtChipOut->setSel(chip,FirstCandidate,1);
      m_highPtChipOut->setDev(chip,FirstCandidate,dev);
      m_highPtChipOut->setPos(chip,FirstCandidate,pos);
      m_highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);
    }
    // HPT 2nd
    pos=m_highPtBoardOut->getPos(chip,highPt2ndBlock);
    dev=m_highPtBoardOut->getDev(chip,highPt2ndBlock);
    if ((dev>-8) && (dev<8)) { // check if |dev|<8
      m_highPtChipOut->setPt(chip,SecondCandidate,PtHigh);
      m_highPtChipOut->setSel(chip,SecondCandidate,2);
      m_highPtChipOut->setDev(chip,SecondCandidate,dev);
      m_highPtChipOut->setPos(chip,SecondCandidate,pos);
      m_highPtChipOut->setHitID(chip,SecondCandidate,highPt2ndBlock);
    }
  }
}

int TGCStripHighPtBoard::getNonLinearDelta(int dev) const
{
  if (dev >=12)         return 8; // set dev=8 in case of out of range
  else if (dev >=10)    return 7;
  else if (dev >=8)     return 6;
  else if (dev >=6)     return 5;
  else if (dev >=4)     return 4;
  else                  return dev;
}
  
} //end of namespace bracket
