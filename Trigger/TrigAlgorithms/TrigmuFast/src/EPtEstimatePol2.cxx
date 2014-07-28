/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <string>
#include <math.h>

#include "TrigmuFast/PtLUTSvc.h"
#include "TrigmuFast/EPtEstimatePol2.h"
#include "TrigmuFast/MuLUT.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

EPtEstimatePol2::EPtEstimatePol2(MuMessageInterface& msg,
				 bool tilt_field,
				 const PtLUTSvc* LUTsvc,
				 MuFeature& feature,
				 const MuLUT* mu_lut)
   : PtEstimateStd(msg,feature,mu_lut), m_tilt_field(tilt_field), pPtLUTSvc(LUTsvc), m_doDebug(true)
{}

EPtEstimatePol2::~EPtEstimatePol2(void)
{}

bool EPtEstimatePol2::run(MuonFeatureDetails* det)
{
   char buf[256];

   const char* fname = "EPtEstimatePol2::run ";

   if( m_doDebug ) m_msg->lock();

   if (m_feature.etaBin < -1) {
      sprintf(buf,"%s Eta bin out of range:%i",fname,m_feature.etaBin);
      if( ! m_doDebug ) m_msg->lock();
      m_msg->displayWarning(buf);
      m_msg->unlock();
      m_feature.ok = 0;
      return false;
   }

   if( m_doDebug ) {
      sprintf(buf,"%s initial m_feature.Pt=%8.2f, m_feature.Charge=%3.1f",fname,m_feature.Pt,m_feature.Charge);
      m_msg->displayDebug(buf);
   }

   const float ZERO_LIMIT = 1e-5;

   // use the TGC PT if the MDT fit is not available
   if ( fabs(m_feature.slope)<ZERO_LIMIT && fabs(m_feature.intercept)<ZERO_LIMIT ) {
      if( m_doDebug ) {
	 sprintf(buf,"%s MDT fit not available. use TGC pT",fname);
	 m_msg->displayDebug(buf);
	 m_msg->unlock();
      }
      return true;
   }

   float tgcPt = fabs(m_feature.Pt) * m_feature.Charge;

   // MDT pT by alpha
   int  side   = (m_feature.EtaMap <= 0.0) ? 0 : 1;
   int  charge = (m_feature.intercept * m_feature.EtaMap) < 0.0 ? 1 : 0;
   float mdtPt = pPtLUTSvc->lookup(side, charge, PtLUTSvc::ALPHAPOL2, m_feature.etaBin,
				   m_feature.phiBin, m_feature.alpha) / 1000;

   if (charge == 0)  mdtPt = -mdtPt;
   if (m_tilt_field) mdtPt = -mdtPt;
   if( m_doDebug ) {
      sprintf(buf,"%s side/charge/m_tilt_field=%i/%i/%i",fname,side,charge,m_tilt_field);
      m_msg->displayDebug(buf);
      sprintf(buf,"%s mdtPt=%8.2f",fname,mdtPt);
      m_msg->displayDebug(buf);
   }

   //
   const float ALPHA_TO_BETA_PT    = 10;
   const float ALPHA_TO_BETA_RATIO = 0.5;

   // use MDT beta if condition allows
   if (fabs(mdtPt) > ALPHA_TO_BETA_PT && fabs(m_feature.beta)>ZERO_LIMIT) {
      float betaPt = pPtLUTSvc->lookup(side, charge, PtLUTSvc::BETAPOL2, m_feature.etaBin,
				       m_feature.phiBin, m_feature.beta) / 1000;
      if (charge == 0)  betaPt = -betaPt;
      if (m_tilt_field) betaPt = -betaPt;
      if( m_doDebug ) {
	 sprintf(buf,"%s betaPt=%8.2f",fname,betaPt);
	 m_msg->displayDebug(buf);
      }
      // original "skip diff" was:
      // Skip diff test if no outer Z
      // if (m_feature.Spoint[2][1] == 0.0 || fabs((betaPt - mdtPt) / mdtPt) < ALPHA_TO_BETA_RATIO)
      //     mdtPt = betaPt;
      if ( fabs((betaPt - mdtPt) / mdtPt) < ALPHA_TO_BETA_RATIO ) mdtPt = betaPt;
      else if ( m_feature.Spoint[2][1] == 0.0 ) {
	 if( fabs(betaPt) > fabs(mdtPt) || (fabs((tgcPt-mdtPt)/mdtPt) > fabs((tgcPt-betaPt)/betaPt)) ) mdtPt = betaPt;
      }
   }

   //
   if(mdtPt!=0.0) {
      m_feature.Pt     = fabs(mdtPt);
      m_feature.Charge = mdtPt / fabs(m_feature.Pt);
   }
   //cout << "EPtEstimatePol2::run m_feature.Pt=" << m_feature.Pt << ", m_feature.Charge=" << m_feature.Charge << endl;
   
   if(det!=0) {
      det->setCharge(m_feature.Charge);
      det->setPt(m_feature.Pt*m_feature.Charge);
   }

   if( m_doDebug ) m_msg->unlock();

   return true;
}
