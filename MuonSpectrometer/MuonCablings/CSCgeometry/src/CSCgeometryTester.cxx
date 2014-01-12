/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "StoreGate/StoreGateSvc.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "CSCgeometry/CSCgeometryTester.h"
#include "CSCgeometry/CSCgeometrySvc.h"
#include <iostream>

using namespace MuonGM;
using namespace std;

CSCgeometryTester::CSCgeometryTester(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_pCSCgeometrySvc(NULL)
{
}

StatusCode CSCgeometryTester::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;

  // initialize CSCgeometrySvc
  StatusCode sc = serviceLocator()->service("CSCgeometrySvc", m_pCSCgeometrySvc);
  if (sc.isFailure() || m_pCSCgeometrySvc == NULL)
    {
      log << MSG::ERROR << "Cannot retrieve service CSCgeometrySvc" << endreq;
      return sc;
    }


  return StatusCode::SUCCESS;
}

StatusCode CSCgeometryTester::execute()
{
  MsgStream log(msgSvc(), name());
  if (log.level() <= MSG::DEBUG)
    log << MSG::DEBUG << "Executing " << name() << endreq;

  if (m_pCSCgeometrySvc) {
    CSCgeometrySvc::CSCIDInfo *idinfo;
    idinfo = new CSCgeometrySvc::CSCIDInfo();

    float eta, phi;
    const int nstep = 100;
    const float TWOPI = 3.141592654*2.0;
    const float step = TWOPI/(float)(nstep/2.0);

    // Loop over eta and phi to get a printout of the rodIDs and the
    // hashIds.
    eta = -2.2;
    phi = 0.0;
    for (int i=0; i<nstep; i++) {

      // Clear idinfo vector.
      idinfo->clear();

      // Look at the other wheel.
      if (i == nstep/2) {
        eta = 2.2;
        phi = 0.0;
      }

      phi += step;
      
      // Get list of ROB and hash IDs for these values of eta & phi.
      m_pCSCgeometrySvc->CSCIdList(eta, phi, idinfo);

      // Write out ROB and Hash IDs.
      int maxsize = (int)idinfo->size();
      for (int i=0; i<maxsize; i++) {
        log << MSG::INFO << "entry: " << i 
	    << " eta:    " << eta 
	    << " phi:    " << phi
            << " ROD id: " <<  hex << idinfo->at(i).robID 
	    << " HashId: " << dec  <<idinfo->at(i).hashID << endreq;
      }
    }

    delete idinfo;

    CSCgeometrySvc::CSCStripInfo Info;
    int nerror = 0, nentry = 0;

    // Retrieve Detector Store.
    StoreGateSvc* detStore=0;
    StatusCode status = service("DetectorStore", detStore);

    const MuonDetectorManager* p_MuonMgr;
    const CscIdHelper  *p_CscIdHelper = NULL;
    if ( status.isSuccess() ) {
      status = detStore->retrieve( p_MuonMgr );
      if ( status.isSuccess() ) p_CscIdHelper = p_MuonMgr->cscIdHelper();
      else log << MSG::ERROR << "Failed to retrieve Detector Store!" << endreq;
    }
    else log << MSG::ERROR << "Failed to retrieve Detector Store!" << endreq;

    // Check all entries of the CSC lookup table to see that there
    // is a valid entry.

    for (int ista=0; ista<=1; ista++) {
      std::string station="CSS";
      if (ista == 1) station = "CSL";

      for (int ieta=-1; ieta<=1; ieta+=2) {
        for (int iphi=1; iphi<=8; iphi++) {
          for (int ilayer=1; ilayer<=4; ilayer++) {
            for (int measPhi=0; measPhi<=1; measPhi++) {
              int maxStrip = 192;
	      if (measPhi==1) maxStrip=48;
              for (int istrip=1; istrip<=maxStrip; istrip++) {

		// Get identifier associated with this strip
                Identifier stripId = p_CscIdHelper->channelID(station, ieta, iphi, 1, 
							      ilayer, measPhi, istrip);

		// Get hash id for this strip
                IdentifierHash Idhash;
                p_CscIdHelper->get_channel_hash(stripId, Idhash);

		// Look up CSC lookup entry
                Info = m_pCSCgeometrySvc->CSCGetInfo(Idhash);
                nentry++;

		if ( (nentry%500) == 0 ) {
                  log << MSG::INFO 
		      << " eta: " << ieta
		      << " phi: " << iphi
		      << " layer: " << ilayer
		      << " measPhi: " << measPhi
		      << " strip: " << istrip
		      << endreq;
		  log << MSG::INFO
		      << " eta: " << Info.eta
		      << " phi: " << Info.phi
		      << " measPhi: " << Info.measPhi
		      << endreq;
		}

		// See if this makes sense.
                if (Info.r == 0 || Info.z == 0) {
                  log << MSG::INFO << "Invalid entry for hash id: " << Idhash
		      << " eta: " << ieta
		      << " phi: " << iphi
		      << " layer: " << ilayer
		      << " measPhi: " << measPhi
		      << " strip: " << istrip
		      << endreq;
                  nerror++;
		}

	      } // loop over strips
	    } // loop over measPhi
	  } // loop over layers
	} // loop over station phi
      } // loop over station eta
    } // loop over station

    log << MSG::INFO << "Found " << nentry << " entries with " 
	<< nerror << " errors in CSC LUT." << endreq;
  } // if service

  return StatusCode::SUCCESS;
}

StatusCode CSCgeometryTester::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Finalizing " << name() << " - package version " << PACKAGE_VERSION << endreq;

  return StatusCode::SUCCESS;
}
