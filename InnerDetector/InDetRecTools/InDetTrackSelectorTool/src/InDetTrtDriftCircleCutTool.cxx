/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTrackSelectorTool/InDetTrtDriftCircleCutTool.h"
#include "StoreGate/ReadCondHandle.h"

StatusCode  InDet::InDetTrtDriftCircleCutTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();


  if(sc.isFailure()){
    msg(MSG::ERROR)<<" Unable to initialize the AlgTool"<<endmsg;
    return StatusCode::FAILURE;
  }

    // Read key
   ATH_CHECK( m_strawReadKey.initialize() );
 
  return StatusCode::SUCCESS;
}
    
StatusCode InDet::InDetTrtDriftCircleCutTool::finalize()
{
  msg(MSG::INFO)  << "Finalize successful" << endmsg;
  return StatusCode::SUCCESS;
}
    
InDet::InDetTrtDriftCircleCutTool::InDetTrtDriftCircleCutTool(const std::string& t, const std::string& n, const IInterface*  p)
  :AthAlgTool(t,n,p),
   m_minOffset(0),
   m_param(false),
   m_useTRT(true)
{
  declareInterface<ITrtDriftCircleCutTool>(this);
  declareProperty("MinOffsetDCs",           m_minOffset );
  declareProperty("UseNewParameterization", m_param     );
  declareProperty("UseActiveFractionSvc",   m_useTRT    );
}

InDet::InDetTrtDriftCircleCutTool::~InDetTrtDriftCircleCutTool()
{}

int InDet::InDetTrtDriftCircleCutTool::minNumberDCs(const Trk::TrackParameters* trkp) const {
  if(!m_param){
    const double TrtEtaBin[6] = {0., 0.65,      0.85,     1.25,     1.80,     2.10};
    const double TrtA     [5] = {     29.,       31.,      20.,      36.,     34. };
    const double TrtB     [5] = { 2./0.65, -11./0.20, 16./0.40, -2./0.55, -24./0.3};
    
    double eta= fabs(trkp->momentum().eta());
    
    for(int i=0; i!=5; ++i) {
      if(eta <= TrtEtaBin[i+1]) {
	return int(TrtA[i]+TrtB[i]*(eta-TrtEtaBin[i])-m_minOffset);
      }
    }
    return int(m_minOffset);
  } else {
    const double TrtEtaBin[7] = {0.,   0.1,  0.6255,   1.07,     1.304,     1.752,     2.0  };
    const double TrtA     [6] = {    33.28,   30.40,  182.38,   -226.18,   -351.56,  -250.2 };
    const double TrtB     [6] = {   -79.4,    18.79,  -371.9,    495.12,    777.5,    377.9 };
    const double TrtC     [6] = {    589.7,  -42.73,  213.3,    -314.96  , -512.3,   -128.7 };
    const double TrtD     [6] = {       0.,   32.53,    0. ,      69.42  ,  111.0,      3.4 };
    const double TrtO     [6] = {       9.,   11.,     10.,        7.,        9.,      12.  };
    
    double eta = fabs(trkp->momentum().eta());

    SG::ReadCondHandle<TRTCond::ActiveFraction> strawReadHandle{m_strawReadKey};
    const TRTCond::ActiveFraction* actF{*strawReadHandle};

    for(int i=0; i!=6; ++i) {
      if(eta <= TrtEtaBin[i+1]) {
	double diff = eta;
	double nDiffTRT = TrtA[i]+TrtB[i]*diff+TrtC[i]*diff*diff+TrtD[i]*diff*diff*diff-TrtO[i];
	double activeF = 1.;
        float phi = trkp->momentum().phi();
        float eta = trkp->momentum().eta();
	if(m_useTRT) activeF = actF->getActiveFraction(eta,phi);
        nDiffTRT = nDiffTRT*activeF;
	if (nDiffTRT>=1) return int(nDiffTRT);
	else return int(m_minOffset);
      }
    }
    return int(m_minOffset);
  }
}
