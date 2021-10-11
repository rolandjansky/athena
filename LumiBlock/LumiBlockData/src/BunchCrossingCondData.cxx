#include "LumiBlockData/BunchCrossingCondData.h"
#include "AthenaKernel/getMessageSvc.h"
#include <algorithm>

const BunchCrossingCondData::bunchTrain_t* BunchCrossingCondData::findTrain(const bcid_type bcid) const {

  const int bcidi=static_cast<int>(bcid);
  const auto it=std::find_if(m_trains.begin(),m_trains.end(),[&bcidi](const bunchTrain_t& bt){return (bt.m_first<=bcidi && bt.m_last>=bcidi);});
  
  if (it==m_trains.end()) {
    return nullptr;
  }
  else {
    return &(*it);
  }

  /*
  auto& it=std::lower_bound(m_trains.begin(),m_trains.end(),bunchTrain_t(bcid,0,0));
  if (it==m_trains.end() || it==m_trains.begin()) return nullptr;

  //it points now tho the train that starts after the bcid we are looking for
  //if the bcid is in a train, it must be the train before. 
  it--;
  if (it->m_first>=bcid && it->m_last<=bcid) {
    return &(*it);
  }
  else {
    return nullptr;
  }
  */


}

int BunchCrossingCondData::distanceFromFront( const bcid_type bcid,
					      const BunchDistanceType type ) const {

  const bunchTrain_t* bt=findTrain(bcid);
  if (!bt) {
    return -1;
  }

  const int dist=bcid-bt->m_first;

  switch( type ) {
  case NanoSec:
    return dist*m_BUNCH_SPACING;
    break;
  case BunchCrossings:
    return dist;
  case FilledBunches:
    return countColliding(bt->m_first,bt->m_last);
  default:
    MsgStream msg(Athena::getMessageSvc(),"BunchCrossingCondData::distanceFromFront");
    msg << MSG::ERROR << "BunchDistanceType not understood!" << endmsg;
    return -1;
  }//end switch
}
  
int BunchCrossingCondData::distanceFromTail( const bcid_type bcid,
					      const BunchDistanceType type ) const {

  const bunchTrain_t* bt=findTrain(bcid);
  if (!bt) {
    return -1;
  }

  const int dist=bt->m_last-bcid;

  switch( type ) {
  case NanoSec:
    return dist*m_BUNCH_SPACING;
    break;
  case BunchCrossings:
    return dist;
  case FilledBunches:
    return countColliding(bt->m_first,bt->m_last);
  default:
    MsgStream msg(Athena::getMessageSvc(),"BunchCrossingCondData::distanceFromTail");
    msg << MSG::ERROR << "BunchDistanceType not understood!" << endmsg;
    return -1;
  }//end switch
}
  
unsigned BunchCrossingCondData::numberOfUnpairedBunches() const {
  return ((m_beam1 | m_beam2) & ~m_luminous).count();
}


std::pair<unsigned,unsigned> BunchCrossingCondData::bunchTrainPopulation(const bcid_type bcid) const {

  const bunchTrain_t* bt=findTrain(bcid);
  if (!bt) {
    return std::pair<unsigned,unsigned>(0,0);
  }
  else {
    return std::pair<unsigned,unsigned>(bt->m_nColl,(bt->m_last-bt->m_first));
  }
}


unsigned BunchCrossingCondData::countColliding(int from, int to) const {
  unsigned ncoll=0;

  if (from<0) {
    //wrap-around ...
    for (int idx=m_MAX_BCID-from;idx<0;++idx) {
      if (m_luminous.test(idx)) ++ncoll;      
    }
    from=0;
  }

  if (to>m_MAX_BCID) {
    for (int idx=0;idx<m_MAX_BCID-to;++idx) {
      if (m_luminous.test(idx)) ++ncoll;
    }
    to=m_MAX_BCID;
  }
	 
  for (int idx=from;idx<to;++idx) {
    if (m_luminous.test(idx)) ++ncoll;      
  }
  return ncoll;
}


int BunchCrossingCondData::gapBeforeTrain( bcid_type bcid,
					   BunchDistanceType type) const {

  const bunchTrain_t* bt=findTrain(bcid);
  if (bt==nullptr) {
    return -1;
  }

  return gapBeforeBunch(bt->m_first,type);
}

int BunchCrossingCondData::gapBeforeBunch( bcid_type bcid,
					   BunchDistanceType type) const {
  int index=bcid-1;
  if (index<0) {
    index=m_MAX_BCID-1;
  }

  int result=0;
  
  while (!m_luminous.test(index) && result<m_MAX_BCID) {
    result++;
    index--;
    if (index<0) {
      index=m_MAX_BCID-1;
    }
  }

  if (type==NanoSec) {
    result*=m_BUNCH_SPACING;
  }

  return result;
}



int BunchCrossingCondData::gapAfterTrain( bcid_type bcid,
					  BunchDistanceType type) const {
  const bunchTrain_t* bt=findTrain(bcid);
  if (bt==nullptr) {
    return -1;
  }
  return gapAfterBunch(bt->m_last,type);
}

int BunchCrossingCondData::gapAfterBunch( bcid_type bcid,
					  BunchDistanceType type) const {
  int index=bcid+1;
  if (index>=m_MAX_BCID) {
    index=0;
  }
  int result=0;
  while (!m_luminous.test(index) && result<m_MAX_BCID) {
    result++;
    index++;
    if (index>=m_MAX_BCID) {
      index=0;
    }
  }

 if (type==NanoSec) {
    result*=m_BUNCH_SPACING;
  }

 return result;
}

BunchCrossingCondData::BunchCrossingType BunchCrossingCondData::bcType(const bcid_type bcid ) const {
  // First the obvious check:
  if (!isFilled(bcid))
  {
    // Check if it's an unpaired bunch:
    if (isUnpaired(bcid))
    {
      return Unpaired;
    }
    // If the previous bunch crossing is the tail of a bunch train:
    if (!distanceFromTail(bcid - 1, BunchCrossings))
    {
      return FirstEmpty;
    }
    // Check if it's in the middle of a bunch train:
    if (findTrain(bcid) != nullptr) {
      return MiddleEmpty;
    }
    // If none of the above are true, it has to be a "simple" empty bunch:
    return Empty;
  }

  // Now we know that the bunch has to be a filled one...

  // If it's not in a train, it has to be a single filled bunch:
  if (!isInTrain(bcid))
    return Single;

  // Let's check if it is close to the front of a bunch train:
  int distance = distanceFromFront(bcid, NanoSec);
  if ((distance >= 0) && (distance <= m_headTailLength))
  {
    return Front;
  }
  // Now let's check if it's close to the tail of a bunch train:
  distance = distanceFromTail(bcid, NanoSec);
  if ((distance >= 0) && (distance <= m_headTailLength))
  {
    return Tail;
  }

  // If none of the above are true, it has to be in the middle of a train:
  return Middle;
}
