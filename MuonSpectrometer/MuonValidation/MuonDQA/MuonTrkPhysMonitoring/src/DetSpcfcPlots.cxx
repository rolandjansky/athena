/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrkPhysMonitoring/DetSpcfcPlots.h"

using namespace MuonTrkPhysMonUtils;

DetSpcfcPlots::DetSpcfcPlots(PlotBase *pParent):PlotBase(pParent),
m_A_EE_UHitRes(0),m_C_EE_UHitRes(0),m_A_BEE_UHitRes(0),m_C_BEE_UHitRes(0),
m_A_CSC_UHitRes(0),m_C_CSC_UHitRes(0),m_HitRes_Eta_Phi_Mean_I(0),
m_HitRes_Eta_Phi_Width_I(0),m_HitRes_Eta_Phi_Mean_M(0),m_HitRes_Eta_Phi_Width_M(0),
m_HitRes_Eta_Phi_Mean_O(0),m_HitRes_Eta_Phi_Width_O(0),m_MDT_TrackResiduals(0),
m_MDT_TR_EC(0),m_MDT_TR_BC(0),m_MDT_TR_BA(0),m_MDT_TR_EA(0),m_MDT_TR_Residuals(0),
m_oResidualPlotsAll(this, "all"),
m_oResidualPlots_s1(this, "Sector 01"),
m_oResidualPlots_s2(this, "Sector 02"),
m_oResidualPlots_s3(this, "Sector 03"),
m_oResidualPlots_s4(this, "Sector 04"),
m_oResidualPlots_s5(this, "Sector 05"),
m_oResidualPlots_s6(this, "Sector 06"),
m_oResidualPlots_s7(this, "Sector 07"),
m_oResidualPlots_s8(this, "Sector 08"),
m_oResidualPlots_s9(this, "Sector 09"),
m_oResidualPlots_s10(this, "Sector 10"),
m_oResidualPlots_s11(this, "Sector 11"),
m_oResidualPlots_s12(this, "Sector 12"),
m_oResidualPlots_s13(this, "Sector 13"),
m_oResidualPlots_s14(this, "Sector 14"),
m_oResidualPlots_s15(this, "Sector 15"),
m_oResidualPlots_s16(this, "Sector 16"){}


void DetSpcfcPlots::initializePlots(){
	
  Book1D(m_A_EE_UHitRes, "m_A_EE_UHitRes",m_sTrackCollectionName+": <Hit Residuals EE - Side A>;;[#mum]", 16,1.,17.);      
  Book1D(m_C_EE_UHitRes, "m_C_EE_UHitRes",m_sTrackCollectionName+": <Hit Residuals EE - Side C>;;[#mum]", 16,1.,17.);
  Book1D(m_A_BEE_UHitRes, "m_A_BEE_UHitRes",m_sTrackCollectionName+": <Hit Residuals BEE - Side A>;;[#mum]", 16,1.,17.);    
  Book1D(m_C_BEE_UHitRes, "m_C_BEE_UHitRes",m_sTrackCollectionName+": <Hit Residuals BEE - Side C>;;[#mum]", 16,1.,17.);
  Book1D(m_A_CSC_UHitRes, "m_A_CSC_UHitRes",m_sTrackCollectionName+": <Hit Residuals CSC - Side A>;;[#mum]", 16,1.,17.);
  Book1D(m_C_CSC_UHitRes, "m_C_CSC_UHitRes",m_sTrackCollectionName+": <Hit Residuals CSC - Side C>;;[#mum]", 16,1.,17.);

  LabelSectorAxis(m_A_EE_UHitRes->GetXaxis());
  LabelSectorAxis(m_C_EE_UHitRes->GetXaxis());
  LabelSectorAxis(m_A_BEE_UHitRes->GetXaxis());
  LabelSectorAxis(m_C_BEE_UHitRes->GetXaxis());
  LabelSectorAxis(m_A_CSC_UHitRes->GetXaxis());
  LabelSectorAxis(m_C_CSC_UHitRes->GetXaxis());

  Book1D(m_MDT_TR_Residuals, "m_MDT_TR_Residuals",m_sTrackCollectionName+": Hit Residuals MDT;[#mum]", 200,-2000.0,2000.0);
  Book1D(m_MDT_TrackResiduals, "m_MDT_TrackResiduals",m_sTrackCollectionName+": <Hit Residuals MDT>;Large Detector #eta Regions;[#mum]", 4,0.,4.);
  Book1D(m_MDT_TR_EC, "m_MDT_TR_EC",m_sTrackCollectionName+": Hit Residuals MDT;[#mum]", 200,-2000.0,2000.0);
  Book1D(m_MDT_TR_BC, "m_MDT_TR_BC",m_sTrackCollectionName+": Hit Residuals MDT;[#mum]",200,-2000.0,2000.0);
  Book1D(m_MDT_TR_BA, "m_MDT_TR_BA",m_sTrackCollectionName+": Hit Residuals MDT;[#mum]", 200,-2000.0,2000.0);
  Book1D(m_MDT_TR_EA, "m_MDT_TR_EA",m_sTrackCollectionName+": Hit Residuals MDT;[#mum]", 200,-2000.0,2000.0);

  Book2D(m_HitRes_Eta_Phi_Mean_I, "m_HitRes_Eta_Phi_Mean_I",m_sTrackCollectionName+": <MDT Hit Residuals> : Inner - [#mum];#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_HitRes_Eta_Phi_Width_I, "m_HitRes_Eta_Phi_Width_I",m_sTrackCollectionName+": MDT Hit Residual Width : Inner - [#mum];#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_HitRes_Eta_Phi_Mean_M, "m_HitRes_Eta_Phi_Mean_M",m_sTrackCollectionName+": <MDT Hit Residuals> : Mid - [#mum];#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_HitRes_Eta_Phi_Width_M, "m_HitRes_Eta_Phi_Width_M",m_sTrackCollectionName+": MDT Hit Residual Width : Mid - [#mum]#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_HitRes_Eta_Phi_Mean_O, "m_HitRes_Eta_Phi_Mean_O",m_sTrackCollectionName+": <MDT Hit Residuals>  : Outer - [#mum];#eta", 30,-3.0,3.0,16,1.,17.);
  Book2D(m_HitRes_Eta_Phi_Width_O, "m_HitRes_Eta_Phi_Width_O",m_sTrackCollectionName+": MDT Hit Residual Width : Outer - [#mum];#eta", 30,-3.0,3.0,16,1.,17.);

  LabelSectorAxis(m_HitRes_Eta_Phi_Mean_I->GetYaxis());
  LabelSectorAxis(m_HitRes_Eta_Phi_Width_I->GetYaxis());
  LabelSectorAxis(m_HitRes_Eta_Phi_Mean_M->GetYaxis());
  LabelSectorAxis(m_HitRes_Eta_Phi_Width_M->GetYaxis());      
  LabelSectorAxis(m_HitRes_Eta_Phi_Mean_O->GetYaxis());
  LabelSectorAxis(m_HitRes_Eta_Phi_Width_O->GetYaxis());

}

void DetSpcfcPlots::fill(const Trk::Perigee* measPerigee, int imdtstationName, bool isMDT, bool isCSC, bool Fill_HR_Histograms, float residual) {

  double phi_sector = PhiSector(measPerigee->parameters()[Trk::phi0]);
	float eta = measPerigee->eta();
  m_oResidualPlotsAll.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);
  if(phi_sector        < 2. ) m_oResidualPlots_s1.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 3. ) m_oResidualPlots_s2.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 4. ) m_oResidualPlots_s3.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 5. ) m_oResidualPlots_s4.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 6. ) m_oResidualPlots_s5.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 7. ) m_oResidualPlots_s6.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 8. ) m_oResidualPlots_s7.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 9. ) m_oResidualPlots_s8.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 10.) m_oResidualPlots_s9.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 11.) m_oResidualPlots_s10.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 12.) m_oResidualPlots_s11.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 13.) m_oResidualPlots_s12.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 14.) m_oResidualPlots_s13.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 15.) m_oResidualPlots_s14.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 16.) m_oResidualPlots_s15.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns
  else if(phi_sector   < 17.) m_oResidualPlots_s16.fill(eta, imdtstationName, isMDT, isCSC, Fill_HR_Histograms, residual);   //In Microns

  double	etaRegion = EtaRegionSmall(measPerigee->eta());
   if (Fill_HR_Histograms) {       
    //[HistFill] Fill MDT Residual Histogram
    if(etaRegion==0.5) m_MDT_TR_EC->Fill(residual);   //In Microns
    if(etaRegion==1.5) m_MDT_TR_BC->Fill(residual);   //In Microns
    if(etaRegion==2.5) m_MDT_TR_BA->Fill(residual);   //In Microns
    if(etaRegion==3.5) m_MDT_TR_EA->Fill(residual);   //In Microns
    if(etaRegion!=-1.0 || etaRegion!=4.5) m_MDT_TR_Residuals->Fill(residual);   //In Microns //<-- this will provide us with a sigma(residual) for the error calc.
  }
}

void DetSpcfcPlots::finalizePlots(){
  for (unsigned int sector = 1; sector < m_vSubNodes.size(); ++sector ){
    if(ResidualPlots* pSubNode = dynamic_cast<ResidualPlots*>(m_vSubNodes[sector])) {
      CalculateSectorResults(m_A_EE_UHitRes, pSubNode->m_A_EE_Residuals, sector);
      CalculateSectorResults(m_C_EE_UHitRes, pSubNode->m_C_EE_Residuals, sector);
      CalculateSectorResults(m_A_BEE_UHitRes, pSubNode->m_A_BEE_Residuals, sector);
      CalculateSectorResults(m_C_BEE_UHitRes, pSubNode->m_C_BEE_Residuals, sector);
      CalculateSectorResults(m_A_CSC_UHitRes, pSubNode->m_A_CSC_Residuals, sector);
      CalculateSectorResults(m_C_CSC_UHitRes, pSubNode->m_C_CSC_Residuals, sector);
    }
  }
  
  FinalizeTrackResidualPlots(m_MDT_TR_EC, 1, "Endcap C");
  FinalizeTrackResidualPlots(m_MDT_TR_BC, 2, "Barrel C");
  FinalizeTrackResidualPlots(m_MDT_TR_BA, 3, "Barrel A");
  FinalizeTrackResidualPlots(m_MDT_TR_EA, 4, "Endcap A");
            
}

void DetSpcfcPlots::CalculateSectorResults(TH1F* summaryPlot, TH1F* inputPlot, int sector){
  summaryPlot->SetBinContent(sector,inputPlot->GetMean(1));
  summaryPlot->SetBinError( sector,inputPlot->GetEntries() ? inputPlot->GetRMS(1)/sqrt(inputPlot->GetEntries()) : 0.0);			
}

void DetSpcfcPlots::FinalizeTrackResidualPlots(TH1F* hist, int iBin, std::string sLabel){
  m_MDT_TrackResiduals->SetBinContent(iBin,hist ->GetMean(1));
  m_MDT_TrackResiduals->SetBinError(  iBin,hist ->GetEntries() ? m_MDT_TR_Residuals->GetRMS(1)/sqrt(hist->GetEntries()) : 0.0);
  m_MDT_TrackResiduals->GetXaxis()->SetBinLabel(  iBin, sLabel.c_str());
}
