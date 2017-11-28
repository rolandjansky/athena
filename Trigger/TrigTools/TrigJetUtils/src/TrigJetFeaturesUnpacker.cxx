///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigJetFeaturesUnpacker.cxx 
// Implementation file for class TrigJetFeaturesUnpacker
/////////////////////////////////////////////////////////////////// 

// TrigJetUitls includes
#include "TrigJetFeaturesUnpacker.h"
#include "setupJetKeyDescriptor.h"
#include "JetCollectionHelper.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "AthenaKernel/errorcheck.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/JetKeyDescriptorCollection.h"
#include "JetEvent/JetMomentMapCollection.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TrigJetFeaturesUnpacker::TrigJetFeaturesUnpacker( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

}

// Destructor
///////////////
TrigJetFeaturesUnpacker::~TrigJetFeaturesUnpacker()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode TrigJetFeaturesUnpacker::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");


  return StatusCode::SUCCESS;
}

StatusCode TrigJetFeaturesUnpacker::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TrigJetFeaturesUnpacker::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  bool jkd_status = false;
  if ( evtStore()->contains<JetKeyDescriptorCollection>("HLT_TrigJetKeyMap") )  {

    const JetKeyDescriptorCollection * jetkeycoll = 0;
    CHECK( evtStore()->retrieve(jetkeycoll, "HLT_TrigJetKeyMap") );
    ATH_MSG_DEBUG(" key descriptor coll pointer: "<<MSG::hex<<jetkeycoll<<MSG::dec<<" size: "<<jetkeycoll->size()<< " own elements?: "<<(jetkeycoll->ownPolicy() == SG::OWN_ELEMENTS)); 
    jkd_status = true;
   
    if ( jetkeycoll->ownPolicy() == SG::OWN_ELEMENTS ) {  
      //this means we're reading back a RAW file
      //have to make sure the JKD will not be double-owned
      std::auto_ptr<JetKeyDescriptorCollection> jetkeycoll_pri(evtStore()->retrievePrivateCopy< JetKeyDescriptorCollection>("HLT_TrigJetKeyMap"));
      for (std::size_t j = 0; j <  jetkeycoll_pri.get()->size(); ++j) {
          if ( jetkeycoll_pri.get()->at(j) != JetKeyDescriptorInstance::instance()->getKeyStore() ) continue;
          JetKeyDescriptor * jetkey_ = 0;
          jetkeycoll_pri.get()->swapElement(j, 0, jetkey_);
          ATH_MSG_DEBUG("  "<<j<<": key descriptor swap pointer: "<<MSG::hex<<jetkey_<<MSG::dec);
          break;
      }
 

      jetkeycoll_pri.reset();  

      ATH_MSG_DEBUG("--- removed ---");
    } else { ATH_MSG_DEBUG("JKD container is a view container");  }

  } //HLT_TrigJetKeyMap in store
  else { ATH_MSG_DEBUG("HLT_TrigJetKeyMap not in store..");  }

  //register JKD in SG as 'JetKeyMap'
  CHECK( TrigJetUtils::setupJetKeyDescriptor(&(*evtStore()) , msg()) );


  //////////////////
  //now JetMomentMap
  /////////////////
  bool jmm_status =  false;
  std::size_t num_jets = 0;
  if  ( evtStore()->contains<JetMomentMapCollection>("HLT_TrigJetRecMomentMap") )  {
     const JetMomentMapCollection * mmcoll = 0;
     CHECK( evtStore()->retrieve( mmcoll, "HLT_TrigJetRecMomentMap" ) );
     jmm_status = true;    

     bool mm_own = (mmcoll->ownPolicy() == SG::OWN_ELEMENTS);
     if ( mm_own) {
        //the container own the elements, we're reading a RAW file
        std::auto_ptr<JetMomentMapCollection> jetmapcoll_pri(evtStore()->retrievePrivateCopy< JetMomentMapCollection>("HLT_TrigJetRecMomentMap"));
        ATH_MSG_DEBUG("JetMomentMapCollection size: "<<jetmapcoll_pri.get()->size()<<" own elements?: "<<(jetmapcoll_pri.get()->ownPolicy() == SG::OWN_ELEMENTS));
        JetMomentMap * jetmap_ = 0;
        num_jets = 0;
        for (std::size_t j = 0; j <  jetmapcoll_pri.get()->size(); ++j) {
            //count jets recorded in map
            const JetMomentMap * mmap = jetmapcoll_pri.get()->at(j);
            ATH_MSG_DEBUG("  "<<j<<": map  pointer: "<<MSG::hex<<mmap<<MSG::dec);
            std::size_t n_ = 0;
            while ( mmap->getRecord(n_+1) ) { n_++; }

            //release first map
            if (!jetmap_)   jetmapcoll_pri.get()->swapElement(j, 0, jetmap_);
            else {  //transfer record
                 for (std::size_t i = 0; i < n_; ++i) {
                     ATH_MSG_DEBUG("i+1="<<i+1<<"  record size: "<<mmap->getRecord(i+1)->size());
                     JetMomentMap::record_t * rec_ = new  JetMomentMap::record_t(*(mmap->getRecord(i+1)));
                     jetmap_->assignRecord(num_jets+i+1, rec_);
                     ATH_MSG_DEBUG("num_jets+i+1="<<num_jets+i+1<<"  record size: "<<jetmap_->getRecord(num_jets+i+1)->size()<<" -  "<<jetmap_->numberOfMoments(num_jets+i+1));
                 }
            } 

            num_jets += n_;
            ATH_MSG_DEBUG("Map idx "<<j<<" has "<< n_ <<" entries. "<<num_jets<<" so far");
        }
  
        jetmapcoll_pri.reset();
 
        //if no moment map, create a dummy/empty one
        if ( !jetmap_) jetmap_ = new JetMomentMap();
        CHECK( evtStore()->record(jetmap_, "TrigJetRecMomentMap",false, false) );

     } //if own_elements
     else { 
        //We have a view container, reading an RDO, TrigNavigationHolder owns the element
        ATH_MSG_DEBUG("JetMomentMapCollection size: "<<mmcoll->size()<<" own elements?: "<<mm_own);
        JetMomentMap * comb_map = new JetMomentMap();
        num_jets = 0;
        for (std::size_t j = 0; j <  mmcoll->size(); ++j) {
            //count jets recorded in map
            const JetMomentMap * mmap = mmcoll->at(j);
            ATH_MSG_DEBUG("  "<<j<<": map  pointer: "<<MSG::hex<<mmap<<MSG::dec);
            std::size_t n_ = 0;
            while ( mmap->getRecord(n_+1) ) { n_++; }
 
            //transfer record
            for (std::size_t i = 0; i < n_; ++i) {
                 ATH_MSG_DEBUG("i+1="<<i+1<<"  record size: "<<mmap->getRecord(i+1)->size());
                 JetMomentMap::record_t * rec_ = new  JetMomentMap::record_t(*(mmap->getRecord(i+1)));
                 comb_map->assignRecord(num_jets+i+1, rec_);
                 ATH_MSG_DEBUG("num_jets+i+1="<<num_jets+i+1<<"  record size: "<<comb_map->getRecord(num_jets+i+1)->size()<<" -  "<<comb_map->numberOfMoments(num_jets+i+1));
             }
            num_jets += n_;
            ATH_MSG_DEBUG("Map idx "<<j<<" has "<< n_ <<" entries. "<<num_jets<<" so far");
        }
       
        CHECK( evtStore()->record(comb_map, "TrigJetRecMomentMap",false, false) );
     } // view container

  } //HLT_TrigJetRecMomentMap in store
  else { 
     ATH_MSG_DEBUG("HLT_TrigJetRecMomentMap not in store..will create an empty JMM"); 
     JetMomentMap * jetmap_ = new JetMomentMap();
     CHECK( evtStore()->record(jetmap_, "TrigJetRecMomentMap",false, false) );
  }


  //////////////////////////////
  //now TrigJetRec JetCollection
  //////////////////////////////
  if  ( evtStore()->contains<JetCollection>("HLT_TrigJetRec") )  { 

      // load the jet container from store gate
      const JetCollection *jetContainer = 0;
      CHECK( evtStore()->retrieve( jetContainer, "HLT_TrigJetRec" ) );
      ATH_MSG_DEBUG("jet container: "<<MSG::hex<<jetContainer<<MSG::dec<<"  own elements?: "<<(jetContainer->ownPolicy() == SG::OWN_ELEMENTS));

      //sanity check
      ATH_MSG_DEBUG("numjets in map: "<<num_jets<<"  in jet coll: "<<jetContainer->size());
      if (num_jets != jetContainer->size() && (jkd_status && jmm_status)) { ATH_MSG_INFO("Number of jets inconsistent..OK if reading a rel16 RAW file"); /*return StatusCode::FAILURE;*/}

      const_cast<JetCollection *>(jetContainer)->m_keyStore = DataLink<JetKeyDescriptor>( (std::string) "JetKeyMap" );
      const_cast<JetCollection *>(jetContainer)->setAuthor("TrigJetRec");
      CHECK(JetCollectionHelper::swapMomentMap(const_cast<JetCollection *>(jetContainer)));
     
      ATH_MSG_DEBUG("jet container momentmap name: "<<jetContainer->getMomentMapName());
      ATH_MSG_DEBUG("jet container momentmap: "<<MSG::hex<<jetContainer->getMomentMap()<<MSG::dec);
      for(JetCollection::const_iterator i = jetContainer->begin(); i != jetContainer->end(); ++i) {
         ATH_MSG_DEBUG("Parent coll: "<<MSG::hex<<(*i)->parentCollection()<<MSG::dec<<" own element: "<<((*i)->parentCollection()->ownPolicy() == SG::OWN_ELEMENTS));
         ATH_MSG_DEBUG("jet author: "<<(*i)->jetAuthorAndCalibTags()<<" id: "<<(*i)->id()<<" dist: "<<std::distance(jetContainer->begin(), i));
      
         if ( (*i)->id() != (std::size_t)std::distance(jetContainer->begin(), i) + 1) {
             JetCollectionHelper::resetJetId(const_cast<JetCollection*>(jetContainer), std::distance(jetContainer->begin(), i));
             ATH_MSG_DEBUG("Resetting jet id to "<< (*i)->id());
          }
         
         ATH_MSG_DEBUG("moment map: "<<MSG::hex<<(*i)->getMomentMap()<<MSG::dec);

         if ( (*i)->parentCollection() != jetContainer ) {
              ATH_MSG_DEBUG("Remapping the JetMomentMap link");
              const_cast<JetCollection *>((*i)->parentCollection())->setAuthor("TrigJetRec");
              CHECK(JetCollectionHelper::swapMomentMap(const_cast<JetCollection *>((*i)->parentCollection())));
              ATH_MSG_DEBUG("moment map: "<<MSG::hex<<(*i)->getMomentMap()<<MSG::dec);
         }

         //save status of the feature unpacking
         float stat = (jkd_status)*100. + (jmm_status)*10. + (num_jets == jetContainer->size())*1.;
         (*i)->setMoment("TrigJetFeaturesUnpacker_Status", stat);

      } //loop over jets

  } //if HLT_TrigJetRec in SG
  else {
    ATH_MSG_WARNING("No HLT_TrigJetRec in store !?");
  }

  return StatusCode::SUCCESS;
}
