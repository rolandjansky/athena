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
#include <bitset>

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

    // Strip hit timing information for Next, Current, Previous and Any BCs
    // Prepare one more strip to create the last one. The additional strip has no hits.
    std::bitset<InDetOverlay::NumberOfStrips+1> stripInfo[InDetOverlay::NumberOfBitSets];
    // Process MC and data in the wafer
    for (unsigned source=InDetOverlay::MCSource; source<InDetOverlay::NumberOfSources; source++) {
      InDetRawDataCollection<SCT_RDORawData>::const_iterator rdo;
      InDetRawDataCollection<SCT_RDORawData>::const_iterator rdoEnd;
      if (source==InDetOverlay::MCSource) { // MC
        rdo = mc.begin();
        rdoEnd = mc.end();
      } else if (source==InDetOverlay::DataSource) { // Data
        rdo = data.begin();
        rdoEnd = data.end();
      } else {
        parent->msg(MSG::WARNING) << "Invalid source " << source << " in mergeCollectionsNew for SCT" << endmsg;
        continue;
      }
      // Loop over all RDOs in the wafer
      for (; rdo!=rdoEnd; rdo++) {
        const SCT3_RawData* rdo3 = dynamic_cast<const SCT3_RawData*>(*rdo);
        if (!rdo3) {
          std::ostringstream os;
          os<<"mergeCollectionNew<SCT_RDORawData>(): wrong datum format. Only SCT3_RawData are produced by SCT_RodDecoder and supported by overlay."
            <<"For the supplied datum  typeid(datum).name() = "<<typeid(**rdo).name();
          throw std::runtime_error(os.str());
        }
        int strip = parent->get_sct_id()->strip(rdo3->identify());
        int stripEnd = strip + rdo3->getGroupSize();
        int timeBin = rdo3->getTimeBin();
        for (; strip<stripEnd and strip<InDetOverlay::NumberOfStrips; strip++) {
          // Fill timing information for each strips, loop over 3 BCs
          for (unsigned int bc=InDetOverlay::NextBC; bc<InDetOverlay::NumberOfBCs; bc++) {
            if (timeBin & (1 << bc)) stripInfo[bc].set(strip);
          }
        }
      }
    }
    // Get OR for AnyBC, loop over 3 BCs
    for (unsigned int bc=InDetOverlay::NextBC; bc<InDetOverlay::NumberOfBCs; bc++) {
      stripInfo[InDetOverlay::AnyBC] |= stripInfo[bc];
    }
    // Check if we need to use Expanded mode by checking if there is at least one hit in Next BC or Previous BC
    bool anyNextBCHits = stripInfo[InDetOverlay::NextBC].any();
    bool anyPreivousBCHits = stripInfo[InDetOverlay::PreviousBC].any();
    bool isExpandedMode = (anyNextBCHits or anyPreivousBCHits);
    // No error information is recorded because this information is not filled in data and no errors are assumed in MC.
    const int ERRORS = 0;
    const std::vector<int> errvec{};
    if (isExpandedMode) {
      // Expanded mode (record strip one by one)
      const int groupSize = 1;
      int tbin = 0;
      for (unsigned int strip=0; strip<InDetOverlay::NumberOfStrips; strip++) {
        if (stripInfo[InDetOverlay::AnyBC][strip]) {
          tbin = 0;
          for (unsigned int bc=InDetOverlay::NextBC; bc<InDetOverlay::NumberOfBCs; bc++) {
            if (stripInfo[bc][strip]) {
              tbin |= (1 << bc);
            }
          }
          unsigned int SCT_Word = (groupSize | (strip << 11) | (tbin <<22) | (ERRORS << 25));
          Identifier rdoId = parent->get_sct_id()->strip_id(idColl, strip) ;
          out_coll->push_back(new SCT3_RawData(rdoId, SCT_Word, &errvec));
        }
      }
    } else {
      // We can record consecutive hits into one RDO if all hits have timeBin of 010.
      unsigned int groupSize = 0;
      const int tbin = (1 << InDetOverlay::CurrentBC);
      for (unsigned int strip=0; strip<InDetOverlay::NumberOfStrips+1; strip++) { // Loop over one more strip to create the last one if any
        if (stripInfo[InDetOverlay::AnyBC][strip]) {
          groupSize++;
        } else {
          if (groupSize>0) {
            unsigned int firstStrip = strip - groupSize;
            unsigned int SCT_Word = (groupSize | (firstStrip << 11) | (tbin <<22) | (ERRORS << 25));
            Identifier rdoId = parent->get_sct_id()->strip_id(idColl, firstStrip) ;
            out_coll->push_back(new SCT3_RawData(rdoId, SCT_Word, &errvec));
            groupSize = 0;
          }
        }
      }
    }

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
