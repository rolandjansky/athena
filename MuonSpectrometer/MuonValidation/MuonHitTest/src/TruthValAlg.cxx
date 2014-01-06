/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHitTest/TruthValAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "HepMC/GenEvent.h"
#include "GeneratorObjects/McEventCollection.h"

#include <TH1.h>

void bookTruthHistograms();

TruthValAlg::TruthValAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),ValAlgInterface(), 
  log( msgSvc(), name )
{
  log << MSG::INFO << "++++++++++++  TruthValAlg created  ++++++++++++"
      << endreq;
}

TruthValAlg::~TruthValAlg()
{
  log << MSG::INFO << " deleting TruthValAlg " << endreq;
}

StatusCode TruthValAlg::initialize()
{
  log << MSG::INFO << " initialize " << endreq;
  StatusCode status;
	
  bookTruthHistograms();
	
  return StatusCode::SUCCESS;
}

StatusCode TruthValAlg::finalize()
{
  log << MSG::INFO << " finalize " << endreq;
	
  return StatusCode::SUCCESS;
}

StatusCode TruthValAlg::execute()
{

  log << MSG::INFO << " execute " << endreq;
  StatusCode sc;
	
  const DataHandle<McEventCollection> mcCollection;
  if (storeGateSvc()->retrieve(mcCollection,"TruthEvent").isSuccess())
    {
      HepMC::GenEvent *evt;
      McEventCollection::const_iterator it = mcCollection->begin();
      evt=(*it);
		
      HepMC::GenEvent::vertex_const_iterator vit;
      vit=evt->vertices_begin();
      double zVert=(*vit)->position().z();
      std::cout <<" Z of main vertex "<<zVert<<std::endl;
	
      TH1 *vvv;	
      if(tHistSvc()->getHist("/truth/Z_vert",vvv).isFailure()) return sc;
      vvv->Fill(zVert);
    }
	
  return StatusCode::SUCCESS;
}


