/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCgeometry/CSCgeometrySvc.h"
#include "StoreGate/StoreGateSvc.h"
///#include "CSCcablingInterface/ICSCcablingServerSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "CSCcabling/CSCcablingSvc.h"

#include "GaudiKernel/SvcFactory.h"
#include "PathResolver/PathResolver.h"

#include <unistd.h>
#include <iostream>

using namespace MuonGM;
using namespace std;

CSCgeometrySvc::CSCStripInfo CSCgeometrySvc::m_NULL_CSCStripInfo;

// Instantiation.
CSCgeometrySvc::CSCgeometrySvc(const std::string& name, ISvcLocator* sl) :
  Service(name, sl),
  m_CSCStripInfo(NULL)
{
  // Define properties to be used in .py file.
  m_fileName = "CSCgeometry.lut";
  m_writeLUTFile = false;
  declareProperty("FileName", m_fileName);
  declareProperty("WriteLUTFile", m_writeLUTFile);
}

// queryInterface
StatusCode CSCgeometrySvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
    if (IID_ICSCgeometrySvc.versionMatch(riid) )  {
      *ppvIF = (CSCgeometrySvc*)this;
    }
    else {
        MsgStream log(messageService(), name());
      log << MSG::DEBUG << name() << " cannot find the interface!" <<endreq;
      return Service::queryInterface(riid, ppvIF);
    }
    return StatusCode::SUCCESS;
}

// Initialize CSCgeometry service
StatusCode CSCgeometrySvc::initialize()
{
  StatusCode sc;

  // Initialize service
  sc = Service::initialize();
  if (sc.isFailure())  return sc;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing " << name() << " - package version " 
      << PACKAGE_VERSION << endreq;

  // Create lookup tables for accessing CSC geometry
  sc = InitCscChamberInfo();
  
  if(sc.isFailure()) return sc;

  // Write out LUT to file if requested.
  if (m_writeLUTFile) {
    sc = rwLUTFile(LUT_WRITE);
    if (sc.isFailure()) {
      log << MSG::ERROR << "WriteLUTFile procedure failed!" << endreq;
      return sc;
    }
  }

  // Done.
  if (log.level() <= MSG::DEBUG) 
    log << MSG::DEBUG << "initialize done" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode CSCgeometrySvc::rwLUTFile(const int rw)
{
  MsgStream log(msgSvc(), name());

  // Try to open the file and store/read the number of entries
  std::string fileName = PathResolver::find_file(m_fileName, "DATAPATH");
  FILE *file_LUT;
  if (rw == LUT_WRITE) {
    file_LUT = fopen(fileName.c_str(), "wb");
    if (!file_LUT) return StatusCode::FAILURE;

  }
  else {
    file_LUT = fopen(fileName.c_str(), "rb");
    if (!file_LUT) return StatusCode::FAILURE;
  }

  log << MSG::INFO << name() << " Opened file " << fileName << endreq;

  // Store the ROD/ROB id lookup here.
  int CSCsize_pos, CSCsize_neg;
  
  if (rw == LUT_WRITE) {
    CSCsize_pos = m_CSCChamber_pos.size();
    CSCsize_neg = m_CSCChamber_neg.size();
    fwrite(&CSCsize_pos, 1, sizeof(CSCsize_pos), file_LUT);
    fwrite(&CSCsize_neg, 1, sizeof(CSCsize_neg), file_LUT);
  }
  else {
    fread(&CSCsize_pos, 1, sizeof(CSCsize_pos), file_LUT);
    fread(&CSCsize_neg, 1, sizeof(CSCsize_neg), file_LUT);
  }

  // Loop over the eta=+1 chambers.
  for (int icham=0; icham<CSCsize_pos; icham++) {
    if (rw == LUT_WRITE) {
      fwrite(&m_CSCChamber_pos[icham], sizeof(CSCGeoLookup), 1, file_LUT);
    }
    else {
      CSCGeoLookup geo;
      fread(&geo, sizeof(CSCGeoLookup), 1, file_LUT);
      m_CSCChamber_pos.push_back(geo);
    }
  }

  // Loop over the eta=-1 chambers.
  for (int icham=0; icham<CSCsize_neg; icham++) {
    if (rw == LUT_WRITE) {
      fwrite(&m_CSCChamber_neg[icham], sizeof(CSCGeoLookup), 1, file_LUT);
    }
    else {
      CSCGeoLookup geo;
      fread(&geo, sizeof(CSCGeoLookup), 1, file_LUT);
      m_CSCChamber_neg.push_back(geo);
    }
  }

  // Store the CSCStripInfo LUT now.

  if (rw == LUT_WRITE) {
    // Store size of LUT
    fwrite(&m_strips, 1, sizeof(m_strips), file_LUT);
  }
  else {
    // Read size of LUT
    fread(&m_strips, sizeof(m_strips), 1, file_LUT);
    m_CSCStripInfo = new CSCStripInfo[m_strips];
    memset(m_CSCStripInfo, 0, m_strips * sizeof(CSCStripInfo));
  }


  // loop over all ids and store info.
  for (int id=0; id<m_strips; id++) {

    // read/write the LUT entry
    if (rw == LUT_WRITE) {
      fwrite(&m_CSCStripInfo[id], sizeof(CSCStripInfo), 1, file_LUT);
    }
    else {
      CSCStripInfo info;
      fread(&info, sizeof(CSCStripInfo), 1, file_LUT);
      m_CSCStripInfo[id] = info;
    }
  }

  // close file
  fclose(file_LUT);

  log << MSG::INFO << name() << " Closed LUT file." << endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode CSCgeometrySvc::finalize()
{
  // Delete anything created in service.
  delete [] m_CSCStripInfo;

  // Done
  return StatusCode::SUCCESS;
}


/** Initialize ROD information from MuonGeoModel **/
StatusCode CSCgeometrySvc::InitCscChamberInfo() {

  CSCcablingSvc* cscCablingSvc=NULL;
  MsgStream log(msgSvc(),name());

  const MuonDetectorManager* p_MuonMgr;
  StatusCode status = StatusCode::SUCCESS;

  // Clear the CSCChamber vectors.
  m_CSCChamber_pos.clear();
  m_CSCChamber_neg.clear();

  // check to see whether LUT file exits. If so, open it and read the LUT.
  std::string fileName = PathResolver::find_file(m_fileName, "DATAPATH");
  if (!fileName.empty()) {

    log << MSG::INFO << name() << " Opening file " << fileName << " to obtain LUT." << endreq;
    StatusCode sc = rwLUTFile(LUT_READ);

    if (sc.isFailure()) {
      log << MSG::ERROR << "Failure to read CSCgeometrySvc LUT." << endreq;
    }

    // Done
    return sc;
  }

  // Retrieve Detector Store.
  StoreGateSvc* detStore=0;
  status = service("DetectorStore", detStore);

  // pointer to CSCIdHelper
  const CscIdHelper  *p_CscIdHelper = NULL;

  if ( status.isSuccess() ) {
    status = detStore->retrieve( p_MuonMgr );
    if ( status.isSuccess() ) p_CscIdHelper = p_MuonMgr->cscIdHelper();
    else log << MSG::ERROR << "Failed to retrieve Detector Store!" << endreq;
  }
  else log << MSG::ERROR << "Failed to retrieve Detector Store!" << endreq;

  // Get CSC cabling service.
  if (StatusCode::SUCCESS != service("CSCcablingSvc", cscCablingSvc))  {
    log << MSG::ERROR << "Failed to get CscCablingSvc!" << endreq;
  }
  else log <<MSG::INFO <<"Retrieved CscCablingSvc." << endreq;


  std::vector<Identifier>::const_iterator  idfirst = p_CscIdHelper->detectorElement_begin();
  std::vector<Identifier>::const_iterator  idlast =  p_CscIdHelper->detectorElement_end();

  // Use module context to get list of detector elements.
  IdContext cscDetElemContext = p_CscIdHelper->module_context();

  // Loop over all modules.        
  for (std::vector<Identifier>::const_iterator i = idfirst; i != idlast; i++)
  {
    Identifier Id = *i;

    // Get hash ID.
    IdentifierHash Idhash;
    p_CscIdHelper->get_hash(Id, Idhash, &cscDetElemContext);

    //    log <<MSG::INFO << "Idhash: " << Idhash << " Id: " << Id << endreq;
 
    // Here is the module 
    const CscReadoutElement* _csc = p_MuonMgr->getCscReadoutElement(Id);
    if (_csc == NULL) continue;
        
    // Find the center of the chamber.
    Amg::Vector3D cscPos = _csc->center();

    // Radius
    double rminMod = cscPos.perp() - _csc->getRsize()/2.0;
    double rmaxMod = cscPos.perp() + _csc->getRsize()/2.0;

    // Find minimum and maximum phi of chamber.
    double dphi = atan2(_csc->getLongSsize()/2., rmaxMod);
    double phi0 = atan2(cscPos.y(), cscPos.x());
    double phimin = phi0 - dphi;
    double phimax = phi0 + dphi;
    if (phimin<0) phimin = phimin + 2*M_PI;
    if (phimax<0) phimax = phimax + 2*M_PI;

    // Determine inner and outer points on chamber.
    double xminMod = rminMod * cos(phi0);
    double xmaxMod = rmaxMod * cos(phi0);

    double yminMod = rminMod * sin(phi0);
    double ymaxMod = rmaxMod * sin(phi0);

    double zminMod = cscPos.z()-_csc->getZsize()/2.;
    double zmaxMod = cscPos.z()+_csc->getZsize()/2.;

    // Set the min and max points
    Amg::Vector3D Pzmin = cscPos;
    Pzmin[0] = xminMod;
    Pzmin[1] = yminMod;
    Pzmin[2] = zminMod;

    Amg::Vector3D Pzmax = cscPos;
    Pzmax[0] = xmaxMod;
    Pzmax[1] = ymaxMod;
    Pzmax[2] = zmaxMod;
        
    // Determine minimum and maximum eta.
    double eta_min = Pzmin.eta();
    double eta_max = Pzmax.eta();
    if (eta_min > eta_max) {
      double tmp_eta = eta_min;
      eta_min = eta_max; eta_max = tmp_eta;
    }

    // Here are the station eta and phi.
    int seta = _csc->getStationEta();
    int sphi = _csc->getStationPhi();

    // Now fill vector accordingly.
    std::vector <CSCGeoLookup> *CSCChamber;
    CSCGeoLookup chamber_info;
    uint32_t subdet;
    if (seta < 0) {
      CSCChamber = &m_CSCChamber_neg;
      subdet = 0x6A;
    }
    else {
      CSCChamber = &m_CSCChamber_pos;
      subdet = 0x69;
    }

    // This assumes a certain correspondence between RODs and phi sectors.
    int irod = sphi-1; 
    uint16_t online_rod;

    // Get online ROD id from offline rod ID.
    if (cscCablingSvc->onlineId(irod, online_rod)){

      chamber_info.phimin = phimin;
      chamber_info.phimax = phimax;
      chamber_info.subdet = subdet;
      chamber_info.rodID  = online_rod;
      chamber_info.hashID = Idhash;

      // Add structure to chamber list.
      CSCChamber->push_back(chamber_info);

    }  
  } // end loop over modules (chambers).

  // Loop over the rods and print out the min and max phi.
  log<< MSG::DEBUG << "eta < 0 ROD info" << endreq;
  for (int i=0; i<(int)cscCablingSvc->nROD(); i++) {
    log<< MSG::DEBUG << "Phi min: " << m_CSCChamber_neg[i].phimin << " Phi max: "
	      << m_CSCChamber_neg[i].phimax << endreq;
  }
  log<< MSG::DEBUG << "eta > 0 ROD info" << endreq;
  for (int i=0; i<(int)cscCablingSvc->nROD(); i++) {
    log << MSG::DEBUG << "Phi min: " << m_CSCChamber_pos[i].phimin << " Phi max: "
	      << m_CSCChamber_pos[i].phimax << endreq;
  }

  /////////////////////////////////////////////////
  // Create CSC LUT here.
  /////////////////////////////////////////////////

  m_strips = p_CscIdHelper->channel_hash_max();
  log << MSG::INFO << "Initialize LUT to size: " << m_strips << endreq;
  m_CSCStripInfo = new CSCStripInfo[m_strips];
  memset(m_CSCStripInfo, 0, m_strips * sizeof(CSCStripInfo));

  std::vector<Identifier>::const_iterator isfirst = p_CscIdHelper->channel_begin();
  std::vector<Identifier>::const_iterator islast  = p_CscIdHelper->channel_end();

  // Loop over all possible strips and fill the lookup table.
  for (std::vector<Identifier>::const_iterator istrip=isfirst; 
       istrip != islast; istrip++) {

    // Get strip hash id.
    Identifier Id = *istrip;
    IdentifierHash stripHashId;
    p_CscIdHelper->get_channel_hash(Id, stripHashId);


    // Check to see whether this is a valid strip.
    if (p_CscIdHelper->valid(Id)) {

      const CscReadoutElement* csc_strip = p_MuonMgr->getCscReadoutElement(Id);

      // Check whether this strip (chamber layer) exists.
      if (csc_strip != NULL) {

	//        int chamberLayer = p_CscIdHelper->chamberLayer(Id);

        // Get appropriate information and store in LUT.
        Amg::Vector3D pos = csc_strip->stripPos(Id);
        int measPhi    = p_CscIdHelper->measuresPhi(Id);

        // Fill LUT entry
        m_CSCStripInfo[stripHashId].eta     = pos.eta();
        m_CSCStripInfo[stripHashId].phi     = pos.phi();
        m_CSCStripInfo[stripHashId].r       = pos.perp();
        m_CSCStripInfo[stripHashId].z       = pos.z();
        m_CSCStripInfo[stripHashId].measPhi = measPhi;
        m_CSCStripInfo[stripHashId].width   = csc_strip->StripWidth(measPhi);
      }
    } // end if valid
  } // end loop over all strips.

  return StatusCode::SUCCESS;
}

/** Returns both ROB(ROD) ID and hash ID for a given eta and  phi **/
/** There can be more than one chamber returned since chambers overlap **/
void CSCgeometrySvc::CSCIdList(float eta, float phi, CSCIDInfo *IDs) const {

  MsgStream log(msgSvc(),name());

  // Look at either the positive or negative table.
  std::vector <CSCGeoLookup>::const_iterator roditr, rodend;
  if (eta < 0) {
    roditr = m_CSCChamber_neg.begin();
    rodend   = m_CSCChamber_neg.end();
  }
  else {
    roditr = m_CSCChamber_pos.begin();
    rodend = m_CSCChamber_pos.end();
  }

  int nloop = 0;
  // Loop over all chambers and see if phi points into chamber.
  for (; roditr != rodend; roditr++) {
    nloop++;


    float pmin = (*roditr).phimin;
    float pmax = (*roditr).phimax;

    // Check if phi falls within chamber coverage.
    bool addId = false;

    // Handle cases where ROD straddles phi=0
    if (pmin > pmax) {
      if (phi > pmin || phi < pmax)    addId = true;
    }

    // Default case
    else if (phi > pmin && phi < pmax) addId = true;

    if (addId) {
      // Structure for holding information.
      CSCRobHash idinfo;

      // Create ROBid and add to IDs.
      uint32_t ROBid = ((*roditr).subdet << 16) | (*roditr).rodID;
      idinfo.robID  = ROBid;
      idinfo.hashID = (*roditr).hashID;
  

      log << MSG::DEBUG << " subdet: " << hex << (*roditr).subdet
	  << " rodID: " << (*roditr).rodID 
          << " ROBid: " << hex << ROBid 
	  << endreq;

      // Add to IDs vector.
      IDs->push_back(idinfo);
    }
  }  // end of loop over chambers.

}


/** Returns CSCStripInfo structure for a given strip hash ID. **/
const CSCgeometrySvc::CSCStripInfo & 
  CSCgeometrySvc::CSCGetInfo( IdentifierHash striphashID ) const{

  // Initialize log stream
  MsgStream log(msgSvc(), name());

  // Check bounds of striphashID
  if ((int) striphashID < 0 || (int)striphashID >= CSCgeometrySvc::m_strips) {
    log << MSG::ERROR << "Invalid strip hash ID: " << striphashID << endreq;
    return m_NULL_CSCStripInfo;
  }

  // Otherwise just get the information from our lookup table.
  return m_CSCStripInfo[striphashID];
}
