/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

// tool for cluster building
#include "STgcClusterization/ISTgcClusterBuilderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::sTgcRdoToPrepDataTool::sTgcRdoToPrepDataTool(const std::string& t,
						   const std::string& n,
						   const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_stgcIdHelper(0),
  m_muonIdHelper(0),
  m_fullEventDone(false),
  m_stgcPrepDataContainer(0)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);
  
  //  template for property decalration
  declareProperty("OutputCollection",    m_stgcPrepDataContainerKey = std::string("STGC_Measurements"),
		  "Muon::sTgcPrepDataContainer to record");
  declareProperty("InputCollection",    m_rdoContainerKey = std::string("sTGCRDO"),
		  "RDO container to read");
  declareProperty("Merge",  m_merge = true);
  declareProperty("ClusterBuilderTool",m_clusterBuilderTool);
}


Muon::sTgcRdoToPrepDataTool::~sTgcRdoToPrepDataTool()
{

}

StatusCode Muon::sTgcRdoToPrepDataTool::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");
  
  /// get the detector descriptor manager
  StoreGateSvc* detStore=0;
  StatusCode sc = serviceLocator()->service("DetectorStore", detStore);
  
  if (sc.isSuccess()) {
    sc = detStore->retrieve( m_muonMgr );
    if (sc.isFailure()) {
      ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
      return sc;
    }
  } else {
    ATH_MSG_ERROR("DetectorStore not found ");
    return sc;
  }
  
  m_stgcIdHelper = m_muonMgr->stgcIdHelper();

  // check if the initialization of the data container is success
  ATH_CHECK(m_stgcPrepDataContainerKey.initialize());

  ATH_CHECK(m_rdoContainerKey.initialize());


  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::sTgcRdoToPrepDataTool::finalize()
{
  //  if (0 != m_stgcPrepDataContainer) m_stgcPrepDataContainer->release();
  return StatusCode::SUCCESS;

}

StatusCode Muon::sTgcRdoToPrepDataTool::processCollection(const STGC_RawDataCollection *rdoColl, 
							  std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of process STGC Collection");

  const IdentifierHash hash = rdoColl->identifyHash();

  sTgcPrepDataCollection* prdColl = nullptr;
  
  // check if the collection already exists, otherwise add it
  if ( m_stgcPrepDataContainer->indexFind(hash) != m_stgcPrepDataContainer->end() ) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    return StatusCode::FAILURE;
  } 
  else {
    prdColl = new sTgcPrepDataCollection(hash);
    idWithDataVect.push_back(hash);

    // set the offline identifier of the collection Id
    IdContext context = m_stgcIdHelper->module_context();
    Identifier moduleId;
    int getId = m_stgcIdHelper->get_id(hash,moduleId,&context);
    if ( getId != 0 ) {
      ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
    } 
    else {
      prdColl->setIdentifier(moduleId);
    }

    if (StatusCode::SUCCESS != m_stgcPrepDataContainer->addCollection(prdColl, hash)) {
      ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container MM Collection with hashID = "
		    << (int)hash );
      return StatusCode::FAILURE;
    }

  }

  std::vector<sTgcPrepData> sTgcprds;
  std::vector<int> sTgcflag;
  std::vector<sTgcPrepData> sTgcWireprds;
  std::vector<sTgcPrepData> sTgcPadprds;
  // convert the RDO collection to a PRD collection
  STGC_RawDataCollection::const_iterator it = rdoColl->begin();
  for ( ; it != rdoColl->end() ; ++it ) {

    ATH_MSG_DEBUG("Adding a new sTgc PrepRawData");

    const STGC_RawData* rdo = *it;
    const Identifier rdoId = rdo->identify();
//    const Identifier elementId = m_stgcIdHelper->elementID(rdoId);
    std::vector<Identifier> rdoList;
    rdoList.push_back(rdoId);
    
    // get the local and global positions
    const MuonGM::sTgcReadoutElement* detEl = m_muonMgr->getsTgcReadoutElement(rdoId);
    Amg::Vector2D localPos;

    bool getLocalPos = detEl->stripPosition(rdoId,localPos);
    if ( !getLocalPos ) {
      ATH_MSG_ERROR("Could not get the local strip position for sTgc");
      return StatusCode::FAILURE;
    } 

    // get the resolution from strip width
    const int gasGap  = m_stgcIdHelper->gasGap(rdoId);
    const int channel = m_stgcIdHelper->channel(rdoId);

    const int charge = (int) rdo->charge();
    const uint16_t bcTag = rdo->bcTag();

    // to be fixed: for now do not set the resolution, it will be added in the 
    // next update
    double width = 0.;

    int channelType = m_stgcIdHelper->channelType(rdoId);

    ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel 
		  << " type: " << channelType );

    if (channelType ==0) { // Pads
      const MuonGM::MuonPadDesign* design = detEl->getPadDesign(rdoId);
      if (!design) {
        ATH_MSG_WARNING("Failed to get design for sTGC pad" );
      } else {
        double widthOLD = design->channelWidth(localPos,true);  //channelWidth returns the Phi Angle for pads, does not account for staggering.
        width = design->inputPhiPitch*M_PI/180.*(detEl->globalPosition().perp());
        ATH_MSG_DEBUG( " Pad old width " << widthOLD << " new width " << width); 
      } 
    } else if (channelType == 1) { // Strips 
      const MuonGM::MuonChannelDesign* design = detEl->getDesign(rdoId);
      if (!design) ATH_MSG_WARNING("Failed to get design for sTGC strip " );
      else 
        width = design->inputPitch;
    } else if (channelType == 2) { // Wires
      const MuonGM::MuonChannelDesign* design = detEl->getDesign(rdoId);
      if (!design) {
        ATH_MSG_WARNING("Failed to get design for sTGC wire" );
      } else { 
        double widthOLD = design->channelWidth(localPos);
// scale error 
        width = design->groupWidth*fabs(design->inputPitch);
        ATH_MSG_DEBUG( " Wires old width " << widthOLD << " new width " << width); 
      } 
   }

    else {
       ATH_MSG_ERROR("Invalid channel Type for sTGC");
       return StatusCode::FAILURE;
    }

    double resolution = width/sqrt(12.); 

    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;  

    ATH_MSG_DEBUG("Adding a new STGC PRD, gasGap: " << gasGap << " channel: " << channel << " type: " << channelType << " resolution " << resolution );

    if(m_merge) {
// eta strips 
      if(channelType==1) {
        sTgcflag.push_back(0);
        sTgcprds.push_back(sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,bcTag));
      } else if (channelType==2) { 
// wires
        sTgcWireprds.push_back(sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,bcTag));
      } else if (channelType==0) { 
// pads 
        sTgcPadprds.push_back(sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,bcTag));
      }
    } else {
      prdColl->push_back(new sTgcPrepData(rdoId,hash,localPos,rdoList,cov,detEl,charge,bcTag));
    } 
  } //end it = rdoColl



  if(m_merge) {

// merge the eta and phi prds that fire closeby strips

    for (unsigned int j=0; j<3; ++j){
// j == 0 loop over eta strips prds
// j == 1 loop over phi wire prds 
// j == 2 loop over pad prds 
      int stripDifference = 2;
      if(j==1) {
        sTgcprds.insert(sTgcprds.end(), sTgcWireprds.begin(), sTgcWireprds.end());
        for (unsigned int i=0; i<sTgcWireprds.size(); ++i){
          sTgcflag.push_back(0);
        }
      }
      if(j==2) {
        sTgcprds.insert(sTgcprds.end(), sTgcPadprds.begin(), sTgcPadprds.end());
        for (unsigned int i=0; i<sTgcPadprds.size(); ++i){
          sTgcflag.push_back(0);
        }
// merge only same channel/strip numbers
        stripDifference = 1;
      }
      for (unsigned int i=0; i<sTgcprds.size(); ++i){
         // skip the merged prds
         if(sTgcflag[i]==1) continue;
 
         bool merge = false;
         unsigned int jmerge = -1;
         Identifier id_prd = sTgcprds[i].identify();
         int strip = m_stgcIdHelper->channel(id_prd);
         int gasGap  = m_stgcIdHelper->gasGap(id_prd);
         int layer   = m_stgcIdHelper->multilayer(id_prd);
         int channelType = m_stgcIdHelper->channelType(id_prd);
         ATH_MSG_VERBOSE("  sTgcprds " <<  sTgcprds.size() <<" index "<< i << " strip " << strip << " gasGap " << gasGap << " layer " << layer << " channelType " << channelType);
         for (unsigned int j=i+1; j<sTgcprds.size(); ++j){
           Identifier id_prdN = sTgcprds[j].identify();
           int stripN = m_stgcIdHelper->channel(id_prdN);
           int gasGapN  = m_stgcIdHelper->gasGap(id_prdN);
           int layerN   = m_stgcIdHelper->multilayer(id_prdN);
           int channelTypeN = m_stgcIdHelper->channelType(id_prdN);
           if( gasGapN==gasGap && layerN==layer && channelType == channelTypeN) {
             ATH_MSG_VERBOSE(" next sTgcprds strip same gasGap and layer index " << j << " strip " << stripN << " gasGap " << gasGapN << " layer " << layerN );
             if(abs(strip-stripN)<stripDifference) {
               merge = true;
               jmerge = j;
               break;
             }
           }
         }
 
         if(!merge) {
           ATH_MSG_VERBOSE(" add isolated sTgcprds strip " << strip << " gasGap " << gasGap << " layer " << layer << " channelType " << channelType);
           std::vector<Identifier> rdoList;
           rdoList.push_back(id_prd);
           double covX = sTgcprds[i].localCovariance()(Trk::locX,Trk::locX);
           Amg::MatrixX* covN = new Amg::MatrixX(1,1);
           covN->setIdentity();
           (*covN)(0,0) = covX;
           sTgcPrepData* prdN = new sTgcPrepData(id_prd, hash, sTgcprds[i].localPosition(), rdoList, covN, sTgcprds[i].detectorElement(), sTgcprds[i].charge(), sTgcprds[i].getBcBitMap());
           prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
           prdColl->push_back(prdN);
         } else {
           unsigned int nmerge = 0;
           std::vector<Identifier> rdoList;
           std::vector<unsigned int> mergeIndices;
           std::vector<int> mergeStrips;
           rdoList.push_back(id_prd);
           sTgcflag[i] = 1;
           mergeIndices.push_back(i);
           mergeStrips.push_back(strip);
           unsigned int nmergeStrips = 1;
           unsigned int nmergeStripsMax = 25;
           for (unsigned int k=0; k < nmergeStripsMax; ++k) {
             for (unsigned int j=jmerge; j<sTgcprds.size(); ++j){
               if(sTgcflag[j] == 1) continue;
               Identifier id_prdN = sTgcprds[j].identify();
               int stripN = m_stgcIdHelper->channel(id_prdN);
               if( abs(mergeStrips[k]-stripN) <= 1 ) {
                 int gasGapN  = m_stgcIdHelper->gasGap(id_prdN);
                 int layerN   = m_stgcIdHelper->multilayer(id_prdN);
                 int channelTypeN = m_stgcIdHelper->channelType(id_prdN);
                 if( gasGapN==gasGap && layerN==layer && channelType==channelTypeN ) {
                   if(mergeStrips[k]==stripN) {
                     sTgcflag[j] = 1;
                     continue;
                   } 
                   nmerge++;
                   rdoList.push_back(id_prdN);
                   sTgcflag[j] = 1;
                   mergeIndices.push_back(j);
                   mergeStrips.push_back(stripN);
                   nmergeStrips++;
                 }
               }
             }
             if(k>=nmergeStrips) break;
           }
           ATH_MSG_VERBOSE(" add merged sTgcprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer );
 
           // start off from strip in the middle
          int stripSum = 0;
           for (unsigned int k =0; k<mergeStrips.size(); ++k) {
             stripSum += mergeStrips[k];
           }
           stripSum = stripSum/mergeStrips.size();
 
           unsigned int j = jmerge;
           for (unsigned int k =0; k<mergeStrips.size(); ++k) {
             if(mergeStrips[k]==stripSum) j = mergeIndices[k];
             ATH_MSG_VERBOSE(" merged strip nr " << k <<  " strip " << mergeStrips[k] << " index " << mergeIndices[k]);
           }
           ATH_MSG_VERBOSE(" Look for strip nr " << stripSum << " found at index " << j);
 
           double covX = sTgcprds[j].localCovariance()(Trk::locX, Trk::locX);
           Amg::MatrixX* covN = new Amg::MatrixX(1,1);
           covN->setIdentity();
           (*covN)(0,0) = 6.*(nmerge + 1.)*covX;
           if(nmerge<=1 || stripDifference==1) (*covN)(0,0) = covX;
           ATH_MSG_VERBOSE(" make merged prepData at strip " << m_stgcIdHelper->channel(sTgcprds[j].identify())  << " channelType " << channelType << " nmerge " << nmerge << " sqrt covX " << sqrt((*covN)(0,0)));
 
           sTgcPrepData* prdN = new sTgcPrepData(sTgcprds[j].identify(), hash, sTgcprds[j].localPosition(), rdoList, covN, sTgcprds[j].detectorElement(), sTgcprds[j].charge(), sTgcprds[j].getBcBitMap());
           prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
           prdColl->push_back(prdN);
         }
       } // end loop sTgcprds[i]
       // clear vector and delete elements
       sTgcflag.clear();
       sTgcprds.clear();
     } // loop over eta and Wire prds
     sTgcWireprds.clear();
     sTgcPadprds.clear();
  }


  return StatusCode::SUCCESS;
}


Muon::sTgcRdoToPrepDataTool::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataTool::setupSTGC_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::sTgcPrepDataContainer>(m_stgcPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_stgcIdHelper->module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_stgcPrepDataContainer = handle.ptr();
    return ADDED;
    
  }
  return ALREADYCONTAINED;
}


const STGC_RawDataContainer* Muon::sTgcRdoToPrepDataTool::getRdoContainer() {

  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("STGC_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of STGC_RawDataContainer failed !");

  return nullptr;
}


void Muon::sTgcRdoToPrepDataTool::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ) 
{


  ATH_MSG_DEBUG("In processRDOContainer");
  const STGC_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  // run in unseeded mode
  for (STGC_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "STGC Hits");

    if(processCollection(rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::sTgcRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, 
						std::vector<IdentifierHash>& idWithDataVect )
{

  ATH_MSG_DEBUG("Size of the input hash id vector: " << idVect.size());

  // clear the output vector of selected data
  idWithDataVect.clear();
  
  SetupSTGC_PrepDataContainerStatus containerRecordStatus = setupSTGC_PrepDataContainer();

  if ( containerRecordStatus == FAILED ) {
    return StatusCode::FAILURE;
  } 

  processRDOContainer(idWithDataVect);

  // check if the full event has already been decoded
  if ( m_fullEventDone ) {
    ATH_MSG_DEBUG ("Full event dcoded, nothing to do");
    return StatusCode::SUCCESS;
  } 


  return StatusCode::SUCCESS;
} 

StatusCode Muon::sTgcRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}

// printout methods
void Muon::sTgcRdoToPrepDataTool::printInputRdo() {


  return;
}


void Muon::sTgcRdoToPrepDataTool::printPrepData() {


  return;
}

