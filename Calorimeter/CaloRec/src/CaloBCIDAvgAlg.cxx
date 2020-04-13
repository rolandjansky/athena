/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloBCIDAvgAlg.h" 
#include "AthenaMonitoringKernel/Monitored.h"

//#define DONTDO

StatusCode CaloBCIDAvgAlg::initialize() {
  ATH_MSG_INFO( " initialize "  );

  ATH_CHECK(m_bcDataKey.initialize(m_isMC));
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_bcidAvgKey.initialize());	     
  ATH_CHECK(m_ofcKey.initialize());
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_minBiasAvgKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_mcSym.initialize());
  ATH_CHECK(m_lumiDataKey.initialize(!m_isMC));
  if (! m_monTool.empty() ) ATH_CHECK( m_monTool.retrieve() );

  ATH_CHECK(detStore()->retrieve(m_lar_on_id,"LArOnlineID"));
  return StatusCode::SUCCESS;
}

StatusCode  CaloBCIDAvgAlg::finalize() {
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------------------

StatusCode CaloBCIDAvgAlg::execute(const EventContext& ctx) const {

  auto timer = Monitored::Timer("TIME_exec");
  auto monitoring = Monitored::Group( m_monTool, timer);

  SG::ReadHandle<xAOD::EventInfo> ei(m_eventInfoKey,ctx);

  SG::ReadCondHandle<ILArOFC> ofcHdl(m_ofcKey,ctx);
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey,ctx);
  const ILArShape* shapes=*shapeHdl;

  SG::ReadCondHandle<ILArMinBiasAverage> minBiasHdl(m_minBiasAvgKey,ctx);
  const ILArMinBiasAverage* minBiasAvg=*minBiasHdl;

  SG::ReadCondHandle<LArMCSym> mcSymHdl(m_mcSym,ctx);
  const LArMCSym* mcSym=*mcSymHdl;

  std::vector<float> avgEshift;
  avgEshift.reserve (mcSym->symIds().size());

  const int bcid = ei->bcid();

  std::vector<float> lumiVec;
  if (m_isMC) {//MC-case
    lumiVec.assign(m_bcidMax,0.0);
    SG::ReadCondHandle<BunchCrossingCondData> bcidHdl(m_bcDataKey,ctx);
    const BunchCrossingCondData* bcData=*bcidHdl; 

    // convert from mu/bunch to lumi in 10**30 units per bunch (for MC only)
    // 25ns*Nbcid*71mb*10**30 = 25e-9*3564*71e-27*1e30 = 6.31 Use 1/6.31 = 0.158478605
    const float xlumiMC = ei->averageInteractionsPerCrossing()*0.158478605;  

    // Calculate Luminosity values ONLY around the places Luminosity will be needed    
    int ii = bcid-38;
    if (ii < 0) ii += m_bcidMax;
    for (int i=bcid-38; i<bcid+38; ++i) {
      lumiVec[ii]=bcData->isFilled(ii)*xlumiMC; 
      ++ii;
      if (ii >= static_cast<int>(m_bcidMax)) ii -= m_bcidMax;
    }
  }
  else {
    //data-case
    SG::ReadCondHandle<LuminosityCondData> lumiData(m_lumiDataKey,ctx);
    lumiVec = lumiData->lbLuminosityPerBCIDVector();
  }

  if ( (bcid > 34) && (bcid < (int)m_bcidMax-34)) { // Not close to bcid boundary    
    //Loop over symmetry cells (online IDs)
    for (const HWIdentifier hwid : mcSym->symIds()) {

      float eOFC=0.; 

      //  get MinBiasAverage
      float MinBiasAverage = minBiasAvg->minBiasAverage(hwid);
      if (MinBiasAverage<0.) MinBiasAverage=0.;

      const auto& ofc = ofcs->OFC_a(hwid,0);
      const auto& samp = shapes->Shape(hwid,0);
      const unsigned int nsamples = ofc.size();

      // choice of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) {
	if (m_lar_on_id->isHECchannel(hwid)) {
	  ifirst=m_firstSampleHEC;
        }
      }

      const unsigned int nshapes = samp.size();
      if (nshapes < nsamples) {
	ATH_MSG_ERROR( " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples  );
	return StatusCode::FAILURE;
      }

      unsigned int ishift = ifirst + bcid ; // correct for first sample
      for (unsigned int i=0;i<nsamples;i++) {
	float sumShape=0.;
	unsigned int k = ishift;
	for (unsigned int j=0;j<nshapes;j++) {
	  const float& lumi = lumiVec[k];
	  sumShape += samp[j]*lumi;
	  k--;
	} 
	eOFC += sumShape*(ofc[i]);
	ishift++;
      }
      eOFC = eOFC * MinBiasAverage;
   
      //std::cout << " index, eOFC " << index << " " << eOFC << std::endl;
      avgEshift.push_back (eOFC);
    }      // loop over cells
  } else { // I am close to the boundary

    for (const HWIdentifier hwid : mcSym->symIds()) {

      float eOFC=0.; 

      //  get MinBiasAverage
      float MinBiasAverage = minBiasAvg->minBiasAverage(hwid);
      if (MinBiasAverage<0.) MinBiasAverage=0.;

      const auto& ofc = ofcs->OFC_a(hwid,0);
      const auto& samp = shapes->Shape(hwid,0);
      const unsigned int nsamples = ofc.size();
      
      // choice of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) {
	if (m_lar_on_id->isHECchannel(hwid)) {
	  ifirst=m_firstSampleHEC;
        }
      }

      const unsigned int nshapes = samp.size();
      if (nshapes < nsamples) {
	ATH_MSG_ERROR( " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples  );
	return StatusCode::FAILURE;
      }

      for (unsigned int i=0;i<nsamples;i++) {
          float sumShape=0.;
          unsigned int ishift = i + ifirst ; // correct for first sample
          for (unsigned int j=0;j<nshapes;j++) {
               unsigned int k;
               if ((bcid+ishift)<j) k = m_bcidMax+bcid+ishift-j;
               else if ((bcid+ishift)>=(m_bcidMax+j)) k = ishift-j+bcid-m_bcidMax;
               else k = bcid+ishift-j;
               float lumi = lumiVec[k];
               sumShape += samp[j]*lumi;
          }
          eOFC += sumShape* (ofc[i] );
      }
      //std::cout << std::endl;
      eOFC = eOFC * MinBiasAverage;
      avgEshift.push_back (eOFC);
    }      // loop over cells
  } // end of the check bcid boundary

#ifdef DONTDO // some debug code, please, ignore
  std::cout << "BCIDAlg corrections for BCID : " << bcid << std::endl;
  for (size_t i = 0; i < mcSym->symIds().size(); i++) {
    const HWIdentifier hwid = mcSym->symIds()[i];
    float eshift=avgEshift[i];
    if ( fabsf(1e9*(eshift)) > 0.001 ){
      unsigned id32=hwid.get_identifier32().get_compact();
      std::cout << "Alg BCID " << bcid << ", cell [" << id32 <<"] = " <<(double)eshift << std::endl;
    }
  }
#endif

  std::unique_ptr<CaloBCIDAverage> result=std::make_unique<CaloBCIDAverage>(mcSym,std::move(avgEshift));
  SG::WriteHandle<CaloBCIDAverage> writeHdl(m_bcidAvgKey,ctx);
  ATH_CHECK(writeHdl.record(std::move(result)));

  return StatusCode::SUCCESS;
}
