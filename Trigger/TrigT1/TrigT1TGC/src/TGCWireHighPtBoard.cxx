/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ref. SOS063V07,S0S061V04 
#include "TrigT1TGC/TGCWireHighPtBoard.hh"
#include "TrigT1TGC/TGCHighPtBoardOut.hh"

#include <iostream>
#include <cstdlib>

namespace LVL1TGCTrigger {

TGCWireHighPtBoard::TGCWireHighPtBoard():
  TGCHighPtBoard()
{
  maxDev=15;
  maxDevOred=20;
  nChOfTSBOut = 96;
  nChInTSBRegion = nChOfTSBOut/3;
  nChOfDSBOut = 64;
  nChOfDSBHit = 3*nChOfDSBOut;
  nChOfTSBHit = nChOfDSBHit + 2*maxDevOred;
}

void TGCWireHighPtBoard::setDecoderIn()
{
  decoderInDSB[ChipA][0] = DSBOut[ChipA][0];
  decoderInDSB[ChipA][1] = DSBOut[ChipA][1];
  decoderInDSB[ChipA][2] = DSBOut[ChipA][2];

  decoderInDSB[ChipB][0] = DSBOut[ChipB][0];
  decoderInDSB[ChipB][1] = DSBOut[ChipB][1];
  decoderInDSB[ChipB][2] = DSBOut[ChipB][2];

  if(adjacentHPB[0]){
    decoderInTSB[ChipA][0] = adjacentHPB[0]->getTSBOut(ChipB,1);
  }
  decoderInTSB[ChipA][1] = TSBOut[ChipA][0];
  decoderInTSB[ChipA][2] = TSBOut[ChipA][1];
  decoderInTSB[ChipA][3] = TSBOut[ChipB][0];

  decoderInTSB[ChipB][0] = TSBOut[ChipA][1];
  decoderInTSB[ChipB][1] = TSBOut[ChipB][0];
  decoderInTSB[ChipB][2] = TSBOut[ChipB][1];
  if(adjacentHPB[1]){
    decoderInTSB[ChipB][3] = adjacentHPB[1]->getTSBOut(ChipA,0);
  }
}

void TGCWireHighPtBoard::clearDecoderIn()
{
  int chip,j;
  for( chip=0; chip<NumberOfChip; chip+=1){
    for( j=0; j<NDecoderInTSB; j+=1) decoderInTSB[chip][j]=0;
    for( j=0; j<NDecoderInDSB; j+=1) decoderInDSB[chip][j]=0;
  }
}

void TGCWireHighPtBoard::decodeSlaveBoardOut(int chip, 
					     TGCHitPattern* tHit, 
					     TGCHitPattern* dHit)
{
  int block,j,ch;

  // for TripletSlaveBoard
#ifdef TGCCOUT
    std::cout << "#--Wire decodeSlaveBoardOut()" << std::endl;
  std::cout << "Triplet" << std::endl;
#endif
  for( j=0; j<NDecoderInTSB; j+=1)
    if(decoderInTSB[chip][j]!=0){
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInTSB-1 << " in a chip  ";
      std::cout<<"WHPB:decoderInTSB["<<chip<<"]["<<j<<"]:";
      decoderInTSB[chip][j]->print();
#endif
      for( block=0; block<decoderInTSB[chip][j]->getNumberOfData(); block+=1)
        if(decoderInTSB[chip][j]->getHit(block)){
          ch=nChOfTSBHit/2+(j-2)*nChOfTSBOut
            +block*nChInTSBRegion+decoderInTSB[chip][j]->getPos(block);
          if((ch>=0)&&(ch<nChOfTSBHit)) tHit->onChannel(ch);
        }
    }

  // for DoubletSlaveBoard
#ifdef TGCCOUT
  std::cout << "Doublet" << std::endl;
#endif
  for( j=0; j<NDecoderInDSB; j+=1)
    if(decoderInDSB[chip][j]!=0){
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInDSB-1 << " in a chip  ";
      std::cout<<"WHPB:decoderInDSB["<<chip<<"]["<<j<<"]:";
      decoderInDSB[chip][j]->print();
#endif
      for( block=0; block<decoderInDSB[chip][j]->getNumberOfData(); block+=1)
        if(decoderInDSB[chip][j]->getHit(block)){
          dHit->onChannel(j*nChOfDSBOut
                          +nChOfDSBOut/2*block+decoderInDSB[chip][j]->getPos(block));
        }
    }

  tHit=0;
  dHit=0;
}

void TGCWireHighPtBoard::doCoincidenceMatrix(int chip, 
					     int block, 
					     const TGCHitPattern* tHit, 
					     const TGCHitPattern* dHit)
{
  // ref. SOS062V05
  int Oring;
  int nChUnitOfDSBHit=nChOfDSBHit/NBlockOfDSBChannel; //ch in a AB CD
  //  int nChSubUnitOfDSBHit = nChUnitOfDSBHit / 2;       //ch in a A B C D

#ifdef TGCCOUT
std::cout << "# Wire doCoincidenceMatrix() chip:" << chip << " block:" << block << std::endl;
#endif
  int dev, DevOrg, ich, DSBCh, TSBCh, ior;
  for(dev=0; dev <= maxDev; dev++){
    for(ich=0; ich < nChUnitOfDSBHit; ich++){
      if(dev>2*maxDev-maxDevOred){
          Oring = 1;
          DevOrg = 2*maxDev-maxDevOred + (dev - (2*maxDev-maxDevOred))*2 - 1;
      }else{
          Oring = 0;
          DevOrg = dev;
      }
      for(ior = 0; ior <= Oring; ior++){
        DSBCh = nChUnitOfDSBHit*block + ich;
        TSBCh = DSBCh + priorSign*(DevOrg + ior) + maxDevOred;
        if(tHit->getChannel(TSBCh) && dHit->getChannel(DSBCh)){
#ifdef TGCDEBUG
          std::cout << "HighPt" << std::endl
		    << " th&&dh==1 priorSign*dev:" << priorSign 
		    << "*" << dev 
		    << " DSBCh:" << DSBCh 
		    << " TSBCh:" << TSBCh << std::endl;
#endif
          // channel of dev>2*maxDev-maxDevOred is ored.
          highPtBoardOut->setPt(chip,block,PtHigh);
          highPtBoardOut->setHit(chip,block,true);
          highPtBoardOut->setDev(chip,block,priorSign*dev);
          highPtBoardOut->setPos(chip,block,ich/(nChOfDSBOut/4));
          return;
        }
      }
    }
    // for dR of opposite sign
    if(dev){
      for(ich=0; ich < nChUnitOfDSBHit; ich++){
        if(dev>2*maxDev-maxDevOred){
            Oring = 1;
            DevOrg = 2*maxDev-maxDevOred + (dev - (2*maxDev-maxDevOred))*2 - 1;
        }else{
            Oring = 0;
            DevOrg = dev;
        }
        for(ior = 0; ior <= Oring; ior++){
          DSBCh = nChUnitOfDSBHit*block + ich;
          TSBCh = DSBCh - priorSign*(DevOrg + ior) + maxDevOred;
          if(tHit->getChannel(TSBCh) && dHit->getChannel(DSBCh)){
#ifdef TGCDEBUG
          std::cout << "HighPt" << std::endl 
		    << " th&&dh==1 priorSign*dev:" << -priorSign 
		    << "*" << dev
		    << " DSBCh:" << DSBCh 
		    << " TSBCh:" << TSBCh << std::endl;
#endif
            // channel of dev>2*maxDev-maxDevOred is ored.
            highPtBoardOut->setPos(chip,block,ich/(nChOfDSBOut/4));
            highPtBoardOut->setDev(chip,block,-priorSign*dev);
            highPtBoardOut->setPt(chip,block,PtHigh);
            highPtBoardOut->setHit(chip,block,true);
            return;
          }
        }
      }
    }
  }
}

} //end of namespace bracket
