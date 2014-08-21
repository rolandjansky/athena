/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//=== IOVDbTest includes
#include "RegistrationServices/IIOVRegistrationSvc.h"

//=== Event Info 
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"

//=== AttributeList
//#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

//=== Tile includes
#include "TileCalibAlgs/TileOFC2DBAlg.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
#include "TileCalibBlobObjs/TileCalibDrawerCmt.h"


TileOFC2DBAlg::TileOFC2DBAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_regSvc(0)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfc("TileCondToolOfc")
{
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfc",    m_tileCondToolOfc  ,"TileCondToolOfc");
  declareProperty("OF2",                m_of2            = true,"true => OF2, false => OF1");
  declareProperty("RunIOVSince",        m_runIOVSince    = IOVTime::MINRUN );
  declareProperty("RunIOVUntil",        m_runIOVUntil    = IOVTime::MAXRUN );
  declareProperty("LbnIOVSince",        m_lbnIOVSince    = IOVTime::MINEVENT );
  declareProperty("LbnIOVUntil",        m_lbnIOVUntil    = IOVTime::MAXEVENT );
  declareProperty("FixedPhases",        m_fixedPhases    = true, "calculate OFCs for 2001 phases" );
  declareProperty("RunType",            m_runType        = "PHY", "PHY or LAS");
}
  
TileOFC2DBAlg::~TileOFC2DBAlg() {
} 



//____________________________________________________________________________
StatusCode TileOFC2DBAlg::initialize() {

  ATH_MSG_INFO( "Initializing for " << m_runType );

  //=== get TileToolTiming
  CHECK( m_tileToolTiming.retrieve() );

  //=== get TileCondToolOfc
  CHECK( m_tileCondToolOfc.retrieve() );

  //=== IOVRegistrationSvc
  CHECK( service("IOVRegistrationSvc", m_regSvc) );

  ATH_MSG_DEBUG( "Found IOVRegistrationSvc " );

  return StatusCode::SUCCESS;
}

//
//_________________________________________________________________________  
StatusCode TileOFC2DBAlg::execute() {

  //=== print run/evt/lbn/time info for each event
  const xAOD::EventInfo* eventInfo(0);
  evtStore()->retrieve(eventInfo);
  ATH_MSG_DEBUG( "Event: ["
                << eventInfo->runNumber() << ","
                << eventInfo->eventNumber() << ":"
                << eventInfo->timeStamp() << "]" );


  //=== Create conditions objects only at run == ?, event == ?
  if (1 != eventInfo->eventNumber()) {
    ATH_MSG_DEBUG( "Event NOT selected for creating conditions objects " );
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG( "Creating conditions objects " );
  }

  unsigned int objVersion = (m_of2) ? 3 : 1;

  //=== build the IOV range
  IOVRange range(IOVTime(m_runIOVSince, m_lbnIOVSince), IOVTime(m_runIOVUntil, m_lbnIOVUntil));

  //=== build COOL folder name based on ???????
  std::string folder("/TILE/ONL01/FILTER/");
  if (m_of2) folder += "OF2/" + m_runType;
  else       folder += "OF1/" + m_runType;
  m_folders.insert(folder);

  //=== create the folder layout
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("TileCalibBlobOfc", "blob");

  //=== create the collection of attribute lists
  CondAttrListCollection* attrListColl = new CondAttrListCollection(true);

  const TileOfcWeightsStruct* m_weights = m_tileCondToolOfc->getOfcWeights(0, 0, 0, 0, true);
  int ndig = m_weights->n_samples;

  // ---------- create fixed phases
  if (m_fixedPhases) {
    ATH_MSG_DEBUG( "m_fixedPhases " << m_fixedPhases );

    unsigned int channel = 0;
    std::vector<int> phases(N_FIXED_PHASES * TileCalibUtils::MAX_GAIN + 1);
    unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(0, 0);

    //=== create attribute list
    coral::AttributeList ofcList(*spec);
    coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();

    //=== create an OFC blob interpreter
    TileCalibDrawerOfc* drawerOfc = TileCalibDrawerOfc::getInstance(blob, objVersion, ndig, -(N_FIXED_PHASES * TileCalibUtils::MAX_GAIN + 1), 1, TileCalibUtils::MAX_GAIN); // nPhases, nChann, nGains

    for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
      for (int phase = -N_FIXED_PHASES; phase <= N_FIXED_PHASES; ++phase) {
        phases.push_back(phase);
      }
    }

    for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
      drawerOfc->setPhases(0, gain, phases);
      for (int phase = -N_FIXED_PHASES; phase <= N_FIXED_PHASES; ++phase) {
        float fphase = phase * PHASE_STEP;
        m_weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, fphase, m_of2);
        for (int isam = 0; isam < ndig; isam++) {
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldA, 0, gain, phase, isam, m_weights->w_a[isam]);
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldB, 0, gain, phase, isam, m_weights->w_b[isam]);
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldG, 0, gain, phase, isam, m_weights->g[isam]);
          if (m_of2) drawerOfc->setOfc(TileCalibDrawerOfc::FieldC, 0, gain, phase, isam, m_weights->w_c[isam]);
          drawerOfc->setOfc(TileCalibDrawerOfc::FieldDG, 0, gain, phase, isam, m_weights->dg[isam]);
        }


        if (msgLvl(MSG::DEBUG)) {

          msg(MSG::DEBUG) << " N Samples " << ndig
                          << " channel " << channel
                          << " drawerIdx " << drawerIdx
                          << " gain " << gain
                          << " phase = " << phase << endmsg;

          msg(MSG::DEBUG) << "gain " << gain << " w_a, phase " << phase << " ";
          for (int isam = 0; isam < ndig; ++isam)
            msg(MSG::DEBUG) << " " << m_weights->w_a[isam];
          msg(MSG::DEBUG) << endmsg;

          msg(MSG::DEBUG) << "gain " << gain << " w_b, phase " << phase << " ";
          for (int isam = 0; isam < ndig; isam++)
            msg(MSG::DEBUG) << " " << m_weights->w_b[isam];
          msg(MSG::DEBUG) << endmsg;

          if (m_of2) {
            msg(MSG::DEBUG) << "gain " << gain << " w_c, phase " << phase << " ";
            for (int isam = 0; isam < ndig; isam++)
              msg(MSG::DEBUG) << " " << m_weights->w_c[isam];
            msg(MSG::DEBUG) << endmsg;
          }

          msg(MSG::DEBUG) << "gain " << gain << " g, phase " << phase << " ";
          for (int isam = 0; isam < ndig; isam++)
            msg(MSG::DEBUG) << " " << m_weights->g[isam];
          msg(MSG::DEBUG) << endmsg;

        }

      }
    }

    //=== use DrawerHash as channel number
    attrListColl->add(drawerIdx, ofcList);

    //=== add the IOV range for this collection
    attrListColl->add(drawerIdx, range);
    //    drawerOfc->dump();
    delete drawerOfc;

    //=== Create empty blobs for all COOL channels
    for (int cchan = 1; cchan < 276; cchan++) {
      //=== create attribute list for this drawer
      coral::AttributeList ofcList(*spec);
      //coral::Blob& blob=ofcList["TileCalibBlobOfc"].data<coral::Blob>();
      attrListColl->add(cchan, ofcList);
      attrListColl->add(cchan, range);
    }

  } else { // take best phase from DB and calculate OFCs for each channel

    std::vector<int> phases(TileCalibUtils::MAX_CHAN);

    //=== create attribute list
    coral::AttributeList ofcList(*spec);
    coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();

    //=== create an OFC blob interpreter
    TileCalibDrawerOfc* drawerOfc = TileCalibDrawerOfc::getInstance(blob, objVersion, ndig, -48, 256, 2); // nPhases, nChann, nGains

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
      phases[channel] = channel;

    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::getMaxDrawer(ros); ++drawer) {
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
        drawerOfc->setPhases(drawerIdx, 0, phases);
        drawerOfc->setPhases(drawerIdx, 1, phases);

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

          for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {

            float phase = m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
            ATH_MSG_DEBUG( "m_tileToolTiming:"
                           << " drawerIdx " << drawerIdx
                           << " channel " << channel
                           << " gain " << gain
                           << " phase " << phase );

            m_weights = m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, phase, m_of2);
            ATH_MSG_DEBUG( " N Samples " << ndig
                           << " ros " << ros
                           << " drawer " << drawer
                           << " channel " << channel
                           << " drawerIdx " << drawerIdx
                           << " gain " << gain
                           << " phase = " << phase );

            double w_a_sum = 0, w_b_sum = 0, w_c_sum = 0;
            double w_ag_sum = 0, w_bg_sum = 0;
            for (int isam = 0; isam < ndig; isam++) {
              if (m_of2) {
                ATH_MSG_DEBUG( " sampling " << isam
                               << " w_a " << m_weights->w_a[isam]
                               << " w_b " << m_weights->w_b[isam]
                               << " w_c " << m_weights->w_c[isam]
                               << "   g " << m_weights->g[isam] );

              } else {
                ATH_MSG_DEBUG( " sampling " << isam
                              << " w_a " << m_weights->w_a[isam]
                              << " w_b " << m_weights->w_b[isam]
                              << "   g " << m_weights->g[isam] );

              }

              w_a_sum += m_weights->w_a[isam];
              w_b_sum += m_weights->w_b[isam];
              w_c_sum += m_weights->w_c[isam];
              w_ag_sum += m_weights->w_a[isam] * m_weights->g[isam];
              w_bg_sum += m_weights->w_b[isam] * m_weights->g[isam];

              drawerOfc->setOfc(0, drawerIdx, gain, channel, isam, m_weights->w_a[isam]);
              drawerOfc->setOfc(1, drawerIdx, gain, channel, isam, m_weights->w_b[isam]);
              if (m_of2) {
                drawerOfc->setOfc(2, drawerIdx, gain, channel, isam, m_weights->w_c[isam]);
                drawerOfc->setOfc(3, drawerIdx, gain, channel, isam, m_weights->g[isam]);
              } else {
                drawerOfc->setOfc(2, drawerIdx, gain, channel, isam, m_weights->g[isam]);
              }
            }

            if (m_of2) ATH_MSG_DEBUG(  " *** SUM: a,b,c " << w_a_sum << " " << w_b_sum << " " << w_c_sum );

          } // for ... gain
        } // for ... channel

        //=== use DrawerHash as channel number
        attrListColl->add(drawerIdx, ofcList);

        //=== add the IOV range for this collection
        attrListColl->add(drawerIdx, range);
        //      drawerOfc->dump();
      } // for ... drawer
    } // for ... ros
    delete drawerOfc;
  }

  //=== add the comment in the comment channel
  coral::AttributeList attrList(*spec);
  coral::Blob& blob = attrList["TileCalibBlobOfc"].data<coral::Blob>();
  TileCalibDrawerCmt* comment = TileCalibDrawerCmt::getInstance(blob, "tilecomm", "OFC weights calculated by TileCondToolOfc");
  delete comment;
  attrListColl->add(TileCalibUtils::getCommentChannel(), attrList);
  attrListColl->add(TileCalibUtils::getCommentChannel(), range);

  //=== recored collection in store gate
  CHECK( detStore()->record(attrListColl, folder) );

  //=======================================
  //=== Print conditions data in detector store
  //=======================================
  ATH_MSG_DEBUG( "Calling printCondObjects" );

  CHECK( printCondObjects() );

  return StatusCode::SUCCESS;
}

//
//________________________________________________________________________
StatusCode TileOFC2DBAlg::finalize() {

  ATH_MSG_INFO( "in finalize()" );

  //=== Register objects in DB
  CHECK( registerCondObjects() );

  ATH_MSG_DEBUG( "Register OK " );

  return StatusCode::SUCCESS;
}


//
//___________________________________________________________________
StatusCode TileOFC2DBAlg::registerCondObjects() {

  ATH_MSG_DEBUG( "entering registerCondObject " );

  for (std::set<std::string>::const_iterator iFolder = m_folders.begin();
      iFolder != m_folders.end(); ++iFolder) {

    const CondAttrListCollection* attrListColl = 0;
    CHECK( detStore()->retrieve(attrListColl, *iFolder) );

    if (0 == attrListColl) {
      ATH_MSG_ERROR( "Has not retrieved AttributeListCollection (ptr is 0) from " << *iFolder );
      return (StatusCode::FAILURE);
    } else {
      ATH_MSG_DEBUG( "Retrieved AttributeListCollection from " << *iFolder );
    }

    //=== register in COOL
    std::string tag = "";
    CHECK( m_regSvc->registerIOV("CondAttrListCollection", *iFolder, tag) );

    ATH_MSG_DEBUG( "Registered folder " << *iFolder << " with tag " << tag  );

  }    //end iFolder

  return StatusCode::SUCCESS;
}

//
//__________________________________________________________________
StatusCode TileOFC2DBAlg::printCondObjects() {

  ATH_MSG_INFO( "in printCondObjects()" );

  //=== loop over all folders
  for (std::set<std::string>::const_iterator iFolder = m_folders.begin();
      iFolder != m_folders.end(); ++iFolder) {

    ATH_MSG_DEBUG( "Printing content of folder: " << *iFolder );

    //=== determine blob name
    std::string blobName("TileCalibBlob");
    if (iFolder->find("/TILE/ONL01/FILTER") != std::string::npos) {
      blobName += "Ofc";
    }

    ATH_MSG_DEBUG( "The blob name is: " << blobName );

    const CondAttrListCollection* attrListColl(0);
    CHECK( detStore()->retrieve(attrListColl, *iFolder) );

    if (!attrListColl) {
      ATH_MSG_ERROR("Has not retrieved AttributeListCollection (ptr is 0) from " << *iFolder );
      return StatusCode::FAILURE;
    }

    //=== find the general comment
    std::string generalComment("");
    CondAttrListCollection::const_iterator iComment = attrListColl->chanAttrListPair( TileCalibUtils::getCommentChannel());
    if (iComment != attrListColl->end()) {
      const coral::Blob& blob = (iComment->second)[blobName].data<coral::Blob>();
      const TileCalibDrawerCmt* cmt = TileCalibDrawerCmt::getInstance(blob);
      generalComment = cmt->getFullComment();
      delete cmt;
    }

    //=== loop over collection
    CondAttrListCollection::const_iterator iColl = attrListColl->begin();
    CondAttrListCollection::const_iterator last = attrListColl->end();
    for (; iColl != last; ++iColl) {

      //=== do not print comment channel
      if (iColl == iComment) continue;

      //=== try to find comment for this drawer
      const coral::Blob& blob = (iColl->second)[blobName].data<coral::Blob>();
      //=== do not print empty blobs
      if (!blob.size()) {
        continue;
      }

      const TileCalibDrawerCmt* cmt = TileCalibDrawerCmt::getInstance(blob);
      std::ostringstream attrStr1;
      (*iColl).second.toOutputStream(attrStr1);
      ATH_MSG_DEBUG( "ChanNum " << (*iColl).first << " Attribute list " << attrStr1.str() );

      // Print out range if it exits
      CondAttrListCollection::ChanNum chanNum = (*iColl).first;
      CondAttrListCollection::iov_const_iterator iovIt = attrListColl->chanIOVPair(chanNum);
      if (iovIt != attrListColl->iov_end()) {
        const IOVRange& range = (*iovIt).second;
        if (range.start().isTimestamp()) {
          ATH_MSG_DEBUG( "Range timestamp :"
                         << " since " << range.start().timestamp()
                         << " till " << range.stop().timestamp() );

        } else {
          ATH_MSG_DEBUG( "Range R/LB :"
                         << " since " << range.start().run() << " " << range.start().event()
                         << " till " << range.stop().run() << " " << range.stop().event() );
        }
      } else {
        ATH_MSG_DEBUG( "No range found " );
      }

      //=== print the comment
      std::string comment = cmt->getFullComment();
      if (!comment.size()) {
        comment = generalComment;
      }
      ATH_MSG_DEBUG( "Comment: " << comment );
      delete cmt;

    }
  }    //end iFolder

  return StatusCode::SUCCESS;
}

