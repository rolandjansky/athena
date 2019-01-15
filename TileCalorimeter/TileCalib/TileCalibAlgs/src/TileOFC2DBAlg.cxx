/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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

#include <vector>

TileOFC2DBAlg::TileOFC2DBAlg(const std::string& name, ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
  , m_regSvc(0)
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCondToolOfc("TileCondToolOfc")

{
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCondToolOfc",    m_tileCondToolOfc, "TileCondToolOfc");
  declareProperty("OF2",                m_of2            = true,"true => OF2, false => OF1");
  declareProperty("RunIOVSince",        m_runIOVSince    = IOVTime::MINRUN );
  declareProperty("RunIOVUntil",        m_runIOVUntil    = IOVTime::MAXRUN );
  declareProperty("LbnIOVSince",        m_lbnIOVSince    = IOVTime::MINEVENT );
  declareProperty("LbnIOVUntil",        m_lbnIOVUntil    = IOVTime::MAXEVENT );
  declareProperty("FixedPhases",        m_fixedPhases    = true, "calculate OFCs for fixed phases" );
  declareProperty("RunType",            m_runType        = "PHY", "PHY or LAS");

  declareProperty("FixedPhasesNumber",   m_nFixedPhases   = 100);
  declareProperty("PhaseStep",          m_phaseStep      = 0.5);
  declareProperty("Modules",            m_modules        = {"AUX01"}, "Modules for which OFC should be stored in DB");
  declareProperty("CreateAllModules",   m_creatAllModules = true, "All missing modules is written to DB with zero size (empty)");
}
  
TileOFC2DBAlg::~TileOFC2DBAlg() {
} 


//____________________________________________________________________________
StatusCode TileOFC2DBAlg::initialize() {

  ATH_MSG_INFO( "Initializing for " << m_runType );

  //=== EventInfo key
  ATH_CHECK( m_eventInfoKey.initialize() );

  //=== get TileToolTiming
  CHECK( m_tileToolTiming.retrieve() );

  //=== get TileCondToolOfc
  CHECK( m_tileCondToolOfc.retrieve() );

  //=== IOVRegistrationSvc
  CHECK( service("IOVRegistrationSvc", m_regSvc) );

  ATH_MSG_DEBUG( "Found IOVRegistrationSvc " );


  std::map<std::string, unsigned int> roses = { {"AUX", 0}, {"LBA", 1}, {"LBC", 2}, {"EBA", 3}, {"EBC", 4} };

  msg(MSG::INFO) << "OFC will be stored in DB for the following modules:";
  for (std::string module : m_modules) {
    msg(MSG::INFO) << " " << module;
    m_drawerIdxs.push_back( TileCalibUtils::getDrawerIdx(roses[module.substr(0, 3)], std::stoi(module.substr(3, 2)) - 1) );
  }
  msg(MSG::INFO) << endmsg;

  if (m_creatAllModules) ATH_MSG_INFO("All other missing modules will be stored in DB with zero size");

  return StatusCode::SUCCESS;
}

//
//_________________________________________________________________________  
StatusCode TileOFC2DBAlg::execute() {

  //=== print run/evt/lbn/time info for each event
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  ATH_CHECK( eventInfo.isValid() );

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

  //unsigned int objVersion = (m_of2) ? 3 : 1;
  unsigned int objVersion = 3; // non efficient, but allows to keep dG also for OF1

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

  float zeroPhase(0.0);
  TileOfcWeightsStruct weights;
  ATH_CHECK( m_tileCondToolOfc->getOfcWeights(0, 0, 0, zeroPhase, true, weights) );
  int ndig = weights.n_samples;

  // ---------- create fixed phases
  if (m_fixedPhases) {
    ATH_MSG_DEBUG( "Fixed phases: " << m_fixedPhases 
                   << ", number of fixed phases: " << m_nFixedPhases
                   << ", phase step: " << m_phaseStep);

    std::vector<float> phases;
    phases.reserve(m_nFixedPhases * 2 + 1);

    //=== create attribute list
    coral::AttributeList ofcList(*spec);
    coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();

    std::vector<bool> coolChannelCreated(TileCalibUtils::MAX_DRAWERIDX, false);

    for (int iPhase = - m_nFixedPhases; iPhase <= m_nFixedPhases; ++iPhase) {
      phases.push_back(iPhase * m_phaseStep);
    }

    unsigned int maxChan(0);
    int nPhases = phases.size();
    TileCalibDrawerOfc* drawerOfc;

    for (unsigned int drawerIdx : m_drawerIdxs) {
      coolChannelCreated[drawerIdx] = true;        
      if (drawerIdx == 0) {
        maxChan = 1;
        //=== create an OFC blob interpreter
        drawerOfc = TileCalibDrawerOfc::getInstance(blob, objVersion, ndig, -nPhases, maxChan, TileCalibUtils::MAX_GAIN); // nPhases, nChann, nGains
      } else {
        //=== create an OFC blob interpreter
        maxChan = TileCalibUtils::MAX_CHAN;
        drawerOfc = TileCalibDrawerOfc::getInstance(blob, objVersion, ndig, -nPhases, maxChan, TileCalibUtils::MAX_GAIN); // nPhases, nChann, nGains
        
      }

      drawerOfc->setPhases(0, 0, phases);      

      for (unsigned int channel = 0; channel < maxChan; ++channel) {
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {

          for (float phase : phases) {
            ATH_CHECK( m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, phase, m_of2, weights) );
            for (int isam = 0; isam < ndig; isam++) {
              drawerOfc->setOfc(TileCalibDrawerOfc::FieldA, channel, gain, phase, isam, weights.w_a[isam]);
              drawerOfc->setOfc(TileCalibDrawerOfc::FieldB, channel, gain, phase, isam, weights.w_b[isam]);
              drawerOfc->setOfc(TileCalibDrawerOfc::FieldG, channel, gain, phase, isam, weights.g[isam]);
              if (objVersion == 3) {
                drawerOfc->setOfc(TileCalibDrawerOfc::FieldC, channel, gain, phase, isam, weights.w_c[isam]);
                drawerOfc->setOfc(TileCalibDrawerOfc::FieldDG, channel, gain, phase, isam, weights.dg[isam]);
              }
            }
            
            
            if (msgLvl(MSG::DEBUG)) {
              
              msg(MSG::DEBUG) << " N Samples " << ndig
                              << " channel " << channel
                              << " drawerIdx " << drawerIdx
                              << " gain " << gain
                              << " phase = " << phase << endmsg;
              
              msg(MSG::DEBUG) << "gain " << gain << " w_a, phase " << phase << " ";
              for (int isam = 0; isam < ndig; ++isam)
                msg(MSG::DEBUG) << " " << weights.w_a[isam];
              msg(MSG::DEBUG) << endmsg;
              
              msg(MSG::DEBUG) << "gain " << gain << " w_b, phase " << phase << " ";
              for (int isam = 0; isam < ndig; isam++)
                msg(MSG::DEBUG) << " " << weights.w_b[isam];
              msg(MSG::DEBUG) << endmsg;
              
              if (m_of2) {
                msg(MSG::DEBUG) << "gain " << gain << " w_c, phase " << phase << " ";
                for (int isam = 0; isam < ndig; isam++)
                  msg(MSG::DEBUG) << " " << weights.w_c[isam];
                msg(MSG::DEBUG) << endmsg;
              }
              
              msg(MSG::DEBUG) << "gain " << gain << " g, phase " << phase << " ";
              for (int isam = 0; isam < ndig; isam++)
                msg(MSG::DEBUG) << " " << weights.g[isam];
              msg(MSG::DEBUG) << endmsg;
              
            }
            
          }
        }
        
        
      }
      
      //=== use DrawerHash as channel number
      attrListColl->add(drawerIdx, ofcList);
      
      //=== add the IOV range for this collection
      attrListColl->add(drawerIdx, range);
      //    drawerOfc->dump();
      delete drawerOfc;
      
    }
    
    //=== Create empty blobs for all COOL channels (for missing drawers)
    if (m_creatAllModules) {
      for (unsigned int coolChannel = 0; coolChannel < TileCalibUtils::MAX_DRAWERIDX; ++coolChannel) {
        if (coolChannelCreated[coolChannel]) continue; 
        //=== create attribute list for this drawer
        coral::AttributeList ofcList(*spec);
        //coral::Blob& blob=ofcList["TileCalibBlobOfc"].data<coral::Blob>();
        attrListColl->add(coolChannel, ofcList);
        attrListColl->add(coolChannel, range);
      }
    }
    
  } else { // take best phase from DB and calculate OFCs for each channel

    std::vector<float> phases(TileCalibUtils::MAX_CHAN);

    //=== create attribute list
    coral::AttributeList ofcList(*spec);
    coral::Blob& blob = ofcList["TileCalibBlobOfc"].data<coral::Blob>();

    //=== create an OFC blob interpreter
    TileCalibDrawerOfc* drawerOfc = TileCalibDrawerOfc::getInstance(blob, objVersion, ndig, -48, 256, 2); // nPhases, nChann, nGains

    for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
      phases[channel] = (float) channel;

    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      for (unsigned int drawer = 0; drawer < TileCalibUtils::getMaxDrawer(ros); ++drawer) {
        unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
        drawerOfc->setPhases(drawerIdx, 0, phases);

        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel) {

          for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {

            float phase = m_tileToolTiming->getSignalPhase(drawerIdx, channel, gain);
            ATH_MSG_DEBUG( "m_tileToolTiming:"
                           << " drawerIdx " << drawerIdx
                           << " channel " << channel
                           << " gain " << gain
                           << " phase " << phase );

            ATH_CHECK( m_tileCondToolOfc->getOfcWeights(drawerIdx, channel, gain, phase, m_of2, weights) );
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
                               << " w_a " << weights.w_a[isam]
                               << " w_b " << weights.w_b[isam]
                               << " w_c " << weights.w_c[isam]
                               << "   g " << weights.g[isam] );

              } else {
                ATH_MSG_DEBUG( " sampling " << isam
                              << " w_a " << weights.w_a[isam]
                              << " w_b " << weights.w_b[isam]
                              << "   g " << weights.g[isam] );

              }

              w_a_sum += weights.w_a[isam];
              w_b_sum += weights.w_b[isam];
              w_c_sum += weights.w_c[isam];
              w_ag_sum += weights.w_a[isam] * weights.g[isam];
              w_bg_sum += weights.w_b[isam] * weights.g[isam];

              drawerOfc->setOfc(0, drawerIdx, gain, channel, isam, weights.w_a[isam]);
              drawerOfc->setOfc(1, drawerIdx, gain, channel, isam, weights.w_b[isam]);
              if (m_of2) {
                drawerOfc->setOfc(2, drawerIdx, gain, channel, isam, weights.w_c[isam]);
                drawerOfc->setOfc(3, drawerIdx, gain, channel, isam, weights.g[isam]);
              } else {
                drawerOfc->setOfc(2, drawerIdx, gain, channel, isam, weights.g[isam]);
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
  const char* user = getenv("USER");
  TileCalibDrawerCmt* comment = TileCalibDrawerCmt::getInstance(blob, (user ? user : "tilecomm"), "OFC weights calculated by TileCondToolOfc");
  delete comment;
  attrListColl->add(TileCalibUtils::getCommentChannel(), attrList);
  attrListColl->add(TileCalibUtils::getCommentChannel(), range);

  spec->release();
  // cppcheck-suppress memleak
  spec = nullptr;

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

