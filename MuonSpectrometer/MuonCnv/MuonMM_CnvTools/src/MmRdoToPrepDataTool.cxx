/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MmRdoToPrepDataTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "TrkEventPrimitives/LocalDirection.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::MmRdoToPrepDataTool::MmRdoToPrepDataTool(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_mmIdHelper(0),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_fullEventDone(false),
  m_mmPrepDataContainer(0) 
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  //  template for property decalration
  declareProperty("OutputCollection",    m_mmPrepDataContainerKey = std::string("MM_Measurements"),
		  "Muon::MMPrepDataContainer to record");
  declareProperty("InputCollection",    m_rdoContainerKey = std::string("MMRDO"),
		  "Muon::MMPrepDataContainer to record");
  
  declareProperty("MergePrds", m_merge = true);
}


Muon::MmRdoToPrepDataTool::~MmRdoToPrepDataTool()
{

}

StatusCode Muon::MmRdoToPrepDataTool::initialize()
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
  
  m_mmIdHelper = m_muonMgr->mmIdHelper();
  
  ATH_CHECK( m_idHelperTool.retrieve() );

  // check if the initialization of the data container is success
  ATH_CHECK(m_mmPrepDataContainerKey.initialize());
  ATH_CHECK(m_rdoContainerKey.initialize());

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataTool::finalize()
{
  //  if (0 != m_mmPrepDataContainer) m_mmPrepDataContainer->release();
  return StatusCode::SUCCESS;

}

StatusCode Muon::MmRdoToPrepDataTool::processCollection(const MM_RawDataCollection *rdoColl, 
							std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of process MM Collection");

  const IdentifierHash hash = rdoColl->identifierHash();

  MMPrepDataCollection* prdColl = nullptr;
  
  // check if the collection already exists, otherwise add it
  if ( m_mmPrepDataContainer->indexFind(hash) != m_mmPrepDataContainer->end() ) {

    ATH_MSG_DEBUG("In processCollection: collection already contained in the MM PrepData container");
    return StatusCode::FAILURE;
  } 
  else {
    prdColl = new MMPrepDataCollection(hash);
    idWithDataVect.push_back(hash);
    
    // set the offline identifier of the collection Id
    IdContext context = m_mmIdHelper->module_context();
    Identifier moduleId;
    int getId = m_mmIdHelper->get_id(hash,moduleId,&context);
    if ( getId != 0 ) {
      ATH_MSG_ERROR("Could not convert the hash Id: " << hash << " to identifier");
    } 
    else {
      ATH_MSG_DEBUG(" dump moduleId " << moduleId );
      prdColl->setIdentifier(moduleId);
    }

    if (StatusCode::SUCCESS != m_mmPrepDataContainer->addCollection(prdColl, hash)) {
      ATH_MSG_DEBUG("In processCollection - Couldn't record in the Container MM Collection with hashID = "
		    << (int)hash );
      return StatusCode::FAILURE;
    }

  }

  std::vector<MMPrepData> MMprds;
  std::vector<int> MMflag;

  // convert the RDO collection to a PRD collection
  MM_RawDataCollection::const_iterator it = rdoColl->begin();
  for ( ; it != rdoColl->end() ; ++it ) {

    ATH_MSG_DEBUG("Adding a new MM PrepRawData");

    const MM_RawData* rdo = *it;
    const Identifier rdoId = rdo->identify();
//    const Identifier elementId = m_mmIdHelper->elementID(rdoId);
    ATH_MSG_DEBUG(" dump rdo " << m_idHelperTool->toString(rdoId ));

    const int time = rdo->time();
    const int charge = rdo->charge();
    std::vector<Identifier> rdoList;
    rdoList.push_back(rdoId);

    Identifier parentID = m_mmIdHelper->parentID(rdoId);
    Identifier layid = m_mmIdHelper->channelID(parentID, m_mmIdHelper->multilayer(rdoId), m_mmIdHelper->gasGap(rdoId),1);
    // get the local and global positions
    const MuonGM::MMReadoutElement* detEl = m_muonMgr->getMMReadoutElement(layid);

    Amg::Vector2D localPos;

    bool getLocalPos = detEl->stripPosition(rdoId,localPos);
    if ( !getLocalPos ) {
      ATH_MSG_ERROR("Could not get the local strip position for MM");
      return StatusCode::FAILURE;
    }
    int stripNumberRDOId = detEl->stripNumber(localPos,layid);
    ATH_MSG_DEBUG(" check strip nr RDOId " << stripNumberRDOId );
    Amg::Vector3D globalPos;
    bool getGlobalPos = detEl->stripGlobalPosition(rdoId,globalPos);
    if ( !getGlobalPos ) {
      ATH_MSG_ERROR("Could not get the global strip position for MM");
      return StatusCode::FAILURE;
    }

//    const Trk::Surface& surf = detEl->surface(rdoId);
//    const Amg::Vector3D* globalPos = surf.localToGlobal(localPos);
    const Amg::Vector3D globalDir(globalPos.x(), globalPos.y(), globalPos.z());
    Trk::LocalDirection localDir;
    const Trk::PlaneSurface& psurf = detEl->surface(layid);
    Amg::Vector2D lpos;
    psurf.globalToLocal(globalPos,globalPos,lpos);
    psurf.globalToLocalDirection(globalDir, localDir);
    float inAngle_XZ = fabs( localDir.angleXZ() / CLHEP::degree);
   
    ATH_MSG_DEBUG(" Surface centre x " << psurf.center().x() << " y " << psurf.center().y() << " z " << psurf.center().z() );
    ATH_MSG_DEBUG(" localPos x " << localPos.x() << " localPos y " << localPos.y() << " lpos recalculated 0 " << lpos[0] << " lpos y " << lpos[1]);

    Amg::Vector3D  gdir = psurf.transform().linear()*Amg::Vector3D(0.,1.,0.);
    ATH_MSG_DEBUG(" MM detector surface direction phi " << gdir.phi() << " global radius hit " << globalPos.perp() << " phi pos " << globalPos.phi() << " global z " << globalPos.z());      

    // get for now a temporary error matrix -> to be fixed
    double resolution = 0.07;
    if (fabs(inAngle_XZ)>3) resolution = ( -.001/3.*fabs(inAngle_XZ) ) + .28/3.;
    double errX = 0;
    const MuonGM::MuonChannelDesign* design = detEl->getDesign(layid);
    if( !design ){
      ATH_MSG_WARNING("Failed to get design for " << m_idHelperTool->toString(layid) );
    }else{
      errX = fabs(design->inputPitch)/sqrt(12);
      ATH_MSG_DEBUG(" strips inputPitch " << design->inputPitch << " error " << errX);
    }
// add strip width to error
    resolution = sqrt(resolution*resolution+errX*errX);

    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
    cov->setIdentity();
    (*cov)(0,0) = resolution*resolution;  

    if(!m_merge) {
      prdColl->push_back(new MMPrepData(rdoId,hash,localPos,rdoList,cov,detEl,time,charge));
    } else {
       MMprds.push_back(MMPrepData(rdoId,hash,localPos,rdoList,cov,detEl,time,charge));
       MMflag.push_back(0);
    } 
  }

  if(m_merge) {
     for (unsigned int i=0; i<MMprds.size(); ++i){
         // skip the merged prds
         if(MMflag[i]==1) continue;
 
         bool merge = false;
         unsigned int jmerge = -1;
         Identifier id_prd = MMprds[i].identify();
         int strip = m_mmIdHelper->channel(id_prd);
         int gasGap  = m_mmIdHelper->gasGap(id_prd);
         int layer   = m_mmIdHelper->multilayer(id_prd);
         ATH_MSG_VERBOSE("  MMprds " <<  MMprds.size() <<" index "<< i << " strip " << strip << " gasGap " << gasGap << " layer " << layer << " z " << MMprds[i].globalPosition().z() );
         for (unsigned int j=i+1; j<MMprds.size(); ++j){
           Identifier id_prdN = MMprds[j].identify();
           int stripN = m_mmIdHelper->channel(id_prdN);
           int gasGapN  = m_mmIdHelper->gasGap(id_prdN);
           int layerN   = m_mmIdHelper->multilayer(id_prdN);
           if( gasGapN==gasGap && layerN==layer ) {
             ATH_MSG_VERBOSE(" next MMprds strip same gasGap and layer index " << j << " strip " << stripN << " gasGap " << gasGapN << " layer " << layerN );
             if(abs(strip-stripN)<2) {
               merge = true;
               jmerge = j;
               break;
             }
           }
         }
 
         if(!merge) {
           ATH_MSG_VERBOSE(" add isolated MMprds strip " << strip << " gasGap " << gasGap << " layer " << layer );
           std::vector<Identifier> rdoList;
           rdoList.push_back(id_prd);
           double covX = MMprds[i].localCovariance()(Trk::locX,Trk::locX);
           Amg::MatrixX* covN = new Amg::MatrixX(1,1);
           covN->setIdentity();
           (*covN)(0,0) = covX;
           MMPrepData* prdN = new MMPrepData(id_prd, hash, MMprds[i].localPosition(), rdoList, covN, MMprds[i].detectorElement());
           prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
           prdColl->push_back(prdN);
         } else {
           unsigned int nmerge = 0;
           std::vector<Identifier> rdoList;
           std::vector<unsigned int> mergeIndices;
           std::vector<int> mergeStrips;
           rdoList.push_back(id_prd);
           MMflag[i] = 1;
           mergeIndices.push_back(i);
           mergeStrips.push_back(strip);
           unsigned int nmergeStrips = 1;
           unsigned int nmergeStripsMax = 25;
           for (unsigned int k=0; k < nmergeStripsMax; ++k) {
             for (unsigned int j=jmerge; j<MMprds.size(); ++j){
               if(MMflag[j] == 1) continue;
               Identifier id_prdN = MMprds[j].identify();
               int stripN = m_mmIdHelper->channel(id_prdN);
               if( abs(mergeStrips[k]-stripN) <= 1 ) {
                 int gasGapN  = m_mmIdHelper->gasGap(id_prdN);
                 int layerN   = m_mmIdHelper->multilayer(id_prdN);
                 if( gasGapN==gasGap && layerN==layer ) {
                   if(mergeStrips[k]==stripN) {
                     MMflag[j] = 1;
                     continue;
                   }
                   nmerge++;
                   rdoList.push_back(id_prdN);
                   MMflag[j] = 1;
                   mergeIndices.push_back(j);
                   mergeStrips.push_back(stripN);
                   nmergeStrips++;
                 }
               }
             }
             if(k>=nmergeStrips) break;
           }
           ATH_MSG_VERBOSE(" add merged MMprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer );
 
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
 
           double covX = MMprds[j].localCovariance()(Trk::locX, Trk::locX);
           Amg::MatrixX* covN = new Amg::MatrixX(1,1);
           covN->setIdentity();
           (*covN)(0,0) = 6.*(nmerge + 1.)*covX;
           if(nmerge<=1) (*covN)(0,0) = covX;
           ATH_MSG_VERBOSE(" make merged prepData at strip " << m_mmIdHelper->channel(MMprds[j].identify()) << " nmerge " << nmerge << " sqrt covX " << sqrt((*covN)(0,0)));

           MMPrepData* prdN = new MMPrepData(MMprds[j].identify(), hash, MMprds[j].localPosition(), rdoList, covN, MMprds[j].detectorElement());
           prdN->setHashAndIndex(prdColl->identifyHash(), prdColl->size());
           prdColl->push_back(prdN);
         }
       } // end loop MMprds[i]
     // clear vector and delete elements
     MMflag.clear();
     MMprds.clear();
  }



  return StatusCode::SUCCESS;
}


Muon::MmRdoToPrepDataTool::SetupMM_PrepDataContainerStatus Muon::MmRdoToPrepDataTool::setupMM_PrepDataContainer() 
{

  if(!evtStore()->contains<Muon::MMPrepDataContainer>(m_mmPrepDataContainerKey.key())){    
    m_fullEventDone=false;
    
    SG::WriteHandle< Muon::MMPrepDataContainer > handle(m_mmPrepDataContainerKey);
    StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(m_mmIdHelper->module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of MicroMega PrepData Container at " << m_mmPrepDataContainerKey.key()); 
      return FAILED;
    }
    m_mmPrepDataContainer = handle.ptr();
    return ADDED;
  }
  return ALREADYCONTAINED;
}


const MM_RawDataContainer* Muon::MmRdoToPrepDataTool::getRdoContainer() {

  auto rdoContainerHandle  = SG::makeHandle(m_rdoContainerKey);
  if(rdoContainerHandle.isValid()) {
    ATH_MSG_DEBUG("MM_getRdoContainer success");
    return rdoContainerHandle.cptr();  
  }
  ATH_MSG_WARNING("Retrieval of MM_RawDataContainer failed !");

  return nullptr;
}


void Muon::MmRdoToPrepDataTool::processRDOContainer( std::vector<IdentifierHash>& idWithDataVect ) 
{

  ATH_MSG_DEBUG("In processRDOContainer");
  const MM_RawDataContainer* rdoContainer = getRdoContainer();
  if (!rdoContainer) {
    return;
  }
  
  // run in unseeded mode
  for (MM_RawDataContainer::const_iterator it = rdoContainer->begin(); it != rdoContainer->end(); ++it ) {
    
    auto rdoColl = *it;
    if (rdoColl->empty()) continue;
    ATH_MSG_DEBUG("New RDO collection with " << rdoColl->size() << "MM Hits");
    if(processCollection(rdoColl, idWithDataVect).isFailure()) {
      ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
    }
  } 
  
  
  return;
}


// methods for ROB-based decoding
StatusCode Muon::MmRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, 
					       std::vector<IdentifierHash>& idWithDataVect )
{
  // clear the output vector of selected data
  idWithDataVect.clear();
  
  //is idVect a right thing to use here? to be reviewed maybe
  ATH_MSG_DEBUG("Size of the RDO container to be decoded: " << idVect.size() ); 

  SetupMM_PrepDataContainerStatus containerRecordStatus = setupMM_PrepDataContainer();

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

StatusCode Muon::MmRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds, 
					       const std::vector<IdentifierHash>& chamberHashInRobs )
{
  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );
  ATH_MSG_DEBUG("Size of the chamberHash" << chamberHashInRobs.size() );

  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{

  ATH_MSG_DEBUG("Size of the robIds" << robIds.size() );

  return StatusCode::SUCCESS;
}


// printout methods
void Muon::MmRdoToPrepDataTool::printInputRdo() {


  return;
}


void Muon::MmRdoToPrepDataTool::printPrepData() {


  return;
}

