/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCDoubletSB.hh"
#include "TrigT1TGC/TGCSlaveBoard.hh"
#include "TrigT1TGC/TGCPatchPanelOut.hh"
#include "TrigT1TGC/TGCHitPattern.hh"
#include <iostream>
#include <cstdlib>


namespace LVL1TGCTrigger {

extern bool g_STRICTWD;
extern bool g_STRICTSD;

TGCDoubletSB::TGCDoubletSB()
  :TGCSlaveBoard(),
   m_priorSign(1),m_numberOfData(2),m_nChInBlock(32),
   m_maxDev(0),m_posMaxDev(0),m_negMaxDev(0),m_nChAdj(0),m_iChBase(0),
   m_MatrixOverlap(4),m_SType(WIREGROUP)
{
}

TGCDoubletSB::~TGCDoubletSB()
{
}

void TGCDoubletSB::createSlaveBoardOut()
{
  if(m_coincidenceOut!=0){
    int i,block;
    for( block=0; block<NumberOfDoubletSBData; block+=1){
      if(m_slaveBoardOut->getHit(block)){
        m_slaveBoardOut->setHit(block,false);
        for( i=0; i<m_nChInBlock; i+=1){
          if(m_coincidenceOut->getChannel(m_nChAdj+m_nChInBlock*block+i)){
            m_slaveBoardOut->setPos(block,i);
            m_slaveBoardOut->setHit(block,true);
            break;
          }
        }// block 0:A 1:B
        m_slaveBoardOut->setbPos(block, m_slaveBoardOut->getPos(block));
#ifdef TGCCOUT
        std::cout << " " << block << std::endl;
        m_slaveBoardOut->getbPos(block)->printb();
#endif
      }
    }
  }
}

void TGCDoubletSB::doCoincidence()
{
  // Local coincidence
  TGCHitPattern** LCOut = doLocalCoincidence(m_patchPanelOut);

  if ( m_slaveBoardOut!= 0 ) delete m_slaveBoardOut;
  m_slaveBoardOut = new TGCSlaveBoardOut(this,m_bid);
  if ( !m_slaveBoardOut ) {
    std::cerr << "TGCDoubletSB::createSlaveBoardOut: Memory allocation failure.";
    exit(1);
  }
  m_slaveBoardOut->clear();
  m_slaveBoardOut->setNumberOfData(NumberOfDoubletSBData);

  if(LCOut!=0){
    // Coincidence matrix
    do3outof4Coincedence(LCOut);

    delete LCOut[0];
    delete LCOut[1];
    delete LCOut[2];
    delete LCOut[3];
    delete [] LCOut;
  }
}

TGCHitPattern** TGCDoubletSB::doLocalCoincidence(const TGCPatchPanelOut* PPOut)
{
  TGCHitPattern** LCOut = 0;
  const TGCHitPattern* inner = PPOut->getHitPattern(0); // layer3,layer4
  const TGCHitPattern* pivot = PPOut->getHitPattern(1);  // layer5,lyaer6

  if((inner!=0)&&(pivot!=0)){
    int iLength = inner->getLength();
    int pLength = pivot->getLength();

    // rearrange bit pattern for coincidence.
    bool* bi = new bool [iLength];
    bool* bp = new bool [pLength];
    int i;
    for( i=0; i<iLength/2; i+=1){
      bi[2*i]=inner->getChannel(i); // smaller in eta [i:Layer3]
      bi[2*i+1]=inner->getChannel(i+iLength/2); // smaller in  phi [i+iLength/2:Layer4]
    }
    for( i=0; i<pLength/2; i+=1){   
      bp[2*i]=pivot->getChannel(i); // smallest in eta [i:Layer5]
      bp[2*i+1]=pivot->getChannel(i+pLength/2); // smaller in phi [i+pLength/2:Layer6]
    }

    // LCOut[0:3] are correspond to A,B,C,D in L1Trigger TDR Fig 12-12 respectively.
    LCOut = new TGCHitPattern* [4];
    LCOut[0] = new TGCHitPattern (iLength); // inner 2 hit
    LCOut[1] = new TGCHitPattern (iLength); // inner 1 hit
    LCOut[2] = new TGCHitPattern (pLength); // pivot 1 hit
    LCOut[3] = new TGCHitPattern (pLength); // pivot 2 hit
    /*
      Wire Strip         bi[1] 0  bi[3] 1        bi[iLength-1]
      C L4  L3 „¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢           „¡„Ÿ„Ÿ„¢          ->eta
      „¤false „¤„Ÿ„Ÿ„£„¤„Ÿ„Ÿ„£           „¤„Ÿ„Ÿ„£   
      D L3  L4     „¡„Ÿ„Ÿ„¢„¡„Ÿ„Ÿ„¢           „¡„Ÿ„Ÿ„¢„¡false          [inner]
      „¤„Ÿ„Ÿ„£„¤„Ÿ„Ÿ„£           „¤„Ÿ„Ÿ„£„¤„Ÿ„Ÿ„£
      bi[0] 0  bi[2] 1        bi[iLength-2]
      ^
      „¡„¢„¡„¢„¡„¢„¡„¢       „¡„¢„¡„¢„¡„¢„¡„¢           |
      „¤„£„¤„£„¤„£„¤„£       „¤„£„¤„£„¤„£„¤„£
      0   1   2   3                      iLength-1
    */
    i=0;
    LCOut[0]->setChannel(i,false);
    LCOut[1]->setChannel(i,(bi[i] & !bi[i+1]));

    for(i = 1; i<iLength-1; i+=1){
      LCOut[0]->setChannel(i,(bi[i-1] & bi[i]));
      LCOut[1]->setChannel(i,((!bi[i-1] &  bi[i] & !bi[i+1]) |
                              (bi[i-1] & !bi[i] &  bi[i+1])));
    }
    i=iLength-1;
    LCOut[0]->setChannel(i,(bi[i-1] & bi[i]));
    LCOut[1]->setChannel(i,(!bi[i-1] &  bi[i]));

#ifdef TGCDEBUG
    int j ;
    std::cout << "C,D i";
    for(j = 0; j<iLength; j++){
      if((j%10)+1==10){
        std::cout << "-";
      }else{
        std::cout << (j%10)+1;
      }
    }
    std::cout << std::endl << "2hit ";
    LCOut[0]->printb();
    std::cout << std::endl << "1hit ";
    LCOut[1]->printb();
    std::cout << std::endl;
#endif    

    /*
      L5  B   A L6  ©„Ÿ„Ÿ„Ÿ  Wire  [pivot]
      L6  B   A L5  © Strip  „ 
      „¡„¢        „    „ 
      „ false     «   «
      „¡„¢„ „        „¡„¢0
      bp[0]„ „ „¤„£       „¤„£
      0 „ „ „¡„¢       „¡„¢1„¡„¢0
      „¤„£„ „ bp[1]  „¤„£ „¤„£
      „¡„¢„ „    0   „¡„¢2„¡„¢1
      bp[2]„ „ „¤„£       „¤„£ „¤„£
      1 „ „ „¡„¢       „¡„¢3„¡„¢2
      „¤„£„ „ bp[4]  „¤„£ „¤„£
      „ „    1
      „¤„£
      „¡„¢
      „ „ 
      „ „ „¡„¢       „¡„¢ pLength-2
      „¤„£„ „        „¤„£
      „¡„¢„ „        „¡„¢ pLength-1      
      false„ „¤„£       „¤„£
      „ „  ª 
      „¤„£ „¤„Ÿ  bp[pLength-1]  
       
    */
    LCOut[2]->setChannel(0, bp[0] & !bp[1]);
    LCOut[3]->setChannel(0, bp[0] & bp[1]);
    for( i=1; i<pLength-1; i+=1){
      LCOut[2]->setChannel(i,((!bp[i-1] &  bp[i] & !bp[i+1]) |
			      ( bp[i-1] & !bp[i] &  bp[i+1])));
      LCOut[3]->setChannel(i,(bp[i] & bp[i+1]));
    }
    
    i=pLength-1;
    LCOut[2]->setChannel(i,!bp[i-1] & bp[i]);
    LCOut[3]->setChannel(i,false);
    
#ifdef TGCDEBUG
    std::cout << "A,B p";
    for(j = 0; j<pLength; j++){
      if((j%10)+1==10){
        std::cout << "-";
      }else{
        std::cout << (j%10)+1;
      }
    }
    std::cout << std::endl << "1hit ";
    LCOut[2]->printb();
    std::cout << std::endl << "2hit ";
    LCOut[3]->printb();
    std::cout << std::endl;
#endif    

    delete [] bi;
    delete [] bp;

  }

  return LCOut;
}

void TGCDoubletSB::do3outof4Coincedence(TGCHitPattern** LCOut)
{
  if(((LCOut[0]!=0)||(LCOut[1]!=0))&&((LCOut[2]!=0)||(LCOut[3]!=0))){
    if ( m_coincidenceOut != 0 ) delete m_coincidenceOut;
    m_coincidenceOut = new TGCHitPattern;
    if ( !m_coincidenceOut ) {
      std::cerr << "TGCDoubletSB::create coincidenceOut: Memory allocation failure.";
      exit(1);
    }
#ifdef TGCDEBUG
    std::cout << "# DoubletSB [do3outof4Coincedence]" <<std::endl;
    std::cout<<"#SB LCOut[0]: inner 2hit";LCOut[0]->print();
    std::cout<<"#SB LCOut[1]: inner 1hit";LCOut[1]->print();
    std::cout<<"#SB LCOut[2]: pivot 1hit";LCOut[2]->print();
    std::cout<<"#SB LCOut[3]: pivot 2hit";LCOut[3]->print();
#endif
    m_coincidenceOut->setLength(LCOut[2]->getLength());
    m_coincidenceOut->clear();
    int i;
    for( i=0; i<NumberOfDoubletSBData; i+=1)
      // select tracks in each submatrix.
      doCoincidenceInSubMatrix(i,LCOut);
  }
}

void TGCDoubletSB::doCoincidenceInSubMatrix(const int block, TGCHitPattern** LCOut)
{
  bool oredHit = false;
  bool hit = false;
  bool A, B, C, D;

  m_slaveBoardOut->setHit(block,false);
  int dev,pCh,iCh;
  for( dev=0; ( dev<=m_maxDev ) && !oredHit; dev+=1){
    int i;
    for(i = 0; i < m_nChAdj + m_nChInBlock + m_MatrixOverlap; i++){
      pCh = (m_nChAdj + m_nChInBlock - m_MatrixOverlap)*block + i;
      iCh = pCh + m_priorSign*dev + m_iChBase;
      if(0 <= iCh){
        if((0 <= iCh)&&(iCh < LCOut[0]->getLength())){
          A = LCOut[0]->getChannel(iCh);
        }else{A = false;}
        if((0 <= iCh)&&(iCh < LCOut[1]->getLength())){
          B = LCOut[1]->getChannel(iCh);
        }else{B = false;}
        if((0 <= pCh)&&(pCh < LCOut[2]->getLength())){
          C = LCOut[2]->getChannel(pCh);
        }else{C = false;}
        if((0 <= pCh)&&(pCh < LCOut[3]->getLength())){
          D = LCOut[3]->getChannel(pCh);
        }else{D = false;}
        hit = element(A, B, C, D);
        if(hit){
          if(block == 0){ //block A
            if((0 <= i) && (i < m_nChAdj+m_nChInBlock)){
              m_coincidenceOut->onChannel(pCh);
            }
          }else{//block B
            if((m_MatrixOverlap <= i) && (i < m_MatrixOverlap+m_nChInBlock+m_nChAdj)){
              m_coincidenceOut->onChannel(pCh);
            }
          }
        }
        oredHit |= hit; 
      }
    }
    if(oredHit){
      if((m_priorSign*dev>=m_negMaxDev)&&(m_priorSign*dev<=m_posMaxDev)){
        m_slaveBoardOut->setDev(block, m_priorSign*dev);
        m_slaveBoardOut->setbDev(block, m_priorSign, dev);
        m_slaveBoardOut->setHit(block, true);
#ifdef TGCDEBUG
	std::cout << "# DoubletSB : delta =" <<  m_priorSign*dev 
		  << " @pivot=" << pCh << " @onner="<< iCh << std::endl; 
#endif
        return;
      } else {
        oredHit = false;
      }
    } else if(dev!=0) {
      // Opposite sign of dr,dphi
      for(i = 0; i < m_nChAdj + m_nChInBlock + m_MatrixOverlap; i++){
        pCh = (m_nChAdj + m_nChInBlock - m_MatrixOverlap)*block + i;
        iCh = pCh - m_priorSign*dev + m_iChBase;
        if(iCh>=0){
          if((0 <= iCh)&&(iCh < LCOut[0]->getLength())){
            A = LCOut[0]->getChannel(iCh);
          }else{A = false;}
          if((0 <= iCh)&&(iCh < LCOut[1]->getLength())){
            B = LCOut[1]->getChannel(iCh);
          }else{B = false;}
          if((0 <= pCh)&&(pCh < LCOut[2]->getLength())){
            C = LCOut[2]->getChannel(pCh);
          }else{C = false;}
          if((0 <= pCh)&&(pCh < LCOut[3]->getLength())){
            D = LCOut[3]->getChannel(pCh);
          }else{D = false;}
          hit = element(A, B, C, D);
          if(hit){
            if(block == 0){ //block A
              if((0 <= i) && (i < m_nChAdj+m_nChInBlock)){
                m_coincidenceOut->onChannel(pCh);
              }
            }else{//block B
              if((m_MatrixOverlap <= i) && (i < m_MatrixOverlap+m_nChInBlock+m_nChAdj)){
                m_coincidenceOut->onChannel(pCh);
              }
            }
          }
          oredHit |= hit; 
        }
      }
      if(oredHit){
        if((-m_priorSign*dev>=m_negMaxDev)&&(-m_priorSign*dev<=m_posMaxDev)){
          m_slaveBoardOut->setDev(block,-m_priorSign*dev);
          m_slaveBoardOut->setbDev(block, -m_priorSign, dev);
          m_slaveBoardOut->setHit(block,true);
#ifdef TGCDEBUG
	std::cout << "# DoubletSB : delta =" <<  -m_priorSign*dev 
		  << " @pivot=" << pCh << " @onner="<< iCh << std::endl; 
#endif
          return;
        } else {
          oredHit = false;
        }
      }
    }
  }
}


bool TGCDoubletSB::element(bool A, bool B, bool C, bool D)
{
  if(g_STRICTWD) {
    if(m_SType == WIREGROUP) return (A & D);
  }
  if (g_STRICTSD) {
    if(m_SType == STRIP) return (A & D);
  }
  
  return ((A & C)|( A & D)|( B & D));
}


} //end of namespace bracket


