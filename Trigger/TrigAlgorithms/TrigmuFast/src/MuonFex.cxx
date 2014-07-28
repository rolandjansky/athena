/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuMessageInterface.h"
#include "TrigmuFast/MuServiceInterface.h"
#include "TrigmuFast/MuonFex.h"
#include "TrigmuFast/SagEstimate.h"
#include "TrigmuFast/SagEstimateCos.h"
#include "TrigmuFast/AlphaBetaEstimate.h"
#include "TrigmuFast/PtEstimate.h"
#include "TrigmuFast/PtFromRad.h"
#include "TrigmuFast/EPtEstimate.h"
#include "TrigmuFast/EPtEstimatePol2.h"
#include "TrigmuFast/StationFitPB.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

#include <math.h>

using namespace std;

float compute_dp(float phi1, float phi2)
{
    float dp = phi2 - phi1;
    if (dp >= M_PI && phi1*phi2 < 0.) {
        if (phi2 < 0.0) phi2 += 2*M_PI;
	if (phi1 < 0.0) phi1 += 2*M_PI;
	dp = phi2 - phi1;
    }
    return dp;
}

bool coverageOK(float phi)
{
    float Phi   = fabsf(phi);
    float step  = M_PI/8.;
    int   count = 0;
    while (Phi > step) {
       Phi -= step;
       count += 1;
    }
    
    float delta = 0;
    if (count%2) delta = M_PI/4.-Phi;
    else         delta = Phi;

    if (delta < .2) return true;

    return false;
}

bool EIcoverageOKforHighPt(float eta, float phi)
{
   const float ei12_eta_min = 1.24;
   const float ei12_eta_max = 1.94;

   if( fabs(eta) > ei12_eta_max ) return false;

   if( fabs(eta) > ei12_eta_min ) return true;

   float absPhi  = fabsf(phi);
   float step    = M_PI/8.;
   int   count   = 0;
   while (absPhi > step) {
      absPhi -= step;
      count += 1;
   }

   float deltaPhi = absPhi;
   if (count%2 == 1) deltaPhi = absPhi - M_PI/8.;

   int sector = 0;
   if( phi >= 0 ) {
      sector = 9;
      if( count==0 ) sector=1;
      else if( count<=2 ) sector = 3; 
      else if( count<=4 ) sector = 5; 
      else if( count<=6 ) sector = 7; 
   }
   else {
      sector = 9;
      if( count==0 ) sector=1;
      else if( count<=2 ) sector = 15; 
      else if( count<=4 ) sector = 13; 
      else if( count<=6 ) sector = 11; 
   }

   //
   const float eta_min   = 1.00;
   const float eta_lower = 1.15;
   const float eta_max   = 1.245; // 1.24 + margin

   const float dphi_half            = 0.23;
   const float dphi_half_lower      = 0.074;
   const float dphi_half_asym_small = 0.07;
   const float dphi_half_asym_large = 0.21;

   if( sector==3 || sector==5 || sector==13 ) {
      if( eta_min < eta && eta < eta_max && fabs(deltaPhi) < dphi_half ) return true;
      return false;
   }

   if( sector==1 || sector==9 ) {
      if( eta_min   < eta && eta < eta_lower && fabs(deltaPhi) < dphi_half )       return true;
      if( eta_lower < eta && eta < eta_max   && fabs(deltaPhi) < dphi_half_lower ) return true;
      return false;
   }

   if( sector==7 ) {
      if( deltaPhi >= 0 && eta_min < eta && eta < eta_max && fabs(deltaPhi) < dphi_half_asym_large ) return true;
      if( deltaPhi < 0  && eta_min < eta && eta < eta_max && fabs(deltaPhi) < dphi_half_asym_small ) return true;
      return false;
   }

   if( sector==11 ) {
      if( deltaPhi >= 0 && eta_min < eta && eta < eta_lower && fabs(deltaPhi) < dphi_half_asym_large ) return true;
      if( deltaPhi < 0  && eta_min < eta && eta < eta_lower && fabs(deltaPhi) < dphi_half_asym_small ) return true;
      return false;
   }

   if( sector==15 ) {
      if( deltaPhi >= 0 && eta_min < eta && eta < eta_lower && fabs(deltaPhi) < dphi_half_asym_small ) return true;
      if( deltaPhi < 0  && eta_min < eta && eta < eta_lower && fabs(deltaPhi) < dphi_half_asym_large ) return true;
      return false;
   }
   
   return false;
}

const string MuonFex::s_type = "MuonFex";
const string MuonFex::s_name = "Muon Features Extraction - Roma 1";

MuonFex::MuonFex(MuMessageInterface& msg,MuServiceInterface& svc,
                 bool useTiltedBField,
                 const TriggerData& trig_data,
                 const TrackFit& track_fit,
                 LayerDigits (*digits)[MAXLAYER])
    : m_msg(&msg),m_svc(&svc),m_useTiltedBField(useTiltedBField),
      m_trigger_data(trig_data),m_track_fit(track_fit),m_digits(digits),
      m_EPtStrategy(MuonFexConsts::EPtStrategy_linear)
{
}

MuonFex::~MuonFex(void)
{
}

MUFAST::ErrorCode
MuonFex::start(MuonFeatureDetails* det)
{
    MUFAST::ErrorCode retval = MUFAST::NO_ERROR;

    // Track fit into stations //////////////////////////////////////////////
    StationFitPB fit(*m_msg,m_track_fit,m_digits,m_super_points);          //
    TrigTimer* fit_time = m_svc->give_timer(fit.number());                 //
                                                                           //
    if(fit_time) fit_time->start();                                        //
    if(!fit.run(det)) {                                                    //
        //if (det!=0) det->setError(MUFAST::STATION_FIT_ERROR);            //
        return MUFAST::STATION_FIT_ERROR;                                  //
    }                                                                      //
    if(fit_time) fit_time->stop();                                         //
                                                                           //
    if (m_msg->info_printout() >> fit.number() & 1)                        //
    {                                                                      //
        m_msg->lock();                                                     //
        fit.printout();                                                    //
	m_msg->unlock();                                                   //
    }                                                                      //
    /////////////////////////////////////////////////////////////////////////


    //if ( !m_svc->isCosmic() && m_track_fit.isEndcap )
    if ( m_track_fit.isEndcap )
    { 
        // Sagitta estimation ///////////////////////////////////////////////
        AlphaBetaEstimate ecurv(*m_msg, m_svc->ptLUTSvc(), m_trigger_data, //
	                         m_track_fit, m_super_points, m_feature);  //
                                                                           //
        TrigTimer* ecurv_time = m_svc->give_timer(ecurv.number());         //
                                                                           //
        if(ecurv_time) ecurv_time->start();                                //
        if(!ecurv.run(det) && !m_svc->isCosmic()) {                        //
	    //if (det!=0) det->setError(MUFAST::EST_ALPHABETA_ERROR);      // 
  	    // return MUFAST::EST_ALPHABETA_ERROR;                         //
	    // Error from AlphaBetaEstimate is eta out of range only
	    // (just set flag to allow monitoring at muFast.cxx)
  	    retval = MUFAST::EST_ALPHABETA_ERROR;
	}                                                                  //
        if(ecurv_time) ecurv_time->stop();                                 //
                                                                           //
        if (m_msg->info_printout() >> ecurv.number() & 1)                  //
        {                                                                  //
            m_msg->lock();                                                 //
            ecurv.printout();                                              //
	    m_msg->unlock();                                               //
        }                                                                  //
        /////////////////////////////////////////////////////////////////////


        // Pt estimation ////////////////////////////////////////////////////
	PtEstimateStd* pt = 0;                                             //
                                                                           //
	if( m_EPtStrategy == MuonFexConsts::EPtStrategy_pol2 ) {           //
	   pt = (PtEstimateStd*) new EPtEstimatePol2(*m_msg,               //
						     m_useTiltedBField,    //
						     m_svc->ptLUTSvc(),    //
						     m_feature,            //
						     m_svc->lut());        // 
	}                                                                  //
	else {                                                             //
	   if( m_EPtStrategy != MuonFexConsts::EPtStrategy_linear ) {      //
	      m_msg->lock();                                               //
	      char buf [256];                                              //
	      sprintf(buf,"MuonFex: unsupported EPtStrategy=%i",           //
		      m_EPtStrategy);                                      //		      
	      m_msg->displayWarning(buf);                                  //
	      sprintf(buf,"MuonFex: using linear strategy...");            //
	      m_msg->displayWarning(buf);                                  //
	      m_msg->unlock();                                             //
	   }                                                               //
	   pt = (PtEstimateStd*) new EPtEstimate(*m_msg,                   //
						 m_useTiltedBField,        //
						 m_svc->ptLUTSvc(),        //
						 m_feature,                //
						 m_svc->lut());            // 
	}                                                                  //
                                                                           //
        TrigTimer* pt_time = m_svc->give_timer(pt->number());              //
                                                                           //
        if(pt_time) pt_time->start();                                      //
        if(!pt->run(det) && !m_svc->isCosmic()) {                          //
	    //if (det!=0) det->setError(MUFAST::EST_PT_ERROR);             //
	   if( pt!= 0 ) delete pt;                                         //
	    return MUFAST::EST_PT_ERROR;                                   //
	}                                                                  //
        if(pt_time) pt_time->stop();                                       //
                                                                           //
        if (m_msg->info_printout() >> pt->number() & 1)                    //
        {                                                                  //
            m_msg->lock();                                                 //
            pt->printout();                                                //
	    m_msg->unlock();                                               //
        }                                                                  //
                                                                           //
	if( pt != 0 ) delete pt;                                           //
        /////////////////////////////////////////////////////////////////////

	
	// Apply extra cuts /////////////////////////////////////////////////
	float eta_trigger = m_trigger_data.tgcMid1[0];                     //
	float DeltaEta=m_trigger_data.tgcMid2[0]-m_trigger_data.tgcMid1[0];//
	float DeltaPhi=compute_dp(m_trigger_data.tgcMid1[1],               //
	                          m_trigger_data.tgcMid2[1]);              //
				                                           //
        bool testEP = (m_svc->hasEcut('1'))?                               //
	          (fabsf(DeltaEta)<0.061 && fabsf(DeltaPhi)<0.032) : true; //
                                                                           //
        float sp_r[3] = {0., 0., 0.};		                           //
        float sp_z[3] = {0., 0., 0.};		                           //
        for(int st=0; st<MAXSTATION; st++) {                               //
            if( m_super_points[st].R > 0. ) {                              //
	      sp_r[st%3] = m_super_points[st].R;                           //
	      sp_z[st%3] = m_super_points[st].Z;                           //
            }                                                              //
        }                                                                  //
	                                                                   //
	bool testMI = (m_svc->hasEcut('2'))? (sp_r[1] != 0.) : true;       //
	                                                                   //
	int segments = 0;                                                  //
	for (int st=0; st<3;st++) if (sp_r[st] != 0.) segments += 1;       //
	                                                                   //
        bool testSE = true;                                                //
	if ( (fabs(eta_trigger) >= 1.5  && fabs(eta_trigger) < 2.0   ) ||  //
	     (fabs(eta_trigger) >  1.25 && fabs(eta_trigger) < 1.5 &&      //
	                       coverageOK(m_trigger_data.tgcMid1[1]) ) ) { //
            if (segments < 2 && m_svc->hasEcut('3')) testSE = false;       //
        }                                                                  //
	                                                                   //
	bool testHighPtTight = true;                                       //
	if( m_svc->hasEcut('4') ) {                                        //
	   bool hasInner  = false;                                         //
	   bool hasMiddle = false;                                         //
	   bool hasOuter  = false;                                         //
	   const float RZ_ZERO_LIMIT = 0.01;                               //
	   const float INNER_Z_CUT   = 10000;                              //
	   const float MIDDLE_Z_CUT  = 16000;                              //
	   for(unsigned int i=0; i<3; i++) {                               //
	      float r = sp_r[i];                                           //
	      float z = sp_z[i];                                           //
	      if( r > RZ_ZERO_LIMIT && fabs(z) > RZ_ZERO_LIMIT ) {         //
		 if     ( fabs(z) < INNER_Z_CUT )  { hasInner  = true; }   //
		 else if( fabs(z) < MIDDLE_Z_CUT ) { hasMiddle = true; }   //
		 else { hasOuter  = true; }                                //
	      }                                                            //
	   }                                                               //
	   if( fabs(eta_trigger)<1.24 ) {                                  //
	      if( EIcoverageOKforHighPt(eta_trigger,                       //
					m_trigger_data.tgcMid1[1]) &&      //
		  ! hasInner  )  testHighPtTight = false;                  //
	      if( ! hasMiddle )  testHighPtTight = false;                  //
	   }                                                               //
	   else if( fabs(eta_trigger)<1.35 ) {                             //
	      if( ! hasInner  )  testHighPtTight = false;                  //
	      if( ! hasMiddle )  testHighPtTight = false;                  //
	   }                                                               //
	   else if( fabs(eta_trigger)<1.94 ) {                             //
	      if( ! hasInner )                testHighPtTight = false;     //
	      if( ! hasMiddle && ! hasOuter ) testHighPtTight = false;     //
	   }                                                               //
	   else if( fabs(eta_trigger)<2.45 ) {                             //
	      if( ! hasMiddle )  testHighPtTight = false;                  //
	      if( ! hasOuter  )  testHighPtTight = false;                  //
	   }                                                               //
	   else {                                                          //
	      if( ! hasMiddle )  testHighPtTight = false;                  //
	   }                                                               //
	}                                                                  //
	                                                                   //
	if ( !(testEP && testMI && testSE && testHighPtTight) ) {          //
	    m_feature.Pt = 0.;                                             //
	    m_feature.EtaMap = 0.;                                         //
	}                                                                  //
	/////////////////////////////////////////////////////////////////////   

    }
    else
    {
    
        SagEstimateStd* sag;	
        // Sagitta estimation ///////////////////////////////////////////////
        if(m_svc->isCosmic()) {                                            //
            sag = new SagEstimateCos(*m_msg,m_trigger_data,m_track_fit,    //
	                           m_super_points,m_feature);              //
        } else {                                                           //
            sag = new SagEstimate(*m_msg,m_trigger_data,m_track_fit,       //
	                          m_super_points,m_feature);               //
        }                                                                  //
                                                                           //
        TrigTimer* sag_time = m_svc->give_timer(sag->number());            //
                                                                           //
        if(sag_time) sag_time->start();                                    //
        if(!sag->run(det)) {                                               //
	    delete sag;                                                    //
	    //if (det!=0) det->setError(MUFAST::EST_SAGITTA_ERROR);        //
	    return MUFAST::EST_SAGITTA_ERROR;                              //
	}                                                                  //
        if(sag_time) sag_time->stop();                                     //
                                                                           //
        if (m_msg->info_printout() >> sag->number() & 1)                   //
        {                                                                  //
            m_msg->lock();                                                 //
            sag->printout();                                               //
	    m_msg->unlock();                                               //
        }                                                                  //
        delete sag;                                                        //
        /////////////////////////////////////////////////////////////////////

        PtEstimateStd* estimate;

        // Pt estimation ////////////////////////////////////////////////////
        if(m_svc->lut()->haveLUTforRadius())                               //
        {                                                                  //
            estimate = new PtFromRad(*m_msg,m_feature,m_svc->lut());       //
        }                                                                  //
        else                                                               //
        {                                                                  //
            estimate = new PtEstimate(*m_msg,m_feature,m_svc->lut());      //
        }                                                                  //
                                                                           //
        TrigTimer* pt_time = m_svc->give_timer(estimate->number());        //
                                                                           //
        if(pt_time) pt_time->start();                                      //
        if(!estimate->run(det))                                            //
	{                                                                  //
	    delete estimate;                                               //
	    //if (det!=0) det->setError(MUFAST::EST_PT_ERROR);             //
	    return MUFAST::EST_PT_ERROR;                                   //
	}                                                                  //
        if(pt_time) pt_time->stop();                                       //
                                                                           //
        if(m_useTiltedBField) m_feature.Charge *= -1.;                     //
                                                                           //
        if (m_msg->info_printout() >> estimate->number() & 1)              //
        {                                                                  //
            m_msg->lock();                                                 //
            estimate->printout();                                          //
	    m_msg->unlock();                                               //
        }                                                                  //
        delete estimate;                                                   //
        /////////////////////////////////////////////////////////////////////

    }

    const float PHI_LIMIT = 3.141592;
    if( fabs(m_feature.phi) > PHI_LIMIT ) {
       float phi = (m_feature.phi>0) ? PHI_LIMIT : -PHI_LIMIT;
       char  buf[256];
       sprintf(buf,"MuonFex: phi out of range=%f, set to=%f",m_feature.phi,phi);
       m_msg->lock();
       if( fabs(fabs(m_feature.phi)-PHI_LIMIT) < 0.01 ) m_msg->displayInfo(buf);
       else m_msg->displayWarning(buf);
       m_msg->unlock();
       m_feature.phi = phi;
    }
    const float ETA_LIMIT = 5.0;
    if( fabs(m_feature.EtaMap) > ETA_LIMIT ) {
       float eta = (m_feature.EtaMap>0) ? ETA_LIMIT : -ETA_LIMIT;
       char  buf[256];
       sprintf(buf,"MuonFex: eta out of range=%f, set to=%f",m_feature.EtaMap,eta);
       m_msg->lock();
       m_msg->displayWarning(buf);
       m_msg->unlock();
       m_feature.EtaMap = eta;
    }
    
    return retval;
}

MuonFeature* MuonFex::muonFeatures(void) const
{
   if( ! m_feature.ok ) return new MuonFeature();

   MuonFeature *mf;

   if( m_svc->isCosmic() ) {
      mf =  new MuonFeature(m_feature.Saddress,
			    m_feature.Pt*m_feature.Charge,
			    m_feature.Spoint[1][0],
			    m_feature.EtaMap,
			    m_feature.phi,
			    m_feature.dir_phi,
			    m_feature.Spoint[1][1],
			    m_feature.Spoint[1][2],
			    m_trigger_data.beta);
   }
   else {
      mf = new MuonFeature(m_feature.Saddress,
			   m_feature.Pt*m_feature.Charge,
			   m_feature.Spoint[0][0],
			   m_feature.EtaMap,
			   m_feature.phi,
			   m_feature.dir_phi,
			   m_feature.Spoint[0][1],
			   m_feature.Spoint[0][2],
			   m_trigger_data.beta);
   }

   // add superpoints

   float sp_r[3];
   float sp_z[3];
   float sp_slope[3];
   for(int i=0; i<3; i++) {
      sp_r[i]=0.; sp_z[i]=0.; sp_slope[i]=0.;
   }
   int st;
   for(st=0; st<MAXSTATION; st++) {
      if( m_super_points[st].R > 0. ) {
	 sp_r[st%3]     = m_super_points[st].R;
	 sp_z[st%3]     = m_super_points[st].Z;
	 sp_slope[st%3] = m_super_points[st].Alin;
      }
   }

   mf->set_sp1(sp_r[0],sp_z[0],sp_slope[0]);
   mf->set_sp2(sp_r[1],sp_z[1],sp_slope[1]);
   mf->set_sp3(sp_r[2],sp_z[2],sp_slope[2]);

   // add pt variables

   mf->set_br(m_feature.Radius,m_feature.Sagitta);
   mf->set_ec(m_feature.alpha, m_feature.beta);

   // add DQ flags, for the moment eta/phi bin info
   // store eta and phi in dq_flag1
   double flag1 = double(int(fabs(m_feature.EtaMap)*1e3))*1e3 + double(m_feature.etaBin);
   flag1 += double(int(fabs(m_feature.PhiMap)*1e3))*1e-4 + double(m_feature.phiBin)*1e-7;
   if( m_feature.EtaMap < 0 ) flag1 += 1e9;
   if( m_feature.PhiMap < 0 ) flag1 += 1e8;

   // m_msg->lock(); 
   // char buf [256];
   // sprintf(buf,"EtaMap/etaBin/PhiMap/phiBin/flag1=%f/%i/%f/%i/%f",
   //         m_feature.EtaMap,m_feature.etaBin,m_feature.PhiMap,m_feature.phiBin,flag1);
   // m_msg->displayInfo(buf); 
   // m_msg->unlock(); 

   // store TGC/RPC readout failure flags in dq_flag2
   // -1: TGC or RPC data normaly read
   // >0: for TGC/RPC readout failure recovery but not implemented
   double flag2 = -1;
   
   mf->set_dq(flag1,flag2);
   
   //
   return mf;
}

void MuonFex::setEPtStrategy(int strategy)
{
   if( (strategy!=MuonFexConsts::EPtStrategy_linear) &&
       (strategy!=MuonFexConsts::EPtStrategy_pol2) ) {
      m_msg->lock(); 
      char buf [256];
      sprintf(buf,"MuonFex::setEPtStrategy   unsupported EPtStrategy=%i",strategy);
      m_msg->displayWarning(buf); 
      sprintf(buf,"MuonFex::setEPtStrategy   using linear strategy...");
      m_msg->displayWarning(buf); 
      m_msg->unlock(); 
      m_EPtStrategy = MuonFexConsts::EPtStrategy_linear;
   }
   else {
      m_EPtStrategy = strategy;
   }
}


