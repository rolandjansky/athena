/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007

#include "InDetRawData/InDetRawData.h"
#include "InDetOverlay/InDetOverlay.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/SystemOfUnits.h"


#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "CxxUtils/make_unique.h"
 

#include "GeneratorObjects/McEventCollection.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/PixelRDO_Container.h"

#include "InDetRawData/TRT_RDORawData.h"
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/SCT3_RawData.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/TRT_ID.h"

#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

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
      } else { // Data
        rdo = data.begin();
        rdoEnd = data.end();
      } 
      // Loop over all RDOs in the wafer
      for (; rdo!=rdoEnd; ++rdo) {
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
  m_sct_id(nullptr),
  m_trt_id(nullptr),
  m_rndmSvc("AtRndmGenSvc",name),
  m_rndmEngineName("InDetOverlay"),
  m_rndmEngine(nullptr),
  m_TRT_LocalOccupancyTool("TRT_LocalOccupancy"),
  m_TRTStrawSummarySvc("InDetTRTStrawStatusSummarySvc",name)
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
  
  
  declareProperty("TRTinputSDO_Name", m_TRTinputSDO_Name="TRT_SDO_Map");
  declareProperty("TRT_HT_OccupancyCorrectionBarrel",m_HTOccupancyCorrectionB=0.110);
  declareProperty("TRT_HT_OccupancyCorrectionEndcap",m_HTOccupancyCorrectionEC=0.090);
  declareProperty("TRT_HT_OccupancyCorrectionBarrelNoE",m_HTOccupancyCorrectionB_noE=0.06);
  declareProperty("TRT_HT_OccupancyCorrectionEndcapNoE",m_HTOccupancyCorrectionEC_noE=0.05);

  declareProperty("RndmSvc",    m_rndmSvc,       "Random Number Service");
  declareProperty("RndmEngine", m_rndmEngineName,"Random engine name");
  
  declareProperty("TRT_LocalOccupancyTool", m_TRT_LocalOccupancyTool);
  declareProperty("TRTStrawSummarySvc",  m_TRTStrawSummarySvc);  
}

//================================================================
StatusCode InDetOverlay::overlayInitialize()
{
  ATH_MSG_INFO("InDetOverlay initialize()");

  if(!m_detStore->retrieve(m_sct_id,"SCT_ID").isSuccess() || !m_sct_id ) {
    msg(MSG::FATAL) << "Cannot retrieve SCT ID helper"  << endmsg;
    return StatusCode::FAILURE;
  }
  
  
  if(m_do_TRT){
  
    if(!m_detStore->retrieve(m_trt_id,"TRT_ID").isSuccess() || !m_sct_id ) {
      ATH_MSG_FATAL("Cannot retrieve TRT ID helper");
      return StatusCode::FAILURE;
    }
  
    // Initialize random number generator
    CHECK(m_rndmSvc.retrieve());
    m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
    if (!m_rndmEngine) {
      ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
      return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
    }

    CHECK( m_TRT_LocalOccupancyTool.retrieve() );


    if (!m_TRTStrawSummarySvc.empty() && m_TRTStrawSummarySvc.retrieve().isFailure() ) {
      ATH_MSG_ERROR ("Failed to retrieve StrawStatus Summary " << m_TRTStrawSummarySvc);
      ATH_MSG_ERROR ("configure as 'None' to avoid its loading.");
      return StatusCode::FAILURE;
    } else {
      if ( !m_TRTStrawSummarySvc.empty()) 
        ATH_MSG_INFO( "Retrieved tool " << m_TRTStrawSummarySvc );
    }



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
      if (m_do_TRT_background ){ 
        SG::ReadHandle<InDetSimDataCollection> sdoContainer(m_TRTinputSDO_Name, m_storeGateMC->name());

        // Calculate occupancy here
        std::map<int, double> occupancy = m_TRT_LocalOccupancyTool->getDetectorOccupancy( dataContainer.cptr() );
        //Merge containers
        overlayTRTContainers(dataContainer.cptr(), mcContainer.cptr(), outputContainer.ptr(), occupancy, *sdoContainer);
      } else if(!m_do_TRT_background){
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

void InDetOverlay::overlayTRTContainers(const TRT_RDO_Container *pileupContainer,
                                        const TRT_RDO_Container *signalContainer,
                                        TRT_RDO_Container *outputContainer,
                                        std::map<int,double>&  occupancyMap,
                                        const InDetSimDataCollection& SDO_Map) 
{

   //There are some use cases where this is empty
   if(pileupContainer != nullptr){
   /** Add data from the data container to the output one */
     TRT_RDO_Container::const_iterator p_data = pileupContainer->begin();
     TRT_RDO_Container::const_iterator p_data_end = pileupContainer->end();

     for(; p_data != p_data_end; ++p_data) {
       IdentifierHash hashId = p_data.hashId();
       auto coll_data = std::make_unique<TRT_RDO_Collection>(hashId); 
       Overlay::copyCollection( (*p_data).cptr(), coll_data.get());

       if ( outputContainer->addCollection(coll_data.release(), p_data.hashId() ).isFailure() ) {
         ATH_MSG_WARNING( "add data Collection failed for output "<< p_data.hashId   () ); // 
       } 
     }
   }

   /** Add data from the ovl container to the output one */
   TRT_RDO_Container::const_iterator p_ovl = signalContainer->begin(); 
   TRT_RDO_Container::const_iterator p_ovl_end = signalContainer->end();

   for(; p_ovl != p_ovl_end; ++p_ovl) {
 
      IdentifierHash coll_id = p_ovl.hashId();
      auto coll_ovl = std::make_unique<TRT_RDO_Collection>(coll_id);    
      Overlay::copyCollection( (*p_ovl).cptr(), coll_ovl.get() ) ;

      /** The newly created stuff will go to the output EventStore SG */
      auto coll_out = std::make_unique<TRT_RDO_Collection>(coll_id);
      coll_out->setIdentifier((*p_ovl)->identify());

      /** Look for the same ID in the main StoreGate EventStore */ 
      auto q = outputContainer->indexFind( coll_id );
      if( q != outputContainer->end() ) {
      /**Need to merge the collections
         Retrieve q */
         std::unique_ptr <TRT_RDO_Collection> coll_data ((TRT_RDO_Collection *) (*q).cptr() );
         int det =  m_trt_id->barrel_ec( (*p_ovl)->identify() );
         mergeTRTCollections(coll_data.get(),coll_ovl.get(),coll_out.get(), occupancyMap[det], SDO_Map);
         
         outputContainer->removeCollection(p_ovl.hashId());
         if (outputContainer->addCollection(coll_out.release(), p_ovl.hashId()).isFailure() ) {
            ATH_MSG_WARNING( "overlay addCollection failed "  ); 
         }
      }
      else {
       /** Copy the complete collection from ovl to output, 
           hopefully preserving the "most derived" type of its raw data */ 
       if ( outputContainer->addCollection(coll_ovl.release(), coll_id).isFailure() ) {
          ATH_MSG_WARNING(  "add mc Collection failed " ); 
      }
    }   
  }                               
}



//================================================================
void InDetOverlay::mergeTRTCollections(TRT_RDO_Collection *mc_coll, 
                                       TRT_RDO_Collection *data_coll, 
                                       TRT_RDO_Collection *out_coll, 
                                       double occupancy,
                                       const InDetSimDataCollection& SDO_Map) 
{
   
  if(mc_coll->identify() != data_coll->identify()) {
    std::ostringstream os;
    os<<"mergeCollectionsNew<generic>(): collection Id mismatch";
    ATH_MSG_FATAL(os.str());
    throw std::runtime_error(os.str());
  }

  const Identifier idColl = mc_coll->identify();

  // ----------------------------------------------------------------
  // debug
  static bool first_time = true;
  if(first_time) {
    first_time = false;
    ATH_MSG_INFO( "mergeTRTCollections():  code is called ");
  }

  // ----------------------------------------------------------------

  TRT_RDO_Collection data(data_coll->identifyHash());
  data.setIdentifier(idColl);
  data_coll->swap(data);

  TRT_RDO_Collection mc(mc_coll->identifyHash());
  mc.setIdentifier(idColl);
  mc_coll->swap(mc);

  //################################################################
  // Merge by copying ptrs from data and mc to mc_coll

  unsigned int idata = 0;
  unsigned int imc   = 0;
 
  while( (idata < data.size()) || (imc < mc.size())) {

    // The RDO that goes to the output at the end of this step.
    TRT_RDORawData *p_rdo(0);
  
    if(imc == mc.size()) {
      // just copy the remaining data inputs
      data.swapElement(idata++, 0, p_rdo);
    }
    else if(idata == data.size()) {
      //just copy the remaining MC digits
      mc.swapElement(imc++, 0, p_rdo);
    }
    else {
      // Need to decide which one goes first.  
      // See comments in TRTDigitization.cxx about the assumption that id1<id2 <=> hash1<hash2
      if( mc[imc]->identify() < data[idata]->identify() ) {
        mc.swapElement(imc++, 0, p_rdo);
      }
      else if(data[idata]->identify() < mc[imc]->identify()) {
        data.swapElement(idata++, 0, p_rdo);
      }
      else {
        // The hits are on the same channel.
        TRT_RDORawData *p2(0);
        data.swapElement(idata++, 0, p2);
        mc.swapElement(imc++, 0, p_rdo);

        TRT_LoLumRawData *pr1 = dynamic_cast<TRT_LoLumRawData*>(p_rdo);
        const TRT_LoLumRawData *pr2 = dynamic_cast<const TRT_LoLumRawData*>(p2);

        if(pr1 && pr2) {
          // the actual merging
          pr1->merge(*pr2);
        

          //If the hit is not already a high level hit 
          if( !(pr1->getWord() & 0x04020100)  ) {
          
            bool isXenonStraw = false;
          
            Identifier rdoId = p_rdo->identify();
            
            if (!m_TRTStrawSummarySvc.empty()) {
              if (m_TRTStrawSummarySvc->getStatusHT(rdoId) == TRTCond::StrawStatus::Good) {
                isXenonStraw = true;
              }
            }

           
            //Determine if the hit is from an electron or not
            bool isElectron = false;
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

            
            unsigned int newword = 0;
            //Get random number 
            int det =  m_trt_id->barrel_ec( pr1->identify() );
            float HTOccupancyCorrection = 0;
            if(isElectron){ 
              HTOccupancyCorrection = abs(det) > 1 ? m_HTOccupancyCorrectionEC : m_HTOccupancyCorrectionB;  
            } else { 
              HTOccupancyCorrection = abs(det) > 1 ? m_HTOccupancyCorrectionEC_noE : m_HTOccupancyCorrectionB_noE; 
            }

            if( isXenonStraw && ( occupancy * HTOccupancyCorrection > CLHEP::RandFlat::shoot( m_rndmEngine, 0, 1) ) ) 
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

    out_coll->push_back(p_rdo);
  } // <= while
}






//================================================================
//EOF
