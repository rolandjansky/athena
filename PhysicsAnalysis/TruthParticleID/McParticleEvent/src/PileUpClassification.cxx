/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "McParticleEvent/PileUpClassification.h"


void PileUpClassification::findEventIterators(PileuType_t putype, McEventCollection::const_iterator &fEvt, McEventCollection::const_iterator &lEvt ){

  McEventCollection::const_iterator maxIterator = lEvt;
  //McEventCollection::const_iterator minIterator = fEvt;

  int intervallN =-1; // -1 means no interval search.


  switch(putype){
  case SIGNAL:
    lEvt = fEvt; lEvt++;
    break;
  case ALL:
    break;
  case INTIME:
    {
      // Special case : there's no SEPARATOR between SIGNAL and INTIME :
      fEvt++; lEvt = fEvt; lEvt++;
      for(; lEvt < maxIterator; lEvt++){
        if( (*lEvt)->signal_process_id() == 0 ) break;
      }
    break;
    }
  case OUTOFTIME:
    intervallN = 1;
    break;
  case RESTOFMB:
    intervallN = 2;
    break;
  case CAVERN:
    intervallN = 3;
    break;
  case ALLMINBIAS:
    fEvt++;
    break;
  default:
    break;
  };


  // if needed, search the correct interval
  while(intervallN>0){
    for(; fEvt < maxIterator; fEvt++){
      if( (*fEvt)->signal_process_id() == 0 ) { fEvt++; break; }
    }
    lEvt = fEvt;
    for(; lEvt < maxIterator; lEvt++){
      if( (*lEvt)->signal_process_id() == 0 ) break;
    }
    intervallN--;
  }

  // don't overstep your bounds!
  if (lEvt>maxIterator) lEvt=maxIterator;
  if (fEvt>maxIterator) fEvt=maxIterator;

}

void PileUpClassification::findEventIterators(PileuType_t putype, const McEventCollection* evts, size_t &firstIndex, size_t &lastIndex ){

  McEventCollection::const_iterator fEvt = evts->begin();
  McEventCollection::const_iterator lEvt = evts->end();
  findEventIterators(putype, fEvt, lEvt);
  firstIndex = fEvt - evts->begin();
  lastIndex  = lEvt - evts->begin();
}

PileUpClassification::PileuType_t PileUpClassification::pileUpType(const McEventCollection* evts, size_t evtIndex){
  if(evtIndex == 0 ) return SIGNAL;

  McEventCollection::const_iterator fEvt = evts->begin();
  McEventCollection::const_iterator lEvt = evts->end();
  McEventCollection::const_iterator itEvt = fEvt;
  itEvt++;
  PileuType_t type = INTIME;
  while( itEvt != lEvt){
    if( size_t(itEvt - fEvt) == evtIndex ){ return type ;}
    itEvt++;
    if( (*itEvt)->signal_process_id() == 0 ) {  // a SEPARATOR
      itEvt++; type = (PileuType_t) (type+1); 
    }
  }
  return  UNKONW;
}

void  PileUpClassification::fillPileUpType(const McEventCollection* evts, std::vector<size_t> &evtToPUType){
  evtToPUType.resize(evts->size());
  McEventCollection::const_iterator fEvt = evts->begin();
  McEventCollection::const_iterator lEvt = evts->end();
  // 1st GenEvent is always SIGNAL
  evtToPUType[0] = SIGNAL; fEvt++;
  // increment automatically following GenEvents
  PileuType_t type = INTIME;
  int c=1;
  for(; fEvt != lEvt; fEvt++){
    if( (*fEvt)->signal_process_id() == 0 ) type= (PileuType_t)(type+1);
    evtToPUType[c] = type;
    c++;
  }

}
