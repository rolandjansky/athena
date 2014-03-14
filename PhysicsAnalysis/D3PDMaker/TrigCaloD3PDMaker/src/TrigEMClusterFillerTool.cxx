/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  D3PD::TrigEMClusterFillerTool.cxx
*/
#include "TrigEMClusterFillerTool.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "AthenaKernel/errorcheck.h"

using namespace std;

namespace D3PD {

   TrigEMClusterFillerTool::TrigEMClusterFillerTool(const std::string& type, 
                                                    const std::string& name, 
                                                    const IInterface* parent)
      : BlockFillerTool<TrigEMCluster>(type, name, parent)
   {
      // Avoid coverity warnings.
      mWriteBasics = true;
      mWriteHypoVars = true;
      mWriteDetails = true;
      book().ignore();

      declareProperty("WriteBasics", mWriteBasics=false, 
                      "Write basic information");
      declareProperty("WriteHypoVars", mWriteHypoVars=false, 
                      "Write variables used in hypo");
      declareProperty("WriteDetails", mWriteDetails=false, 
                      "Write detailed variables");
   }

   StatusCode TrigEMClusterFillerTool::book() {

      if (mWriteBasics) {
         CHECK(addVariable("E", m_energy, "Energy (calibrated)"));
         CHECK(addVariable("Et", m_et, "Et (calibrated)"));
         CHECK(addVariable("eta", m_eta, "Eta (calibrated)"));
         CHECK(addVariable("phi", m_phi, "Phi (calibrated)"));
      }
      if (mWriteHypoVars) {
         CHECK(addVariable("E237", m_e237,
                           "Energy in a 3x7 cluster (no calibration) around hottest cell"));
         CHECK(addVariable("E277", m_e277,
                           "Energy in a 7x7 cluster (no calibration) around hottest cell"));
         CHECK(addVariable("fracs1", m_fracs1,
                           "Energy in a 7 strips (around hottest strip) minus energy "
                           "in 3 strips divided by energy in 3 strips"));
         CHECK(addVariable("weta2", m_weta2,
                           "cluster width (based on a 3x5 cluster - 2nd layer)"));
         CHECK(addVariable("Ehad1", m_ehad1,
                           "hadronic Energy (first hadronic layer)"));
         CHECK(addVariable("eta1", m_Eta1,
                           "Eta sampling 1 (strip layer)"));
         CHECK(addVariable("emaxs1", m_emaxs1,
                           "maximum energy in sampling 1 (strip layer)"));
         CHECK(addVariable("e2tsts1", m_e2tsts1,
                           "second maximum energy in sampling 1 (strip layer)"));
         CHECK(addVariable("E233", m_e233,
                           "Energy in a 3x3 cluster (no calibration) around hottest cell"));
         CHECK(addVariable("wstot", m_wstot,
                           "width in first layer"));
      }
      if  (mWriteDetails) {
         CHECK(addVariable("energyInSample", m_energyInSample,
                           "Energy (calibrated) per sampling layer"));
      }

      return StatusCode::SUCCESS;
   }

   StatusCode TrigEMClusterFillerTool::fill(const TrigEMCluster& x) {

      if (mWriteBasics) {
         *m_energy = x.energy();
         *m_et = x.et();
         *m_eta = x.eta();
         *m_phi = x.phi();
      }
      if (mWriteHypoVars) {
         *m_e237 = x.e237();
         *m_e277 = x.e277();
         *m_fracs1 = x.fracs1();
         *m_weta2 = x.weta2();
         *m_ehad1 = x.ehad1();
         *m_Eta1 = x.Eta1();
         *m_emaxs1 = x.emaxs1();
         *m_e2tsts1 = x.e2tsts1();
         *m_e233 = x.e233();
         *m_wstot = x.wstot();
      }
      if  (mWriteDetails) {
         m_energyInSample->clear();
         m_energyInSample->push_back(x.energyInSample(CaloSampling::PreSamplerB));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EMB1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EMB2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EMB3));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::PreSamplerE));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EME1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EME2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::EME3));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::HEC0));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::HEC1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::HEC2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::HEC3));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileBar0));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileBar1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileBar2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileGap1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileGap2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileGap3));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileExt0));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileExt1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::TileExt2));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::FCAL0));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::FCAL1));
         m_energyInSample->push_back(x.energyInSample(CaloSampling::FCAL2));
      }

//          PreSamplerB=0, EMB1, EMB2, EMB3,       // LAr barrel
//          PreSamplerE, EME1, EME2, EME3,       // LAr EM endcap 
//          HEC0, HEC1, HEC2, HEC3,            // Hadronic end cap cal.
//          TileBar0, TileBar1, TileBar2,      // Tile barrel
//          TileGap1, TileGap2, TileGap3,      // Tile gap (ITC & scint)
//          TileExt0, TileExt1, TileExt2,      // Tile extended barrel
//          FCAL0, FCAL1, FCAL2,               // Forward EM endcap
//          Unknown

      return StatusCode::SUCCESS;
   }

} // namespace D3PD
