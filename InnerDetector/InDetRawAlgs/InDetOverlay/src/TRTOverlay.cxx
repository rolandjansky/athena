/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "HepMC/GenParticle.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetOverlay/TRTOverlay.h"
#include "InDetRawData/InDetRawData.h"
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_RDORawData.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "IDC_OverlayBase/IDC_OverlayHelpers.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

namespace Overlay
{
  // Specialize mergeChannelData() for the TRT
  template<> void mergeChannelData(TRT_RDORawData &r1,
                                   const TRT_RDORawData &r2,
                                   TRTOverlay *)
  {
    // ----------------------------------------------------------------
    // FIXME: That should really be a call to r1.merge(r2);

    TRT_LoLumRawData *pr1 = dynamic_cast<TRT_LoLumRawData*>(&r1);
    const TRT_LoLumRawData *pr2 = dynamic_cast<const TRT_LoLumRawData*>(&r2);

    if (pr1 && pr2) {
      // the actual merging
      pr1->merge(*pr2);
    }
  } // mergeChannelData()

  // Specialize copyCollection() for the TRT
  template<>
  std::unique_ptr<TRT_RDO_Collection> copyCollection(const IdentifierHash &hashId,
                                                     const TRT_RDO_Collection *collection)
  {
    auto outputCollection = std::make_unique<TRT_RDO_Collection>(hashId);
    if (!collection) {
      return outputCollection;
    }

    outputCollection->setIdentifier(collection->identify());

    for (const TRT_RDORawData *existingDatum : *collection) {
      // Owned by the collection
      auto *datumCopy = new TRT_LoLumRawData(existingDatum->identify(), existingDatum->getWord());
      outputCollection->push_back(datumCopy);
    }

    return outputCollection;
  }
} // namespace Overlay


TRTOverlay::TRTOverlay(const std::string &name, ISvcLocator *pSvcLocator)
  : IDC_OverlayBase(name, pSvcLocator)
{
  declareProperty("TRT_HT_OccupancyCorrectionBarrel", m_HTOccupancyCorrectionB=0.110);
  declareProperty("TRT_HT_OccupancyCorrectionEndcap", m_HTOccupancyCorrectionEC=0.090);
  declareProperty("TRT_HT_OccupancyCorrectionBarrelNoE", m_HTOccupancyCorrectionB_noE=0.060);
  declareProperty("TRT_HT_OccupancyCorrectionEndcapNoE", m_HTOccupancyCorrectionEC_noE=0.050);
}

StatusCode TRTOverlay::initialize()
{
  ATH_MSG_DEBUG("Initializing...");

  if (!m_includeBkg) {
    ATH_MSG_DEBUG("Disabling use of background RDOs...");
    ATH_CHECK( m_bkgInputKey.assign("") );
  }

  // Check and initialize keys
  ATH_CHECK( m_bkgInputKey.initialize(!m_bkgInputKey.key().empty()) );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_bkgInputKey);
  ATH_CHECK( m_signalInputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey: " << m_signalInputKey);
  ATH_CHECK( m_outputKey.initialize() );
  ATH_MSG_VERBOSE("Initialized WriteHandleKey: " << m_outputKey);
  ATH_CHECK( m_signalInputSDOKey.initialize() );
  ATH_MSG_VERBOSE("Initialized ReadHandleKey for SDO: " << m_signalInputSDOKey);

  // Retrieve TRT ID helper
  if (!detStore()->retrieve(m_trtId, "TRT_ID").isSuccess() || !m_trtId) {
    ATH_MSG_FATAL("Cannot retrieve TRT ID helper");
    return StatusCode::FAILURE;
  }

  // Initialize random number generator
  CHECK(m_rndmSvc.retrieve());

  // Retrieve TRT local occupancy tool
  CHECK(m_TRT_LocalOccupancyTool.retrieve());

  if (!m_TRTStrawSummarySvc.empty() && m_TRTStrawSummarySvc.retrieve().isFailure() ) {
    ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
    ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
    return StatusCode::FAILURE;
  } else {
    if ( !m_TRTStrawSummarySvc.empty()) 
      ATH_MSG_INFO( "Retrieved tool " << m_TRTStrawSummarySvc );
  }



  return StatusCode::SUCCESS;
}

StatusCode TRTOverlay::execute() {
  ATH_MSG_DEBUG("execute() begin");

  // Reading the input RDOs
  ATH_MSG_VERBOSE("Retrieving input RDO containers");

  const TRT_RDO_Container *bkgContainerPtr = nullptr;
  if (m_includeBkg) {
    SG::ReadHandle<TRT_RDO_Container> bkgContainer(m_bkgInputKey);
    if (!bkgContainer.isValid()) {
      ATH_MSG_ERROR("Could not get background TRT RDO container " << bkgContainer.name() << " from store " << bkgContainer.store());
      return StatusCode::FAILURE;
    }
    bkgContainerPtr = bkgContainer.cptr();

    ATH_MSG_DEBUG("Found background TRT RDO container " << bkgContainer.name() << " in store " << bkgContainer.store());
    ATH_MSG_DEBUG("TRT Background = " << Overlay::debugPrint(bkgContainer.cptr()));
  }

  SG::ReadHandle<TRT_RDO_Container> signalContainer(m_signalInputKey);
  if (!signalContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal TRT RDO container " << signalContainer.name() << " from store " << signalContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal TRT RDO container " << signalContainer.name() << " in store " << signalContainer.store());
  ATH_MSG_DEBUG("TRT Signal     = " << Overlay::debugPrint(signalContainer.cptr()));

  SG::ReadHandle<InDetSimDataCollection> signalSDOContainer(m_signalInputSDOKey);
  if (!signalSDOContainer.isValid()) {
    ATH_MSG_ERROR("Could not get signal TRT SDO map container " << signalSDOContainer.name() << " from store " << signalSDOContainer.store());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Found signal TRT SDO map container " << signalSDOContainer.name() << " in store " << signalSDOContainer.store());

  // Creating output RDO container
  SG::WriteHandle<TRT_RDO_Container> outputContainer(m_outputKey);
  ATH_CHECK(outputContainer.record(std::make_unique<TRT_RDO_Container>(signalContainer->size())));
  ATH_MSG_DEBUG("Recorded output TRT RDO container " << outputContainer.name() << " in store " << outputContainer.store());

  if (outputContainer.isValid()) {
    // Calculate occupancy here
    if (m_includeBkg) {
      std::map<int, double> occupancy = m_TRT_LocalOccupancyTool->getDetectorOccupancy(bkgContainerPtr);

      //Merge containers
      overlayTRTContainers(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr(), occupancy, *signalSDOContainer);
    } else {
      ATH_CHECK(overlayContainer(bkgContainerPtr, signalContainer.cptr(), outputContainer.ptr()));
    }

    ATH_MSG_DEBUG("TRT Result   = " << Overlay::debugPrint(outputContainer.ptr()));
  }

  ATH_MSG_DEBUG("execute() end");
  return StatusCode::SUCCESS;
}

void TRTOverlay::overlayTRTContainers(const TRT_RDO_Container *bkgContainer,
                                      const TRT_RDO_Container *signalContainer,
                                      TRT_RDO_Container *outputContainer,
                                      std::map<int, double> &occupancyMap,
                                      const InDetSimDataCollection &SDO_Map)
{
   /** Add data from the background container to the output one */
   if (bkgContainer) {
     TRT_RDO_Container::const_iterator p_bkg = bkgContainer->begin();
     TRT_RDO_Container::const_iterator p_bkg_end = bkgContainer->end();

     for(; p_bkg != p_bkg_end; ++p_bkg) {
       IdentifierHash hashId = p_bkg.hashId();
       auto coll_bkg = Overlay::copyCollection(hashId, *p_bkg);

       if (outputContainer->addCollection(coll_bkg.release(), p_bkg.hashId() ).isFailure()) {
         ATH_MSG_WARNING("add background Collection failed for output " << p_bkg.hashId());
       }
     }
   }

   ATHRNG::RNGWrapper* rngWrapper = m_rndmSvc->getEngine(this);
   rngWrapper->setSeed( name(), Gaudi::Hive::currentContext() );
   CLHEP::HepRandomEngine *rndmEngine(*rngWrapper);

   /** Add data from the signal container to the output one */
   TRT_RDO_Container::const_iterator p_signal = signalContainer->begin();
   TRT_RDO_Container::const_iterator p_signal_end = signalContainer->end();

   for (; p_signal != p_signal_end; ++p_signal) {

      IdentifierHash coll_id = p_signal.hashId();
      auto coll_signal = Overlay::copyCollection( coll_id, *p_signal ) ;

      /** The newly created stuff will go to the output EventStore SG */
      auto coll_out = std::make_unique<TRT_RDO_Collection>(coll_id);
      coll_out->setIdentifier((*p_signal)->identify());

      /** Look for the same ID in the main StoreGate EventStore */
      auto q = outputContainer->indexFind( coll_id );
      if ( q != outputContainer->end() ) {
      /**Need to merge the collections
         Retrieve q */
         std::unique_ptr <TRT_RDO_Collection> coll_bkg ((TRT_RDO_Collection *) *q);
         int det =  m_trtId->barrel_ec( (*p_signal)->identify() );
         mergeTRTCollections(coll_bkg.get(), coll_signal.get(), coll_out.get(), occupancyMap[det], SDO_Map, rndmEngine);

         outputContainer->removeCollection(p_signal.hashId());
         if (outputContainer->addCollection(coll_out.release(), coll_id).isFailure() ) {
            ATH_MSG_WARNING( "overlay addCollection failed " );
         }
      }
      else {
       /** Copy the complete collection from signal to output,
           hopefully preserving the "most derived" type of its raw data */
       if ( outputContainer->addCollection(coll_signal.release(), coll_id).isFailure() ) {
          ATH_MSG_WARNING("add signal Collection failed for output " << coll_id);
      }
    }
  }
}

void TRTOverlay::mergeTRTCollections(TRT_RDO_Collection *bkgCollection,
                                     TRT_RDO_Collection *signalCollection,
                                     TRT_RDO_Collection *outputCollection,
                                     double occupancy,
                                     const InDetSimDataCollection& SDO_Map,
                                     CLHEP::HepRandomEngine* rndmEngine)
{

  if (bkgCollection->identify() != signalCollection->identify()) {
    std::ostringstream os;
    os << "mergeTRTCollections<generic>(): collection Id mismatch";
    ATH_MSG_FATAL(os.str());
    throw std::runtime_error(os.str());
  }

  const Identifier idColl = bkgCollection->identify();

  // ----------------------------------------------------------------
  // debug
  static bool first_time = true;
  if (first_time) {
    first_time = false;
    ATH_MSG_INFO( "mergeTRTCollections():  code is called ");
  }

  // ----------------------------------------------------------------

  TRT_RDO_Collection bkg(bkgCollection->identifyHash());
  bkg.setIdentifier(idColl);
  bkgCollection->swap(bkg);

  TRT_RDO_Collection sig(signalCollection->identifyHash());
  sig.setIdentifier(idColl);
  signalCollection->swap(sig);

  //################################################################
  // Merge by copying ptrs from background and signal to signalCollection

  unsigned int ibkg = 0;
  unsigned int isig = 0;

  while( (ibkg < bkg.size()) || (isig < sig.size())) {

    // The RDO that goes to the output at the end of this step.
    TRT_RDORawData *p_rdo(0);

    if(isig == sig.size()) {
      // just copy the remaining data inputs
      bkg.swapElement(ibkg++, 0, p_rdo);
    }
    else if(ibkg == bkg.size()) {
      //just copy the remaining MC digits
      sig.swapElement(isig++, 0, p_rdo);
    }
    else {
      // Need to decide which one goes first.
      // See comments in TRTDigitization.cxx about the assumption that id1<id2 <=> hash1<hash2
      if( sig[isig]->identify() < bkg[ibkg]->identify() ) {
        sig.swapElement(isig++, 0, p_rdo);
      }
      else if(bkg[ibkg]->identify() < sig[isig]->identify()) {
        bkg.swapElement(ibkg++, 0, p_rdo);
      }
      else {
        // The hits are on the same channel.
        TRT_RDORawData *p2(0);
        bkg.swapElement(ibkg++, 0, p2);
        sig.swapElement(isig++, 0, p_rdo);

        TRT_LoLumRawData *pr1 = dynamic_cast<TRT_LoLumRawData*>(p_rdo);
        const TRT_LoLumRawData *pr2 = dynamic_cast<const TRT_LoLumRawData*>(p2);

        if(pr1 && pr2) {
          // the actual merging
          pr1->merge(*pr2);

          //If the hit is not already a high level hit
          if( !(pr1->getWord() & 0x04020100) ) {

            //Determine if the hit is from an electron or not
            bool isElectron = false;
            Identifier rdoId = p_rdo->identify();
            InDetSimDataCollection::const_iterator sdoIter(SDO_Map.find(rdoId));
            if( sdoIter != SDO_Map.end() ){
              const std::vector< InDetSimData::Deposit >& deposits = sdoIter->second.getdeposits();
              for ( const auto& deposit: deposits ){
                const auto& particleLink = deposit.first;
                if( particleLink.isValid() ){
                  if( abs( particleLink->pdg_id() ) == 11 ){
                    isElectron = true;
                  }
                }
              }
            }

            // Determine what type of straw was hit
            bool isXenonStraw = false;
            if (!m_TRTStrawSummarySvc.empty()) {
              if (m_TRTStrawSummarySvc->getStatusHT(rdoId) == TRTCond::StrawStatus::Good) {
                isXenonStraw = true;
              }
            }



            unsigned int newword = 0;
            //Get random number
            int det =  m_trtId->barrel_ec( pr1->identify() );
            float HTOccupancyCorrection = 0;
            if(isElectron){
              HTOccupancyCorrection = abs(det) > 1 ? m_HTOccupancyCorrectionEC : m_HTOccupancyCorrectionB;
            } else {
              HTOccupancyCorrection = abs(det) > 1 ? m_HTOccupancyCorrectionEC_noE : m_HTOccupancyCorrectionB_noE;
            }

            if( isXenonStraw && occupancy * HTOccupancyCorrection > CLHEP::RandFlat::shoot( rndmEngine, 0, 1) )
              newword += 1 << (26-9);
            //
            TRT_LoLumRawData newrdo( pr1->identify(), newword);
            pr1->merge(newrdo);
          }
        } else {
          ATH_MSG_WARNING("TRT RDO is the wrong format");
        }

        Overlay::mergeChannelData(*p_rdo, *p2, this);
        delete p2;
      }
    }

    outputCollection->push_back(p_rdo);
  } // <= while
}
