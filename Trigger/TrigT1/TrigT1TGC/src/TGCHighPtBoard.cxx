/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtBoardOut.hh"
#include "TrigT1TGC/TGCHighPtBoard.hh"
#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include <iostream>
#include <cmath>
#include <cstdlib>

namespace LVL1TGCTrigger {

TGCHighPtBoard::TGCHighPtBoard():
  m_highPtChipOut(0), m_highPtBoardOut(0), m_lowPtBoardOut(0),
  m_id(-1),m_bid(-1),m_idSectorLogic(-1),m_type(-1),m_region(-1),
  m_priorSign(1),m_maxNumberOfHPBData(2),
  m_maxDev(0), m_maxDevOred(0),
  m_nChOfTSBOut(0), m_nChOfDSBOut(0), m_nChOfDSBHit(0), m_nChOfTSBHit(0), 
  m_nChInTSBRegion(0)
{
  for( int i=0; i<NumberOfChip; i+=1){
    for( int j=0; j<NumberOfDSBOut; j+=1){
      m_DSB[i][j]=0;
      m_DSBOut[i][j]=0;
    }
    for( int j=0; j<NumberOfTSBOut; j+=1){
      m_TSB[i][j]=0;
      m_TSBOut[i][j]=0;
    }
  }

  for( int i=0; i<NumberOfAdjacentHPB; i+=1)
    m_adjacentHPB[i]=0;

  for( int i=0; i<NumberOfChip; i+=1){
    for( int j=0; j<NDecoderInTSB; j+=1) m_decoderInTSB[i][j]=0;
    for( int j=0; j<NDecoderInDSB; j+=1) m_decoderInDSB[i][j]=0;
  }
}

TGCHighPtBoard::~TGCHighPtBoard()
{
  int i,j;
  for( i=0; i<NumberOfChip; i+=1){
    for( j=0; j<NumberOfDSBOut; j+=1){
      m_DSB[i][j]=0;
      if(m_DSBOut[i][j]!=0){
        delete m_DSBOut[i][j];
        m_DSBOut[i][j]=0;
      }
    }
    for( j=0; j<NumberOfTSBOut; j+=1){
      m_TSB[i][j]=0;
      if(m_TSBOut[i][j]!=0){
        delete m_TSBOut[i][j];
        m_TSBOut[i][j]=0;
      }
    }
  }

  for( i=0; i<NumberOfChip; i+=1){
    for( j=0; j<NDecoderInTSB; j+=1){
      if(m_decoderInTSB[i][j]!=0) delete m_decoderInTSB[i][j];
      m_decoderInTSB[i][j]=0;
    }
    for( j=0; j<NDecoderInDSB; j+=1){
      if(m_decoderInDSB[i][j]!=0) delete m_decoderInDSB[i][j];
      m_decoderInDSB[i][j]=0;
    }
  }

  for( i=0; i<NumberOfAdjacentHPB; i+=1)  
    m_adjacentHPB[i]=0;
}

void TGCHighPtBoard::clockIn(int bidIn)
{
  if(m_bid!=bidIn){
#ifdef TGCCOUT
    std::cout << "#TGCHighPtBoard::clockIn collect" << std::endl;
#endif
    m_bid = bidIn;
    deleteSlaveBoardOut();
    collectInput();
    return;
  }else{
#ifdef TGCCOUT
    std::cout << "#TGCHighPtBoard::clockIn  BID:" << m_bid 
	      << " TYPE:" << ((m_type==WHPB) ? "WHPB" : "SHPB" )
	      << " ID:" << m_id 
	      << " REGION:" << ( (m_region==FORWARD) ? "FWD" : "END")
	      <<std::endl;
#endif
    createHighPtChipOut();	//Feeds to SL
    createHighPtBoardOut();	//Internal Use
    createLowPtBoardOut();	//Internal Use
    doCoincidence();
    showResult();
    eraseHighPtOutput();
    eraseLowPtOutput();
  }
  return;
}

void TGCHighPtBoard::deleteSlaveBoardOut()
{
  int i,j;
  for( j=0; j<NumberOfChip; j+=1){
    for( i=0; i<NumberOfDSBOut; i+=1){
      if(m_DSBOut[j][i]!=0){
        delete m_DSBOut[j][i];
        m_DSBOut[j][i]=0;
      }
    }
    for( i=0; i<NumberOfTSBOut; i+=1){
      if(m_TSBOut[j][i]!=0){
        delete m_TSBOut[j][i];
        m_TSBOut[j][i]=0;
      }
    }
  }
}

void TGCHighPtBoard::collectInput()
{
  int i,j;
  for( j=0; j<NumberOfChip; j+=1){
    for( i=0; i<NumberOfDSBOut; i+=1)
      if(m_DSB[j][i]){
        m_DSBOut[j][i]=m_DSB[j][i]->getOutput();
        m_DSB[j][i]->eraseOutput();
        if(m_DSBOut[j][i]) m_DSBOut[j][i]->print();
      }
    for( i=0; i<NumberOfTSBOut; i+=1) 
      if(m_TSB[j][i]){
        m_TSBOut[j][i]=m_TSB[j][i]->getOutput();
        m_TSB[j][i]->eraseOutput();
        if(m_TSBOut[j][i]) m_TSBOut[j][i]->print();
      }
  }
}

TGCHighPtChipOut*  TGCHighPtBoard::getOutput()
{
  return m_highPtChipOut;
}

void TGCHighPtBoard::eraseOutput()
{
    m_highPtChipOut=0;
}

void TGCHighPtBoard::eraseHighPtOutput()
{
  if(m_highPtBoardOut!=0){
    delete m_highPtBoardOut;
    m_highPtBoardOut=0;
  }
}

void TGCHighPtBoard::eraseLowPtOutput()
{
  if(m_lowPtBoardOut!=0){
    delete m_lowPtBoardOut;
    m_lowPtBoardOut=0;
  }
}

void TGCHighPtBoard::doCoincidence()
{
  int chip;

  setDecoderIn();

  for( chip=0; chip<NumberOfChip; chip+=1){
    TGCHitPattern* TSBHit = new TGCHitPattern (m_nChOfTSBHit);
    TGCHitPattern* DSBHit = new TGCHitPattern (m_nChOfDSBHit);

    decodeSlaveBoardOut(chip, TSBHit, DSBHit);

    int block;
    for(block=0; block < NBlockOfDSBChannel; block++){
      doCoincidenceMatrix(chip, block, TSBHit, DSBHit);
      if(m_decoderInDSB[chip][block/2])
        if(m_decoderInDSB[chip][block/2]->getHit(block%2)){
          m_decoderInDSB[chip][block/2]->print(block%2);
        }
      // load SLB outputs to lowpTOutput
      loadLowPtOutput(chip, block, m_decoderInDSB[chip][block/2]); 
    }
    trackSelector(chip,PtHigh);
    trackSelector(chip,PtLow);
    highLowSelector(chip);

    delete TSBHit;
    TSBHit=0;
    delete DSBHit;
    DSBHit=0;
  }

  clearDecoderIn();
}


void TGCHighPtBoard::loadLowPtOutput(int chip, int block,
				     TGCSlaveBoardOut* dataDSB)
{
  if(dataDSB!=0){
    if(dataDSB->getHit(block%2)){
      int pos=dataDSB->getPos(block%2);
      int dev=dataDSB->getDev(block%2);
      m_lowPtBoardOut->setPt(chip,block,PtLow);
      m_lowPtBoardOut->setHit(chip,block,true);
      m_lowPtBoardOut->setDev(chip,block,dev);
      m_lowPtBoardOut->setPos(chip,block,pos/(m_nChOfDSBOut/4));  // !
    }else{
      m_lowPtBoardOut->setHit(chip,block,false);
    }
  }else{
    m_lowPtBoardOut->setHit(chip,block,false);
  }
}

TGCSlaveBoardOut* TGCHighPtBoard::getTSBOut(int chip, int port)
{
  return m_TSBOut[chip][port];
}

void TGCHighPtBoard::trackSelector(int chip, int ptIn)
{
  int i,iblock,a,b;
  int dev[NBlockOfDSBChannel];
  int block[NBlockOfDSBChannel];

  for( iblock=0; iblock<NBlockOfDSBChannel; iblock+=1){
    block[iblock]=iblock;
    if(ptIn==PtHigh){
      if(m_highPtBoardOut->getHit(chip,iblock)) dev[iblock]=m_highPtBoardOut->getDev(chip,iblock);
      else dev[iblock]=99;
    } else {		// Low-Pt track
      if(m_lowPtBoardOut->getHit(chip,iblock)){
        dev[iblock]=abs(m_lowPtBoardOut->getDev(chip,iblock))+m_maxDev+1;
        if(m_lowPtBoardOut->getDev(chip,iblock)<0) dev[iblock]=-dev[iblock];
      } else dev[iblock]=99;
    }
  }

  // sort in ascending order of |dev|.
  for(iblock=1;iblock<NBlockOfDSBChannel;iblock+=1){
    a=dev[iblock];
    b=block[iblock];
    i=iblock-1;
    while((i>=0)&&((abs(dev[i])>abs(a))||((abs(dev[i])==abs(a))&&(dev[i]!=a)&&(m_priorSign*dev[i]<0)))){
      dev[i+1]=dev[i];
      block[i+1]=block[i];
      i--;
    }
    dev[i+1]=a;
    block[i+1]=b;
  }

  for(iblock=0;iblock<m_maxNumberOfHPBData;iblock+=1){
    if(ptIn==PtHigh){
      if(m_highPtBoardOut->getHit(chip,block[iblock]))
        m_highPtBoardOut->setSel(chip,block[iblock],iblock+1);
    } else {		// Low-Pt track
      if(m_lowPtBoardOut->getHit(chip,block[iblock]))
        m_lowPtBoardOut->setSel(chip,block[iblock],iblock+1);
    }
  }
}

void TGCHighPtBoard::highLowSelector(int chip)
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
    m_highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
    m_highPtChipOut->setSel(chip,FirstCandidate,1);
    m_highPtChipOut->setDev(chip,FirstCandidate,dev);
    m_highPtChipOut->setPos(chip,FirstCandidate,pos);
    m_highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);

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
    m_highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
    m_highPtChipOut->setSel(chip,FirstCandidate,1);
    m_highPtChipOut->setDev(chip,FirstCandidate,dev);
    m_highPtChipOut->setPos(chip,FirstCandidate,pos);
    m_highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);
    // HPT 2nd
    pos=m_highPtBoardOut->getPos(chip,highPt2ndBlock);
    dev=m_highPtBoardOut->getDev(chip,highPt2ndBlock);
    m_highPtChipOut->setPt(chip,SecondCandidate,PtHigh);
    m_highPtChipOut->setSel(chip,SecondCandidate,2);
    m_highPtChipOut->setDev(chip,SecondCandidate,dev);
    m_highPtChipOut->setPos(chip,SecondCandidate,pos);
    m_highPtChipOut->setHitID(chip,SecondCandidate,highPt2ndBlock);
  }
}

void TGCHighPtBoard::createHighPtChipOut()
{
  if( m_highPtChipOut != 0 ) delete m_highPtChipOut;
  m_highPtChipOut = new  TGCHighPtChipOut(this, m_bid);
  m_highPtChipOut->clear();
}

void TGCHighPtBoard::createHighPtBoardOut()
{
  if( m_highPtBoardOut != 0 ) delete m_highPtBoardOut;
  m_highPtBoardOut = new  TGCHighPtBoardOut(this, m_bid);
  m_highPtBoardOut->clear();
}

void TGCHighPtBoard::createLowPtBoardOut()
{
  if( m_lowPtBoardOut != 0 ) delete m_lowPtBoardOut;
  m_lowPtBoardOut = new  TGCHighPtBoardOut(this, m_bid);
  m_lowPtBoardOut->clear();
}


void TGCHighPtBoard::showResult() const
{ 

#ifdef TGCCOUT
  int i,j,k, chip, iBlock, iCandidate;
  std::cout << "#HighPtBoard::showResult()" << std::endl;
  std::cout<<"#HPB [Input] bid=" << m_bid;
  std::cout.width(2);
  std::cout << " type=" << ((m_type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (m_region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << m_id << std::endl;
  for( j=0; j<NumberOfTSBOut; j+=1){
    for( i=0; i<NumberOfChip; i+=1){
      if(m_TSBOut[i][j]!=0){
        for( k=0; k<m_TSBOut[i][j]->getNumberOfData(); k+=1)
          if(m_TSBOut[i][j]->getHit(k)){
            std::cout << "\t[";
            std::cout.width(2);
            std::cout << "type=" << m_TSBOut[i][j]->getOrigin()->getTypeName(m_TSBOut[i][j]->getOrigin()->getType());
            std::cout.width(2);
            std::cout << " region=" << ((m_TSBOut[i][j]->getOrigin()->getRegion()==Endcap) ? "END" : "FWD");
            std::cout.width(2);
            std::cout << " id=" <<m_TSBOut[i][j]->getOrigin()->getId();
            std::cout.width(2);
            std::cout << " iChip=" << i;
            std::cout.width(2);
            std::cout << " iTSBOut=" << j;
            std::cout.width(2);
            std::cout << " k=" << k;
            std::cout.width(3);
            std::cout << " pos=" << m_TSBOut[i][j]->getPos(k);
            std::cout << "]" << std::endl;
          }
      }
      if(m_DSBOut[i][j]!=0){
        for( k=0; k<m_DSBOut[i][j]->getNumberOfData(); k+=1)
          if(m_DSBOut[i][j]->getHit(k)){
            std::cout<<"\t[";
            std::cout.width(2);
            std::cout << "type=" << m_DSBOut[i][j]->getOrigin()->getTypeName(m_DSBOut[i][j]->getOrigin()->getType());
            std::cout.width(2);
            std::cout << " region=" <<((m_DSBOut[i][j]->getOrigin()->getRegion()==Endcap) ? "END" : "FWD");
            std::cout.width(2);
            std::cout << " id=" <<m_DSBOut[i][j]->getOrigin()->getId();
            std::cout.width(2);
            std::cout << " i=" << i;
            std::cout.width(2);
            std::cout << " j=" << j;
            std::cout.width(2);
            std::cout << " k=" << k;
            std::cout.width(3);
            std::cout << " pos=" << m_DSBOut[i][j]->getPos(k);
            std::cout.width(3);
            std::cout << " dev=" << m_DSBOut[i][j]->getDev(k);
            std::cout << "]" << std::endl;
        }
      }
    }
  }
  std::cout<<std::endl;

  std::cout << "#HPB [Intermediate] bid=" << m_bid;
  std::cout.width(2);
  std::cout << " type=" << ((m_type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (m_region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << m_id << std::endl;
  for( chip=0; chip<NumberOfChip; chip+=1){
    std::cout<<"\t[";
    std::cout.width(2);
    std::cout << "iChip=" << chip << "]" << std::endl;

    // m_highPtBoardOut 
    for(iBlock=0; iBlock<NBlockOfDSBChannel; iBlock+=1){
      std::cout<<"\t [highPtBoardOut: ";
      std::cout.width(2);
      std::cout << "block=" << iBlock;
      if(m_highPtBoardOut->getHit(chip,iBlock)){
        std::cout.width(2);
        std::cout << " pos=" << m_highPtBoardOut->getPos(chip,iBlock);
        std::cout.width(4);
        std::cout << " dev=" << m_highPtBoardOut->getDev(chip,iBlock);
      }
      std::cout << "]" << std::endl;
    }

    // m_lowPtBoardOut 
    for(iBlock=0; iBlock<NBlockOfDSBChannel; iBlock+=1){
      std::cout<<"\t [lowPtBoardOut: ";
      std::cout.width(2);
      std::cout << "block=" << iBlock;
      if(m_lowPtBoardOut->getHit(chip,iBlock)){
        std::cout.width(2);
        std::cout << " pos=" << m_lowPtBoardOut->getPos(chip,iBlock);
        std::cout.width(4);
        std::cout << " dev=" << m_lowPtBoardOut->getDev(chip,iBlock);
      }
      std::cout << "]" << std::endl;
    }
  }

  std::cout << "#HPB [Output] bid=" << m_bid;
  std::cout.width(2);
  std::cout << " type=" << ((m_type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (m_region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << m_id << std::endl;
  for( chip=0; chip<NumberOfChip; chip+=1){
    std::cout<<"\t[";
    std::cout.width(2);
    std::cout << "iChip=" << chip << "]" << std::endl;
    for( iCandidate=0; iCandidate<2; iCandidate+=1){
      if(m_highPtChipOut->getSel(chip,iCandidate)){
        std::cout<<"\t [";
        std::cout.width(2);
        std::cout << "HitID=" << m_highPtChipOut->getHitID(chip,iCandidate);
        std::cout << " Sel:" <<  m_highPtChipOut->getSel(chip,iCandidate);
        if(m_highPtChipOut->getPt(chip,iCandidate)==PtHigh)
          std::cout<<" Pt:1[high]";
        else
          std::cout<<" Pt:0[low ]";
        std::cout.width(2);
        std::cout << " pos=" << m_highPtChipOut->getPos(chip,iCandidate);
        std::cout.width(4);
        std::cout << " dev=" << m_highPtChipOut->getDev(chip,iCandidate);
        std::cout << "]" << std::endl;
      }
    }
  }
#endif
}


TGCHighPtBoard::TGCHighPtBoard(const TGCHighPtBoard& right)
  :m_highPtChipOut(0), m_highPtBoardOut(0), m_lowPtBoardOut(0),
   m_id(right.m_id),
   m_bid(right.m_bid),
   m_idSectorLogic(right.m_idSectorLogic),
   m_type(right.m_type),
   m_region(right.m_region),
   m_priorSign(right.m_priorSign),
   m_maxNumberOfHPBData(right.m_maxNumberOfHPBData),
   m_maxDev(right.m_maxDev),
   m_maxDevOred(right.m_maxDevOred),
   m_nChOfTSBOut(0), m_nChOfDSBOut(0), m_nChOfDSBHit(0), m_nChOfTSBHit(0),
   m_nChInTSBRegion(0)
{
  for(int j=0; j<NumberOfChip; j+=1){
    for(int i=0; i<NumberOfDSBOut; i+=1){
      m_DSB[j][i] = right.m_DSB[j][i];
      m_DSBOut[j][i]=0;
    }
    for(int i=0; i<NumberOfTSBOut; i+=1){
      m_TSB[j][i] = right.m_TSB[j][i];
      m_TSBOut[j][i]=0;
    }
  }
  for(int i=0; i<NumberOfAdjacentHPB; i+=1){
    m_adjacentHPB[i]=right.m_adjacentHPB[i];
  }
}

TGCHighPtBoard& TGCHighPtBoard::operator=(const TGCHighPtBoard& right)
{
  if ( this != &right ) {
    m_id = right.m_id;
    m_bid = right.m_bid;
    m_idSectorLogic = right.m_idSectorLogic;
    m_type = right.m_type;
    m_region = right.m_region;
    m_priorSign = right.m_priorSign;
    m_maxNumberOfHPBData = right.m_maxNumberOfHPBData;
    m_maxDev = right.m_maxDev;
    m_maxDevOred = right.m_maxDevOred;

    if (m_highPtChipOut!=0) delete m_highPtChipOut;
    if (right.m_highPtChipOut !=0) m_highPtChipOut = new TGCHighPtChipOut(*right.m_highPtChipOut);
    else m_highPtChipOut =0;

    if (m_highPtBoardOut!=0) delete m_highPtBoardOut;
    if( right.m_highPtBoardOut !=0) m_highPtBoardOut = new TGCHighPtBoardOut(*right.m_highPtBoardOut);
    else m_highPtBoardOut =0;
    

    if (m_lowPtBoardOut!=0) delete m_lowPtBoardOut;
    if( right.m_lowPtBoardOut !=0) m_lowPtBoardOut = new TGCHighPtBoardOut(*right.m_lowPtBoardOut);
    else m_lowPtBoardOut = 0;

    for(int j=0; j<NumberOfChip; j+=1){
      for(int i=0; i<NumberOfDSBOut; i+=1){
        m_DSB[j][i] = right.m_DSB[j][i];
        if(m_DSBOut[j][i]!=0){
          delete m_DSBOut[j][i];
          m_DSBOut[j][i]= new TGCSlaveBoardOut(*right.m_DSBOut[j][i]);
        }
      }
      for(int i=0; i<NumberOfTSBOut; i+=1){
        m_TSB[j][i] = right.m_TSB[j][i];
        if(m_TSBOut[j][i]!=0){
          delete m_TSBOut[j][i];
          m_TSBOut[j][i]= new TGCSlaveBoardOut(*right.m_TSBOut[j][i]);
        }
      }
    }
    for(int i=0; i<NumberOfAdjacentHPB; i+=1){
      m_adjacentHPB[i]=right.m_adjacentHPB[i];
    }
    m_nChOfTSBOut = right.m_nChOfTSBOut;
    m_nChOfDSBOut = right.m_nChOfDSBOut;
    m_nChOfDSBHit = right.m_nChOfDSBHit;
    m_nChOfTSBHit = right.m_nChOfTSBHit;
    m_nChInTSBRegion = right.m_nChInTSBRegion;
  }
  return *this;
}


} //end of namespace bracket
