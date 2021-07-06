/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TBMWPCRec.h"


TBMWPCRec::TBMWPCRec(const std::string& name,
				 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
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
  m_mwpc_names[0]="X2"; m_mwpc_names[1]="Y2";m_mwpc_names[2]="X3";m_mwpc_names[3]="Y3";m_mwpc_names[4]="X4";
  m_mwpc_names[5]="Y4";m_mwpc_names[6]="X5";m_mwpc_names[7]="Y5";

  return StatusCode::SUCCESS;
}

StatusCode
TBMWPCRec::execute()
{
  ATH_MSG_DEBUG ( "In execute()" );

  // Reconstruct MWPC :
  TBMWPCRawCont * mwpcrawCont;
  StatusCode sc = evtStore()->retrieve(mwpcrawCont, m_SGkey);
  if (sc.isFailure()){
    ATH_MSG_INFO ( "TBObjectReco: Retrieval of "<<m_SGkey<<" failed" );
    
  }else {
    ATH_MSG_DEBUG ( "TBMWPCRec : Retrieval of "<<m_SGkey<<" succeed : cont size=" << mwpcrawCont->size());
    
    TBMWPCCont * mwpcCont = new TBMWPCCont();

    // Loop over MWPC
    for (const TBMWPCRaw* mwpcraw : *mwpcrawCont) {
      std::string name = mwpcraw->getDetectorName();
      unsigned int ind=0;
      // Find calibration index for this MWPC
      while(ind<8) 
	{
	  if(name==m_mwpc_names[ind]) break; 
	  else ind++;
	}
      if(ind==8) {
        ATH_MSG_ERROR( "No calibrations for MWPC" <<name);
        continue;
      }

      // build new MWPC
      TBMWPC * mwpc = new TBMWPC(name);
      
      std::vector<int> cwireno = mwpcraw->getCwireno();
      std::vector<int> nwires = mwpcraw->getNwires();
      
      std::vector<float> cluspos;
      std::vector<float> clussize;
      
      cluspos.clear(); clussize.clear();

      unsigned int nclus = cwireno.size();
      if(nclus!=nwires.size()) {
        ATH_MSG_INFO ( " Problem with cluster number  in MWPC plane "<<ind);
        continue;
      }
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
	
	ATH_MSG_DEBUG ( " coor= "<<coor<<"cm , w="<<hwidth<<"cm");

	cluspos.push_back(coor);
	clussize.push_back(hwidth);	

      }

      mwpc->setCPos(cluspos);
      mwpc->setClusterSizeC(clussize);
      mwpc->setXchambers(mwpcraw->isX());

      mwpc->setOverflow(mwpcraw->isOverflow());

      mwpcCont->push_back(mwpc);
    }

    ATH_MSG_DEBUG ( " recording "<<m_SGrecordkey);
    sc = evtStore()->record(mwpcCont,m_SGrecordkey);
    if ( sc.isFailure( ) ) {
      ATH_MSG_FATAL ( "Cannot record MWPCCont" );
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
  return StatusCode::SUCCESS;
}
