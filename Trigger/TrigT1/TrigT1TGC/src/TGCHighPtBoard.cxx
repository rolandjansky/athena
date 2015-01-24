/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCHighPtBoardOut.hh"
#include "TrigT1TGC/TGCHighPtBoard.hh"
#include "TrigT1TGC/TGCHighPtChipOut.hh"
#include <iostream>
#include <cmath>
#include <cstdlib>

namespace LVL1TGCTrigger {

TGCHighPtBoard::TGCHighPtBoard():
  highPtChipOut(0), highPtBoardOut(0), lowPtBoardOut(0),
  id(-1),bid(-1),idSectorLogic(-1),type(-1),region(-1),
  priorSign(1),maxNumberOfHPBData(2),
  maxDev(0), maxDevOred(0),
  nChOfTSBOut(0), nChOfDSBOut(0), nChOfDSBHit(0), nChOfTSBHit(0), 
  nChInTSBRegion(0)
{
  for( int i=0; i<NumberOfChip; i+=1){
    for( int j=0; j<NumberOfDSBOut; j+=1){
      DSB[i][j]=0;
      DSBOut[i][j]=0;
    }
    for( int j=0; j<NumberOfTSBOut; j+=1){
      TSB[i][j]=0;
      TSBOut[i][j]=0;
    }
  }

  for( int i=0; i<NumberOfAdjacentHPB; i+=1)
    adjacentHPB[i]=0;

  for( int i=0; i<NumberOfChip; i+=1){
    for( int j=0; j<NDecoderInTSB; j+=1) decoderInTSB[i][j]=0;
    for( int j=0; j<NDecoderInDSB; j+=1) decoderInDSB[i][j]=0;
  }
}

TGCHighPtBoard::~TGCHighPtBoard()
{
  int i,j;
  for( i=0; i<NumberOfChip; i+=1){
    for( j=0; j<NumberOfDSBOut; j+=1){
      DSB[i][j]=0;
      if(DSBOut[i][j]!=0){
        delete DSBOut[i][j];
        DSBOut[i][j]=0;
      }
    }
    for( j=0; j<NumberOfTSBOut; j+=1){
      TSB[i][j]=0;
      if(TSBOut[i][j]!=0){
        delete TSBOut[i][j];
        TSBOut[i][j]=0;
      }
    }
  }

  for( i=0; i<NumberOfChip; i+=1){
    for( j=0; j<NDecoderInTSB; j+=1){
      if(decoderInTSB[i][j]!=0) delete decoderInTSB[i][j];
      decoderInTSB[i][j]=0;
    }
    for( j=0; j<NDecoderInDSB; j+=1){
      if(decoderInDSB[i][j]!=0) delete decoderInDSB[i][j];
      decoderInDSB[i][j]=0;
    }
  }

  for( i=0; i<NumberOfAdjacentHPB; i+=1)  
    adjacentHPB[i]=0;
}

void TGCHighPtBoard::clockIn(int bidIn)
{
  if(bid!=bidIn){
#ifdef TGCCOUT
    std::cout << "#TGCHighPtBoard::clockIn collect" << std::endl;
#endif
    bid = bidIn;
    deleteSlaveBoardOut();
    collectInput();
    return;
  }else{
#ifdef TGCCOUT
    std::cout << "#TGCHighPtBoard::clockIn  BID:" << bid 
	      << " TYPE:" << ((type==WHPB) ? "WHPB" : "SHPB" )
	      << " ID:" << id 
	      << " REGION:" << ( (region==FORWARD) ? "FWD" : "END")
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
      if(DSBOut[j][i]!=0){
        delete DSBOut[j][i];
        DSBOut[j][i]=0;
      }
    }
    for( i=0; i<NumberOfTSBOut; i+=1){
      if(TSBOut[j][i]!=0){
        delete TSBOut[j][i];
        TSBOut[j][i]=0;
      }
    }
  }
}

void TGCHighPtBoard::collectInput()
{
  int i,j;
  for( j=0; j<NumberOfChip; j+=1){
    for( i=0; i<NumberOfDSBOut; i+=1)
      if(DSB[j][i]){
        DSBOut[j][i]=DSB[j][i]->getOutput();
        DSB[j][i]->eraseOutput();
        if(DSBOut[j][i]) DSBOut[j][i]->print();
      }
    for( i=0; i<NumberOfTSBOut; i+=1) 
      if(TSB[j][i]){
        TSBOut[j][i]=TSB[j][i]->getOutput();
        TSB[j][i]->eraseOutput();
        if(TSBOut[j][i]) TSBOut[j][i]->print();
      }
  }
}

TGCHighPtChipOut*  TGCHighPtBoard::getOutput()
{
  return highPtChipOut;
}

void TGCHighPtBoard::eraseOutput()
{
    highPtChipOut=0;
}

void TGCHighPtBoard::eraseHighPtOutput()
{
  if(highPtBoardOut!=0){
    delete highPtBoardOut;
    highPtBoardOut=0;
  }
}

void TGCHighPtBoard::eraseLowPtOutput()
{
  if(lowPtBoardOut!=0){
    delete lowPtBoardOut;
    lowPtBoardOut=0;
  }
}

void TGCHighPtBoard::doCoincidence()
{
  int chip;

  setDecoderIn();

  for( chip=0; chip<NumberOfChip; chip+=1){
    TGCHitPattern* TSBHit = new TGCHitPattern (nChOfTSBHit);
    TGCHitPattern* DSBHit = new TGCHitPattern (nChOfDSBHit);

    decodeSlaveBoardOut(chip, TSBHit, DSBHit);

    int block;
    for(block=0; block < NBlockOfDSBChannel; block++){
      doCoincidenceMatrix(chip, block, TSBHit, DSBHit);
      if(decoderInDSB[chip][block/2])
        if(decoderInDSB[chip][block/2]->getHit(block%2)){
          decoderInDSB[chip][block/2]->print(block%2);
        }
      // load SLB outputs to lowpTOutput
      loadLowPtOutput(chip, block, decoderInDSB[chip][block/2]); 
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
      lowPtBoardOut->setPt(chip,block,PtLow);
      lowPtBoardOut->setHit(chip,block,true);
      lowPtBoardOut->setDev(chip,block,dev);
      lowPtBoardOut->setPos(chip,block,pos/(nChOfDSBOut/4));  // !
    }else{
      lowPtBoardOut->setHit(chip,block,false);
    }
  }else{
    lowPtBoardOut->setHit(chip,block,false);
  }
}

TGCSlaveBoardOut* TGCHighPtBoard::getTSBOut(int chip, int port)
{
  return TSBOut[chip][port];
}

void TGCHighPtBoard::trackSelector(int chip, int ptIn)
{
  int i,iblock,a,b;
  int dev[NBlockOfDSBChannel];
  int block[NBlockOfDSBChannel];

  for( iblock=0; iblock<NBlockOfDSBChannel; iblock+=1){
    block[iblock]=iblock;
    if(ptIn==PtHigh){
      if(highPtBoardOut->getHit(chip,iblock)) dev[iblock]=highPtBoardOut->getDev(chip,iblock);
      else dev[iblock]=99;
    } else {		// Low-Pt track
      if(lowPtBoardOut->getHit(chip,iblock)){
        dev[iblock]=abs(lowPtBoardOut->getDev(chip,iblock))+maxDev+1;
        if(lowPtBoardOut->getDev(chip,iblock)<0) dev[iblock]=-dev[iblock];
      } else dev[iblock]=99;
    }
  }

  // sort in ascending order of |dev|.
  for(iblock=1;iblock<NBlockOfDSBChannel;iblock+=1){
    a=dev[iblock];
    b=block[iblock];
    i=iblock-1;
    while((i>=0)&&((abs(dev[i])>abs(a))||((abs(dev[i])==abs(a))&&(dev[i]!=a)&&(priorSign*dev[i]<0)))){
      dev[i+1]=dev[i];
      block[i+1]=block[i];
      i--;
    }
    dev[i+1]=a;
    block[i+1]=b;
  }

  for(iblock=0;iblock<maxNumberOfHPBData;iblock+=1){
    if(ptIn==PtHigh){
      if(highPtBoardOut->getHit(chip,block[iblock]))
        highPtBoardOut->setSel(chip,block[iblock],iblock+1);
    } else {		// Low-Pt track
      if(lowPtBoardOut->getHit(chip,block[iblock]))
        lowPtBoardOut->setSel(chip,block[iblock],iblock+1);
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
    switch (highPtBoardOut->getSel(chip,iblock)){
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
    switch (lowPtBoardOut->getSel(chip,iblock)){
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
      pos=lowPtBoardOut->getPos(chip,lowPt1stBlock);
      dev=lowPtBoardOut->getDev(chip,lowPt1stBlock);
      highPtChipOut->setPt(chip,FirstCandidate,PtLow);
      highPtChipOut->setSel(chip,FirstCandidate,1);
      highPtChipOut->setDev(chip,FirstCandidate,dev);
      highPtChipOut->setPos(chip,FirstCandidate,pos);
      highPtChipOut->setHitID(chip,FirstCandidate,lowPt1stBlock);
    }
    if (lowPt2ndBlock >= 0){
      // LPT 2nd
      pos=lowPtBoardOut->getPos(chip,lowPt2ndBlock);
      dev=lowPtBoardOut->getDev(chip,lowPt2ndBlock);
      highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      highPtChipOut->setSel(chip,SecondCandidate,2);
      highPtChipOut->setDev(chip,SecondCandidate,dev);
      highPtChipOut->setPos(chip,SecondCandidate,pos);
      highPtChipOut->setHitID(chip,SecondCandidate,lowPt2ndBlock);
    }
    return;
  } else if (highPt2ndBlock < 0){	// 1 HPT Matrix Output
    // HPT 1st
    pos=highPtBoardOut->getPos(chip,highPt1stBlock);
    dev=highPtBoardOut->getDev(chip,highPt1stBlock);
    highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
    highPtChipOut->setSel(chip,FirstCandidate,1);
    highPtChipOut->setDev(chip,FirstCandidate,dev);
    highPtChipOut->setPos(chip,FirstCandidate,pos);
    highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);

    if ((lowPt1stBlock >= 0) &&
	!((lowPt1stBlock == highPt1stBlock) && ((lowPtBoardOut->getPos(chip,lowPt1stBlock)) == (highPtBoardOut->getPos(chip,highPt1stBlock))))
	){			//for vi }
      // LPT 1st (as 2nd candidate)
      pos=lowPtBoardOut->getPos(chip,lowPt1stBlock);
      dev=lowPtBoardOut->getDev(chip,lowPt1stBlock);
      highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      highPtChipOut->setSel(chip,SecondCandidate,2);
      highPtChipOut->setDev(chip,SecondCandidate,dev);
      highPtChipOut->setPos(chip,SecondCandidate,pos);
      highPtChipOut->setHitID(chip,SecondCandidate,lowPt1stBlock);
      return;
    } else if ((lowPt2ndBlock >= 0) &&
	       !((lowPt2ndBlock == highPt1stBlock) && ((lowPtBoardOut->getPos(chip,lowPt2ndBlock)) == (highPtBoardOut->getPos(chip,highPt1stBlock))))
	       ){
      
      // LPT 2nd (as 2nd candidate)
      pos=lowPtBoardOut->getPos(chip,lowPt2ndBlock);
      dev=lowPtBoardOut->getDev(chip,lowPt2ndBlock);
      highPtChipOut->setPt(chip,SecondCandidate,PtLow);
      highPtChipOut->setSel(chip,SecondCandidate,2);
      highPtChipOut->setDev(chip,SecondCandidate,dev);
      highPtChipOut->setPos(chip,SecondCandidate,pos);
      highPtChipOut->setHitID(chip,SecondCandidate,lowPt2ndBlock);
      return;
    }
  } else {				// 2 HPT Matrix Outputs
    // HPT 1st
    pos=highPtBoardOut->getPos(chip,highPt1stBlock);
    dev=highPtBoardOut->getDev(chip,highPt1stBlock);
    highPtChipOut->setPt(chip,FirstCandidate,PtHigh);
    highPtChipOut->setSel(chip,FirstCandidate,1);
    highPtChipOut->setDev(chip,FirstCandidate,dev);
    highPtChipOut->setPos(chip,FirstCandidate,pos);
    highPtChipOut->setHitID(chip,FirstCandidate,highPt1stBlock);
    // HPT 2nd
    pos=highPtBoardOut->getPos(chip,highPt2ndBlock);
    dev=highPtBoardOut->getDev(chip,highPt2ndBlock);
    highPtChipOut->setPt(chip,SecondCandidate,PtHigh);
    highPtChipOut->setSel(chip,SecondCandidate,2);
    highPtChipOut->setDev(chip,SecondCandidate,dev);
    highPtChipOut->setPos(chip,SecondCandidate,pos);
    highPtChipOut->setHitID(chip,SecondCandidate,highPt2ndBlock);
  }
}

void TGCHighPtBoard::createHighPtChipOut()
{
  if( highPtChipOut != 0 ) delete highPtChipOut;
  highPtChipOut = new  TGCHighPtChipOut(this, bid);
  highPtChipOut->clear();
}

void TGCHighPtBoard::createHighPtBoardOut()
{
  if( highPtBoardOut != 0 ) delete highPtBoardOut;
  highPtBoardOut = new  TGCHighPtBoardOut(this, bid);
  highPtBoardOut->clear();
}

void TGCHighPtBoard::createLowPtBoardOut()
{
  if( lowPtBoardOut != 0 ) delete lowPtBoardOut;
  lowPtBoardOut = new  TGCHighPtBoardOut(this, bid);
  lowPtBoardOut->clear();
}


void TGCHighPtBoard::showResult() const
{ 

#ifdef TGCCOUT
  int i,j,k, chip, iBlock, iCandidate;
  std::cout << "#HighPtBoard::showResult()" << std::endl;
  std::cout<<"#HPB [Input] bid=" << bid;
  std::cout.width(2);
  std::cout << " type=" << ((type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << id << std::endl;
  for( j=0; j<NumberOfTSBOut; j+=1){
    for( i=0; i<NumberOfChip; i+=1){
      if(TSBOut[i][j]!=0){
        for( k=0; k<TSBOut[i][j]->getNumberOfData(); k+=1)
          if(TSBOut[i][j]->getHit(k)){
            std::cout << "\t[";
            std::cout.width(2);
            std::cout << "type=" << TSBOut[i][j]->getOrigin()->getTypeName(TSBOut[i][j]->getOrigin()->getType());
            std::cout.width(2);
            std::cout << " region=" << ((TSBOut[i][j]->getOrigin()->getRegion()==Endcap) ? "END" : "FWD");
            std::cout.width(2);
            std::cout << " id=" <<TSBOut[i][j]->getOrigin()->getId();
            std::cout.width(2);
            std::cout << " iChip=" << i;
            std::cout.width(2);
            std::cout << " iTSBOut=" << j;
            std::cout.width(2);
            std::cout << " k=" << k;
            std::cout.width(3);
            std::cout << " pos=" << TSBOut[i][j]->getPos(k);
            std::cout << "]" << std::endl;
          }
      }
      if(DSBOut[i][j]!=0){
        for( k=0; k<DSBOut[i][j]->getNumberOfData(); k+=1)
          if(DSBOut[i][j]->getHit(k)){
            std::cout<<"\t[";
            std::cout.width(2);
            std::cout << "type=" << DSBOut[i][j]->getOrigin()->getTypeName(DSBOut[i][j]->getOrigin()->getType());
            std::cout.width(2);
            std::cout << " region=" <<((DSBOut[i][j]->getOrigin()->getRegion()==Endcap) ? "END" : "FWD");
            std::cout.width(2);
            std::cout << " id=" <<DSBOut[i][j]->getOrigin()->getId();
            std::cout.width(2);
            std::cout << " i=" << i;
            std::cout.width(2);
            std::cout << " j=" << j;
            std::cout.width(2);
            std::cout << " k=" << k;
            std::cout.width(3);
            std::cout << " pos=" << DSBOut[i][j]->getPos(k);
            std::cout.width(3);
            std::cout << " dev=" << DSBOut[i][j]->getDev(k);
            std::cout << "]" << std::endl;
        }
      }
    }
  }
  std::cout<<std::endl;

  std::cout << "#HPB [Intermediate] bid=" << bid;
  std::cout.width(2);
  std::cout << " type=" << ((type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << id << std::endl;
  for( chip=0; chip<NumberOfChip; chip+=1){
    std::cout<<"\t[";
    std::cout.width(2);
    std::cout << "iChip=" << chip << "]" << std::endl;

    // highPtBoardOut 
    for(iBlock=0; iBlock<NBlockOfDSBChannel; iBlock+=1){
      std::cout<<"\t [highPtBoardOut: ";
      std::cout.width(2);
      std::cout << "block=" << iBlock;
      if(highPtBoardOut->getHit(chip,iBlock)){
        std::cout.width(2);
        std::cout << " pos=" << highPtBoardOut->getPos(chip,iBlock);
        std::cout.width(4);
        std::cout << " dev=" << highPtBoardOut->getDev(chip,iBlock);
      }
      std::cout << "]" << std::endl;
    }

    // lowPtBoardOut 
    for(iBlock=0; iBlock<NBlockOfDSBChannel; iBlock+=1){
      std::cout<<"\t [lowPtBoardOut: ";
      std::cout.width(2);
      std::cout << "block=" << iBlock;
      if(lowPtBoardOut->getHit(chip,iBlock)){
        std::cout.width(2);
        std::cout << " pos=" << lowPtBoardOut->getPos(chip,iBlock);
        std::cout.width(4);
        std::cout << " dev=" << lowPtBoardOut->getDev(chip,iBlock);
      }
      std::cout << "]" << std::endl;
    }
  }

  std::cout << "#HPB [Output] bid=" << bid;
  std::cout.width(2);
  std::cout << " type=" << ((type==WHPB) ? "WHPB" : "SHPB" );
  std::cout.width(2);
  std::cout << " region=" << ( (region==FORWARD) ? "FWD" : "END");
  std::cout.width(2);
  std::cout << " id=" << id << std::endl;
  for( chip=0; chip<NumberOfChip; chip+=1){
    std::cout<<"\t[";
    std::cout.width(2);
    std::cout << "iChip=" << chip << "]" << std::endl;
    for( iCandidate=0; iCandidate<2; iCandidate+=1){
      if(highPtChipOut->getSel(chip,iCandidate)){
        std::cout<<"\t [";
        std::cout.width(2);
        std::cout << "HitID=" << highPtChipOut->getHitID(chip,iCandidate);
        std::cout << " Sel:" <<  highPtChipOut->getSel(chip,iCandidate);
        if(highPtChipOut->getPt(chip,iCandidate)==PtHigh)
          std::cout<<" Pt:1[high]";
        else
          std::cout<<" Pt:0[low ]";
        std::cout.width(2);
        std::cout << " pos=" << highPtChipOut->getPos(chip,iCandidate);
        std::cout.width(4);
        std::cout << " dev=" << highPtChipOut->getDev(chip,iCandidate);
        std::cout << "]" << std::endl;
      }
    }
  }
#endif
}


TGCHighPtBoard::TGCHighPtBoard(const TGCHighPtBoard& right)
  :highPtChipOut(0), highPtBoardOut(0), lowPtBoardOut(0),
   id(right.id),
   bid(right.bid),
   idSectorLogic(right.idSectorLogic),
   type(right.type),
   region(right.region),
   priorSign(right.priorSign),
   maxNumberOfHPBData(right.maxNumberOfHPBData),
   maxDev(right.maxDev),
   maxDevOred(right.maxDevOred),
   nChOfTSBOut(0), nChOfDSBOut(0), nChOfDSBHit(0), nChOfTSBHit(0),
   nChInTSBRegion(0)
{
  for(int j=0; j<NumberOfChip; j+=1){
    for(int i=0; i<NumberOfDSBOut; i+=1){
      DSB[j][i] = right.DSB[j][i];
      DSBOut[j][i]=0;
    }
    for(int i=0; i<NumberOfTSBOut; i+=1){
      TSB[j][i] = right.TSB[j][i];
      TSBOut[j][i]=0;
    }
  }
  for(int i=0; i<NumberOfAdjacentHPB; i+=1){
    adjacentHPB[i]=right.adjacentHPB[i];
  }
}

TGCHighPtBoard& TGCHighPtBoard::operator=(const TGCHighPtBoard& right)
{
  if ( this != &right ) {
    id = right.id;
    bid = right.bid;
    idSectorLogic = right.idSectorLogic;
    type = right.type;
    region = right.region;
    priorSign = right.priorSign;
    maxNumberOfHPBData = right.maxNumberOfHPBData;
    maxDev = right.maxDev;
    maxDevOred = right.maxDevOred;

    if (highPtChipOut!=0) delete highPtChipOut;
    if (right.highPtChipOut !=0) highPtChipOut = new TGCHighPtChipOut(*right.highPtChipOut);
    else highPtChipOut =0;

    if (highPtBoardOut!=0) delete highPtBoardOut;
    if( right.highPtBoardOut !=0) highPtBoardOut = new TGCHighPtBoardOut(*right.highPtBoardOut);
    else highPtBoardOut =0;
    

    if (lowPtBoardOut!=0) delete lowPtBoardOut;
    if( right.lowPtBoardOut !=0) lowPtBoardOut = new TGCHighPtBoardOut(*right.lowPtBoardOut);
    else lowPtBoardOut = 0;

    for(int j=0; j<NumberOfChip; j+=1){
      for(int i=0; i<NumberOfDSBOut; i+=1){
        DSB[j][i] = right.DSB[j][i];
        if(DSBOut[j][i]!=0){
          delete DSBOut[j][i];
          DSBOut[j][i]= new TGCSlaveBoardOut(*right.DSBOut[j][i]);
        }
      }
      for(int i=0; i<NumberOfTSBOut; i+=1){
        TSB[j][i] = right.TSB[j][i];
        if(TSBOut[j][i]!=0){
          delete TSBOut[j][i];
          TSBOut[j][i]= new TGCSlaveBoardOut(*right.TSBOut[j][i]);
        }
      }
    }
    for(int i=0; i<NumberOfAdjacentHPB; i+=1){
      adjacentHPB[i]=right.adjacentHPB[i];
    }
    nChOfTSBOut = right.nChOfTSBOut;
    nChOfDSBOut = right.nChOfDSBOut;
    nChOfDSBHit = right.nChOfDSBHit;
    nChOfTSBHit = right.nChOfTSBHit;
    nChInTSBRegion = right.nChInTSBRegion;
  }
  return *this;
}


} //end of namespace bracket
