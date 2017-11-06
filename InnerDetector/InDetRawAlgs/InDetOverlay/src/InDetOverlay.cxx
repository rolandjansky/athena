/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

#include "InDetRawData/InDetRawData.h"
#include "InDetOverlay/InDetOverlay.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"
 

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

//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "InDetReadoutGeometry/PixelDetectorManager.h"
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
                            <<endmsg;
    }

    // ----------------------------------------------------------------
    // FIXME: That should really be a call to r1.merge(r2);

    TRT_LoLumRawData *pr1 = dynamic_cast<TRT_LoLumRawData*>(&r1);
    const TRT_LoLumRawData *pr2 = dynamic_cast<const TRT_LoLumRawData*>(&r2);

    if(pr1 && pr2) {
      // the actual merging
      pr1->merge (*pr2);
    }
    else {
      static bool first_time = true;
      if(first_time) {
        first_time = false;
        parent->msg(MSG::INFO)<<"Overlay::mergeChannelData(): "
                              <<"TRT specific code for "
                              <<typeid(TRT_RDORawData).name()
                              <<" is not implemented"
                              <<endmsg;
      }
    } // else - dyncasts
  } // mergeChannelData()

  //================================================================
  template<> void copyCollection(const InDetRawDataCollection<TRT_RDORawData> *input_coll, InDetRawDataCollection<TRT_RDORawData>*copy_coll){
    
    copy_coll->setIdentifier(input_coll->identify());
    InDetRawDataCollection<TRT_RDORawData>::const_iterator firstData = input_coll->begin();
    InDetRawDataCollection<TRT_RDORawData>::const_iterator lastData = input_coll->end();	
    for ( ; firstData != lastData; ++firstData)
      {	
	const Identifier ident = (*firstData)->identify();
	const unsigned int word = (*firstData)->getWord();
	TRT_LoLumRawData * newData=new TRT_LoLumRawData(ident,word);
	copy_coll->push_back (newData );
      }
  }
  
  template<> void copyCollection(const InDetRawDataCollection<SCT_RDORawData> *input_coll, InDetRawDataCollection<SCT_RDORawData>*copy_coll){
    
    copy_coll->setIdentifier(input_coll->identify());
    InDetRawDataCollection<SCT_RDORawData>::const_iterator firstData = input_coll->begin();
    InDetRawDataCollection<SCT_RDORawData>::const_iterator lastData = input_coll->end();	
    for ( ; firstData != lastData; ++firstData)
      {	
	const Identifier ident = (*firstData)->identify();
	const unsigned int word = (*firstData)->getWord();
	SCT3_RawData* oldData = dynamic_cast<SCT3_RawData*>(*firstData);	
	std::vector<int> errorHit=oldData->getErrorCondensedHit();
	SCT3_RawData * newData=new SCT3_RawData(ident,word, &errorHit);
	copy_coll->push_back (newData );
      }
  }
  
  template<> void copyCollection(const InDetRawDataCollection<PixelRDORawData> *input_coll, InDetRawDataCollection<PixelRDORawData>*copy_coll){
   
    copy_coll->setIdentifier(input_coll->identify());
    InDetRawDataCollection<PixelRDORawData>::const_iterator firstData = input_coll->begin();
    InDetRawDataCollection<PixelRDORawData>::const_iterator lastData = input_coll->end();	
    for ( ; firstData != lastData; ++firstData)
      {	
	const Identifier ident = (*firstData)->identify();
	const unsigned int word = (*firstData)->getWord();
	Pixel1RawData * newData=new Pixel1RawData(ident,word);
	copy_coll->push_back (newData );
      }
  }
  //================================================================
  namespace {   // helper functions for SCT merging
    typedef SCT3_RawData  SCT_RDO_TYPE;

    typedef std::multimap<int, const SCT_RDO_TYPE*> StripMap;

    void fillStripMap(StripMap *sm, const InDetRawDataCollection<SCT_RDORawData> &rdo_coll, const std::string& collectionName, InDetOverlay *parent) {
      for(InDetRawDataCollection<SCT_RDORawData>::const_iterator i=rdo_coll.begin(); i!=rdo_coll.end(); ++i) {
        const SCT_RDO_TYPE *rdo = dynamic_cast<const SCT_RDO_TYPE*>(&**i);
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

  template<> void mergeCollectionsNew(InDetRawDataCollection<SCT_RDORawData> *mc_coll,
                                      InDetRawDataCollection<SCT_RDORawData> *data_coll,
				      InDetRawDataCollection<SCT_RDORawData> *out_coll,
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
                            <<typeid(*mc_coll).name()
                            <<endmsg;
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
    if(mc_coll->identify() != data_coll->identify()) {
      std::ostringstream os;
      os<<"mergeCollectionsNew<SCT_RDORawData>(): collection Id mismatch";
      parent->msg(MSG::FATAL)<<os.str()<<endmsg;
      throw std::runtime_error(os.str());
    }

    const Identifier idColl = parent->get_sct_id()->wafer_id(mc_coll->identifyHash());

    // Empty the input collections and move RDOs to local vectors.
    InDetRawDataCollection<SCT_RDORawData> mc(mc_coll->identifyHash());
    mc.setIdentifier(idColl);
    mc_coll->swap(mc);

    InDetRawDataCollection<SCT_RDORawData> data(data_coll->identifyHash());
    data.setIdentifier(idColl);
    data_coll->swap(data);

    // Expand encoded RDOs into individual hit strips.
    // Each strip number is linked to the original RDO.
    StripMap sm;
    fillStripMap(&sm, mc, "MC", parent);
    fillStripMap(&sm, data, "data", parent);

    // collect all the hits
    StripMap::const_iterator p=sm.begin();
    while(p!= sm.end()) {

      // We have a strip.  compute the group, collect all contributing RDOs.
      const int firstStrip = p->first;

      // Get all strips for the current RDO
      std::set<const SCT_RDO_TYPE*> origRDOs;
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

      for(std::set<const SCT_RDO_TYPE*>::const_iterator origRdoIter = origRDOs.begin(); origRdoIter!=origRDOs.end(); ++origRdoIter) {
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

      out_coll->push_back(mergedRDO);

    } // "collect all strips" loop over sm

  } // mergeCollectionsNew()

} // namespace

//================================================================
InDetOverlay::InDetOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  IDC_OverlayBase(name, pSvcLocator),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_sct_id(0)
{

  //change via postExec indetovl.do_XXX=True

  declareProperty("do_TRT", m_do_TRT=true);
  declareProperty("do_TRT_background", m_do_TRT_background=true);
  declareProperty("mainInputTRT_Name", m_mainInputTRT_Name="TRT_RDOs");
  declareProperty("overlayInputTRT_Name", m_overlayInputTRT_Name="TRT_RDOs");

  declareProperty("do_SCT", m_do_SCT=true);
  declareProperty("do_SCT_background", m_do_SCT_background=true);
  declareProperty("mainInputSCT_Name", m_mainInputSCT_Name="SCT_RDOs");
  declareProperty("overlayInputSCT_Name", m_overlayInputSCT_Name="SCT_RDOs");

  declareProperty("do_Pixel", m_do_Pixel=true);
  declareProperty("do_Pixel_background", m_do_Pixel_background=true);
  declareProperty("mainInputPixelName", m_mainInputPixel_Name="PixelRDOs");
  declareProperty("overlayInputPixelName", m_overlayInputPixel_Name="PixelRDOs");
  //  declareProperty("OutputPixelName", m_OutputPixel_Name="PixelRDOs");
}

//================================================================
StatusCode InDetOverlay::overlayInitialize()
{
  ATH_MSG_INFO("InDetOverlay initialize()");

  if(!m_detStore->retrieve(m_sct_id,"SCT_ID").isSuccess() || !m_sct_id ) {
    msg(MSG::FATAL) << "Cannot retrieve SCT ID helper"  << endmsg;
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
    SG::ReadHandle<TRT_RDO_Container> dataContainer(m_mainInputTRT_Name, m_storeGateData->name());
    if(!dataContainer.isValid()) {   
      ATH_MSG_WARNING("Could not get data TRT container \""<<m_mainInputTRT_Name<<"\"");
    }

    ATH_MSG_INFO("TRT Data   = "<<shortPrint(dataContainer.cptr()));

    ATH_MSG_VERBOSE("Retrieving MC  input TRT container");
    SG::ReadHandle<TRT_RDO_Container> mcContainer(m_overlayInputTRT_Name, m_storeGateMC->name());
    if(!mcContainer.isValid()) {
      ATH_MSG_WARNING("Could not get MC TRT container \""<<m_overlayInputTRT_Name<<"\"");
    }
    ATH_MSG_INFO("TRT MC     = "<<shortPrint(mcContainer.cptr()));
   
   SG::WriteHandle<TRT_RDO_Container> outputContainer(m_mainInputTRT_Name, m_storeGateOutput->name());
   outputContainer = CxxUtils::make_unique<TRT_RDO_Container>(dataContainer->size());

   if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) {
     if (m_do_TRT_background ) overlayContainerNew(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
     else if(!m_do_TRT_background){
       TRT_RDO_Container nobkg;
       overlayContainerNew(&nobkg , mcContainer.cptr() , outputContainer.ptr());
     }
     ATH_MSG_INFO("TRT Result = "<<shortPrint(outputContainer.cptr()));
   }
  }
  
  //----------------------------------------------------------------
  if(m_do_SCT) {
    ATH_MSG_VERBOSE("Retrieving data input SCT container");
    SG::ReadHandle<SCT_RDO_Container> dataContainer(m_mainInputSCT_Name, m_storeGateData->name());
    if(!dataContainer.isValid()) {
      ATH_MSG_WARNING("Could not get data SCT container \""<<m_mainInputSCT_Name<<"\"");
    }
    ATH_MSG_INFO("SCT Data   = "<<shortPrint(dataContainer.cptr(), 50));

    ATH_MSG_VERBOSE("Retrieving MC  input SCT container");
    SG::ReadHandle<SCT_RDO_Container> mcContainer(m_overlayInputSCT_Name, m_storeGateMC->name());
    if(!mcContainer.isValid()) {
      ATH_MSG_WARNING("Could not get MC SCT container \""<<m_overlayInputSCT_Name<<"\"");
    }
    ATH_MSG_INFO("SCT MC     = "<<shortPrint(mcContainer.cptr(), 50));
   
    SG::WriteHandle<SCT_RDO_Container> outputContainer(m_mainInputSCT_Name, m_storeGateOutput->name());
    outputContainer = CxxUtils::make_unique<SCT_RDO_Container>(dataContainer->size());
    
    if(dataContainer.isValid() && mcContainer.isValid() && outputContainer.isValid()) {
      if(m_do_SCT_background) overlayContainerNew(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
      else if(!m_do_SCT_background){
	SCT_RDO_Container nobkg;
	overlayContainerNew(&nobkg , mcContainer.cptr() , outputContainer.ptr());
       }
      ATH_MSG_INFO("SCT Result = "<<shortPrint(outputContainer.ptr(), 50));   
    }
  }

  //----------------------------------------------------------------
  if(m_do_Pixel) {
    ATH_MSG_VERBOSE("Retrieving data input Pixel container");
    SG::ReadHandle<PixelRDO_Container> dataContainer(m_mainInputPixel_Name, m_storeGateData->name());
    if(!dataContainer.isValid()) {
      ATH_MSG_WARNING("Could not get data Pixel container \""<<m_mainInputPixel_Name<<"\"");
    }
    ATH_MSG_INFO("Pixel Data   = "<<shortPrint(dataContainer.cptr()));

    ATH_MSG_VERBOSE("Retrieving MC  input Pixel container");
    SG::ReadHandle<PixelRDO_Container> mcContainer(m_overlayInputPixel_Name, m_storeGateMC->name());
    if(!mcContainer.isValid()) {
      ATH_MSG_WARNING("Could not get MC Pixel container \""<<m_overlayInputPixel_Name<<"\"");
    }
    ATH_MSG_INFO("Pixel MC     = "<<shortPrint(mcContainer.cptr()));

/*
// Get geo model manager
const InDetDD::PixelDetectorManager* geo;
if ( detStore()->retrieve(geo, "Pixel").isFailure()) {
   ATH_MSG_ERROR("Could not get Pixel GeoModel Manager!");
   return StatusCode::RECOVERABLE;
}
//Loop over pixel RDO container
PixelRDO_Container::const_iterator containerItr;
for (containerItr=mcContainer->begin(); containerItr!=mcContainer->end(); ++containerItr) {
  const DataVector<PixelRDORawData>* rdoCollection = *containerItr;
  DataVector<PixelRDORawData>::const_iterator collectionItr;
  double avgx=0; int nx=0;
  for (collectionItr=rdoCollection->begin(); collectionItr!=rdoCollection->end(); ++collectionItr) {
    PixelRDORawData *rdoData = *collectionItr;
    const Identifier id = rdoData->identify();
    const InDetDD::SiDetectorElement *element = geo->getDetectorElement(id);
    if (!element->isInnermostPixelLayer()) {//only keep IBL
      //rdoCollection->remove(collectionItr);
      continue;
    }
    Amg::Vector2D localPos = element->localPositionOfCell(id);
    Amg::Vector2D rawlocalPos = element->rawLocalPositionOfCell(id);
    Amg::Vector3D globalPos = element->globalPosition(localPos);
    Amg::Vector3D rawglobalPos = element->globalPosition(rawlocalPos);
    //ATH_MSG_INFO("localPos: "<<localPos.x()<<" "<<localPos.y());
    //ATH_MSG_INFO("rawlocalPos: "<<rawlocalPos.x()<<" "<<rawlocalPos.y());
    //ATH_MSG_INFO("globalPos: "<<globalPos.x()<<" "<<globalPos.y()<<" "<<globalPos.z());
    //ATH_MSG_INFO("rawglobalPos: "<<rawglobalPos.x()<<" "<<rawglobalPos.y()<<" "<<rawglobalPos.z());
    avgx+=globalPos.x(); nx++;
  }
  if (nx>0){ATH_MSG_INFO("avgx ="<<avgx/(double)nx);}
}
*/
    SG::WriteHandle<PixelRDO_Container> outputContainer(m_mainInputPixel_Name, m_storeGateOutput->name());
    outputContainer = CxxUtils::make_unique<PixelRDO_Container>(dataContainer->size());
    
    if(dataContainer.isValid() && mcContainer.isValid()&&outputContainer.isValid()) {  
      if(m_do_Pixel_background) overlayContainerNew(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr());
      else if(!m_do_Pixel_background){
	PixelRDO_Container nobkg;
	overlayContainerNew(&nobkg , mcContainer.cptr() , outputContainer.ptr());
       }
      ATH_MSG_INFO("Pixel Result = "<<shortPrint(outputContainer.ptr()));
    }
  }

  //----------------------------------------------------------------
  ATH_MSG_DEBUG("InDetOverlay::execute() end");
  return StatusCode::SUCCESS;
}

//================================================================
//EOF
