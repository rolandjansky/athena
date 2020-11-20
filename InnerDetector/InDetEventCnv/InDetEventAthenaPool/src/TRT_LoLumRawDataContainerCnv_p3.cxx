/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "InDetEventAthenaPool/InDetRawDataCollection_p1.h"
#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TRT_LoLumRawDataContainerCnv_p3.h"
#include "TRT_RDO_Elements.h"
#include "MsgUtil.h"

void TRT_LoLumRawDataContainerCnv_p3::transToPers(const TRT_RDO_Container* transCont, InDetRawDataContainer_p3* persCont, MsgStream &log)
{

  // The transient model has a container holding collections (up to 14912 TRT detector elements/modules)
  // and the collections hold channels (e.g. up to 24 straws in the TRT endcaps). There are a total of 350848 TRT straws.
  // The persistent model flattens this so that the persistent container has two vectors: 1) all collections, 2) all RDO
  // The persistent collections then only maintain indexes into the container's vector of all channels.
  // So here we loop over all collection and add their channels to the container's vector, saving the indexes in the collection.

  // Hard-code the number of collections their ids and their sizes to generate collections and channels for the whole detector
  const unsigned int dummy_digit=0;
  const unsigned int trt_number_of_collections=14912;
  const unsigned int trt_number_of_channels=350848;
  const unsigned int trt_channel_id_increment=0x20u;

  std::vector<unsigned int> trt_collection_id;   // There are 14912 elements/modules in the TRT
  std::vector<unsigned int> trt_collection_size; // Each element has a specific number of straws

  set_vectors_of_collection_ids_and_size(trt_collection_id,
                                         trt_collection_size,
                                         trt_number_of_collections);

  unsigned int trt_collection_index=0; // module index (0,14911), initialized first collection
  unsigned int trt_channel_index=0;    // straw index (0,350847)
  unsigned int trt_channel_id;         // straw id (32-bit), initialized to the first channel in the first collection
  unsigned int tcoll_id;               // transient collection id from transCont
  unsigned int tchan_id;               // transient channel id from transCont
  unsigned int tchan_word;             // transient channel word from transCont

  typedef TRT_RDO_Container TRANS;
  TRANS::const_iterator it_transColl     = transCont->begin(); // The transient container has an incomplete list of collections
  TRANS::const_iterator it_transCollEnd  = transCont->end();   // and channels, we can access them with this iterator

  persCont->m_collections.resize(trt_number_of_collections);
  persCont->m_rawdata.resize(trt_number_of_channels);

  // Loop over all existing transient collections, add missing collections and missing channels
  for (; it_transColl != it_transCollEnd; it_transColl++) {

    const TRT_RDO_Collection& collection = (**it_transColl);
    tcoll_id = collection.identify().get_identifier32().get_compact();

    // create collections and channels until we find a match to a transient container
    while ( trt_collection_id[trt_collection_index] != tcoll_id ) {
      // The transient collection is missing; create the persistent collection with empty digits
      trt_channel_id = trt_collection_id[trt_collection_index];
      InDetRawDataCollection_p1& pcollection = persCont->m_collections[trt_collection_index];
      pcollection.m_id     = 0;
      pcollection.m_hashId = 0; // no hashId for this empty collection
      pcollection.m_begin  = 0;
      pcollection.m_end    = 0;
      // Fill all the channels with with empty digits.
      for (unsigned int i=0; i<trt_collection_size[trt_collection_index]; ++i) {
        persCont->m_rawdata[trt_channel_index] = dummy_digit;
        trt_channel_id += trt_channel_id_increment;
        trt_channel_index++;
      }
      trt_collection_index++;
    }

    // Here we have a match to a transient collection
    // create the persistent collection just as we did above
    trt_channel_id = trt_collection_id[trt_collection_index];
    InDetRawDataCollection_p1& pcollection = persCont->m_collections[trt_collection_index];
    pcollection.m_id     = 0;
    pcollection.m_hashId = (unsigned int) collection.identifyHash(); // Required by the overlay alogrithm (not the reco algorithms)
    pcollection.m_begin  = 0;
    pcollection.m_end    = 0;

    const unsigned int collection_end = trt_channel_index+trt_collection_size[trt_collection_index];

    // Fill all the channels; some with existing digits, the others with empty digits.
    for (unsigned int i = 0; i < collection.size(); ++i) {

      // Get the channel id and word from the digitization transient object
      // We will use the id to determine when to insert missing straws to create a container that represents the complete detector.
      // We will only store the digit word.
      const TRT_LoLumRawData* tchan0 = dynamic_cast<const TRT_LoLumRawData*>(collection[i]);
      tchan_id   = tchan0->identify().get_identifier32().get_compact(); // the i'th channel id in the transient collection
      tchan_word = tchan0->getWord();                                   // the i'th channel word in the transient collection

      while ( trt_channel_id != tchan_id) {
        persCont->m_rawdata[trt_channel_index] = dummy_digit;
        trt_channel_id += trt_channel_id_increment;
        trt_channel_index++;
      }

      // Here we have a matching transient channel; write it.
      persCont->m_rawdata[trt_channel_index]=tchan_word;
      trt_channel_id += trt_channel_id_increment;
      trt_channel_index++;

    } // end transient channel loop

    // write all other missing channels
    while (trt_channel_index != collection_end) {
      persCont->m_rawdata[trt_channel_index] = dummy_digit;
      trt_channel_id += trt_channel_id_increment;
      trt_channel_index++;
    }

    // increment to the next collection
    trt_collection_index++;

  } // end transient collection loop

  // Finally, create any remaining missing collections
    while ( trt_collection_index < trt_number_of_collections ) {
      // The transient collection is missing; create the persistent collection with empty digits
      trt_channel_id = trt_collection_id[trt_collection_index];
      InDetRawDataCollection_p1& pcollection = persCont->m_collections[trt_collection_index];
      pcollection.m_id     = 0;
      pcollection.m_hashId = 0; // no hashId for this empty collection
      pcollection.m_begin  = 0;
      pcollection.m_end    = 0;

      // Fill all the channels with with empty digits.
      for (unsigned int i=0; i<trt_collection_size[trt_collection_index]; ++i) {
        persCont->m_rawdata[trt_channel_index] = dummy_digit;
        trt_channel_id += trt_channel_id_increment;
        trt_channel_index++;
      }
      trt_collection_index++;
    }

  MSG_DEBUG(log," Prepared " << persCont->m_collections.size() << "Collections");
  MSG_DEBUG(log," ***  Writing TRT_RDO_Container (TRT_LoLumRawData concrete type)");

  trt_collection_id.clear();
  trt_collection_size.clear();

} // end of transToPers


void  TRT_LoLumRawDataContainerCnv_p3::persToTrans(const InDetRawDataContainer_p3* persCont, TRT_RDO_Container* transCont, MsgStream &log)
{

  // The transient model has a container holding collections (up to 14912 TRT detector elements/modules)
  // and the collections hold channels (e.g. up to 24 straws in the TRT endcaps). There are a total of 350848 TRT straws.
  // The persistent model flattens this so that the persistent container has two vectors: 1) all collections, 2) all RDO
  // The persistent collections then only maintain indexes into the container's vector of all channels.
  // So here we loop over all collection and extract their channels from the vector.

  // Hard-code the number of collections, their ids and their sizes to generate collections and channels for the whole detector
  const unsigned int dummy_digit=0;
  const unsigned int trt_number_of_collections=14912;
  const unsigned int trt_channel_id_increment=0x20u;
  std::vector<unsigned int> trt_collection_id;   // There are 14912 elements/modules in the TRT
  std::vector<unsigned int> trt_collection_size; // Each element has a specific number of straws

  set_vectors_of_collection_ids_and_size(trt_collection_id,
                                         trt_collection_size,
                                         trt_number_of_collections);

  unsigned int trt_channel_id;
  unsigned int trt_channel_index=0;
  unsigned int trt_channel_index_old;
  unsigned int total_number_of_channels=0;

  TRT_RDO_Collection* tcoll=0;         // transient collection to be constructed

  MSG_DEBUG(log," Reading " << persCont->m_collections.size() << "Collections");
  if (persCont->m_collections.size() != trt_number_of_collections)
    log << MSG::ERROR << "TRT_LoLumRawDataContainerCnv_p3::persToTrans expected 14912 collections but got " << persCont->m_collections.size() << ". We should be reading the whole detector!" << endmsg;

  for (unsigned int trt_collection_index=0; trt_collection_index<trt_number_of_collections; ++trt_collection_index) {

    // Create trans collection - in NOT owner of TRT_RDO_RawData (SG::VIEW_ELEMENTS); IDet collection don't have the Ownership policy c'tor

    // count the number of non-dummy digits and skip to the next persistent collection if there are none.
    unsigned int nchans = trt_collection_size[trt_collection_index];
    trt_channel_index_old = trt_channel_index; // the beginning of this collection
    unsigned int mchans = 0;
    for (unsigned int ichan = 0; ichan < nchans; ++ichan) {
      const unsigned int pword = persCont->m_rawdata[trt_channel_index];
      if ( pword != dummy_digit ) mchans++;
      trt_channel_index++;
    }
    if (!mchans) continue;

    // This collection has "mchans" digits
    total_number_of_channels += mchans;

    // Create the transeint collection and fill with channels
    tcoll = new TRT_RDO_Collection(IdentifierHash(trt_collection_index));
    tcoll->setIdentifier(Identifier(trt_collection_id[trt_collection_index]));
    tcoll->resize(mchans);

    trt_channel_id = trt_collection_id[trt_collection_index]; // the id of the first channel in this collection
    trt_channel_index = trt_channel_index_old; // go back to the beginning of this collection and process it again

    unsigned int jchan=0; // counter for the non-dummy digits
    for (unsigned int ichan = 0; ichan < nchans; ++ichan) {
      const unsigned int pword = persCont->m_rawdata[trt_channel_index];
      if ( pword == dummy_digit ) {
        // advance to the next straw
        trt_channel_id += trt_channel_id_increment;
        trt_channel_index++;
        continue; // don't write a dummy digit
      }

      TRT_LoLumRawData* tchan = new TRT_LoLumRawData(Identifier(trt_channel_id),pword);

      (*tcoll)[jchan] = tchan;
      jchan++;

      // advance to the next straw
      trt_channel_id += trt_channel_id_increment;
      trt_channel_index++;
    }

    // register the rdo collection in IDC with hash - faster addCollection
    StatusCode sc = transCont->addCollection(tcoll, IdentifierHash(trt_collection_index));
    if (sc.isFailure()) throw std::runtime_error("Failed to add collection to ID Container");
    MSG_VERBOSE(log,"AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id / coll_id = " << trt_collection_index << " / " << trt_collection_id[trt_collection_index] << ", added to Identifiable container.");
  }

  trt_collection_id.clear();
  trt_collection_size.clear();

  MSG_DEBUG(log," ***  Reading TRT_RDO_Container (TRT_LoLumRawData concrete type)");

} // end of persToTrans

//================================================================
TRT_RDO_Container* TRT_LoLumRawDataContainerCnv_p3::createTransient(const InDetRawDataContainer_p3* persObj, MsgStream& log) {
  std::unique_ptr<TRT_RDO_Container> trans(std::make_unique<TRT_RDO_Container>(m_trtId->straw_layer_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
