/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/MuonTrackRecordValAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "HepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"

#include "TrackRecord/TrackRecord.h"
#include "TrackRecord/TrackRecordCollection.h"
 
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include <TH1.h>

void bookTruthHistograms();

MuonTrackRecordValAlg::MuonTrackRecordValAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),ValAlgInterface(), 
  log( msgSvc(), name )
{ 
  declareProperty("DumpTrackRecord",m_dumpTrackRecord = true); 
  //     declareProperty("threshold",m_cutOff=100.*CLHEP::MeV);
  //     declareProperty("OnlyMuon",m_OnlyMuon=true);
     
  log << MSG::INFO << "++++++++++++  MuonTrackRecordValAlg created  ++++++++++++"
      << endreq;
}

MuonTrackRecordValAlg::~MuonTrackRecordValAlg()
{
  log << MSG::INFO << " deleting MuonTrackRecordValAlg " << endreq;
}

StatusCode MuonTrackRecordValAlg::initialize()
{
  StatusCode status;
     
  // retrieve the StoreGate Service
  status = service("StoreGateSvc",m_sgSvc);
  if (status.isFailure()) {
    std::cout << "Could not retrieve StoreGateSvc!" <<std::endl;
  } else {
    std::cout<< "Retrieved StoreGateSvc." << std::endl;
  }
     
 
  log << MSG::INFO << " initialize " << endreq;
 	
  bookTruthHistograms();
	
  return StatusCode::SUCCESS;
}

StatusCode MuonTrackRecordValAlg::finalize()
{
  log << MSG::INFO << " finalize " << endreq;
	
  return StatusCode::SUCCESS;
}

StatusCode MuonTrackRecordValAlg::execute()
{

  //     if (m_dumpTrackRecord) {

  log << MSG::INFO << " MuonTrackRecordValAlg: execute " << endreq;
  StatusCode status;
	
  TH1 *Pt_truth;
  TH1 *pdgId;
  TH1 *PosX_truth;
  TH1 *PosY_truth;
  TH1 *PosZ_truth;
  TH1 *PX_truth;
  TH1 *PY_truth;
  TH1 *PZ_truth;

  status=tHistSvc()->getHist("/truth/Pt_truth",Pt_truth);
  status=tHistSvc()->getHist("/truth/pdgId",pdgId);
  status=tHistSvc()->getHist("/truth/PosX_truth",PosX_truth);
  status=tHistSvc()->getHist("/truth/PosY_truth",PosY_truth);
  status=tHistSvc()->getHist("/truth/PosZ_truth",PosZ_truth);
  status=tHistSvc()->getHist("/truth/PX_truth",PX_truth);
  status=tHistSvc()->getHist("/truth/PY_truth",PY_truth);
  status=tHistSvc()->getHist("/truth/PZ_truth",PZ_truth);	


  //
  // TrackRecord Hits
  //
 
  const DataHandle <TrackRecordCollection> collTR;
       
  //       std::string location = "MuonEntryRecord"; //for 11 release
  std::string location = "MuonEntryLayer";
  status = m_sgSvc->retrieve(collTR,location);

  if (status.isFailure() ) 
    {
      log << MSG::ERROR << "Could not find TrackRecordCollection at " << location << " trying MuonEntryRecord" << endreq;
      
      location = "MuonEntryRecord";
      status = m_sgSvc->retrieve(collTR,location);
      if (status.isFailure() ) 
	{
	  log << MSG::ERROR << "Could not find TrackRecordCollection at " << location <<endreq;
	} 
    }
  else  if (status.isSuccess()) 
    {
      log<<MSG::DEBUG<<"TrackRecordCollection retrieved "<<collTR->size()<<" TrackRecord hits!" <<endreq;              

      TrackRecordCollection::const_iterator iterTR;
	 
      // loop over all tracks- cbnt_ttr
      for (iterTR=collTR->begin();iterTR!=collTR->end();++iterTR)
	{
	  log << MSG::INFO << "Track found with pdg id= " << (*iterTR)->GetPDGCode() << " with energy "<< (*iterTR)->GetEnergy() << endreq;

	  HepGeom::Point3D<double> vertex = (*iterTR)->GetPosition();
	  HepGeom::Vector3D<double> momentum = (*iterTR)->GetMomentum();
         
	  PDGCode = (*iterTR)->GetPDGCode()                 ;
	  pdgId->Fill(PDGCode);
	  
	  Pt = (*iterTR)->GetEnergy()/sqrt(1.+sinh(momentum.eta())*sinh(momentum.eta()) );
	  Pt_truth->Fill(Pt);	 

	  ///////////////////////////////////////////////////////////////////////
	  //    Vertex Position		 
	  PosX    = vertex.x()            ;
	  PosX_truth->Fill(PosX);
         
	  PosY    = vertex.y()            ;
	  PosY_truth->Fill(PosY);
	 
	  PosZ    = vertex.z()            ;
	  PosZ_truth->Fill(PosZ);

	  //double rhoVertex = vertex.perp()/10. ; 
	  double phiVertex = vertex.phi() ;
	  if ( phiVertex < 0. ) phiVertex += 2.*M_PI; 
	  //double zVertex = vertex.z()/10. ;
	  ///////////////////////////////////////////////////////////////////////	 
	  PX      = Pt*cos(momentum.phi())  ;
	  PX_truth->Fill(PX);
	 
	  PY      = Pt*sin(momentum.phi())  ;
	  PY_truth->Fill(PY);
	 
	  PZ      = Pt*sinh(momentum.eta()) ;	  
	  PZ_truth->Fill(PZ);
            
	  double ttrphi = momentum.phi();
	  if ( ttrphi < 0. ) ttrphi += 2.*M_PI; 
	 
	  //double ttreta = momentum.pseudoRapidity();
	 
	}
	 
    }
  //	 } // m_dumpTrackRecord
  	  
  return StatusCode::SUCCESS;
}


