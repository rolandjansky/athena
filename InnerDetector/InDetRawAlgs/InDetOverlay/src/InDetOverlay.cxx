/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

// FIXME: add merge() methods to  InDetRawData  classes and get rid of the hack.
#define private public
#define protected public
#include "InDetRawData/InDetRawData.h"
#undef private
#undef protected


#include "InDetOverlay/InDetOverlay.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

#include "GeneratorObjects/McEventCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "InDetRawData/TRT_RDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/SCT3_RawData.h"
//#include "InDetRawData/Pixel1RawData.h"

#include "InDetIdentifier/SCT_ID.h"

#include <iostream>
#include <sstream>
#include <typeinfo>

//================================================================
namespace Overlay {

  // Specialize mergeChannelData() for the TRT
  template<> void mergeChannelData(TRT_RDORawData& r1, const TRT_RDORawData& r2, IDC_OverlayBase* parent) {

    // ----------------------------------------------------------------
    // demo
    static bool first_time = true;
    if(first_time) {
      first_time = false;
      parent->msg(MSG::INFO)<<"Overlay::mergeChannelData(): "
                            <<"TRT specific code is called for "
                            <<typeid(TRT_RDORawData).name()
                            <<endreq;
    }

    // ----------------------------------------------------------------
    // FIXME: That should really be a call to r1.merge(r2);

    TRT_LoLumRawData *pr1 = dynamic_cast<TRT_LoLumRawData*>(&r1);
    const TRT_LoLumRawData *pr2 = dynamic_cast<const TRT_LoLumRawData*>(&r2);

    if(pr1 && pr2) {
      // the actual merging
      pr1->m_word |= pr2->m_word;
    }
    else {
      static bool first_time = true;
      if(first_time) {
        first_time = false;
        parent->msg(MSG::INFO)<<"Overlay::mergeChannelData(): "
                              <<"TRT specific code for "
                              <<typeid(TRT_RDORawData).name()
                              <<" is not implemented"
                              <<endreq;
      }
    } // else - dyncasts
  } // mergeChannelData()


  //================================================================
  namespace {   // helper functions for SCT merging
    typedef SCT3_RawData  SCT_RDO_TYPE;

    typedef std::multimap<int, SCT_RDO_TYPE*> StripMap;

    void fillStripMap(StripMap *sm, const InDetRawDataCollection<SCT_RDORawData> &rdo_coll, const std::string& collectionName, InDetOverlay *parent) {
      for(InDetRawDataCollection<SCT_RDORawData>::const_iterator i=rdo_coll.begin(); i!=rdo_coll.end(); ++i) {
        SCT_RDO_TYPE *rdo = dynamic_cast<SCT_RDO_TYPE*>(&**i);
        if(!rdo) {
          std::ostringstream os;
          os<<"mergeCollection<SCT_RDORawData>(): wrong datum format for the '"<<collectionName<<"' collection. Only SCT3_RawData are produced by SCT_RodDecoder and supported by overlay.   For the supplied datum  typeid(datum).name() = "<<typeid(**i).name();
          throw std::runtime_error(os.str());
        }

        int stripBegin = parent->get_sct_id()->strip(rdo->identify());
        int stripEnd = stripBegin + rdo->getGroupSize();
        for(int strip = stripBegin; strip < stripEnd; ++strip) {
          sm->insert(std::make_pair(strip, rdo));
        }
      }
    }
  } // namespace - helper functions for SCT merging


  //================

  template<> void mergeCollectionsNew(InDetRawDataCollection<SCT_RDORawData> *data_coll,
                                      InDetRawDataCollection<SCT_RDORawData> *mc_coll,
                                      IDC_OverlayBase *tmp)
  {
    // We want to use the SCT_ID helper provided by InDetOverlay, thus the constraint
    InDetOverlay *parent = dynamic_cast<InDetOverlay*>(tmp);
    if(!parent) {
      std::ostringstream os;
      os<<"mergeCollectionsNew<SCT_RDORawData>() called by a wrong parent algorithm?  Must be InDetOverlay.";
      throw std::runtime_error(os.str());
    }

    // ----------------------------------------------------------------
    // debug
    static bool first_time = true;
    if(first_time) {
      first_time = false;
      parent->msg(MSG::INFO)<<"InDetOverlay::mergeCollectionsNew(): "
                            <<" SCT specific code is called for "
                            <<typeid(*data_coll).name()
                            <<endreq;
    }

    // ----------------------------------------------------------------
    // Useful reading:
    //
    // Real data are handled like this:
    //
    // https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetEventCnv/SCT_RawDataByteStreamCnv/trunk/src/SCT_RodDecoder.cxx?rev=332542
    //
    // Note that SCT3 is hardcoded (no chance to get SCT1 from real data).
    // The SCT3 format is also the default in digitization:
    //
    // https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDigitization/SCT_Digitization/trunk/src/SCT_Digitization.cxx?rev=355953

    // So we'll just go with SCT3.
    //
    // The client code that consumes SCT RDOs:
    //
    // http://alxr.usatlas.bnl.gov/lxr/source/atlas/InnerDetector/InDetRecTools/SiClusterizationTool/src/SCT_ClusteringTool.cxx


    // ----------------------------------------------------------------
    if(data_coll->identify() != mc_coll->identify()) {
      std::ostringstream os;
      os<<"mergeCollectionsNew<SCT_RDORawData>(): collection Id mismatch";
      parent->msg(MSG::FATAL)<<os.str()<<endreq;
      throw std::runtime_error(os.str());
    }

    const Identifier idColl = parent->get_sct_id()->wafer_id(data_coll->identifyHash());

    // Empty the input collections and move RDOs to local vectors.
    InDetRawDataCollection<SCT_RDORawData> data(data_coll->identifyHash());
    data.setIdentifier(idColl);
    data_coll->swap(data);

    InDetRawDataCollection<SCT_RDORawData> mc(mc_coll->identifyHash());
    mc.setIdentifier(idColl);
    mc_coll->swap(mc);

    // Just an alias for the currently empty collection
    InDetRawDataCollection<SCT_RDORawData> *output = data_coll;

    // Expand encoded RDOs into individual hit strips.
    // Each strip number is linked to the original RDO.
    StripMap sm;
    fillStripMap(&sm, data, "data", parent);
    fillStripMap(&sm, mc, "MC", parent);

    // collect all the hits
    StripMap::const_iterator p=sm.begin();
    while(p!= sm.end()) {

      // We have a strip.  compute the group, collect all contributing RDOs.
      const int firstStrip = p->first;

      // Get all strips for the current RDO
      std::set<SCT_RDO_TYPE*> origRDOs;
      origRDOs.insert(p->second);
      int currentStrip = firstStrip;
      while(++p != sm.end()) {

        // The following condition forces the "condensed" mode, with
        // one RDO representing a group of adjacent hit strips.  It
        // looks like SCT_RodDecoder switches between "expanded" and
        // "condensed" modes based on the input byte stream; but here
        // we don't have information to do this.  So follow the
        // digitization code and hardcode the "condensed" mode.
        // NB: change currentStrip+2 to currentStrip+1 if want to
        // switch to the expanded mode.
        if(p->first < currentStrip + 2) {
          origRDOs.insert(p->second);
          currentStrip = p->first;
        }
        else {
          break;
        }
      }

      // We've got info on all strips for the current RDO here.
      // Create one.
      const int groupSize = 1 + currentStrip - firstStrip;

      int tbin = 0;
      int ERRORS = 0;
      std::vector<int> errvec;

      for(std::set<SCT_RDO_TYPE*>::const_iterator origRdoIter = origRDOs.begin(); origRdoIter!=origRDOs.end(); ++origRdoIter) {
        tbin |= (*origRdoIter)->getTimeBin();

        if((*origRdoIter)->FirstHitError()) {
          ERRORS |= 0x10;
        }
        if((*origRdoIter)->SecondHitError()) {
          ERRORS |= 0x20;
        }

        std::vector<int> origRdoErrorVec((*origRdoIter)->getErrorCondensedHit());
        errvec.insert(errvec.end(), origRdoErrorVec.begin(), origRdoErrorVec.end());
      }

      unsigned int SCT_Word = (groupSize | (firstStrip << 11) | (tbin <<22) | (ERRORS << 25)) ;
      Identifier rdoId = parent->get_sct_id()->strip_id(idColl, firstStrip) ;
      SCT3_RawData *mergedRDO = new SCT3_RawData(rdoId, SCT_Word, &errvec);

      output->push_back(mergedRDO);

    } // "collect all strips" loop over sm

  } // mergeCollectionsNew()

} // namespace

//================================================================
InDetOverlay::InDetOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_OverlayBase(name, pSvcLocator),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_sct_id(0)
{

  declareProperty("do_TRT", m_do_TRT=true);
  declareProperty("mainInputTRT_Name", m_mainInputTRT_Name="TRT_RDOs");
  declareProperty("overlayInputTRT_Name", m_overlayInputTRT_Name="TRT_RDOs");

  declareProperty("do_SCT", m_do_SCT=true);
  declareProperty("mainInputSCT_Name", m_mainInputSCT_Name="SCT_RDOs");
  declareProperty("overlayInputSCT_Name", m_overlayInputSCT_Name="SCT_RDOs");

  declareProperty("do_Pixel", m_do_Pixel=true);
  declareProperty("mainInputPixelName", m_mainInputPixel_Name="PixelRDOs");
  declareProperty("overlayInputPixelName", m_overlayInputPixel_Name="PixelRDOs");
}

//================================================================
StatusCode InDetOverlay::overlayInitialize()
{
  ATH_MSG_INFO("InDetOverlay initialize()");

  if(!m_detStore->retrieve(m_sct_id,"SCT_ID").isSuccess() || !m_sct_id ) {
    msg(MSG::FATAL) << "Cannot retrieve SCT ID helper"  << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode InDetOverlay::overlayFinalize()
{
  ATH_MSG_INFO("InDetOverlay finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode InDetOverlay::overlayExecute() {
  ATH_MSG_DEBUG("InDetOverlay::execute() begin");

  //----------------------------------------------------------------
  if(m_do_TRT) {
    ATH_MSG_VERBOSE("Retrieving data input TRT container");
    std::auto_ptr<TRT_RDO_Container> cdata(m_storeGateData->retrieve<TRT_RDO_Container>(m_mainInputTRT_Name));
    if(!cdata.get()) {
      ATH_MSG_WARNING("Could not get data TRT container \""<<m_mainInputTRT_Name<<"\"");
    }

    ATH_MSG_DEBUG("TRT Data   = "<<shortPrint(cdata));

    ATH_MSG_VERBOSE("Retrieving MC  input TRT container");
    std::auto_ptr<TRT_RDO_Container> cmc(m_storeGateMC->retrievePrivateCopy<TRT_RDO_Container>(m_overlayInputTRT_Name));
    if(!cmc.get()) {
      ATH_MSG_WARNING("Could not get MC TRT container \""<<m_overlayInputTRT_Name<<"\"");
    }
    ATH_MSG_DEBUG("TRT MC     = "<<shortPrint(cmc));

    if(cdata.get() && cmc.get()) {
      overlayContainerNew(cdata, cmc);
      ATH_MSG_DEBUG("TRT Result = "<<shortPrint(cdata));

      if( !m_storeGateOutput->record(cdata, m_mainInputTRT_Name).isSuccess() ) {
        ATH_MSG_WARNING("Failed to record TRT overlay container to output store ");
      }
    }

    //----------------
    // This kludge is a work around for problems created by another kludge:
    // Digitization algs keep a pointer to their output Identifiable Container and reuse
    // the same object over and other again.   So unlike any "normal" per-event object
    // this IDC is not a disposable one, and we should not delete it.
    //cmc.release();
  }

  //----------------------------------------------------------------
  if(m_do_SCT) {
    ATH_MSG_VERBOSE("Retrieving data input SCT container");
    std::auto_ptr<SCT_RDO_Container> cdata(m_storeGateData->retrieve<SCT_RDO_Container>(m_mainInputSCT_Name));
    if(!cdata.get()) {
      ATH_MSG_WARNING("Could not get data SCT container \""<<m_mainInputSCT_Name<<"\"");
    }
    ATH_MSG_DEBUG("SCT Data   = "<<shortPrint(cdata.get(), 50));

    ATH_MSG_VERBOSE("Retrieving MC  input SCT container");
    std::auto_ptr<SCT_RDO_Container> cmc(m_storeGateMC->retrievePrivateCopy<SCT_RDO_Container>(m_overlayInputSCT_Name));
    if(!cmc.get()) {
      ATH_MSG_WARNING("Could not get MC SCT container \""<<m_overlayInputSCT_Name<<"\"");
    }
    ATH_MSG_DEBUG("SCT MC     = "<<shortPrint(cmc, 50));

    if(cdata.get() && cmc.get()) {
      overlayContainerNew(cdata, cmc);
      ATH_MSG_DEBUG("SCT Result = "<<shortPrint(cdata, 50));

      if( !m_storeGateOutput->record(cdata, m_mainInputSCT_Name).isSuccess() ) {
        ATH_MSG_WARNING("Failed to record SCT overlay container to output store ");
      }
    }

    //----------------
    // This kludge is a work around for problems created by another kludge:
    // Digitization algs keep a pointer to their output Identifiable Container and reuse
    // the same object over and other again.   So unlike any "normal" per-event object
    // this IDC is not a disposable one, and we should not delete it.
    //cmc.release();
  }

  //----------------------------------------------------------------
  if(m_do_Pixel) {
    ATH_MSG_VERBOSE("Retrieving data input Pixel container");
    std::auto_ptr<PixelRDO_Container> cdata(m_storeGateData->retrieve<PixelRDO_Container>(m_mainInputPixel_Name));
    if(!cdata.get()) {
      ATH_MSG_WARNING("Could not get data Pixel container \""<<m_mainInputPixel_Name<<"\"");
    }
    ATH_MSG_DEBUG("Pixel Data   = "<<shortPrint(cdata));

    ATH_MSG_VERBOSE("Retrieving MC  input Pixel container");
    std::auto_ptr<PixelRDO_Container> cmc(m_storeGateMC->retrievePrivateCopy<PixelRDO_Container>(m_overlayInputPixel_Name));
    if(!cmc.get()) {
      ATH_MSG_WARNING("Could not get MC Pixel container \""<<m_overlayInputPixel_Name<<"\"");
    }
    ATH_MSG_DEBUG("Pixel MC     = "<<shortPrint(cmc));

    if(cdata.get() && cmc.get()) {
      overlayContainerNew(cdata, cmc);
      ATH_MSG_DEBUG("Pixel Result = "<<shortPrint(cdata));

      if( !m_storeGateOutput->record(cdata, m_mainInputPixel_Name).isSuccess() ) {
        ATH_MSG_WARNING("Failed to record Pixel overlay container to output store ");
      }
    }

    //----------------
    // This kludge is a work around for problems created by another kludge:
    // Digitization algs keep a pointer to their output Identifiable Container and reuse
    // the same object over and other again.   So unlike any "normal" per-event object
    // this IDC is not a disposable one, and we should not delete it.
    //cmc.release();
  }

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("Processing MC truth data");

  // Main stream is normally real data without any MC info.
  // In tests we may use a MC generated file instead of real data.
  // Remove truth info from the main input stream, if any.
  //
  // Here we handle just InDet-specific truth classes.
  // (McEventCollection is done by the base.)

  //Simply don not copy: removeAllObjectsOfType<InDetSimDataCollection>(&*m_storeGateData);

  // Now copy InDet-specific MC truth objects to the output.
  copyAllObjectsOfType<InDetSimDataCollection>(&*m_storeGateOutput, &*m_storeGateMC);

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("InDetOverlay::execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
