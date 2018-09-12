/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloRec/CaloBCIDAvgAlg.h" 
#include "xAODEventInfo/EventID.h"
#include "xAODEventInfo/EventInfo.h"


CaloBCIDAvgAlg::CaloBCIDAvgAlg (const std::string& type, 
				const std::string& name, 
				const IInterface* parent) :
  AthReentrantAlgorithms(type, name, parent),
  m_bunchCrossingTool("BunchCrossingTool"),
  declareProperty("LumiTool",m_lumiTool,"Tool handle for Luminosity");
  declareProperty("BunchCrossingTool",m_bunchCrossingTool,"Tool handle for bunch crossing tool");
}
                                                                                
//-----------------------------------------------------------------

CaloBCIDAvgAlg::~CaloBCIDAvgAlg() {}


//-------------------------------------------------------------------

StatusCode CaloBCIDAvgAlg::initialize() {

  ATH_MSG_INFO( " initialize "  );

// get LumiTool
  if (m_isMC) {
    ATH_CHECK( m_bunchCrossingTool.retrieve() );
    ATH_MSG_DEBUG(" -- bunch crossing Tool retrieved");
    m_lumiTool.disable();
  
  } else {
    ATH_CHECK( m_lumiTool.retrieve() );
    ATH_MSG_DEBUG(" -- Lumi Tool retrieved");
    m_bunchCrossingTool.disable();
  }

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_bcidAvgKey.initialize());	     
  ATH_CHECK(m_ofcKey.initialize());
  ATH_CHECK(m_shapeKey.initialize());
  ATH_CHECK(m_minBiasAvgKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_mcSym.initialize());

  ATH_CHECK(detStore()->retrieve(m_lar_on_id,"LArOnlineID"));
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------------------

StatusCode CaloBCIDAvgAlg::execute_r(const EventContext& ctx) const {

  SG::ReadHandle<xAOD::EventInfo> ei(m_eventInfoKey,ctx);

  SG::ReadCondHandle<ILArOFC> ofcHdl(m_ofcKey,ctx);
  const ILArOFC* ofcs=*ofcHdl;

  SG::ReadCondHandle<ILArShape> shapeHdl(m_shapeKey,ctx);
  const ILArShape* shapes=*shapeHdl;

  SG::ReadCondHandle<ILArMinBiasAverage> minBiasHdl(m_minBiasAvgKey,ctx);
  const ILArMinBiadAverage* minBiasAvg=*minBiasHdl;

  std::unordered_map<Identifier,float> avgEShift;

  const int bcid = eventInfo->bcid();

  // convert from mu/bunch to lumi in 10**30 units per bunch (for MC only)
  // 25ns*Nbcid*71mb*10**30 = 25e-9*3564*71e-27*1e30 = 6.31 Use 1/6.31 = 0.158478605
  const float xlumiMC = m_isMC ? ei->averageInteractionsPerCrossing()*0.158478605 : 0.0;  

  // Calculate Luminosity values ONLY around the places Luminosity will be needed
  const std::vector<float> lumiVec=accumulateLumi(bcid,xlumiMC);


  //std::cout << " start loop over cells  bcid= " << bcid << std::endl;
  //if (!m_isMC) std::cout << "   lumi for this bcid " << m_lumiTool->lbLuminosityPerBCID(bcid) << std::endl;
  //size_t total = m_hwid_sym.size();

  if ( (bcid > 34) && (bcid < m_bcidMax-34)) { // Not close to bcid boundary    
    //Loop over symmetry cells (online IDs)
    for (const HWIdentifier hwid : mcSym->symIds()) {

      float eOFC=0.; 
      //  get MinBiasAverage
      float MinBiasAverage = minBiasAvg(hwid);

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      const auto& ofc = ofcs->ofca(hwid,0);
      const auto& samp = shape->Shape(hwid,0);
      const unsigned int nsamples = ofc.size();

      // choise of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) {
	if (m_lar_on_id->isHECChannel(hwid))
	  ifirst=m_firstSampleHEC;
      }

      const unsigned int nshapes = samp.size();
      if (nshapes < nsamples) {
	ATH_MSG_ERROR( " Not enough samples in Shape " << nshapes << "   less than in OFC " << nsamples  );
	return StatusCode::FAILURE;
      }

      unsigned int ishift = ifirst + bcid ; // correct for first sample
      for (unsigned int i=0;i<nsamples;i++) {
	float sumShape=0.;
	//unsigned int ishift = i + ifirst + bcid ; // correct for first sample
	unsigned int k = ishift;
	for (unsigned int j=0;j<nshapes;j++) {
	  float lumi = lumiVec[k];
	  sumShape += samp[j]*lumi;
	  k--;
	}
	eOFC += sumShape*(ofc[i]);
	ishift++;
      }
      eOFC = eOFC * MinBiasAverage;
   
      //std::cout << " index, eOFC " << index << " " << eOFC << std::endl;
      avgEshift[hwid]=eOFC;
    }      // loop over cells
  } else { // I am close to the boundary

    for (const HWIdentifier hwid : mcSym->symIds()) {

      float eOFC=0.; 
      //  get MinBiasAverage
      float MinBiasAverage = minBiasAvg(hwid);

      if (MinBiasAverage<0.) MinBiasAverage=0.;

      const auto& ofc = ofcs->ofca(id,0);
      const auto& samp = shape->Shape(id,0);
      const unsigned int nsamples = ofc.size();
      
      // choise of first sample : i.e sample on the pulse shape to which first OFC sample is applied
      unsigned int ifirst= 0;
      if (nsamples==4) {
	if (m_lar_on_id->isHECChannel(hwid))
	  ifirst=m_firstSampleHEC;
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
      avgEshift[hwid]=eOFC;
    }      // loop over cells
  } // end of the check bcid boundary

#ifdef DONTDO // some debug code, please, ignore
  std::cout << "corrections for BCID : " << bcid << std::endl;
  for (const HWIdentifier hwid : mcSym->symIds()) {
    if ( fabsf(1e9*(avgEshift[hwid])) > 0.001 ){
      std::cout << "cell [" << index1 <<"] = " <<(double)(avgEshift[hwid]) 
		<< std::endl;
    }
  }
#endif

  std::unique_ptr<CaloBCIDAverage> result=std::make_unique<CaloBCIDAverage>(mcSym,std::move(avgEshift));
  SG::WriteHandle<CaloBCIDAverage> writeHdl(m_bcidAvgKey,ctx);
  ATH_CHECK(writeHdl.record(result));

  return StatusCode::SUCCESS;
}

std::vector<float> CaloBCIDAvgAlg::accumulateLumi(const unsigned int bcid, const float xlumiMC) const {

  std::vector<float> lumiVec(m_bcidmax,0.0);

  unsigned int keep_samples=32;
  unsigned int keep_ofcsamples=32;

  if ( (bcid > keep_samples+5) && (bcid < m_bcidMax-keep_ofcsamples-5))  {
    unsigned int a=bcid-(keep_samples+4);
    unsigned int b=bcid+(keep_ofcsamples+4);
    for(unsigned int i=a;i<b;i++){
      float lumi=0.0;
      if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
      else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
      lumiVec[i]=(lumi);
    }
  } 
  else {
    int a=bcid-(keep_samples+4);
    if ( a < 0 ) a=0;
    unsigned int b=bcid+(keep_ofcsamples+4);
    if ( b >= m_bcidMax ) b=m_bcidMax;
    for(unsigned int i=(unsigned int)a;i<b;i++){
      float lumi=0.0;
      if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
      else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
      lumiVec[i]=(lumi);
    }

    for(unsigned int i=0;i<keep_ofcsamples+4;i++){
      float lumi=0.0;
      if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
      else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
      lumiVec[i]=(lumi);
    }

    for(unsigned int i=m_bcidMax-keep_samples-5;i<m_bcidMax;i++){
      float lumi=0.0;
      if (m_isMC) lumi= m_bunchCrossingTool->bcIntensity(i)*xlumiMC;   // convert to luminosity per bunch in 10**30 units
      else lumi = m_lumiTool->lbLuminosityPerBCID(i);  // luminosity in 10**30 units
      lumiVec[i]=(lumi);
    }

  }
  return lumiVec;
}

