/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CscOverlay/CscOverlay.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGauss.h"

namespace {
  constexpr uint16_t MAX_AMPL = 4095; // 12-bit ADC
}

//================================================================
CscOverlay::CscOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//================================================================
StatusCode CscOverlay::initialize()
{
  ATH_MSG_DEBUG("CscOverlay initialized");

  /** access to the CSC Identifier helper */
  ATH_CHECK(detStore()->retrieve(m_cscHelper, "CSCIDHELPER"));
  ATH_MSG_DEBUG(" Found the CscIdHelper. ");

  /** CSC calibratin tool for the Condtiions Data base access */
  ATH_CHECK(m_cscCalibTool.retrieve());

  // get cscRdoDecoderTool
  ATH_CHECK(m_cscRdoDecoderTool.retrieve());

  //random number initialization
  ATH_CHECK(m_rndmSvc.retrieve());

  ATH_CHECK( m_bkgInputKey.initialize() );
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_CHECK( m_outputKey.initialize() );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::execute() {
  ATH_MSG_DEBUG("execute() begin");


  SG::ReadHandle<CscRawDataContainer> bkgContainer(m_bkgInputKey);
  if(!bkgContainer.isValid()) {
    ATH_MSG_ERROR("Could not get background CscRawDataContainer called " << bkgContainer.name() << " from store " << bkgContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found background CscRawDataContainer called " << bkgContainer.name() << " in store " << bkgContainer.store());

  SG::ReadHandle<CscRawDataContainer> signalContainer(m_signalInputKey);
  if(!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal CscRawOverlayContainer called " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal CscRawOverlayContainer called " << signalContainer.name() << " in store " << signalContainer.store());

  SG::WriteHandle<CscRawDataContainer> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<CscRawDataContainer>(bkgContainer->size())));
  if (!outputContainer.isValid()) {
    ATH_MSG_ERROR("Could not record output CscRawOverlayContainer called " << outputContainer.name() << " to store " << outputContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Recorded output CscRawOverlayContainer called " << outputContainer.name() << " in store " << outputContainer.store());

  // now do the overlay
  ATH_CHECK(overlayContainer(bkgContainer.cptr(), signalContainer.cptr(), outputContainer.ptr()));


  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode CscOverlay::overlayContainer(const CscRawDataContainer *bkgContainer,
                                        const CscRawDataContainer *signalContainer,
                                        CscRawDataContainer *outputContainer)
{
  ATH_MSG_DEBUG("overlayContainer() begin");

  // Get all the hashes for the signal and background containers
  const std::vector<IdentifierHash> bkgHashes = bkgContainer->GetAllCurrentHashes();
  const std::vector<IdentifierHash> signalHashes = signalContainer->GetAllCurrentHashes();

  // The MC signal container should typically be smaller than bkgContainer,
  // because the latter contains all the noise, minimum bias and pile up.
  // Thus we firstly iterate over signal hashes and store them in a map.
  std::map<IdentifierHash, bool> overlapMap;
  for (const IdentifierHash &hashId : signalHashes) {
    overlapMap.emplace(hashId, false);
  }

  // Now loop through the background hashes and copy unique ones over
  for (const IdentifierHash &hashId : bkgHashes) {
    auto search = overlapMap.find(hashId);
    if (search == overlapMap.end()) {
      // Copy the background collection
      std::unique_ptr<CscRawDataCollection> bkgCollection
        = copyCollection(bkgContainer->indexFindPtr(hashId));

      if (outputContainer->addCollection(bkgCollection.get(), hashId).isFailure()) {
        ATH_MSG_ERROR("Adding background Collection with hashId " << hashId << " failed");
        return StatusCode::FAILURE;
      } else {
        bkgCollection.release();
      }
    } else {
      // Flip the overlap flag
      search->second = true;
    }
  }

  // Setup random engine
  ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
  rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
  CLHEP::HepRandomEngine *rndmEngine(*rngWrapper);

  // Finally loop through the map and process the signal and overlay if
  // necessary
  for (const auto &[hashId, overlap] : overlapMap) {
    // Retrieve the signal collection
    const CscRawDataCollection *signalCollection = signalContainer->indexFindPtr(hashId);

    // Output collection
    std::unique_ptr<CscRawDataCollection> outputCollection{};

    if (overlap) { // Do overlay
      // Retrieve the background collection
      const CscRawDataCollection *bkgCollection = bkgContainer->indexFindPtr(hashId);

      // Create the output collection with background collection as a base
      // TODO: should it be signal collection?
      outputCollection = copyCollection(bkgCollection, true);

      // Merge the collections
      mergeCollections(bkgCollection, signalCollection, outputCollection.get(), rndmEngine);
    } else {
      // Create the output collection with signal collection as a base
      outputCollection = copyCollection(signalCollection, true);

      uint16_t numSamples = signalCollection->numSamples();

      for (const CscRawData *data : *signalCollection) {
        if (!data) {
          ATH_MSG_WARNING("NULL pointer to Digit!");
          continue;
        }

        // Copy the digit and add noise
        std::vector<uint16_t> samples;

        uint16_t width = data->width();
        uint32_t hashOffset = data->hashId();

        // Loop over width
        for (uint16_t j = 0; j < width; ++j) {
          uint32_t stripHash = hashOffset + j;
          double stripNoise = m_cscCalibTool->stripNoise(stripHash, false);
          // Get the samples
          std::vector<uint16_t> stripSamples;
          bool extractSamplesStatus = data->samples(j, numSamples, stripSamples);
          if (!extractSamplesStatus) {
            ATH_MSG_WARNING("Unable to extract samples for strip " << j
                            << " Online Cluster width = " << width
                            << " for number of Samples = " << numSamples
                            << " continuing ...");
          } else {
            for (uint16_t sample : stripSamples) {
              double sampleNoise = CLHEP::RandGauss::shoot(rndmEngine, 0.0, stripNoise);
              // TODO: rounding issue - some strange type conversion is going on
              // uint16_t adcCount = sample + std::lrint(sampleNoise);
              float adcCount = sample + sampleNoise;
              if (adcCount > MAX_AMPL) {
                ATH_MSG_DEBUG("value out of range (copying over signal): " << adcCount << " "
                              << " Setting it to max value = " << MAX_AMPL
                              << " IdentifierHash is " << stripHash);
                adcCount = MAX_AMPL;
              }
              // TODO: rounding issue
              // samples.push_back(adcCount);
              samples.push_back( (uint16_t) rint(adcCount) );
            }
          }
        }

        // Copy over the digit with the updated samples
        auto rdo = std::make_unique<CscRawData>(samples, data->address(), data->identify(), data->time(), data->rpuID(), data->width(), data->hashId());

        // Perform some checks
        bool good = true;
        for (uint16_t j = 0; j < width; ++j) {
          const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(rdo.get(), j);
          if (!m_cscHelper->valid(channelId)) {
            ATH_MSG_WARNING("Invalid CSC Identifier! - skipping " << channelId);
            good = false;
            break;
          }
        }
        if (good) {
          outputCollection->push_back(rdo.release());
        }
      }
    }

    if (outputContainer->addCollection(outputCollection.get(), hashId).isFailure()) {
      ATH_MSG_ERROR("Adding overlaid Collection with hashId " << hashId << " failed");
      return StatusCode::FAILURE;
    } else {
      outputCollection.release();
    }
  }

  ATH_MSG_DEBUG("overlayContainer>() end");
  return StatusCode::SUCCESS;
}

// Copy CscRawDataCollection
std::unique_ptr<CscRawDataCollection>
CscOverlay::copyCollection(const CscRawDataCollection *collection,
                           bool propertiesOnly) const
{
  auto outputCollection = std::make_unique<CscRawDataCollection>(collection->identify());

  // Copy property values to the new collection
  outputCollection->setIdentifyHash(collection->identifyHash());
  outputCollection->set_eventType(collection->eventType());
  outputCollection->setRodId(collection->rodId());
  outputCollection->setSubDetectorId(collection->subDetectorId());
  outputCollection->set_samplingPhase(collection->samplingPhase());
  outputCollection->set_triggerType(collection->triggerType());
  outputCollection->set_firstBitSummary(collection->firstBitSummary());
  outputCollection->set_scaAddress(collection->scaAddress());
  for (uint8_t dataType : collection->dataType()) {
    outputCollection->addDataType(dataType);
  }

  // Return if only properties requested
  if (propertiesOnly) {
    return outputCollection;
  }

  for (const CscRawData *existingDatum : *collection) {
    // Owned by the collection
    auto *datumCopy = new CscRawData(*existingDatum);
    outputCollection->push_back(datumCopy);
  }

  return outputCollection;
}

void CscOverlay::spuData( const CscRawDataCollection * coll, const uint16_t spuID, std::vector<const CscRawData*>& data) {
  data.clear();  if ( !coll ) return;
  CscRawDataCollection::const_iterator idata = coll->begin();
  CscRawDataCollection::const_iterator edata = coll->end();
  for ( ; idata != edata; ++idata ) {
      if ( (*idata)->rpuID() == spuID ) data.push_back( *idata );
  }
  ATH_MSG_DEBUG("spuData(): made data vector of size "<<data.size()<<" for SPU "<<spuID);
}

bool CscOverlay::needtoflip(const int address) const {
         int measuresPhi  = ( (address & 0x00000100) >>  8);
         if (address<2147483640 && measuresPhi) {
           int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
           if (stationEta>0) {       return true;          }
         }
         return false;
}


//================================================================
void CscOverlay::mergeCollections(const CscRawDataCollection *bkgCollection,
                                  const CscRawDataCollection *signalCollection,
                                  CscRawDataCollection *outputCollection,
                                  CLHEP::HepRandomEngine *rndmEngine)
{
  ATH_MSG_DEBUG("mergeCollection() begin");

  // number of ADC samples in  the both data stream
  unsigned int nSigSamples  = bkgCollection->numSamples();
  unsigned int nOvlSamples  = signalCollection->numSamples();

  // sampling times in both data streams
  unsigned int dataSamplingTime = bkgCollection->rate();
  unsigned int ovlSamplingTime  = signalCollection->rate();

  if ( dataSamplingTime != ovlSamplingTime ) {
    ATH_MSG_ERROR("Overlay of inconsistent data - sampling times not the same "
                  << dataSamplingTime << " ns " << ovlSamplingTime << " ns");
    throw std::runtime_error("mergeCollections(): sampling time mismatch");
  }

  if ( nSigSamples != nOvlSamples ) {
    ATH_MSG_ERROR("Overlay of inconsistent data - number of samples not the same "
                  << nSigSamples << " " << nOvlSamples);
    throw std::runtime_error("mergeCollections(): number of samples mismatch");
  }

  /** loop over the SPU - collecting thr data by layer
      do the overlay by igas layer in a chamber */
  uint16_t clusterCounts[] = {0,0,0,0,0,0,0,0,0,0};
  uint16_t rpuCount[] = {0,0};
  for ( uint16_t spuID=0; spuID<10; ++spuID) {

    std::vector<const CscRawData*> sigData; // real data
    this->spuData(bkgCollection, spuID, sigData);

    std::vector<const CscRawData*> ovlData; // simulation
    this->spuData(signalCollection, spuID, ovlData);

    /** for the non-precision strips, all the 4 layers in a chamber
        are in the same SPU, we need to recover the data by chamber layer */
    int layer = 0;
    if ( spuID == 4 || spuID == 9 ) layer=4;
    for ( int j=0; j<=layer; ++j ) {
       std::map< int,std::vector<uint16_t> > sigSamples;
       std::map< int,std::vector<uint16_t> > ovlSamples;
       uint32_t sigHash;
       uint32_t ovlHash;
       uint32_t sigAddress = this->stripData( sigData, nSigSamples, sigSamples, sigHash, spuID, j , true); // real data
       uint32_t ovlAddress = this->stripData( ovlData, nOvlSamples, ovlSamples, ovlHash, spuID, j , false); // simulation
       if (sigSamples.size()==0 && ovlSamples.size()==0) continue;

       uint32_t hash      = std::min( sigHash, ovlHash );
       uint32_t address   = std::min( sigAddress, ovlAddress );
       if (sigSamples.size()!=0 && ovlSamples.size()!=0 && needtoflip(address)){
         ATH_MSG_DEBUG("Looking for overlap of hashes and addresses within witdths because needtoflip");
         msg() << MSG::VERBOSE ;
         std::set<int> sig;  int lastindex=-1;
         for (std::map< int,std::vector<uint16_t> >::const_iterator si=sigSamples.begin(); si!=sigSamples.end(); ++si) {
           if (si!=sigSamples.begin() && si->first-lastindex!=1) break;
           lastindex=si->first;
           sig.insert(si->first); msg() << si->first << " ";
         }
         msg()<<endmsg;
         bool overlap=false;
         msg() <<MSG::VERBOSE ;
         for (std::map< int,std::vector<uint16_t> >::const_iterator so=ovlSamples.begin(); so!=ovlSamples.end(); ++so) {
           //add 1 to beginning and end of list because adjacent counts as overlap
           msg() << (so->first)-1 << " ";
           if (sig.find((so->first)-1)!=sig.end()) {overlap=true; msg() << "!!";}
           msg() << (so->first) << " ";
           if (sig.find((so->first))!=sig.end()) {overlap=true; msg() << "!!";}
           msg() << (so->first)+1 << " ";
           if (sig.find((so->first)+1)!=sig.end()) {overlap=true; msg() << "!!";}
         }
         msg()<<endmsg;
         if (!overlap){
           ATH_MSG_DEBUG("Taking max of hashes and addresses because needtoflip and no overlap");
           hash      = std::max( sigHash, ovlHash );
           address   = std::max( sigAddress, ovlAddress );
         }
       }

       //for checks
       std::set<int> insertedstrips, readstrips;
       for (std::map< int,std::vector<uint16_t> >::const_iterator s=sigSamples.begin(); s!=sigSamples.end(); ++s){readstrips.insert(s->first);}
       for (std::map< int,std::vector<uint16_t> >::const_iterator si=ovlSamples.begin(); si!=ovlSamples.end(); ++si){readstrips.insert(si->first);}

       std::vector<CscRawData*> datums = this->overlay(sigSamples, ovlSamples,address, spuID, outputCollection->identify(), hash, rndmEngine);
       if ( datums.size()==0 ) {         ATH_MSG_WARNING("datums is size 0!");       }
       for (unsigned int di=0; di<datums.size(); ++di){
         CscRawData* datum=datums[di];
         hash = datum->hashId();
         address = datum->address();
         int stripstart        = (  address & 0x000000FF) + 1 + 0;
         ATH_MSG_DEBUG("Datum in layer="<<j<<" has hash="<<hash<<" address="<<address<<" stripstart="<<stripstart<<", "<< *datum );
         if (datum->width()==0) {
           ATH_MSG_WARNING("Datum has 0 width!");
           continue;
         }

          //perform some checks
          int stationName =  ( ( address & 0x00010000) >> 16 ) + 50;
          int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
          int stationPhi  =  ( ( address & 0x0000E000) >> 13 ) + 1;
          Identifier me= m_cscHelper->elementID(stationName,stationEta,stationPhi);
          ATH_MSG_VERBOSE("stationName,Eta,Phi="<<stationName<<","<<stationEta<<","<<stationPhi<<" - me="<<me);
          bool good=true;
          for (unsigned int j=0; j<datum->width(); ++j) {
            int chamberLayer = ( (address & 0x00000800) >> 11) + 0;
            std::string det=m_cscCalibTool->getDetDescr();
            if ( det.find ("ATLAS-") != std::string::npos )
              chamberLayer = ( (address & 0x00000800) >> 11) + 1;
            int wireLayer    = ( (address & 0x00000600) >>  9) + 1;
            int measuresPhi  = ( (address & 0x00000100) >>  8);
            int strip        = (  address & 0x000000FF) + 1 + j;
            ATH_MSG_VERBOSE("det,chamberlayer,wirelayer,measuresphi,strip="<<det<<","<<chamberLayer<<","<<wireLayer<<","<<measuresPhi<<","<<strip);
            // Added to Online -> Offline id  in A side number is opposite bug#56002
            if (measuresPhi) {
              int stationEta  =  ( ((address & 0x00001000) >> 12 ) == 0x0) ? -1 : 1;
              if (stationEta>0) {
                strip = 49-strip;
                ATH_MSG_VERBOSE("FLIP strip, now strip="<<strip);
              }
            }
            insertedstrips.insert(strip);//for checks
            Identifier mechan= m_cscHelper->channelID(me,chamberLayer,wireLayer,measuresPhi,strip);
            ATH_MSG_VERBOSE("mechan="<<mechan);
            const Identifier channelId = m_cscRdoDecoderTool->channelIdentifier(datum, j);
            if(!(m_cscHelper->valid(channelId))) {
              ATH_MSG_WARNING("Invalid CSC Identifier in merge! - skipping " << channelId );
              good=false;
            }
          else{ATH_MSG_DEBUG("Valid CSC Identifier in merge " << channelId);}
          }
          if (good){        outputCollection->push_back(datum);   }
          else{     continue;     }

          //keep count
          clusterCounts[spuID] += 1;
          if ( spuID <= 4 ) rpuCount[0] = 5;
          else if ( spuID > 4 && spuID <= 9 ) rpuCount[1] = 11;
       }//loop over datum

       //check
       if (readstrips!=insertedstrips){
         ATH_MSG_WARNING("Readstrips != Insertedstrips: ");
         for (std::set<int>::const_iterator i = readstrips.begin(); i!=readstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
         for (std::set<int>::const_iterator i = insertedstrips.begin(); i!=insertedstrips.end(); ++i){std::cout<<*i<<" ";} std::cout<<std::endl;
       }

    }
  }
  for (unsigned int i=0; i<10; ++i) outputCollection->set_spuCount(i,clusterCounts[i]);
  for (unsigned int i=0; i<2; ++i)  { if (rpuCount[i] != 0) outputCollection->addRPU(rpuCount[i]); }
  // FIXME --- need to be able to reset the dataType - should add a new method to CscRawDataCollection for this
  ATH_MSG_DEBUG("mergeCollection<>() end ");
}

uint32_t CscOverlay::stripData ( const std::vector<const CscRawData*>& data,
                                 const unsigned int numSamples,
                                 std::map< int,std::vector<uint16_t> >& samples,
                                 uint32_t& hash,
                                 const uint16_t spuID,
                                 const int gasLayer, bool isdata)
{
  ATH_MSG_DEBUG("stripData<>() begin: gasLayer="<<gasLayer<<" spuID="<<spuID<<" isdata="<<isdata);

  samples.clear();
  IdContext context = m_cscHelper->channel_context();

  uint32_t maxInt  = 2147483640;
  uint32_t address = maxInt;
  hash             = maxInt;

  //int max = 192;
  //if ( spuID == 4 || spuID == 9 ) max = 48;

  /** loop over the data in the SPU */
  std::vector<const CscRawData*>::const_iterator idata = data.begin();
  std::vector<const CscRawData*>::const_iterator edata = data.end();
  for ( ; idata != edata; ++idata ) {
    const CscRawData * datum = *idata;
    uint32_t hashOffset = datum->hashId();

    /** find the strip Identifier given the strip hash ID */
    Identifier stripId;
    m_cscHelper->get_id(hashOffset, stripId, &context);
    unsigned int strip = static_cast<unsigned int> ( m_cscHelper->strip( stripId ) );
    int layer          = m_cscHelper->wireLayer( stripId );
    uint16_t width = datum->width();

    /** create the map only layer by layer
        for the precision strip, we set gasLayer=0 because the spuID tells you the gas layer
        for the non-precision strips, we need to explicitly get the gas layer number form the Identifier */
    bool non_precision = (gasLayer==layer) && (spuID==4 || spuID==9);
    bool precision     = (gasLayer==0) && (!(spuID==4 || spuID==9));
    bool check = precision || non_precision;
    if ( !check ) {
      //ATH_MSG_DEBUG("Not precision or non_precision, skipping layer="<<layer<<", gasLayer="<<gasLayer<<", spuID="<<spuID);
      continue;
    }

    //ACH - move down here after layer continue...
    unsigned int newaddress = datum->address();
    //if we're going to later flip the data strip for bug#56002
    if (isdata && needtoflip(newaddress)) {
      ATH_MSG_VERBOSE("needtoflip in stripdata, newaddress was = "<<newaddress<<", strip was = "<<strip);

      //old way
      //newaddress= newaddress- (width-1);//apparently need to shift the address to the highest strip

      //new way
      uint32_t oldFirstStrip  = uint32_t (newaddress & 0x000000FF);
      uint32_t newFirstStrip  = uint32_t (47-oldFirstStrip) - width +1;//starts at 0
      newaddress=newaddress - oldFirstStrip + newFirstStrip;

      uint32_t oldStrip  = uint32_t (strip & 0x000000FF);
      uint32_t newStrip  = uint32_t (49-oldStrip);//starts at 1
      strip=strip - oldStrip + newStrip;

      ATH_MSG_VERBOSE("needtoflip in stripdata, newaddress now = "<<newaddress<<", strip now = "<<strip);
    }

    if ( needtoflip(newaddress) ){
      if (hash == maxInt) hash=0;
      if (address == maxInt) address=0;
      if ( hashOffset > hash ) hash = hashOffset;
      if ( newaddress > address ) address = newaddress;
    }
    else{
      if ( hashOffset < hash ) hash = hashOffset;
      if ( newaddress < address ) address = newaddress;
    }

    ATH_MSG_DEBUG("stripData(): width="<<width<<" hashOffset="<<hashOffset<<" datumaddress="<<datum->address()<<" layer="<<layer<<" strip="<<strip<<", hash="<<hash<<" address="<<address);

    for (unsigned int j=0; j<width; ++j) {
       std::vector<uint16_t> adcs;
       bool extractSamples = datum->samples(j, numSamples, adcs);
       if ( !extractSamples ) {
         ATH_MSG_WARNING("Unable to extract samples for strip " << j
                         << " Online Cluster width = " << width << " for number of Samples = " << numSamples);
       }
       else {
         int newstrip = (strip+j);
         if (false && isdata && needtoflip(address)){
           newstrip = strip-j;
           ATH_MSG_VERBOSE("needtoflip in stripdata, newstrip is "<<newstrip);
         }
         samples.insert ( std::make_pair( newstrip, adcs) );
       }
    }
  }

  ATH_MSG_DEBUG("stripData<>() end: hash=" << hash << " address=" << address);
  return address;
}

std::vector<CscRawData*> CscOverlay::overlay( const std::map< int,std::vector<uint16_t> >& sigSamples,
                                              const std::map< int,std::vector<uint16_t> >& ovlSamples,
                                              const uint32_t address,
                                              const uint16_t spuID,
                                              const uint16_t collId,
                                              const uint32_t hash,
                                              CLHEP::HepRandomEngine *rndmEngine)
{
  ATH_MSG_DEBUG("overlay<>() begin: hash="<<hash<<" address="<<address);
  std::vector<CscRawData*> datas;
  CscRawData * rawData = nullptr;
  int max = 192;
  if ( spuID == 4 || spuID == 9 ) max = 48;
  std::vector<uint16_t> samples;
  std::map< int,std::vector<uint16_t> >::const_iterator sig;
  std::map< int,std::vector<uint16_t> >::const_iterator ovl;
  uint16_t width = 0;
  int myhash=hash; int myaddress=address;

  max+=1;//go one past the end, to get the clusters ending right
  for ( int i=1; i<=max; ++i) {
    sig = sigSamples.find(i);
    ovl = ovlSamples.find(i);
    bool used=false;

    if ( sig != sigSamples.end() && ovl == ovlSamples.end() ) { // real data only
      ATH_MSG_VERBOSE("data only for i="<<i);
      for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
        samples.push_back( (*sig).second.at(j) );
        assert((*sig).second.at(j)<=MAX_AMPL);
      }
      width++; used=true;
    }
    else if ( sig == sigSamples.end() && ovl != ovlSamples.end() ) { // simulation only
      ATH_MSG_VERBOSE("simulation only for i="<<i);
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double noise = m_cscCalibTool->stripNoise( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*ovl).second.size(); ++j ) {
          double theNoise = CLHEP::RandGauss::shoot(rndmEngine, 0.0, noise);
          float adcCount = (*ovl).second.at(j) + theNoise ;//add noise
          if ( adcCount > MAX_AMPL ) {
            ATH_MSG_DEBUG("value out of range (adding noise): " << adcCount << " "
                << " Setting it to max value = " << MAX_AMPL
                          << " IdentifierHash is " << (myhashw));
            adcCount = MAX_AMPL;
          }
          samples.push_back( (uint16_t) rint(adcCount) );
       }
       width++; used=true;
    }
    else if ( sig != sigSamples.end() && ovl != ovlSamples.end() ) { // real data + MC
      ATH_MSG_VERBOSE("data and simulation for i="<<i);
      int myhashw=myhash+width; if (needtoflip(myaddress)) {myhashw=myhash-width;}
      double pedestal = m_cscCalibTool->stripPedestal( (myhashw), false );//in ADC counts
       for ( unsigned int j=0; j<(*sig).second.size(); ++j ) {
          float adcCount = (*sig).second.at(j) + (*ovl).second.at(j) - pedestal ;//subtract pedestal only (data already has noise)
          if ( adcCount > MAX_AMPL ) {
            ATH_MSG_DEBUG("value out of range (adding data+MC samples - pedestal): " << adcCount << " "
                << " Setting it to max value = " << MAX_AMPL
                          << " IdentifierHash is " << (myhashw));
            adcCount = MAX_AMPL;
          }
          samples.push_back( (uint16_t) rint(adcCount) );
       }
       width++; used=true;
    }

    if ( used==false && datas.size()>0 ){
      if (needtoflip(myaddress)) {myhash-=1; myaddress-=1;}
      else {myhash+=1; myaddress+=1;}
    }

    //If a break is detected in the strip cluster, start a new CscRawData object...
    //and adjust the hash and address, etc.
    if ( (used==false||i==max) && samples.size()>0){
      if (datas.size()>0 && needtoflip(myaddress)) {myhash-=width; myaddress-=width;}
      rawData = new CscRawData( samples, myaddress, collId, spuID, width );
      rawData->setHashID(myhash);
      rawData->setTime(0);//ACH - TODO: should be made significantly more clever!
      datas.push_back(rawData);
      ATH_MSG_DEBUG("overlay<>() add datum: hash="<<myhash<<" address="<<myaddress<<" width="<<width);
      samples.clear();
      if (!needtoflip(myaddress)) {myhash+=width; myaddress+=width;}
      width=0;
    }

  }
  ATH_MSG_DEBUG("overlay<>() end: CscRawDatas size="<<datas.size());
  return datas;
}
