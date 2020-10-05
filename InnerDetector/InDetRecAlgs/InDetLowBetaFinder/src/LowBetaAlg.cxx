/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// LowBetaAlg.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#include "InDetLowBetaFinder/LowBetaAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/AlgTool.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetIdentifier/TRT_ID.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"

#include "TRT_ConditionsServices/ITRT_CalDbTool.h"
#include "TRT_ConditionsData/RtRelation.h"
#include "TRT_ConditionsData/BasicRtRelation.h"

#include "TRT_ElectronPidTools/ITRT_ToT_dEdx.h"

/////////////////////////////////////////////////////////////////////////////

namespace InDet
{

  LowBetaAlg::LowBetaAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthAlgorithm(name, pSvcLocator),
    m_trtId(0),
    m_TRTdetMgr(0),
    m_minTRThits(5),
    m_RcorrZero(11.0),
    m_RcorrOne(-3.0),
    m_RcorrTwo(0.0),
    m_TimingOffset(0.0),
    m_mcswitch(false),
    m_trackParticleCollection("InDetTrackParticles"),
    m_InDetLowBetaOutputName("InDetLowBetaCandidates"),
    m_trtconddbTool("TRT_CalDbTool",this),
    m_TrtTool(0),
    m_TRTdEdxTool(),
    m_TrtToolsSuccess{},
    m_TrtToolInitSuccess{}
  {
    declareProperty("MinimumTRThitsForIDpid", m_minTRThits);
    declareProperty("TracksName",             m_trackParticleCollection);
    declareProperty("UnslimmedTracksContainer", m_UnslimmedTracksContainerName="CombinedInDetTracks");
    declareProperty("CSMP_Rcorrection_Zero",  m_RcorrZero);
    declareProperty("CSMP_Rcorrection_One",   m_RcorrOne);
    declareProperty("CSMP_Rcorrection_Two",   m_RcorrTwo);
    declareProperty("CSMP_TimingOffset",      m_TimingOffset);
    declareProperty("InDetLowBetaOutputName", m_InDetLowBetaOutputName);
    declareProperty("MC_flag", m_mcswitch);
    declareProperty("TRTCalDbTool", m_trtconddbTool);
    declareProperty("TRT_ToT_dEdx_Tool", m_TRTdEdxTool);
    
  }
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  StatusCode LowBetaAlg::initialize(){

    ATH_CHECK(detStore()->retrieve(m_trtId, "TRT_ID"));
    m_TrtToolInitSuccess = !(initializeTrtToolBetaLiklihood().isFailure());

    ATH_CHECK(m_TRTdEdxTool.retrieve( EnableTool{ !m_TRTdEdxTool.name().empty() } ) );
    ATH_CHECK(m_trtconddbTool.retrieve());
    ATH_CHECK( m_trackParticleCollection.initialize() );
    ATH_CHECK( m_UnslimmedTracksContainerName.initialize() );
    ATH_CHECK( m_InDetLowBetaOutputName.initialize() );
    // Read handle for AtlasFieldCacheCondObj
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
    
    return StatusCode::SUCCESS;
  }
  // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  StatusCode LowBetaAlg::execute() {

    std::vector<float> CSMP_indicators;
    SG::WriteHandle<InDet::InDetLowBetaContainer> myInDetLowBetaContainer(m_InDetLowBetaOutputName);
    if (myInDetLowBetaContainer.record( std::make_unique<InDet::InDetLowBetaContainer>()).isFailure()) {
      ATH_MSG_ERROR( "Unable to record InDetLowBeta Container in TDS" );
      return StatusCode::FAILURE;
    }

    EventContext ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
    if (fieldCondObj == nullptr) {
      ATH_MSG_ERROR("execute: Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
      return StatusCode::FAILURE;
    }
    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    if (fieldCache.solenoidOn()){//B field

      if (!m_trackParticleCollection.key().empty()){

        SG::ReadHandle<xAOD::TrackParticleContainer> trackTES ( m_trackParticleCollection);
	if (!trackTES.isValid()) {
	    ATH_MSG_WARNING( "Could not find TrackParticleCollection " << m_trackParticleCollection.key() << " in StoreGate." );
        }
	else{

	  for (const xAOD::TrackParticle *trk: *trackTES){

            std::unique_ptr<InDet::InDetLowBetaCandidate> CSMP_Candidate;
	    uint8_t Trt_hits = 0;
            if (trk->summaryValue(Trt_hits, xAOD::numberOfTRTHits)) {
              const Trk::Track*  origtrack =  trk->track();
              if (origtrack) {
                double  Trk_pt = trk->pt();
                if ((Trt_hits != 0) && (Trk_pt > 400.0) ){
                  const Trk::Track& newtrack = Trk::Track(*origtrack);

                  CSMP_indicators.clear();
                  CSMP_indicators = ChargedSMPindicators(newtrack);

                  if (CSMP_indicators.size() > 1){
                    if(  ( (CSMP_indicators[5] > 15000.0)  || ((int)CSMP_indicators[4] > 10))   && ((unsigned int)CSMP_indicators[6] > m_minTRThits) )  {

                      float TRTToTdEdx           = CSMP_indicators[0];
                      float TRTTrailingEdge      = CSMP_indicators[1];
                      float TRTTGapdEdx          = CSMP_indicators[2];
                      int   TRTNLastBits         = (int)CSMP_indicators[3];
                      float TRTToolsdEdx         = CSMP_indicators[10];
                      float TRTLikelihoodBeta    = CSMP_indicators[7];
                      float TRTLikelihoodError   = CSMP_indicators[8];
                      float TRTHighTbits         = CSMP_indicators[9];
                      CSMP_Candidate = 
                          std::make_unique<InDet::InDetLowBetaCandidate>(TRTToTdEdx,
                                                                         TRTTrailingEdge,
                                                                         TRTTGapdEdx,
                                                                         TRTNLastBits,
                                                                         TRTToolsdEdx,
                                                                         TRTLikelihoodBeta,
                                                                         TRTLikelihoodError,
                                                                         TRTHighTbits);
		    }
	          }//check for valid CSMP
                }
	      }//original track exists
            }
            myInDetLowBetaContainer->push_back(CSMP_Candidate.release());
         }//track loop
        }// track collection exists
      }
    }//B field

    return StatusCode::SUCCESS;
  }

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  StatusCode LowBetaAlg::finalize() {
    finalizeTrtToolBetaLiklihood();
    return StatusCode::SUCCESS;
  }

  //============================================================================================

  std::vector<float> InDet::LowBetaAlg::ChargedSMPindicators(const Trk::Track& track) const
  {

    //m_mcswitch = false;

    float RcorrZero  = 9.06;
    float RcorrOne   = -3.437;
    float RcorrTwo   = 2.6573;
    float RcorrThree = -1.5598;
    float RcorrFour  = 0.15772;
    
    float st_R_zero = 25.545;
    float st_R_one =  2.7807;
    float st_R_two =  16.753;
    float st_R_three = -11.605;
    float st_R_four = 2.9147;


    float st_eta_zero =  -0.2733;
    float st_eta_one =  -0.0815;
    float st_eta_two =  2.1757;
    float st_eta_three = 0.1631;
    float st_eta_four =  8.3143;
    float st_eta_five = -0.1066;
    float st_eta_six = -4.4437;
    
    float tb_eta_zero =  -0.804;
    float tb_eta_one =  0.0166;
    float tb_eta_two =  0.7375;
    float tb_eta_three = -0.02612;
    float tb_eta_four =  2.611;
    float tb_eta_five = 0.01778;
    float tb_eta_six  = -1.405;
    
    float EC_st_eta_zero =  -2.385;
    float EC_st_eta_one = 46.107;
    float EC_st_eta_two = -95.367;
    float EC_st_eta_three = 79.6244;
    float EC_st_eta_four = -30.333;
    float EC_st_eta_five = 4.371;
    
    float EC_tb_eta_zero = -1.831;
    float EC_tb_eta_one = 16.451;
    float EC_tb_eta_two =  -32.532;
    float EC_tb_eta_three = 26.645;
    float EC_tb_eta_four = -10.04;
    float EC_tb_eta_five = 1.4354;
    
    if(m_mcswitch){
      
      st_eta_zero =  0.146;
      st_eta_one =  0.0201;
      st_eta_two =  9.341;
      st_eta_three = -0.08474;
      st_eta_four =  -3.253;
      st_eta_five = 0.06936;
      st_eta_six = 0.08367;
      
      tb_eta_zero =  -0.09813;
      tb_eta_one =  0.0062;
      tb_eta_two =  3.051;
      tb_eta_three = -0.0254;
      tb_eta_four =  -1.088;
      tb_eta_five = 0.02041;
      tb_eta_six  = 0.0307;
      
      EC_st_eta_zero =  -7.99;
      EC_st_eta_one = 69.589;
      EC_st_eta_two = -129.318;
      EC_st_eta_three = 102.96;
      EC_st_eta_four = -38.1655;
      EC_st_eta_five = 5.4164;
      
      EC_tb_eta_zero = -2.957;
      EC_tb_eta_one = 22.8;
      EC_tb_eta_two =  -41.215;
      EC_tb_eta_three = 32.0628;
      EC_tb_eta_four = -11.6181;
      EC_tb_eta_five = 1.6113;
    }

    // output vector for the algorithms
    typedef std::vector<float> CSMP_ind;
    CSMP_ind Discriminators;

    double pTrk = 0.0;
    
    // Check for perigee:
    const Trk::TrackParameters* perigee = track.perigeeParameters();
    if (!perigee) {
      return Discriminators;
    }

    // Get parameters at perigee and check that they are reasonable:
    const AmgVector(5)& parameterVector = perigee->parameters();
    double qOverP = parameterVector[Trk::qOverP];
    double theta  = parameterVector[Trk::theta];
    double trk_eta  = -log(tan(theta/2.0));
    double trk_z0   = parameterVector[Trk::z0];
    if (tan(theta/2.0) < 0.0001) {
      ATH_MSG_DEBUG( "  Track has negative theta or is VERY close to beampipe! (tan(theta/2) < 0.0001) " );
      return Discriminators; // to allow RVO
    }

    if (qOverP <1e-5) {
      ATH_MSG_DEBUG( " Track momentum infinite! (i.e. q/p = 0) " );
      return Discriminators; // to allow RVO
    }
    else pTrk = fabs(1.0 / qOverP);
    double trk_pt = pTrk*sin(theta);
    

    // ------------------------------------------------------------------------------------
    // Loop over TRT hits on track, and calculate LowBeta discriminators
    // ----------------------------------------------- -------------------------------------
    
    unsigned int TRThits       = 0;
    unsigned int HighTbits  = 0;
    unsigned int goodTRThits   = 0;
    //unsigned int TotalBitsOverThreshold     = 0;
    int NLastBits        = 0;
    unsigned int HighThresholdHits   = 0;
    float te_product_sum = 0.0;
    float te_quotient_sum = 0.0;
    float le_product_sum = 0.0;
    float le_quotient_sum = 0.0;
    float sq_dist_sum = 0.0;
    double RCorrectedTotBits    = 0.0;
    double RCorrTotalBitsOverThreshold = 0.0;
    double RCorrectedAverageBitsOverThreshold = 0.0;
    double TrailingEdge = 0.0;
    //double TrailingEdgeSigma = 0.0;
    //double TrailingEdgeError = 0.0;
    double LikelihoodBeta = 0.0;
    double LikelihoodError = 0.0;
    std::vector<float> LikelihoodValues;
    double dEdx = 0.0;
    bool bitpatternfail = false;
    float trtdistance = 0 ;
    double highestToT = -999.0;
    double highestTGap = -999.0;

    //Arclength variables
    float trackxlast = 0.;
    float trackylast = 0.;
    float trackzlast = 0.;

    
    // Start TGap variables
    float totoverl =0.;
    double tegap = 0.;
    int hont =0;
    int hontb = 0;
    int honte =0;
    int hbad =0;

    // Check for track states:
    const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track.trackStateOnSurfaces();
   
    if (recoTrackStates) {
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();


      // Loop over track states on surfaces (i.e. generalized hits):
      for ( ; tsosIter != tsosIterEnd; ++tsosIter) { //generalized hits
	
	// Measurement disallows outliers - should it? Check with Wolfgang!
	const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
	if (measurement) { //measurement
	  
	  // Get drift circle (ensures that hit is from TRT):
	  const InDet::TRT_DriftCircleOnTrack *driftcircle = dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
	  if (driftcircle) { //driftcircle

	    TRThits++;
	    
	    //// Part of TRT hit belongs to (+-1: Barrel, +-2: Wheels).
	    //int HitPart = m_trtId->barrel_ec(driftcircle->identify());
	    
	    // Distance from track to anode in straw.
	    double TrkAnodeDist = 0.0000000;
	    TrkAnodeDist = driftcircle->localParameters()[Trk::driftRadius];
	    
	    float trackx =  driftcircle->globalPosition().x();
	    float tracky =  driftcircle->globalPosition().y();
	    float trackz =  driftcircle->globalPosition().z();
	    
	    //
	    //Track arclength_xy
	    double trackR = sqrt(trackx*trackx + tracky*tracky);

	    double chordlength=sqrt(trackx*trackx+tracky*tracky+(trackz-trk_z0)*(trackz-trk_z0));
	    double Rmom=fabs(trk_pt)*510.0/340;  // ( mm and MeV)
	    //double trackfullhalfangle=asin(1050.0/(2.0*Rmom));
	    //double maxtrack=2*trackfullhalfangle*Rmom;
	    //if(Rmom < 1050.0/2.0)maxtrack=3.1425*Rmom;
	    
	    double arc0;
	    if(TRThits==1){   // initial hit coordinates
	      double trackz0=trackz;
	      double tracky0=tracky;
	      double trackx0=trackx;
	      double Rxy0=trackR;
	      double Rtot0=chordlength;
	      double slantcor=Rtot0/Rxy0;
	      double halfangle=asin(Rxy0/(2.0*Rmom));
	      arc0=Rmom*2.0*halfangle*slantcor;
	      trtdistance=arc0;
	      trackxlast=trackx0;
	      trackylast=tracky0;
	      trackzlast=trackz0;
	    }

	    double dx=trackx-trackxlast;
	    double dy=tracky-trackylast;
	    double dz=trackz-trackzlast;
	    trtdistance=trtdistance+sqrt(dx*dx+dy*dy+dz*dz);
	    trackxlast=trackx;
	    trackylast=tracky;
	    trackzlast=trackz;
	    

	    if (driftcircle->prepRawData()){//bitpattern?
	      unsigned int BitPattern = driftcircle->prepRawData()->getWord();
	      
	      unsigned int CheckBit;
	      //char bitstring[28] = "";
	      int NHTbits = 0;
	      int NLTbits = 0;
	      int TE_bit  = 0;
	      int LE_bit  = 0;
	      bool LE_found = false;
	      double trailingbittime = 0.0;
	      double leadingbittime = 0.0;
	      float TOT   = 0.0;

	      Identifier DCoTId = driftcircle->identify();
	      int bec = m_trtId->barrel_ec(DCoTId);
	      int phimod =m_trtId->phi_module(DCoTId);
	      int layer =m_trtId->layer_or_wheel(DCoTId);
	      int strawlayer =m_trtId->straw_layer(DCoTId);
	      int str =m_trtId->straw(DCoTId);
	      
	      //get TRT calibration from DB:
	      Identifier TRTlocal =m_trtId->straw_id(bec, phimod, layer, strawlayer, str);
	      double t0 = m_trtconddbTool->getT0(TRTlocal);

	      // Get bit pattern and bits over threshold:
	      for (int j=0; j < 27; j++) {
		CheckBit = (1 << (26-j));
		//if ((BitPattern & CheckBit) == 0) {bitstring[j] = '0';}
		//else                              {bitstring[j] = '1';}
		
		if (j%9 == 0) {if ((BitPattern & CheckBit) != 0) NHTbits++;}
		else {
		  if ((BitPattern & CheckBit) != 0) {
		    NLTbits++;
		    TE_bit = j;
		    if (j>9) TE_bit = TE_bit -1;
		    if (j>18) TE_bit = TE_bit -1;

		    
		    if (!LE_found){
		      LE_bit = j;
		      if (j>9) LE_bit = LE_bit -1;
		      if (j>18) LE_bit = LE_bit -1;

		      LE_found = true;
		    }
		  }
		}
		if (j == 26) {if ((BitPattern & CheckBit) != 0) NLastBits++;}
		
	      }
	      
	      TOT = (TE_bit - LE_bit)+1;
	      if(NHTbits > 0) HighThresholdHits++;
	      HighTbits += NHTbits;
	      double f_TrkAnodeDist = fabs(TrkAnodeDist);

	      trailingbittime = double(TE_bit)*3.125-t0+m_TimingOffset;
	      leadingbittime  = double(LE_bit)*3.125-t0+m_TimingOffset;

	      if ( (trtdistance > 0.0) && (f_TrkAnodeDist <= 1.9) && (f_TrkAnodeDist >= 0.01) && (TOT == NLTbits) ){
	
		//Straw Radius Correction
		RCorrectedTotBits = NLTbits*1.0-(RcorrZero+RcorrOne*(f_TrkAnodeDist)+RcorrTwo*(pow(f_TrkAnodeDist,2))+ RcorrThree*(pow(f_TrkAnodeDist,3))+RcorrFour*(pow(f_TrkAnodeDist,4)));
		trailingbittime = trailingbittime-(st_R_zero+st_R_one*f_TrkAnodeDist+st_R_two*pow(f_TrkAnodeDist,2)+st_R_three*pow(f_TrkAnodeDist,3)+st_R_four*pow(f_TrkAnodeDist,4));
		
		//Eta Corrections
		if ((bec == 1) || (bec == -1)) {
		  trailingbittime = trailingbittime-(st_eta_zero+st_eta_one*(trk_eta)+st_eta_two*(pow(trk_eta,2))+st_eta_three*(pow(trk_eta,3))
						     +st_eta_four*(pow(trk_eta,4))+st_eta_five*(pow(trk_eta,5))+st_eta_six*(pow(trk_eta,6)));
		  
		  RCorrectedTotBits = RCorrectedTotBits-(tb_eta_zero + tb_eta_one*(trk_eta)+tb_eta_two*(pow(trk_eta,2))+tb_eta_three*(pow(trk_eta,3))
							 +tb_eta_four*(pow(trk_eta,4))+tb_eta_five*(pow(trk_eta,5))+tb_eta_six*(pow(trk_eta,6)));
		}
		else if (trk_eta > 0) {
		  trailingbittime = trailingbittime-(EC_st_eta_zero+EC_st_eta_one*trk_eta+EC_st_eta_two*(pow(trk_eta,2))+EC_st_eta_three*(pow(trk_eta,3))+EC_st_eta_four*(pow(trk_eta,4))+EC_st_eta_five*(pow(trk_eta,5)));
		  RCorrectedTotBits = RCorrectedTotBits-(EC_tb_eta_zero+EC_tb_eta_one*trk_eta+EC_tb_eta_two*trk_eta*trk_eta+EC_tb_eta_three*(pow(trk_eta,3))+EC_tb_eta_four*(pow(trk_eta,4))+EC_tb_eta_five*(pow(trk_eta,5)));
		}
		else {
		  trailingbittime = trailingbittime-(EC_st_eta_zero-EC_st_eta_one*trk_eta+EC_st_eta_two*(pow(trk_eta,2))-EC_st_eta_three*(pow(trk_eta,3))+EC_st_eta_four*(pow(trk_eta,4))-EC_st_eta_five*(pow(trk_eta,5)));
		  RCorrectedTotBits = RCorrectedTotBits-(EC_tb_eta_zero-EC_tb_eta_one*trk_eta+EC_tb_eta_two*trk_eta*trk_eta-EC_tb_eta_three*(pow(trk_eta,3))+ EC_tb_eta_four*(pow(trk_eta,4))- EC_tb_eta_five*(pow(trk_eta,5)));
		}
		//End of  Eta Corrections

	

		RCorrTotalBitsOverThreshold += RCorrectedTotBits;
		te_product_sum += trailingbittime*trtdistance;
		te_quotient_sum += trailingbittime/trtdistance;
		le_product_sum += leadingbittime*trtdistance;
		le_quotient_sum += leadingbittime/trtdistance;
		sq_dist_sum += trtdistance*trtdistance;
		goodTRThits++;

		if (RCorrectedTotBits > highestToT) highestToT = RCorrectedTotBits;
	      }


	      // Start TGap code for track
	      bool Valid = true;
	      float eta = trk_eta;
	      //float sinth =  1./cosh(eta);
	      //float costh = tanh(eta);
	      //if (costh ==0) costh = 0.0001;
	      if((bec!=1 && bec!=-1)&&fabs(eta)<0.7) Valid = false; //0.7
	      if((bec==1 || bec ==-1)&&fabs(eta)>1.1) Valid = false;  //0.7
	      
	      float Rdrift =  TrkAnodeDist;
	      float Rtrack = Rdrift;
	      int Ltbit= LE_bit;
	      int Ttbit= TE_bit;
	      
	      // Add the cut on holes (from HO)
	      if( Ttbit-Ltbit+1 != NLTbits)Valid = false;

	      float tot = TOT;
	      
	      if ( (fabs(Rdrift)>0) && Valid && (trtdistance > 0.0)) {//rdrift condition
		
		//float length, projlength;
		float projlength;
		projlength = sqrt(4.- Rtrack*Rtrack);
		
		//if (bec == 1 || bec == -1) length = projlength/sinth; // track path length in a straw
		//if (bec == 2 || bec == -2) length = fabs( projlength/costh); // track path length in a straw
		
		if (projlength > 0) totoverl = tot*3.125/projlength;
		if (totoverl>35.)  {
		  hbad +=1;
		  }
		
		hont += 1;  // #hits on track
		if (bec==-1 || bec==1)  hontb += 1;
		if (bec==-2 || bec==2)  honte += 1;
		
		double gap = 0.0;
		double gapE = 0.0;

		
		if (m_mcswitch == 1 && (bec==1 || bec==-1))     gap = (double) (22.5  - Ttbit);  // MC BARREL
		if (m_mcswitch == 1 && (bec!=1 && bec!=-1))     gapE = (double) (22.5 - Ttbit);// ENDCAP MC
		if (m_mcswitch ==1 && (bec==1 || bec==-1))      gap = gap + t0/3.125 - 5.571; // correct for t0 Barrel  new MC
		if (m_mcswitch ==1 && (bec!=1 && bec!=-1))      gapE = gapE + t0/3.125 - 5.0656;// correct for t0 Endcap new MC
		
		
		if (m_mcswitch ==1 && (bec==1 || bec==-1))      gap = gap*0.66; //MC BARREL
		if (m_mcswitch ==1 && (bec!=1 && bec!=-1))      gap = 0.74*gapE;//MC ENDCAP
		
		if (m_mcswitch ==0 && (bec==1 || bec==-1))     gap = (double) (20.   - Ttbit);// FOR BARREL DATA
		if (m_mcswitch ==0 && (bec!=1 && bec!=-1))     gapE = (double) (20. - Ttbit);   //ENDCAP DATA
		if (m_mcswitch ==0 && (bec==1 || bec==-1))     gap = gap + t0/3.125 - 2.9498; //correct for t0 Barrel DATA
		if (m_mcswitch ==0 && (bec!=1 && bec!=-1))     gapE = gapE + t0/3.125 - 2.9075;//correct for t0 Endcap DATA
		if (m_mcswitch ==0 && (bec==1 || bec==-1))     gap = (double) gap*0.66;// FOR BARREL DATA
		if (m_mcswitch ==0 && (bec!=1 && bec!=-1))     gap = (double) gapE*0.98;   //ENDCAP DATA
		
		
		double TGap = -999.;  // trailing edge gap corrected for R- and eta dependence (by Ben Weinert)
				
		//  ========Barrel DATA ToTal=================================================
		if (m_mcswitch ==0 &&(bec==1 || bec==-1)) {
		  if (fabs(eta)<.1 && fabs(eta)>0.0) TGap = gap/((.00488801*pow(fabs(Rtrack),7.96055)-2.34833*pow(fabs(Rtrack),1.72825)+7.54480));
		  if (fabs(eta)<.2 && fabs(eta)>0.1) TGap = gap/((.00670660*pow(fabs(Rtrack),7.50896)-2.34480*pow(fabs(Rtrack),1.73470)+7.52650));
		  if (fabs(eta)<.3 && fabs(eta)>0.2) TGap = gap/((.00881969*pow(fabs(Rtrack),7.09242)-2.36686*pow(fabs(Rtrack),1.72337)+7.52207));
		  if (fabs(eta)<.4 && fabs(eta)>0.3) TGap = gap/((.0131694*pow(fabs(Rtrack),6.59031)-2.39547*pow(fabs(Rtrack),1.72373)+7.52161));
		  if (fabs(eta)<.5 && fabs(eta)>0.4) TGap = gap/((.0236602*pow(fabs(Rtrack),5.87639)-2.43456*pow(fabs(Rtrack),1.72005)+7.48029));
		  if (fabs(eta)<.6 && fabs(eta)>0.5) TGap = gap/((.0547144*pow(fabs(Rtrack),4.99237)-2.47208*pow(fabs(Rtrack),1.74454)+7.36295));
		  if (fabs(eta)<.7 && fabs(eta)>0.6) TGap = gap/((.0723406*pow(fabs(Rtrack),4.76311)-2.47704*pow(fabs(Rtrack),1.74222)+7.14952));
		  if (fabs(eta)<.8 && fabs(eta)>0.7) TGap = gap/((.137036*pow(fabs(Rtrack),4.23644)-2.48428*pow(fabs(Rtrack),1.79610)+6.85699));
		  if (fabs(eta)<.9 && fabs(eta)>0.8) TGap = gap/((.285357*pow(fabs(Rtrack),3.71480)-2.56711*pow(fabs(Rtrack),1.87888)+6.50990));
		  if (fabs(eta)<1. && fabs(eta)>0.9) TGap = gap/((.241327*pow(fabs(Rtrack),3.91980)-2.43298*pow(fabs(Rtrack),1.87156)+6.08410));
		  if (fabs(eta)<1.1 && fabs(eta)>1.) TGap = gap/((.442936*pow(fabs(Rtrack),3.59687)-2.60530*pow(fabs(Rtrack),1.98021)+5.78001));
		}
		//=========Barrel MC Total===============================
		if (m_mcswitch ==1 && (bec==1 || bec==-1)) {
		  if(fabs(eta)<.1 && fabs(eta)>0.0) TGap = gap/((.00000123128*pow(fabs(Rtrack),19.3147)-1.06782*pow(fabs(Rtrack),2.29312)+5.62686));
		  if (fabs(eta)<.2 && fabs(eta)>0.1) TGap = gap/((.00000170888*pow(fabs(Rtrack),18.8841)-1.07002*pow(fabs(Rtrack),2.29326)+5.60175));
		  if (fabs(eta)<.3 && fabs(eta)>0.2) TGap = gap/((.000000918115*pow(fabs(Rtrack),19.7785)-1.09601*pow(fabs(Rtrack),2.25662)+5.68679));
		  if (fabs(eta)<.4 && fabs(eta)>0.3) TGap = gap/((.00000142286*pow(fabs(Rtrack),19.1558)-1.09694*pow(fabs(Rtrack),2.24469)+5.50375));
		  if (fabs(eta)<.5 && fabs(eta)>0.4) TGap = gap/((.00000483637*pow(fabs(Rtrack),17.5425)-1.09179*pow(fabs(Rtrack),2.25701)+5.39363));
		  if (fabs(eta)<.6 && fabs(eta)>0.5) TGap = gap/((.0000114224*pow(fabs(Rtrack),16.3431)-1.09874*pow(fabs(Rtrack),2.23981)+5.28368));
		  if (fabs(eta)<.7 && fabs(eta)>0.6) TGap = gap/((.0000101658*pow(fabs(Rtrack),16.5468)-1.07799*pow(fabs(Rtrack),2.24312)+5.13952));
		  if (fabs(eta)<.8 && fabs(eta)>0.7) TGap = gap/((.0000109867*pow(fabs(Rtrack),16.4814)-1.00968*pow(fabs(Rtrack),2.29445)+4.95406));
		  if (fabs(eta)<.9 && fabs(eta)>0.8) TGap = gap/((.00000887578*pow(fabs(Rtrack),16.8073)-.945814*pow(fabs(Rtrack),2.32505)+4.76174));
		  if (fabs(eta)<1. && fabs(eta)>0.9) TGap = gap/((.0000721159*pow(fabs(Rtrack),13.8838)-.794514*pow(fabs(Rtrack),2.52852)+4.48742));
		  if (fabs(eta)<1.1 && fabs(eta)>1.) TGap = gap/((-.00147754*pow(fabs(Rtrack),2.05287)-.949485*pow(fabs(Rtrack),2.05281)+4.40570));
		}
		//============ENDCAP DATA===========================================
		if (m_mcswitch==0 &&(bec!=1 && bec!=-1)){
		  if (fabs(eta)<0.9 && fabs(eta)>0.8) TGap = gap/((.280989*pow(fabs(Rtrack),3.78618)-2.52785*pow(fabs(Rtrack),1.98014)+8.22240));
		  if (fabs(eta)<1.0 && fabs(eta)>0.9) TGap = gap/((.0848913*pow(fabs(Rtrack),4.73693)-2.38324*pow(fabs(Rtrack),1.83445)+8.41772));
		  if (fabs(eta)<1.1 && fabs(eta)>1.0) TGap = gap/((.0709059*pow(fabs(Rtrack),4.86962)-2.41818*pow(fabs(Rtrack),1.80467)+8.58839));
		  if (fabs(eta)<1.2 && fabs(eta)>1.1) TGap = gap/((.0543339*pow(fabs(Rtrack),5.10085)-2.41581*pow(fabs(Rtrack),1.78433)+8.69492));
		  if (fabs(eta)<1.3 && fabs(eta)>1.2) TGap = gap/((.0304298*pow(fabs(Rtrack),5.72427)-2.41395*pow(fabs(Rtrack),1.74796)+8.79894));
		  if (fabs(eta)<1.4 && fabs(eta)>1.3) TGap = gap/((.0427503*pow(fabs(Rtrack),5.35505)-2.42229*pow(fabs(Rtrack),1.77298)+8.85050));
		  if (fabs(eta)<1.5 && fabs(eta)>1.4) TGap = gap/((.0281426*pow(fabs(Rtrack),5.80661)-2.40467*pow(fabs(Rtrack),1.74990)+8.88722));
		  if (fabs(eta)<1.6 && fabs(eta)>1.5) TGap = gap/((.0300813*pow(fabs(Rtrack),5.72420)-2.39001*pow(fabs(Rtrack),1.75507)+8.89574));
		  if (fabs(eta)<1.7 && fabs(eta)>1.6) TGap = gap/((.0217449*pow(fabs(Rtrack),5.96287)-2.38264*pow(fabs(Rtrack),1.70580)+8.89384));
		  if (fabs(eta)<1.8 && fabs(eta)>1.7) TGap = gap/((.0153687*pow(fabs(Rtrack),6.24080)-2.36323*pow(fabs(Rtrack),1.67239)+8.91872));
		  if (fabs(eta)<1.9 && fabs(eta)>1.8) TGap = gap/((.0111157*pow(fabs(Rtrack),6.57710)-2.32227*pow(fabs(Rtrack),1.65943)+8.94240));
		  if (fabs(eta)<2.0 && fabs(eta)>1.9) TGap = gap/((.00890940*pow(fabs(Rtrack),6.86340)-2.26815*pow(fabs(Rtrack),1.65027)+8.90461));
		}
		//========ENDCAP MC===============================
		if (m_mcswitch ==1 && (bec!=1 && bec!=-1)) {
		  if (fabs(eta)<0.9 && fabs(eta)>0.8) TGap = gap/((.00000282950*pow(fabs(Rtrack),18.3737)-1.05569*pow(fabs(Rtrack),2.27545)+5.97535));
		  if (fabs(eta)<1.0 && fabs(eta)>0.9) TGap = gap/((.00000719904*pow(fabs(Rtrack),16.9277)-1.13237*pow(fabs(Rtrack),2.25351)+6.25901));
		  if (fabs(eta)<1.1 && fabs(eta)>1.0) TGap = gap/((.00000180076*pow(fabs(Rtrack),18.7808)-1.23004*pow(fabs(Rtrack),2.17603)+6.51411));
		  if (fabs(eta)<1.2 && fabs(eta)>1.1) TGap = gap/((.000000652098*pow(fabs(Rtrack),20.1089)-1.32235*pow(fabs(Rtrack),2.11510)+6.74394));
		  if (fabs(eta)<1.3 && fabs(eta)>1.2) TGap = gap/((.000000158218*pow(fabs(Rtrack),22.0124)-1.41428*pow(fabs(Rtrack),2.04860)+6.95308));
		  if (fabs(eta)<1.4 && fabs(eta)>1.3) TGap = gap/((.0000000495484*pow(fabs(Rtrack),23.5834)-1.47129*pow(fabs(Rtrack),2.01003)+7.11830));
		  if (fabs(eta)<1.5 && fabs(eta)>1.4) TGap = gap/((.000000393189*pow(fabs(Rtrack),20.5908)-1.51725*pow(fabs(Rtrack),1.98343)+7.25927));
		  if (fabs(eta)<1.6 && fabs(eta)>1.5) TGap = gap/((.000000345984*pow(fabs(Rtrack),20.9028)-1.53850*pow(fabs(Rtrack),1.96704)+7.34837));
		  if (fabs(eta)<1.7 && fabs(eta)>1.6) TGap = gap/((.000000777268*pow(fabs(Rtrack),19.7132)-1.54808*pow(fabs(Rtrack),1.93878)+7.37986));
		  if (fabs(eta)<1.8 && fabs(eta)>1.7) TGap = gap/((.000000314758*pow(fabs(Rtrack),20.9848)-1.54640*pow(fabs(Rtrack),1.91034)+7.40800));
		  if (fabs(eta)<1.9 && fabs(eta)>1.8) TGap = gap/((.000000850534*pow(fabs(Rtrack),19.6195)-1.52880*pow(fabs(Rtrack),1.90915)+7.43214));
		  if (fabs(eta)<2.0 && fabs(eta)>1.9) TGap = gap/((.00000245275*pow(fabs(Rtrack),18.1615)-1.49520*pow(fabs(Rtrack),1.92057)+7.42649));
		  if (fabs(eta)<2.1 && fabs(eta)>2.0) TGap = gap/((.0000271320*pow(fabs(Rtrack),14.8130)-1.50875*pow(fabs(Rtrack),1.90915)+7.38428));
		  if (fabs(eta)<2.2 && fabs(eta)>2.1) TGap = gap/((.0000687527*pow(fabs(Rtrack),17.5651)-.0315462*pow(fabs(Rtrack),8.53188)+4.39271));
		  //=============================================================================================
		}
		

		if (TGap<0 && TGap>-999.) TGap=0.;
		if (TGap>0.) tegap +=  TGap;  // sum of corrected gaps
		if (TGap >  highestTGap)highestTGap = TGap;
		
	      }

	      
	    }//bitpattern?
	    //else bitpatternfail = true;
	  } //driftcircle
	} //measurement
      } //generalized hits

      //float htfrac =  (HighThresholdHits*1.0)/(TRThits*1.0);
	     

      if(goodTRThits > 5){

	RCorrTotalBitsOverThreshold = RCorrTotalBitsOverThreshold - highestToT;
	RCorrectedAverageBitsOverThreshold = double(RCorrTotalBitsOverThreshold)/double(goodTRThits-1)+1.0;
	TrailingEdge = (te_product_sum/sq_dist_sum)*300 + 1.0;
	if ((te_quotient_sum*te_quotient_sum*300*300)/goodTRThits - TrailingEdge*TrailingEdge > 0){
	  //TrailingEdgeSigma = sqrt((te_quotient_sum*te_quotient_sum*300*300)/goodTRThits - TrailingEdge*TrailingEdge);
	  //TrailingEdgeError = TrailingEdgeSigma/sqrt(goodTRThits);
	}
	//else TrailingEdgeError = -999.0;

	tegap = tegap - highestTGap;
	if (tegap>0.&& hont>1.0) tegap = tegap/(hont-1.0); // average TE gap for a track
	

	if ( !m_TRTdEdxTool.name().empty() ) dEdx =  m_TRTdEdxTool->dEdx((&track));
	else 	dEdx =  -999.0;
      }
      else{
	TrailingEdge = -999.0;
	//TrailingEdgeError = -999.0;
	LikelihoodBeta = -999.0;
	LikelihoodError = -999.0;
	RCorrectedAverageBitsOverThreshold = -999.0;
	tegap = -999.0;
	dEdx =  -999.0;
      }
      
    } //track states
    
    if (bitpatternfail) {
      RCorrectedAverageBitsOverThreshold =-999.;
      TrailingEdge=-999.;
      //TrailingEdgeError=-999.;
      NLastBits=-999;
      LikelihoodBeta = -999.;
      LikelihoodError = -999.;
      HighTbits  = -999;
      dEdx       = -999.;
      tegap = -999.0;
      ATH_MSG_WARNING( "No TRT Bitpattern found for some straws" );
    }
    else if (m_TrtToolInitSuccess)
      {
	if (pTrk > 15000.0){
	  // Determine LikelihoodBeta and LikelihoodError
	  LikelihoodValues = callTrtToolBetaLiklihood(track);
	  LikelihoodBeta   = LikelihoodValues[0];
	  LikelihoodError  = LikelihoodValues[1];
	}
      }
     
    //CSMP_ind Discriminators;

    Discriminators.push_back(RCorrectedAverageBitsOverThreshold);
    Discriminators.push_back(TrailingEdge);
    Discriminators.push_back(tegap);
    Discriminators.push_back(float(NLastBits));
    Discriminators.push_back(HighThresholdHits);
    Discriminators.push_back(pTrk);
    Discriminators.push_back(float(goodTRThits));
    Discriminators.push_back(LikelihoodBeta);
    Discriminators.push_back(LikelihoodError);
    Discriminators.push_back(float(HighTbits));
    Discriminators.push_back(dEdx);
			     
    return Discriminators;
  }
  
}//end of namespace
