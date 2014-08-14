/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/ResidualPlots.h"

void ResidualPlots::initializePlots(){	
	Book1D(m_A_EE_Residuals, "m_A_EE_Residuals", m_sTrackCollectionName+": Hit Residuals EE - Side A : "+m_sSector+";[#mum]" , 100,-2000.0,2000.0);
	Book1D(m_C_EE_Residuals, "m_C_EE_Residuals", m_sTrackCollectionName+": Hit Residuals EE - Side C : "+m_sSector+";[#mum]" , 100,-2000.0,2000.0);
	Book1D(m_A_BEE_Residuals, "m_A_BEE_Residuals", m_sTrackCollectionName+": Hit Residuals BEE - Side A : "+m_sSector+";[#mum]", 100,-2000.0,2000.0);  
	Book1D(m_C_BEE_Residuals, "m_C_BEE_Residuals", m_sTrackCollectionName+": Hit Residuals BEE - Side C : "+m_sSector+";[#mum]", 100,-2000.0,2000.0);
	Book1D(m_A_CSC_Residuals, "m_A_CSC_Residuals", m_sTrackCollectionName+": Hit Residuals CSC - Side A : "+m_sSector+";[#mum]", 200,-1500.0,1500.0);
	Book1D(m_C_CSC_Residuals, "m_C_CSC_Residuals", m_sTrackCollectionName+": Hit Residuals CSC - Side C : "+m_sSector+";[#mum]", 200,-1500.,1500.);
	Book2D(m_HR_Eta_I, "m_HR_Eta_S01_I", m_sTrackCollectionName+": MDT Hit Residuals : "+m_sSector+" : I;[#mum];#eta", 200,-2000.0,2000.0,30,-3.0,3.0);
	Book2D(m_HR_Eta_M, "m_HR_Eta_S01_M", m_sTrackCollectionName+": MDT Hit Residuals : "+m_sSector+" : M;[#mum];#eta", 200,-2000.0,2000.0,30,-3.0,3.0);
	Book2D(m_HR_Eta_O, "m_HR_Eta_S01_O", m_sTrackCollectionName+": MDT Hit Residuals : "+m_sSector+" : O;[#mum];#eta", 200,-2000.0,2000.0,30,-3.0,3.0);
}

void ResidualPlots::fill(float eta, int imdtstationName, bool isMDT, bool isCSC, bool Fill_HR_Histograms, float residual) {

  if (isMDT){
    if (imdtstationName == 6) {       
      if ( eta < 0 ) m_C_BEE_Residuals->Fill(residual*1000.0);   //In Microns //<-- this will provide us with a sigma(residual) for the error calc.
      if ( eta > 0 ) m_A_BEE_Residuals->Fill(residual*1000.0);   //In Microns //<-- this will provide us with a sigma(residual) for the error calc.
    }
    
    if (imdtstationName == 14 || imdtstationName == 15 ){          
	    if ( eta < 0 ) m_C_EE_Residuals->Fill(residual*1000.0);   //In Microns //<-- this will provide us with a sigma(residual) for the error calc.
      if ( eta > 0 ) m_A_EE_Residuals->Fill(residual*1000.0);   //In Microns //<-- this will provide us with a sigma(residual) for the error calc.
                
    }
    
    if (Fill_HR_Histograms){
	    int i = imdtstationName;
      if ( (i == 0) || (i == 1) || (i == 7) || (i ==52) || (i ==12) || (i ==13) || (i ==35) || (i ==36) || (i ==49)  ) m_HR_Eta_I->Fill(residual*1000.0,eta);
      if ( (i == 2) || (i == 3) || (i == 8) || (i ==16) || (i ==17) || (i ==18) || (i ==37) || (i ==38) || (i == 6) || (i ==14) || (i ==15) ) m_HR_Eta_M->Fill(residual*1000.0, eta); 
      if ( (i == 4) || (i == 5) || (i == 9) || (i ==10) || (i ==11) || (i ==20) || (i ==21)  ) m_HR_Eta_O->Fill(residual*1000.0, eta);
    }
  }
	else if (isCSC){
		if ( eta < 0 ) m_C_CSC_Residuals->Fill(residual*1000.0);
		if ( eta > 0 ) m_A_CSC_Residuals->Fill(residual*1000.0);
	}

}








