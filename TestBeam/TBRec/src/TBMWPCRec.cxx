/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "TBRec/TBMWPCRec.h"

//#include "GaudiKernel/AlgFactory.h"
// static const AlgFactory<TBMWPCRec> s_factory;
// const IAlgFactory& TBMWPCRecFactory = s_factory;





TBMWPCRec::TBMWPCRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  Algorithm(name,pSvcLocator),
  m_StoreGate(0)
 {
  // job options

   declareProperty("SGkey",  m_SGkey="MWPCRawCont");
   declareProperty("SGrecordkey",  m_SGrecordkey="MWPCCont");



   declareProperty("MWPCwirestep",  m_mwpc_wirestep);
   declareProperty("MWPChalfsize",  m_mwpc_halfsize);

   declareProperty("MWPCinvX",  m_mwpc_invX);
   declareProperty("MWPCinvY",  m_mwpc_invY);

 }

TBMWPCRec::~TBMWPCRec()
{ }

StatusCode
TBMWPCRec::initialize()
{
  ///////////////////////
  // Allocate Services //
  ///////////////////////

  // message service
  MsgStream log(messageService(),name());
  StatusCode sc;
  
  sc = service( "StoreGateSvc", m_StoreGate);
  
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() 
  	<< ": Unable to locate Service StoreGateSvc" << endreq;
    return sc;
  } 

  m_mwpc_names[0]="X2"; m_mwpc_names[1]="Y2";m_mwpc_names[2]="X3";m_mwpc_names[3]="Y3";m_mwpc_names[4]="X4";
  m_mwpc_names[5]="Y4";m_mwpc_names[6]="X5";m_mwpc_names[7]="Y5";

  return sc;
}

StatusCode
TBMWPCRec::execute()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////
  MsgStream log(messageService(),name());
  StatusCode sc;
  
  log << MSG::DEBUG << "In execute()" << endreq;

  // Reconstruct MWPC :
  TBMWPCRawCont * mwpcrawCont;
  sc = m_StoreGate->retrieve(mwpcrawCont, m_SGkey);
  if (sc.isFailure()){
    log << MSG::INFO << "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" << endreq;
    
  }else {
    log << MSG::DEBUG << "TBMWPCRec : Retrieval of "<<m_SGkey<<" succeed : cont size=" << mwpcrawCont->size()<< endreq;
    
    TBMWPCCont * mwpcCont = new TBMWPCCont();
    TBMWPCRawCont::const_iterator it_bc   = mwpcrawCont->begin();
    TBMWPCRawCont::const_iterator last_bc   = mwpcrawCont->end();

    // Loop over MWPC
    for(;it_bc != last_bc;it_bc++){
      const TBMWPCRaw * mwpcraw= (*it_bc);
      std::string name = mwpcraw->getDetectorName();
      unsigned int ind=0;
      // Find calibration index for this MWPC
      while(ind<8) 
	{
	  if(name==m_mwpc_names[ind]) break; 
	  else ind++;
	}
      if(ind==8){log<<MSG::ERROR<< "No calibrations for MWPC" <<name<<endreq;continue;}

      // build new MWPC
      TBMWPC * mwpc = new TBMWPC(name);
      
      std::vector<int> cwireno = mwpcraw->getCwireno();
      std::vector<int> nwires = mwpcraw->getNwires();
      
      std::vector<float> cluspos;
      std::vector<float> clussize;
      
      cluspos.clear(); clussize.clear();

      unsigned int nclus = cwireno.size();
      if(nclus!=nwires.size()) { log << MSG::INFO << " Problem with cluster number  in MWPC plane "<<ind<< endreq;continue;}
      for(unsigned int clus=0;clus<nclus;clus++){

	// Cluster center (cm) (from 2002 code)
	float coor = -m_mwpc_halfsize[ind]+m_mwpc_wirestep[ind]*cwireno[clus];
	coor += (nwires[clus]%2==0) ? 0.5*m_mwpc_wirestep[ind] : 0;
	if (mwpcraw->isX())
	  coor *= m_mwpc_invX[ind];
	else
	  coor *= m_mwpc_invY[ind];
	
	// Halfsize of cluster (cm) (from 2002 code)
	float hwidth = 0.5*nwires[clus]*m_mwpc_wirestep[ind];
	
	log << MSG::DEBUG << " coor= "<<coor<<"cm , w="<<hwidth<<"cm"<< endreq;

	cluspos.push_back(coor);
	clussize.push_back(hwidth);	

      }

      mwpc->setCPos(cluspos);
      mwpc->setClusterSizeC(clussize);
      mwpc->setXchambers(mwpcraw->isX());

      mwpc->setOverflow(mwpcraw->isOverflow());

      mwpcCont->push_back(mwpc);
    }

    log << MSG::DEBUG << " recording "<<m_SGrecordkey<<endreq;
    sc = m_StoreGate->record(mwpcCont,m_SGrecordkey);
    if ( sc.isFailure( ) ) {
      log << MSG::FATAL << "Cannot record MWPCCont" << endreq;
    }
  }

  if ( sc.isFailure( ) ) {
     setFilterPassed(false);
  } else {
     setFilterPassed(true);
  }
  return StatusCode::SUCCESS;

}


StatusCode 
TBMWPCRec::finalize()
{

  ////////////////////////////
  // Re-Allocating Services //
  ////////////////////////////


  return StatusCode::SUCCESS;
}
