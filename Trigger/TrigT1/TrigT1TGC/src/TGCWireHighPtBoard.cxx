/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  m_maxDev=15;
  m_maxDevOred=20;
  m_nChOfTSBOut = 96;
  m_nChInTSBRegion = m_nChOfTSBOut/3;
  m_nChOfDSBOut = 64;
  m_nChOfDSBHit = 3*m_nChOfDSBOut;
  m_nChOfTSBHit = m_nChOfDSBHit + 2*m_maxDevOred;
}

void TGCWireHighPtBoard::setDecoderIn()
{
  m_decoderInDSB[ChipA][0] = m_DSBOut[ChipA][0];
  m_decoderInDSB[ChipA][1] = m_DSBOut[ChipA][1];
  m_decoderInDSB[ChipA][2] = m_DSBOut[ChipA][2];

  m_decoderInDSB[ChipB][0] = m_DSBOut[ChipB][0];
  m_decoderInDSB[ChipB][1] = m_DSBOut[ChipB][1];
  m_decoderInDSB[ChipB][2] = m_DSBOut[ChipB][2];

  if(m_adjacentHPB[0]){
    m_decoderInTSB[ChipA][0] = m_adjacentHPB[0]->getTSBOut(ChipB,1);
  }
  m_decoderInTSB[ChipA][1] = m_TSBOut[ChipA][0];
  m_decoderInTSB[ChipA][2] = m_TSBOut[ChipA][1];
  m_decoderInTSB[ChipA][3] = m_TSBOut[ChipB][0];

  m_decoderInTSB[ChipB][0] = m_TSBOut[ChipA][1];
  m_decoderInTSB[ChipB][1] = m_TSBOut[ChipB][0];
  m_decoderInTSB[ChipB][2] = m_TSBOut[ChipB][1];
  if(m_adjacentHPB[1]){
    m_decoderInTSB[ChipB][3] = m_adjacentHPB[1]->getTSBOut(ChipA,0);
  }
}

void TGCWireHighPtBoard::clearDecoderIn()
{
  int chip,j;
  for( chip=0; chip<NumberOfChip; chip+=1){
    for( j=0; j<NDecoderInTSB; j+=1) m_decoderInTSB[chip][j]=0;
    for( j=0; j<NDecoderInDSB; j+=1) m_decoderInDSB[chip][j]=0;
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
    if(m_decoderInTSB[chip][j]!=0){
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInTSB-1 << " in a chip  ";
      std::cout<<"WHPB:decoderInTSB["<<chip<<"]["<<j<<"]:";
      m_decoderInTSB[chip][j]->print();
#endif
      for( block=0; block<m_decoderInTSB[chip][j]->getNumberOfData(); block+=1)
        if(m_decoderInTSB[chip][j]->getHit(block)){
          ch=m_nChOfTSBHit/2+(j-2)*m_nChOfTSBOut
            +block*m_nChInTSBRegion+m_decoderInTSB[chip][j]->getPos(block);
          if((ch>=0)&&(ch<m_nChOfTSBHit)) tHit->onChannel(ch);
        }
    }

  // for DoubletSlaveBoard
#ifdef TGCCOUT
  std::cout << "Doublet" << std::endl;
#endif
  for( j=0; j<NDecoderInDSB; j+=1)
    if(m_decoderInDSB[chip][j]!=0){
#ifdef TGCCOUT
      std::cout << "index:" << j << "/" << NDecoderInDSB-1 << " in a chip  ";
      std::cout<<"WHPB:decoderInDSB["<<chip<<"]["<<j<<"]:";
      m_decoderInDSB[chip][j]->print();
#endif
      for( block=0; block<m_decoderInDSB[chip][j]->getNumberOfData(); block+=1)
        if(m_decoderInDSB[chip][j]->getHit(block)){
          dHit->onChannel(j*m_nChOfDSBOut
                          +m_nChOfDSBOut/2*block+m_decoderInDSB[chip][j]->getPos(block));
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
  int nChUnitOfDSBHit=m_nChOfDSBHit/NBlockOfDSBChannel; //ch in a AB CD
  //  int nChSubUnitOfDSBHit = nChUnitOfDSBHit / 2;       //ch in a A B C D

#ifdef TGCCOUT
std::cout << "# Wire doCoincidenceMatrix() chip:" << chip << " block:" << block << std::endl;
#endif
  int dev, DevOrg, ich, DSBCh, TSBCh, ior;
  for(dev=0; dev <= m_maxDev; dev++){
    for(ich=0; ich < nChUnitOfDSBHit; ich++){
      if(dev>2*m_maxDev-m_maxDevOred){
          Oring = 1;
          DevOrg = 2*m_maxDev-m_maxDevOred + (dev - (2*m_maxDev-m_maxDevOred))*2 - 1;
      }else{
          Oring = 0;
          DevOrg = dev;
      }
      for(ior = 0; ior <= Oring; ior++){
        DSBCh = nChUnitOfDSBHit*block + ich;
        TSBCh = DSBCh + m_priorSign*(DevOrg + ior) + m_maxDevOred;
        if(tHit->getChannel(TSBCh) && dHit->getChannel(DSBCh)){
#ifdef TGCDEBUG
          std::cout << "HighPt" << std::endl
		    << " th&&dh==1 priorSign*dev:" << m_priorSign 
		    << "*" << dev 
		    << " DSBCh:" << DSBCh 
		    << " TSBCh:" << TSBCh << std::endl;
#endif
          // channel of dev>2*m_maxDev-m_maxDevOred is ored.
          m_highPtBoardOut->setPt(chip,block,PtHigh);
          m_highPtBoardOut->setHit(chip,block,true);
          m_highPtBoardOut->setDev(chip,block,m_priorSign*dev);
          m_highPtBoardOut->setPos(chip,block,ich/(m_nChOfDSBOut/4));
          return;
        }
      }
    }
    // for dR of opposite sign
    if(dev){
      for(ich=0; ich < nChUnitOfDSBHit; ich++){
        if(dev>2*m_maxDev-m_maxDevOred){
            Oring = 1;
            DevOrg = 2*m_maxDev-m_maxDevOred + (dev - (2*m_maxDev-m_maxDevOred))*2 - 1;
        }else{
            Oring = 0;
            DevOrg = dev;
        }
        for(ior = 0; ior <= Oring; ior++){
          DSBCh = nChUnitOfDSBHit*block + ich;
          TSBCh = DSBCh - m_priorSign*(DevOrg + ior) + m_maxDevOred;
          if(tHit->getChannel(TSBCh) && dHit->getChannel(DSBCh)){
#ifdef TGCDEBUG
          std::cout << "HighPt" << std::endl 
		    << " th&&dh==1 priorSign*dev:" << -m_priorSign 
		    << "*" << dev
		    << " DSBCh:" << DSBCh 
		    << " TSBCh:" << TSBCh << std::endl;
#endif
            // channel of dev>2*m_maxDev-m_maxDevOred is ored.
            m_highPtBoardOut->setPos(chip,block,ich/(m_nChOfDSBOut/4));
            m_highPtBoardOut->setDev(chip,block,-m_priorSign*dev);
            m_highPtBoardOut->setPt(chip,block,PtHigh);
            m_highPtBoardOut->setHit(chip,block,true);
            return;
          }
        }
      }
    }
  }
}

} //end of namespace bracket
