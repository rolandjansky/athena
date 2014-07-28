/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/OnlineSurvey.h"
#include "TrigmuFast/LVL1emu.h"
#include "TrigmuFast/MdtPatRec.h"
#include "TrigmuFast/MuonFex.h"
#include "TrigmuFast/Datatypes.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1D.h"
#include "TH2D.h"


void OnlineSurvey::bookHistograms( ITHistSvc *rootHistSvc, std::string name) {
    
    std::string path = name;

    m_InnerHitMul  = new TH1D("h131", "Hit multiplicity in the INNER road",
                             50, -.5, 50.5);
    if ( rootHistSvc->regHist(path + "h131", m_InnerHitMul) != StatusCode::SUCCESS ) return;

    
    m_MiddleHitMul = new TH1D("h132","Hit multiplicity in the MIDDLE road",
			      50, -.5, 50.5);
    if ( rootHistSvc->regHist(path + "h132", m_MiddleHitMul) != StatusCode::SUCCESS ) return;

    
    m_OuterHitMul  = new TH1D("h133","Hit multiplicity in the OUTER road",
			     50, -.5, 50.5);
    if ( rootHistSvc->regHist(path + "h133", m_OuterHitMul) != StatusCode::SUCCESS ) return;


    m_FitPChi2     = new TH1D("h151", "Prob Chi quad", 100, 0., 1.);
    if ( rootHistSvc->regHist(path + "h151", m_FitPChi2) != StatusCode::SUCCESS ) return;





    m_FitRS         = new TH1D("h152", "Fit Residual", 400, -.04, .04);
    if ( rootHistSvc->regHist(path + "h152", m_FitRS) != StatusCode::SUCCESS ) return;


    m_FitRSlowPChi2 = new TH1D("h153", "Fit Residual (low prob chi quad)",
                               400, -.04, .04);
    if ( rootHistSvc->regHist(path + "h153", m_FitRSlowPChi2) != StatusCode::SUCCESS ) return;

    m_FitRSmidPChi2 = new TH1D("h154","Fit Residual (middle prob chi quad)",
			        400, -.04, .04);
    if ( rootHistSvc->regHist(path + "h154", m_FitRSmidPChi2) != StatusCode::SUCCESS ) return;


    m_FitRShigPChi2 = new TH1D("h155","Fit Residual (hi prob chi quad)",
			        400, -.04, .04);
    if ( rootHistSvc->regHist(path + "h155", m_FitRShigPChi2) != StatusCode::SUCCESS ) return;





    m_Sagitta = new TH1D("h161", "sagitta", 100, -30., 30.);
    if ( rootHistSvc->regHist(path + "h161", m_Sagitta) != StatusCode::SUCCESS ) return;

    m_SagVSPtLarge = new TH2D("h171", "1/s vs Pt on Large Chambers",
                              100, 0., 100., 30, 0.,3.);
    if ( rootHistSvc->regHist(path + "h171", m_SagVSPtLarge) != StatusCode::SUCCESS ) return;


    m_SagVSPtSmall = new TH2D("h172", "1/s vs Pt on Small Chambers",
                              100, 0., 100., 30, 0., 3.);
    if ( rootHistSvc->regHist(path + "h172", m_SagVSPtSmall) != StatusCode::SUCCESS ) return;
   
    m_Efficiency = new TH1D("h181", "Efficiency of level 2 trigger",
                            2, -.5, 1.5);
    if ( rootHistSvc->regHist(path + "h181", m_Efficiency) != StatusCode::SUCCESS ) return;    
}

// CID 23212,213: UNINIT_CTOR 
void OnlineSurvey::initVariables(void)
{
   m_aPt     = 0;
   m_eff     = 0;
   m_innHits = 0;
   m_midHits = 0;
   m_outHits = 0;
   m_pos     = 0;
   m_saginv  = 0;
   m_sagitta = 0;
}

OnlineSurvey::OnlineSurvey(ITHistSvc *rootHistSvc) {
   initVariables(); // CID 23212,213: UNINIT_CTOR 
   bookHistograms(rootHistSvc, "/EXPERT/mufast/");
}

OnlineSurvey::OnlineSurvey(ITHistSvc *rootHistSvc, std::string name) {
   initVariables(); // CID 23212,213: UNINIT_CTOR 
   bookHistograms(rootHistSvc, name);
}

OnlineSurvey::OnlineSurvey() {
   initVariables(); // CID 23212,213: UNINIT_CTOR 
   voidHistograms();
}

OnlineSurvey::~OnlineSurvey(void)
{
    if( m_InnerHitMul )   delete m_InnerHitMul;
    if( m_MiddleHitMul )  delete m_MiddleHitMul;
    if( m_OuterHitMul )   delete m_OuterHitMul;
    if( m_FitPChi2 )      delete m_FitPChi2;
    if( m_FitRS )         delete m_FitRS;
    if( m_FitRSlowPChi2 ) delete m_FitRSlowPChi2;
    if( m_FitRSmidPChi2 ) delete m_FitRSmidPChi2;
    if( m_FitRShigPChi2 ) delete m_FitRShigPChi2;
    if( m_Sagitta )       delete m_Sagitta;
    if( m_SagVSPtLarge )  delete m_SagVSPtLarge;
    if( m_SagVSPtSmall )  delete m_SagVSPtSmall;
    if( m_Efficiency )    delete m_Efficiency;

}

void 
OnlineSurvey::voidHistograms(void)
{
    m_InnerHitMul   = 0;
    m_MiddleHitMul  = 0;
    m_OuterHitMul   = 0;
    m_FitPChi2      = 0;
    m_FitRS         = 0;
    m_FitRSlowPChi2 = 0;
    m_FitRSmidPChi2 = 0;
    m_FitRShigPChi2 = 0;
    m_Sagitta       = 0;
    m_SagVSPtLarge  = 0;
    m_SagVSPtSmall  = 0;
    m_Efficiency    = 0;
}

void
//OnlineSurvey::update(LVL1emu& LVL1EMU,MdtPatRec& MDTPATREC, MuonFex& FEX)
OnlineSurvey::update(MdtPatRec& MDTPATREC, MuonFex& FEX)
{
    // for the time being don't do anything with the LVL1 Emulation
    m_residuals.clear();
    m_res_inner.clear();
    m_res_middle.clear();
    m_res_outer.clear();
    
    m_innHits = -1;
    m_midHits = -1;
    m_outHits = -1;
    
    const SpointData* point = FEX.super_points();
    LayerDigits (*digip)[MAXLAYER] = MDTPATREC.digits().digi;
            
    
    float norm = 0.;
      
    for(int st=0;st<MAXSTATION;st++) 
    {
	int count = 0;

        for(int ly=0;ly<MAXLAYER;ly++) 
	{
	    DigitData* firstDig = digip[st][ly].first;
	    norm = (digip[st][ly].isEndcap)? 10. : 1.;
            if(firstDig)
	    {
                ++count;
                DigitData* tmpDig = firstDig;
                
		switch(st%3)
	        {
	            case(0):
	                m_res_inner.push_back(firstDig->Residual/norm);
			break;
		
	            case(1):
	                m_res_middle.push_back(firstDig->Residual/norm);
			break;
		
	            case(2):
	                m_res_outer.push_back(firstDig->Residual/norm);
			break;
		
	            default:
	                break;
	        }
		
		
                while(firstDig!=tmpDig->ring) 
		{
                    ++count;
                    tmpDig = tmpDig->ring;
                }
		
	        switch(st%3)
	        {
	            case(0):
	                //if (m_InnerHitMul) m_InnerHitMul->Fill((float)count,1.);
		        m_innHits = count;
			m_res_inner.push_back(tmpDig->Residual/norm);
			break;
		
	            case(1):
	                //if (m_MiddleHitMul) m_MiddleHitMul->Fill((float)count,1.);
		        m_midHits = count;
			m_res_middle.push_back(tmpDig->Residual/norm);
			break;
		
	            case(2):
	                //if (m_OuterHitMul) m_OuterHitMul->Fill((float)count,1.);
		        m_outHits = count;
			m_res_outer.push_back(tmpDig->Residual/norm);
			break;
		
	            default:
	                break;
	        }		
		
            }
        }
    }
    
    
    for(int st=0;st<MAXSTATION;st++) 
    {
        if (m_FitPChi2) m_FitPChi2->Fill(point[st].PChi2,1.);
        for(int i=0;i<8;i++)
	{
	    float rs=point[st].Residual[i];
	    if(rs) 
	    {
                if(m_FitRS) m_FitRS->Fill(rs/norm,1.);
                m_residuals.push_back(rs/norm);
		
		if(point[st].PChi2<=.3 && m_FitRSlowPChi2) m_FitRSlowPChi2->Fill(rs/norm,1.);
                else if (point[st].PChi2>.3&&point[st].PChi2<=.6 && m_FitRSmidPChi2)
	            m_FitRSmidPChi2->Fill(rs/norm,1.);
                else if (point[st].PChi2>.6 && m_FitRShigPChi2)
		m_FitRShigPChi2->Fill(rs/norm,1.);
            }
	}
    }
    
    
    const MuFeature& FEATURE = FEX.feature();
    const int ok        = FEATURE.ok;
    const float sagitta = fabsf(FEATURE.Sagitta);
    
    m_saginv = (sagitta)? 1./sagitta : 9999.;
    m_sagitta = (sagitta)? FEATURE.Sagitta : 9999.;
    m_pos = FEATURE.Saddress;
    m_aPt = (ok)? fabsf(FEATURE.Pt) : -9999.;

    if(ok) 
    {
        if (m_Efficiency) m_Efficiency->Fill(1.,1.);
	m_eff = 1;
	
        if (m_Sagitta) m_Sagitta->Fill(FEATURE.Sagitta,1.);
	    
        if(sagitta != 0) {
	    if(m_pos==0) if (m_SagVSPtLarge) m_SagVSPtLarge->Fill(m_aPt,m_saginv,1.);
	    if(m_pos==2) if (m_SagVSPtSmall) m_SagVSPtSmall->Fill(m_aPt,m_saginv,1.);
        }
    } else 
    {
        if (m_Efficiency) m_Efficiency->Fill(0.,1.);
	m_eff = 0;
    }

}

