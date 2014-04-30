/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ref. SOS054V06,SOS053V04
#include "TrigT1TGC/TGCInnerSB.hh"
#include <iostream>
#include <cstdlib>


namespace LVL1TGCTrigger {

extern bool g_STRICTST;

TGCInnerSB::TGCInnerSB():TGCSlaveBoard()
{}

void TGCInnerSB::createSlaveBoardOut()
{
  if(coincidenceOut!=0){
    if ( slaveBoardOut!=0 ) delete slaveBoardOut;
    slaveBoardOut = new  TGCSlaveBoardOut(this, bid);
    if ( !slaveBoardOut ) {
      std::cerr << "TGCInnerSB::createSlaveBoardOut: Memory allocation failure.";
      exit(1);
    }
    slaveBoardOut->clear();
    slaveBoardOut->setNumberOfData(NumberOfInnerSBData);

    // fill SlaveBoardOut.
    // select largest R hit in each sections.
    int lengthOfSection = lengthOfCoincidenceOut/NumberOfInnerSBData;
    int i,j;
#ifdef TGCDEBUG
    std::cout <<" Type :" << getType() << "  id:" << getId()
	      <<" lengthOfCoincidenceOut= "<< lengthOfCoincidenceOut 
	      <<" NumberOfInnerSBData= "<<NumberOfInnerSBData  
	      <<" lengthOfSection= "<<lengthOfSection<<std::endl;
#endif
    for( i=0; i<NumberOfInnerSBData; i+=1){// i=3:d 2:c 1:b 0:a, 7:d 6:c 5:b 4:a
      slaveBoardOut->setHit(i,false);
      for( j=0; j<lengthOfSection; j+=1) {
        if(coincidenceOut->getChannel(j+i*lengthOfSection)){
          slaveBoardOut->setPos(i,j);
          slaveBoardOut->setHit(i,true);
#ifdef TGCDEBUG
	  std::cout <<" Hit @"<< i << "  section:" << j <<std::endl;
#endif
          break;
        }
      }
      if(slaveBoardOut->getHit(i)){
          slaveBoardOut->setbPos(i, slaveBoardOut->getPos(i));
#ifdef TGCCOUT
          slaveBoardOut->getbPos(i)->printb();
	  std::cout << " " << i << std::endl;
#endif
      }
    }
  }
}

void TGCInnerSB::doCoincidence()
{
  TGCHitPattern* pattern[2];
  pattern[0] = patchPanelOut->getHitPattern(0);
  pattern[1] = patchPanelOut->getHitPattern(1);

  int length;
  if(pattern[0]!=0){
    length = pattern[0]->getLength();
  }else if(pattern[1]!=0){
    length = pattern[1]->getLength();
  }else
    length = -1;
  
  if(length>0){
    lengthOfCoincidenceOut = 2*length;
    if(coincidenceOut!=0) delete coincidenceOut;
    coincidenceOut = new TGCHitPattern(lengthOfCoincidenceOut);

    // rearrange bit pattern for coincidence.
    bool* b = new bool [lengthOfCoincidenceOut];

    int j;
    for( j=0; j<lengthOfCoincidenceOut; j+=1){
      b[j]=false;
    }

    //   rearrange signal pattern for 2layer for 1/2 coincidence.  
    // ! the order to rearrange is chosen by Daniel table P.  
    int i;
    for( i=0; i<length/2; i+=1){
      if(pattern[1]!=0){
        b[2*i+1]   = pattern[1]->getChannel(i+length/2);// smaller in phi
        b[2*i] = pattern[1]->getChannel(i);
      }
      if(pattern[0]!=0){
        b[length+2*i+1]   = pattern[0]->getChannel(i+length/2);// smaller in phi
	b[length+2*i] = pattern[0]->getChannel(i); 
      }
    }

#ifdef TGCDEBUG
    std::cout <<" Type :" << getType() << "  id:" << getId() << std::endl;
    for( j=0; j<lengthOfCoincidenceOut; j+=1){
      if (b[j]) std::cout << "*";
      else      std::cout << "-";
    }
    std::cout <<std::endl;
#endif
    // perform 1/2 coincidence
    int block;
    for( block=0; block<2; block+=1){
      int base=block*length;
/*
        b &  c
       +b & !a & !c
       +a &  c & !b
       +c & !b & !d
       +b &  d & !c
*/

      if(g_STRICTST){
	for(int i=base+1; i<base+length; i++){
	  coincidenceOut->setChannel(i,( b[i-1] & b[i] ));
	}
      } else {

	i=base;
	coincidenceOut->setChannel(i,( b[i] & !b[i+1] ));
	
	i=base+1;
	coincidenceOut->setChannel(i,(( b[i-1] &  b[i] )|
				      ( b[i-1] & !b[i] )|
				      ( b[i]   & !b[i-1] & !b[i+1] )|
				      ( b[i-1] &  b[i+1] & !b[i] )));
	
	for( i=base+2; i<base+length-1; i+=1){
	  coincidenceOut->setChannel(i,(( b[i-1] &  b[i] )|
					( b[i-1] & !b[i-2] & !b[i] )|
					( b[i-2] &  b[i]   & !b[i-1] )|
					( b[i]   & !b[i-1] & !b[i+1] )|
					( b[i-1] &  b[i+1] & !b[i] )));
	}
	i=base+length-1;
	coincidenceOut->setChannel(i,(( b[i-1] &  b[i] )|
				      ( b[i-1] & !b[i-2] & !b[i] )|
				      ( b[i-2] &  b[i]   & !b[i-1] )|
				      ( b[i]   & !b[i-1] )));
	
      }

#ifdef TGCCOUT
      std::cout << "InnerCoincidence OUT ";
      coincidenceOut->printb();
      std::cout << std::endl;
#endif
	
    }
    
    delete [] b;
  }
}

} //end of namespace bracket
