/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */

#include "BunchCrossingCondAlg.h"
#include "CoolKernel/IObject.h"
#include "CoralBase/Blob.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include <charconv>
#include <cstdint>

#include "CoralBase/AttributeListException.h"
#include "TrigConfL1Data/BunchGroupSet.h"

StatusCode BunchCrossingCondAlg::initialize() {
  if (m_mode == 2) {
    ATH_CHECK( m_trigConfigSvc.retrieve() );
  }
  ATH_CHECK( m_fillParamsFolderKey.initialize( m_mode == 0 || m_mode == 1 ) );
  ATH_CHECK( m_lumiCondDataKey.initialize( m_mode == 3 ) );
  ATH_CHECK( m_outputKey.initialize() );
  return StatusCode::SUCCESS;
}


StatusCode BunchCrossingCondAlg::execute (const EventContext& ctx) const {

  SG::WriteCondHandle<BunchCrossingCondData> writeHdl(m_outputKey, ctx);
  if (writeHdl.isValid()) {
    ATH_MSG_DEBUG("Found valid write handle");
    return StatusCode::SUCCESS;
  }

  //Output object & range:
  auto bccd=std::make_unique<BunchCrossingCondData>();
  EventIDRange range;

  if (m_mode == 2) { // use trigger bunch groups
    const std::vector< TrigConf::BunchGroup >& bgs =
        m_trigConfigSvc->bunchGroupSet()->bunchGroups();
    // bunch group 1 = paired
    ATH_MSG_INFO("BG1 bunches " << bgs[1].bunches() );
    for (const auto& pos : bgs[1].bunches() ) {
      bccd->m_beam1.set(pos);
      bccd->m_beam2.set(pos);
      bccd->m_luminous.set(pos);
    }
    // in Run 1 we don't have bunch group information to determine beam 1 or beam 2 unpaired
    // so test if we have at least 15 bunch groups, then assume BG13/14 are the unpaired bunches
    if (bgs.size() >= 15) {
      for (const auto& pos : bgs[13].bunches() ) {
        bccd->m_beam1.set(pos);
      }
      for (const auto& pos : bgs[14].bunches() ) {
        bccd->m_beam2.set(pos);
      }
    }
    // find trains
    bccd->m_trains=findTrains(bccd->m_luminous, m_maxBunchSpacing,m_minBunchesPerTrain);
    // we will only trust the validity for 1 LB but that's OK
    const auto& thisevt = ctx.eventID();
    range = EventIDRange(EventIDBase(thisevt.run_number(), EventIDBase::UNDEFEVT, 
                                      EventIDBase::UNDEFNUM, 0, thisevt.lumi_block()),
                          EventIDBase(thisevt.run_number(), EventIDBase::UNDEFEVT,
                                      EventIDBase::UNDEFNUM, 0, thisevt.lumi_block()+1));
  }

  if (m_mode == 3) { // use luminosity data
    SG::ReadCondHandle<LuminosityCondData> prefLumiHdl{m_lumiCondDataKey, ctx};
    ATH_CHECK( prefLumiHdl.range (range) );

    // consider BCID filled if mu < 1000*average mu
    float avMu = prefLumiHdl->lbAverageInteractionsPerCrossing();
    const auto& lumiVec = prefLumiHdl->lbLuminosityPerBCIDVector();
    float cutLumi = avMu/1000.f*prefLumiHdl->muToLumi();

    for (size_t bcid = 0; bcid < LuminosityCondData::TOTAL_LHC_BCIDS; ++bcid ) {
      if (lumiVec[bcid] > cutLumi) {
        bccd->m_beam1.set(bcid);
        bccd->m_beam2.set(bcid);
        bccd->m_luminous.set(bcid);
      }
    }
    // find trains
    bccd->m_trains=findTrains(bccd->m_luminous, m_maxBunchSpacing,m_minBunchesPerTrain);
  }

  if (m_mode == 0 || m_mode == 1) { // use FILLPARAMS (data) or /Digitization/Parameters (MC)

    SG::ReadCondHandle<AthenaAttributeList> fillParamsHdl (m_fillParamsFolderKey, ctx);
    ATH_CHECK( fillParamsHdl.range (range) );

    const AthenaAttributeList* attrList=*fillParamsHdl;


    if (m_mode == 1) {
      ATH_MSG_INFO("Assuming MC case");
      ATH_MSG_INFO("Got AttributeList with size " << attrList->size());
      std::string sbunches;
      try {
        const coral::Attribute& attr=(*attrList)[std::string("BeamIntensityPattern")];
        if (attr.isNull()) {
          ATH_MSG_ERROR("Got NULL attribute for BeamIntensityPattern");
          return StatusCode::FAILURE;
        }
        sbunches = attr.data< std::string >();
      } catch (coral::AttributeListException& e) {
        ATH_MSG_ERROR(e.what());
        ATH_MSG_ERROR("Failed to get Attribute 'BeamIntensityPattern' from AttributeList in " << m_fillParamsFolderKey);
        return StatusCode::FAILURE;
      }
      
      const float minBunchIntensity=0.001;
      std::vector<float> bunches=tokenize(sbunches);
      if (!bunches.empty()) {
        // Check if the pattern "fits into" the LHC:
        if( BunchCrossingCondData::m_MAX_BCID % bunches.size() ) {

          ATH_MSG_INFO( "Bunch pattern doesn't \"fit into\" " << BunchCrossingCondData::m_MAX_BCID );
          // The loop doesn't go all the way up to MAX_BCID/2 in order not to produce "weird"
          // patterns half way. This should be pretty safe to do, because the MC BCIDs will
          // only be in the range defined by the pattern from the metadata.
          for( int i = 0; i < ( BunchCrossingCondData::m_MAX_BCID / 2 - 20 ); ++i ) {
            const int pos1 = i % bunches.size();
            const int pos2 = bunches.size() - 1 - ( i % bunches.size() );
            if( bunches[ pos1 ] > minBunchIntensity) {
              bccd->m_beam1.set(i);
              bccd->m_beam2.set(i);
              bccd->m_luminous.set(i);
            }
            if( bunches[ pos2 ] > minBunchIntensity) {
              bccd->m_beam1.set(BunchCrossingCondData::m_MAX_BCID - 1 -i);
              bccd->m_beam2.set(BunchCrossingCondData::m_MAX_BCID - 1 -i);
              bccd->m_luminous.set(BunchCrossingCondData::m_MAX_BCID - 1 -i);
            }
          }
    
        } else {	
          // If the sample size fits into the number of available bunches, the algorithm
          // is pretty simple:
          ATH_MSG_INFO( "Bunch pattern \"fits into\" " << BunchCrossingCondData::m_MAX_BCID );
          for( int i = 0; i < BunchCrossingCondData::m_MAX_BCID; ++i ) {
            const int pos = i % bunches.size();
            if( bunches[ pos ] > minBunchIntensity) {
              bccd->m_beam1.set(i);
              bccd->m_beam2.set(i);
              bccd->m_luminous.set(i);
            }
          }
        }
        //Filled bcid-bitsets, now extract trains:
        bccd->m_trains=findTrains(bccd->m_luminous, m_maxBunchSpacing,m_minBunchesPerTrain);
      }
      else {//got no bunches from metadata
        ATH_MSG_INFO("Bunch structure information not found in metadata");
        ATH_MSG_INFO("Will consider all BCIDs as single filled bunches (no trains)");
        bccd->m_beam1.set();
        bccd->m_beam2.set();
        bccd->m_luminous.set();
      }
    }
    else { // mode == 0, Data-case
      if ((*attrList)["BCIDmasks"].isNull()) {
        ATH_MSG_ERROR( "BunchCode is NULL in " << m_fillParamsFolderKey.key() << "!" );
        return StatusCode::FAILURE;
      }
      

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
      //Filled bcid-bitsets, now extract trains
      bccd->m_trains=findTrains(bccd->m_luminous, m_maxBunchSpacing,m_minBunchesPerTrain);
    }//end else is data
  }
  
  ATH_CHECK( writeHdl.record (range, std::move (bccd)) );
  return StatusCode::SUCCESS;
}


std::vector<BunchCrossingCondData::bunchTrain_t> BunchCrossingCondAlg::findTrains(const std::bitset< BunchCrossingCondData::m_MAX_BCID>& collidingBCIDs, const int maxSpacingInTrain, const unsigned minBunchesPerTrain) const {
  
  const int MAX_BCID=collidingBCIDs.size();
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
    

  if (holes.empty()) {
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

  ATH_MSG_INFO("Found " << result1.size() << " Bunch trains having at least " << minBunchesPerTrain << " colliding bunches and separated by at least " << maxSpacingInTrain << " bcids");


  if (msgLvl(MSG::VERBOSE)) {
    for (auto& r : result1) {
      msg(MSG::VERBOSE) << "Train " << r.m_first << " - " << r.m_last << ", " << r.m_nColl << " colliding bcids" << endmsg;
    }
  }

  return result1;
}


/**
 * This helper function is used to convert a string of type
 * "[0.0, 0.0, 1.0, 1.0, 1.0]" into a vector of floats. As it happens, the
 * digitization stores the bunch pattern in such a fancy style...
 *
 * @param pattern The pattern extracted from the MC file metadata
 * @returns The "decoded" bunch pattern
 */
std::vector<float> BunchCrossingCondAlg::tokenize( const std::string& pattern ) const {
  ATH_MSG_VERBOSE("Input to tokenize: " << pattern);
  
  std::vector< float > result;
  const char* c= pattern.data();
  const char* cEnd= c + pattern.size();
  while(c<cEnd) {
    //This loop swallows actually any string containing numbers
    //separated by non-numbers
    char* end;
    float f=std::strtof(c,&end);
    if (c==end) {//Can't convert, try next
      c++;
    }
    else { //got a value
      result.push_back(f);
      c=end;
    }
  }//end while loop
  return result;
}
