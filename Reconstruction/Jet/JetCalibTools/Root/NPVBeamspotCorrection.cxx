/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** Return approximate correction on the number of reconstructed vertices
 * due to different beamspot longitudinal lengths.
 *
 * Author: S. Pagan Griso <spagangriso@lbl.gov>
 * Adapted for ApplyJetCalibration/JetCalibTools by Joe Taenzer <joseph.taenzer@cern.ch>
 *
 * Description: Use GetNVertexBsCorrected(...) to get the
 *  corrected number of vertices expected on average with
 *  a beamspot length of 47mm, assuming your input value
 *  refers to a sample with beamspot length of 66mm (MC12a).
 *
 * Disclaimer: The following is based on a statistical extrapolation
 *  based on low-mu data. This is meant to give a feeling of the
 *  magnitude of the effect on average. 
 *  This function can be used as a "x-axis" scaling in order to 
 *  see the magnitude of the effect on quantities depending on 
 *  the number of vertices, or to roughly estimate if the residual
 *  discrepancy observed in the number of reconstructed vertices after
 *  a \<mu\> re-weighting is consistent with this effect.
 *
 * Other notes: If you are in need to get the correction for 
 *  different beamspot sizes, please just get in contact with me
 *  and I can easily provide you with a more comphrensive set of simple tools
 *  to do that.
 */

#include "JetCalibTools/CalibrationMethods/NPVBeamspotCorrection.h"

NPVBeamspotCorrection::NPVBeamspotCorrection()
  : m_g_nvtx_nreco_bs66mm(0), m_g_nvtx_nreco_bs47mm(0), m_invGraph(0), m_NPVmin(0), m_NPVmax(0)
{
  initNPVBeamspotCorrection();
}

NPVBeamspotCorrection::~NPVBeamspotCorrection()
{
    if (m_g_nvtx_nreco_bs66mm) delete m_g_nvtx_nreco_bs66mm;
    if (m_g_nvtx_nreco_bs47mm) delete m_g_nvtx_nreco_bs47mm; 
    if (m_invGraph) delete m_invGraph; 
}

void NPVBeamspotCorrection::initNPVBeamspotCorrection() {

  printf("  \n\nInitializing the NPV Beamspot correction tool\n");

  m_g_nvtx_nreco_bs47mm = NVtx_NReconstructible_bs47mm();
  m_g_nvtx_nreco_bs66mm = NVtx_NReconstructible_bs66mm();

  if (m_invGraph==0) {
    m_invGraph = new TGraph();
    for (int i=0;i<m_g_nvtx_nreco_bs66mm->GetN();++i) 
      m_invGraph->SetPoint(i,m_g_nvtx_nreco_bs66mm->GetY()[i],m_g_nvtx_nreco_bs66mm->GetX()[i]);
  }
  m_NPVmin = m_g_nvtx_nreco_bs66mm->GetY()[0];
  m_NPVmax = m_g_nvtx_nreco_bs66mm->GetY()[m_g_nvtx_nreco_bs66mm->GetN()-1];
}

double NPVBeamspotCorrection::GetNVertexBsCorrection(double nRecoVtx) 
{
  if (nRecoVtx<m_NPVmin) error(Form("Nonsense input to GetNVertexBsCorrection: NPV=%.2f",nRecoVtx));
  // if outside range, linear extrapolation from last valid value
  if (nRecoVtx>=m_NPVmax) return m_g_nvtx_nreco_bs47mm->Eval(m_invGraph->Eval(m_NPVmax))/m_NPVmax*nRecoVtx;

  return m_g_nvtx_nreco_bs47mm->Eval(m_invGraph->Eval(nRecoVtx));
}

TGraph* NPVBeamspotCorrection::NVtx_NReconstructible_bs47mm() {
   TGraph *graph = new TGraph(41);
   graph->SetName("g_shadowedAverage_bs47mm");
   graph->SetPoint(0, 0, 0);
   graph->SetPoint(1, 1, 1);
   graph->SetPoint(2, 2, 1.97111);
   graph->SetPoint(3, 3, 2.91499);
   graph->SetPoint(4, 4, 3.83313);
   graph->SetPoint(5, 5, 4.72692);
   graph->SetPoint(6, 6, 5.59763);
   graph->SetPoint(7, 7, 6.44645);
   graph->SetPoint(8, 8, 7.27445);
   graph->SetPoint(9, 9, 8.08265);
   graph->SetPoint(10, 10, 8.87199);
   graph->SetPoint(11, 11, 9.64332);
   graph->SetPoint(12, 12, 10.3975);
   graph->SetPoint(13, 13, 11.1352);
   graph->SetPoint(14, 14, 11.8572);
   graph->SetPoint(15, 15, 12.5641);
   graph->SetPoint(16, 16, 13.2567);
   graph->SetPoint(17, 17, 13.9353);
   graph->SetPoint(18, 18, 14.6007);
   graph->SetPoint(19, 19, 15.2532);
   graph->SetPoint(20, 20, 15.8935);
   graph->SetPoint(21, 21, 16.5219);
   graph->SetPoint(22, 22, 17.1389);
   graph->SetPoint(23, 23, 17.7449);
   graph->SetPoint(24, 24, 18.3404);
   graph->SetPoint(25, 25, 18.9255);
   graph->SetPoint(26, 26, 19.5008);
   graph->SetPoint(27, 27, 20.0665);
   graph->SetPoint(28, 28, 20.623);
   graph->SetPoint(29, 29, 21.1705);
   graph->SetPoint(30, 30, 21.7094);
   graph->SetPoint(31, 31, 22.2399);
   graph->SetPoint(32, 32, 22.7622);
   graph->SetPoint(33, 33, 23.2767);
   graph->SetPoint(34, 34, 23.7835);
   graph->SetPoint(35, 35, 24.2829);
   graph->SetPoint(36, 36, 24.7751);
   graph->SetPoint(37, 37, 25.2604);
   graph->SetPoint(38, 38, 25.7388);
   graph->SetPoint(39, 39, 26.2106);
   graph->SetPoint(40, 40, 26.6759);
   return graph;
}

TGraph* NPVBeamspotCorrection::NVtx_NReconstructible_bs66mm() {
   TGraph *graph = new TGraph(41);
   graph->SetName("g_shadowedAverage_bs66mm");
   graph->SetPoint(0, 0, 0);
   graph->SetPoint(1, 1, 1);
   graph->SetPoint(2, 2, 1.97943);
   graph->SetPoint(3, 3, 2.93912);
   graph->SetPoint(4, 4, 3.87986);
   graph->SetPoint(5, 5, 4.8024);
   graph->SetPoint(6, 6, 5.70743);
   graph->SetPoint(7, 7, 6.59561);
   graph->SetPoint(8, 8, 7.46756);
   graph->SetPoint(9, 9, 8.32386);
   graph->SetPoint(10, 10, 9.16509);
   graph->SetPoint(11, 11, 9.99175);
   graph->SetPoint(12, 12, 10.8044);
   graph->SetPoint(13, 13, 11.6034);
   graph->SetPoint(14, 14, 12.3893);
   graph->SetPoint(15, 15, 13.1624);
   graph->SetPoint(16, 16, 13.9233);
   graph->SetPoint(17, 17, 14.6723);
   graph->SetPoint(18, 18, 15.4097);
   graph->SetPoint(19, 19, 16.136);
   graph->SetPoint(20, 20, 16.8514);
   graph->SetPoint(21, 21, 17.5562);
   graph->SetPoint(22, 22, 18.2509);
   graph->SetPoint(23, 23, 18.9356);
   graph->SetPoint(24, 24, 19.6106);
   graph->SetPoint(25, 25, 20.2763);
   graph->SetPoint(26, 26, 20.9329);
   graph->SetPoint(27, 27, 21.5806);
   graph->SetPoint(28, 28, 22.2197);
   graph->SetPoint(29, 29, 22.8504);
   graph->SetPoint(30, 30, 23.4729);
   graph->SetPoint(31, 31, 24.0874);
   graph->SetPoint(32, 32, 24.6941);
   graph->SetPoint(33, 33, 25.2933);
   graph->SetPoint(34, 34, 25.8851);
   graph->SetPoint(35, 35, 26.4697);
   graph->SetPoint(36, 36, 27.0473);
   graph->SetPoint(37, 37, 27.6179);
   graph->SetPoint(38, 38, 28.1819);
   graph->SetPoint(39, 39, 28.7394);
   graph->SetPoint(40, 40, 29.2904);
   return graph;
}
