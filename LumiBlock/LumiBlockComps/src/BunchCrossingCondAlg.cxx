/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */

#include "BunchCrossingCondAlg.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include <stdint.h>
#include "CoralBase/Blob.h"
#include "CoolKernel/IObject.h"

StatusCode BunchCrossingCondAlg::initialize() {
  ATH_CHECK( m_fillParamsFolderKey.initialize() );
  ATH_CHECK( m_outputKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode BunchCrossingCondAlg::execute (const EventContext& ctx) const {

  SG::ReadCondHandle<AthenaAttributeList> fillParamsHdl (m_fillParamsFolderKey, ctx);
  EventIDRange range;
  ATH_CHECK( fillParamsHdl.range (range) );

  const AthenaAttributeList* attrList=*fillParamsHdl;

  if ((*attrList)["BCIDmasks"].isNull()) {
    ATH_MSG_ERROR( "BunchCode is NULL in " << m_fillParamsFolderKey.key() << "!" );
    return StatusCode::FAILURE;
  }

  //Output object:
  auto bccd=std::make_unique<BunchCrossingCondData>();


  // Do everything here for now, but should copy this to a vanilla object based on attrList
  cool::UInt32 nb1 = (*attrList)["Beam1Bunches"].data<cool::UInt32>();
  cool::UInt32 nb2 = (*attrList)["Beam2Bunches"].data<cool::UInt32>();
  cool::UInt32 ncol = (*attrList)["LuminousBunches"].data<cool::UInt32>();

  ATH_MSG_DEBUG( "Beam1Bunches: " << nb1 );
  ATH_MSG_DEBUG( "Beam2Bunches: " << nb2 );
  ATH_MSG_DEBUG( "LuminousBunches: " << ncol );

  const coral::Blob& blob = (*attrList)["BCIDmasks"].data<coral::Blob>();

  if (m_isRun1) {   //Assume run1 layout as explained at https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CoolOnlineData
    ATH_MSG_DEBUG("Assuming run 1 database");
    // Verify length
    if ( static_cast<cool::UInt32>( blob.size() ) != 2 * (nb1 + nb2 + ncol)) {
      ATH_MSG_WARNING( "BCIDmasks length " << blob.size() << " != 2 * " << (nb1+nb2+ncol) );
      return StatusCode::SUCCESS;
    }
    const uint16_t* blobAddr=static_cast<const uint16_t*>(blob.startingAddress());
    //Decode beam1 filling:
    for (size_t idx=0;idx<nb1;++idx) {
      const uint32_t bcid=blobAddr[idx];
      bccd->m_beam1.set(bcid);
    }
        
    //Decode beam2 filling:
    for (size_t idx=nb1;idx<nb2;++idx) {
      const uint32_t bcid=blobAddr[idx];
      bccd->m_beam2.set(bcid);
    }

    //Decode colliding:
    for (size_t idx=nb2;idx<ncol;++idx) {
      const uint32_t bcid=blobAddr[idx];
      bccd->m_luminous.set(bcid);
    }
  }
  else { 
    ATH_MSG_DEBUG("Assuming run 2/3 database");
    //Assume run2 layout as explained at https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CoolOnlineData
    //Verify length
    if (blob.size()!=BunchCrossingCondData::m_MAX_BCID) {
      ATH_MSG_ERROR("Found blob with unexpected length " << blob.size() << "(expect " << BunchCrossingCondData::m_MAX_BCID <<") in folder " << m_fillParamsFolderKey.key());
      return StatusCode::FAILURE;
    }
    const uint8_t* blobAddr=static_cast<const uint8_t*>(blob.startingAddress());
    for (uint32_t bcid=0;bcid<BunchCrossingCondData::m_MAX_BCID;++bcid) {
      if (blobAddr[bcid] & 0x1) {
	bccd->m_beam1.set(bcid);
      }
      if (blobAddr[bcid] & 0x2) {
	bccd->m_beam2.set(bcid);
      }
      if ((blobAddr[bcid] & 0x3) == 0x3) {
	bccd->m_luminous.set(bcid);
      }
    }

    //Consistency checks: 
    if (bccd->m_beam1.count()!= nb1) {
      ATH_MSG_WARNING("Found " << bccd->m_beam1.count() << " bunches in beam1, expected " << nb1);
    }

    if (bccd->m_beam2.count()!= nb2) {
      ATH_MSG_WARNING("Found " << bccd->m_beam2.count() << " bunches in beam2, expected " << nb2);
    }

    if (bccd->m_luminous.count()!= ncol) {
      ATH_MSG_WARNING("Found " << bccd->m_luminous.count() << " colliding bunches, expected " << ncol);
    }
  }//end else run2 
	
  bccd->m_trains=findTrains(bccd->m_luminous, m_maxBunchSpacing,m_minBunchesPerTrain);
  
  SG::WriteCondHandle<BunchCrossingCondData> writeHdl(m_outputKey, ctx);
  ATH_CHECK( writeHdl.record (range, std::move (bccd)) );
  return StatusCode::SUCCESS;
}


std::vector<BunchCrossingCondData::bunchTrain_t> BunchCrossingCondAlg::findTrains(const std::bitset< BunchCrossingCondData::m_MAX_BCID>& collidingBCIDs, const int maxSpacingInTrain, const unsigned minBunchesPerTrain) const {
  
  constexpr int MAX_BCID=collidingBCIDs.size();
  std::vector<bunchTrain_t> result;
    
  std::vector<std::pair<int,int> > holes;
  int start=0;
  int stop=0;
  
  while (stop<MAX_BCID) { //Outer loop, over holes
    for (start=stop;stop<MAX_BCID && !collidingBCIDs.test(stop); ++stop) {};//Count consecutive holes
    //stop points now to the next filled bcid (or MAX)
    if ((stop-start)>maxSpacingInTrain) {
	holes.emplace_back(start,stop);
      }//end if >maxSpacingInTrain
    ++stop;
  }//end outer loop
  

  ATH_MSG_DEBUG("Found " << holes.size() << " gaps larger than " << maxSpacingInTrain << " in the bunch structure");
  if (msgLvl(MSG::VERBOSE)) {
    for (auto& h : holes) {
      msg(MSG::VERBOSE) << "Hole: " << h.first << " - " << h.second << endmsg;
    }
  }
    

  if (holes.size()<1) {
    ATH_MSG_ERROR("Looks like we have bunch train spanning the entire ring w/o any gap. Really?");
    return result;
  }


  if (holes.size()>1) { 
    //generic case of having at least 2 gaps int the bunch structure
    for (unsigned i=0;i<holes.size()-1;++i) {
      //Count colliding bunches in this train:
      unsigned ncoll=0;
      for (int idx=holes[i].second;idx<holes[i+1].first-1;++idx) {
	if (collidingBCIDs.test(idx)) {
	  ++ncoll;
	}
      }
      result.emplace_back(holes[i].second,holes[i+1].first-1,ncoll);
    }
  }

  if (holes.size()==1 || (holes.front().first!=0 && holes.back().second!=MAX_BCID-1))  {
    //Special case of only one hole and/or first/last BCIDs populated (train across the wrap-around)

    //To simplify the distanceToFront and distanceToTail methods we duplicate this train
    //Once as first train wiht a starting point in the negative range, and
    //once as last train with a ending point beyond MAX_BCID

    //Count the number of collisions in this train:
    unsigned ncoll=0;
    for (int idx=0;idx<holes.front().first;++idx) {
      if (collidingBCIDs.test(idx)) {
	++ncoll;
      }
    }
    for (int idx=holes.back().second;idx<MAX_BCID;++idx) {
      if (collidingBCIDs.test(idx)) {
	++ncoll;
      }
    }

    BunchCrossingCondData::bunchTrain_t firsttrain(holes.back().second-MAX_BCID, holes.front().first-1,ncoll);
    BunchCrossingCondData::bunchTrain_t lasttrain(holes.back().second,MAX_BCID+holes.front().first-1,ncoll);
    result.insert(result.begin(),firsttrain);
    result.push_back(lasttrain);
  }//end if wrap-around populated


  ATH_MSG_DEBUG("Found " << result.size() << " Bunch trains separated by gaps of at least " << maxSpacingInTrain << " bcids ");
  
  //copy the vector, cleaning out trains with too few colliding bunches
  std::vector<bunchTrain_t> result1;
  result1.reserve(result.size());
  for (const bunchTrain_t& train: result) {
    if (train.m_nColl >= minBunchesPerTrain) {
      result1.emplace_back(train);
    }
  }

  ATH_MSG_DEBUG("Found " << result1.size() << " Bunch trains have at least " << minBunchesPerTrain << " colliding bunches");


  if (msgLvl(MSG::VERBOSE)) {
    for (auto& r : result1) {
      msg(MSG::VERBOSE) << "Train " << r.m_first << " - " << r.m_last << ", " << r.m_nColl << " colliding bcids" << endmsg;
    }
  }

  return result1;
}
