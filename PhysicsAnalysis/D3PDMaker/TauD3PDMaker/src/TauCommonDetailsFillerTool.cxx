/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauCommonDetailsFillerTool.cxx
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date Sept, 2009
 * @brief Block filler tool for tau common details.
 */


#include "TauCommonDetailsFillerTool.h"
#include "tauEvent/TauJet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
    
    
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
    TauCommonDetailsFillerTool::TauCommonDetailsFillerTool (const std::string& type,
							    const std::string& name,
							    const IInterface* parent)
	: BlockFillerTool<Analysis::TauJet> (type, name, parent)
    {
      book().ignore(); // Avoid coverity warnings.
    }
    
    
/**
 * @brief Book variables for this block.
 */
    StatusCode TauCommonDetailsFillerTool::book()
    {
	CHECK( addVariable ("etOverPtLeadTrk",   m_etOverPtLeadTrk)  );
	CHECK( addVariable ("ipZ0SinThetaSigLeadTrk",  m_ipZ0SinThetaSigLeadTrk )  );
	CHECK( addVariable ("leadTrkPt", m_leadTrkPt  )  );
	CHECK( addVariable ("nLooseTrk", m_nLooseTrk  )  );
	CHECK( addVariable ("nLooseConvTrk", m_nLooseConvTrk  )  );
	CHECK( addVariable ("nProngLoose", m_nProngLoose  )  );
	CHECK( addVariable ("ipSigLeadTrk", m_ipSigLeadTrk  )  );
	CHECK( addVariable ("ipSigLeadLooseTrk", m_ipSigLeadLooseTrk  )  );
	CHECK( addVariable ("etOverPtLeadLooseTrk", m_etOverPtLeadLooseTrk  )  );
	CHECK( addVariable ("leadLooseTrkPt", m_leadLooseTrkPt  )  );
	CHECK( addVariable ("chrgLooseTrk",  m_chrgLooseTrk )  );
	CHECK( addVariable ("massTrkSys", m_massTrkSys  )  );
	CHECK( addVariable ("trkWidth2", m_trkWidth2  )  );
	CHECK( addVariable ("trFlightPathSig",  m_trFlightPathSig )  );
	CHECK( addVariable ("etEflow",  m_etEflow )  );
	CHECK( addVariable ("mEflow",  m_mEflow )  );
	CHECK( addVariable ("nPi0",  m_nPi0 )  );
	CHECK( addVariable ("ele_E237E277", m_ele_E237E277  )  );
	CHECK( addVariable ("ele_PresamplerFraction",  m_ele_PresamplerFraction )  );
	CHECK( addVariable ("ele_ECALFirstFraction", m_ele_ECALFirstFraction  )  );
	CHECK( addVariable ("EM_TES_scale", m_EM_TES_scale) );
	CHECK( addVariable ("LC_TES_precalib", m_LC_TES_precalib) );
	CHECK( addVariable ("TRTHTOverLT_LeadTrk", m_TRTRatio) );

	CHECK( addVariable ("seedCalo_EMRadius", m_seedCalo_EMRadius  )  );
	CHECK( addVariable ("seedCalo_hadRadius",  m_seedCalo_hadRadius )  );
	CHECK( addVariable ("seedCalo_etEMAtEMScale",  m_seedCalo_etEMAtEMScale )  );
	CHECK( addVariable ("seedCalo_etHadAtEMScale", m_seedCalo_etHadAtEMScale  )  );
	CHECK( addVariable ("seedCalo_isolFrac", m_seedCalo_isolFrac  )  );
	CHECK( addVariable ("seedCalo_centFrac", m_seedCalo_centFrac  )  );
	CHECK( addVariable ("seedCalo_stripWidth2", m_seedCalo_stripWidth2  )  );
	CHECK( addVariable ("seedCalo_nStrip", m_seedCalo_nStrip  )  );
	CHECK( addVariable ("seedCalo_eta", m_seedCalo_eta  )  );
	CHECK( addVariable ("seedCalo_phi", m_seedCalo_phi  )  );
	CHECK( addVariable ("seedCalo_nIsolLooseTrk", m_seedCalo_nIsolLooseTrk  )  );
	CHECK( addVariable ("seedCalo_trkAvgDist", m_seedCalo_trkAvgDist  )  );
	CHECK( addVariable ("seedCalo_trkRmsDist", m_seedCalo_trkRmsDist  )  );
	CHECK( addVariable ("seedCalo_caloIso", m_seedCalo_caloIso  )  );
	CHECK( addVariable ("seedCalo_caloIsoCorrected", m_seedCalo_caloIsoCorrected  )  );
	CHECK( addVariable ("seedCalo_dRmax", m_seedCalo_dRmax  )  );
	CHECK( addVariable ("seedCalo_lead2ClusterEOverAllClusterE", m_seedCalo_lead2ClusterEOverAllClusterE  )  );
	CHECK( addVariable ("seedCalo_lead3ClusterEOverAllClusterE", m_seedCalo_lead3ClusterEOverAllClusterE  )  );
	CHECK( addVariable ("seedCalo_etHadCalib", m_seedCalo_etHadCalib )  );
	CHECK( addVariable ("seedCalo_etEMCalib", m_seedCalo_etEMCalib )  );
	CHECK( addVariable ("numTopoClusters", m_numTopoClusters  )  );
	CHECK( addVariable ("numEffTopoClusters", m_numEffTopoClusters  )  );
	CHECK( addVariable ("topoInvMass", m_topoInvMass  )  );
	CHECK( addVariable ("effTopoInvMass", m_effTopoInvMass  )  );
	CHECK( addVariable ("topoMeanDeltaR", m_topoMeanDeltaR  )  );
	CHECK( addVariable ("effTopoMeanDeltaR", m_effTopoMeanDeltaR  )  );
	CHECK( addVariable ("numCells", m_numCells  )  );
	CHECK( addVariable ("seedTrk_EMRadius", m_seedTrk_EMRadius  )  );
	CHECK( addVariable ("seedTrk_isolFrac", m_seedTrk_isolFrac  )  );
	CHECK( addVariable ("seedTrk_etChrgHadOverSumTrkPt", m_seedTrk_etChrgHadOverSumTrkPt  )  );
	CHECK( addVariable ("seedTrk_isolFracWide", m_seedTrk_isolFracWide  )  );
	CHECK( addVariable ("seedTrk_etHadAtEMScale", m_seedTrk_etHadAtEMScale  )  );
	CHECK( addVariable ("seedTrk_etEMAtEMScale", m_seedTrk_etEMAtEMScale  )  );
	CHECK( addVariable ("seedTrk_etEMCL", m_seedTrk_etEMCL  )  );
	CHECK( addVariable ("seedTrk_etChrgEM", m_seedTrk_etChrgEM  )  );
	CHECK( addVariable ("seedTrk_etNeuEM", m_seedTrk_etNeuEM  )  );
	CHECK( addVariable ("seedTrk_etResNeuEM", m_seedTrk_etResNeuEM  )  );
	CHECK( addVariable ("seedTrk_hadLeakEt", m_seedTrk_hadLeakEt  )  );
	CHECK( addVariable ("seedTrk_sumEMCellEtOverLeadTrkPt", m_seedTrk_sumEMCellEtOverLeadTrkPt  )  );
	CHECK( addVariable ("seedTrk_secMaxStripEt", m_seedTrk_secMaxStripEt  )  );
	CHECK( addVariable ("seedTrk_stripWidth2",  m_seedTrk_stripWidth2 )  );
	CHECK( addVariable ("seedTrk_nStrip",  m_seedTrk_nStrip )  );
	CHECK( addVariable ("seedTrk_etChrgHad",  m_seedTrk_etChrgHad )  );
	CHECK( addVariable ("seedTrk_nOtherCoreTrk", m_seedTrk_nOtherCoreTrk  )  );
	CHECK( addVariable ("seedTrk_nIsolTrk", m_seedTrk_nIsolTrk )  );
	CHECK( addVariable ("seedTrk_etIsolEM", m_seedTrk_etIsolEM  )  );
	CHECK( addVariable ("seedTrk_etIsolHad", m_seedTrk_etIsolHad  )  );

	CHECK( addVariable ("cellBasedEnergyRing1", m_cellBasedEnergyRing1) );
	CHECK( addVariable ("cellBasedEnergyRing2", m_cellBasedEnergyRing2) );
	CHECK( addVariable ("cellBasedEnergyRing3", m_cellBasedEnergyRing3) );
	CHECK( addVariable ("cellBasedEnergyRing4", m_cellBasedEnergyRing4) );
	CHECK( addVariable ("cellBasedEnergyRing5", m_cellBasedEnergyRing5) );
	CHECK( addVariable ("cellBasedEnergyRing6", m_cellBasedEnergyRing6) );
	CHECK( addVariable ("cellBasedEnergyRing7", m_cellBasedEnergyRing7) );
	
	
	return StatusCode::SUCCESS;
    }
    
    
/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
    StatusCode TauCommonDetailsFillerTool::fill (const Analysis::TauJet& p)
    {
	const Analysis::TauCommonDetails*  p_taudetails(0);
	p_taudetails = p.details<const Analysis::TauCommonDetails>();
	if (p_taudetails==0)
	{
	    REPORT_MESSAGE (MSG::ERROR) << "TauJet has no associated CommonDetails!!";
	    return StatusCode::SUCCESS;
	}
	/// add 
	*m_etOverPtLeadTrk = p_taudetails->etOverPtLeadTrk();
	*m_ipZ0SinThetaSigLeadTrk = p_taudetails->ipZ0SinThetaSigLeadTrk(); 
	*m_leadTrkPt = p_taudetails->leadTrkPt(); 
	*m_nLooseTrk = p_taudetails->nLooseTrk(); 
	*m_nLooseConvTrk = p_taudetails->nLooseConvTrk(); 
	*m_nProngLoose = p_taudetails->nProngLoose(); 
	*m_ipSigLeadTrk = p_taudetails->ipSigLeadTrk(); 
	*m_ipSigLeadLooseTrk = p_taudetails->ipSigLeadLooseTrk(); 
	*m_etOverPtLeadLooseTrk = p_taudetails->etOverPtLeadLooseTrk(); 
	*m_leadLooseTrkPt = p_taudetails->leadLooseTrkPt(); 
	*m_chrgLooseTrk = p_taudetails->chrgLooseTrk(); 
	*m_massTrkSys = p_taudetails->massTrkSys(); 
	*m_trkWidth2 = p_taudetails->trkWidth2(); 
	*m_trFlightPathSig = p_taudetails->trFlightPathSig(); 
	*m_etEflow = p_taudetails->etEflow(); 
	*m_mEflow = p_taudetails->mEflow(); 
	*m_nPi0 = p_taudetails->nPi0(); 
	*m_ele_E237E277 = p_taudetails->ele_E237E277(); 
	*m_ele_PresamplerFraction = p_taudetails->ele_PresamplerFraction(); 
	*m_ele_ECALFirstFraction = p_taudetails->ele_ECALFirstFraction(); 
	*m_EM_TES_scale = p_taudetails->EM_TES_scale();
	*m_LC_TES_precalib = p_taudetails->LC_TES_precalib();
	*m_TRTRatio = p_taudetails->TRT_NHT_OVER_NLT();
	*m_seedCalo_EMRadius = p_taudetails->seedCalo_EMRadius(); 
	*m_seedCalo_hadRadius = p_taudetails->seedCalo_hadRadius(); 
	*m_seedCalo_etEMAtEMScale = p_taudetails->seedCalo_etEMAtEMScale(); 
	*m_seedCalo_etHadAtEMScale = p_taudetails->seedCalo_etHadAtEMScale(); 
	*m_seedCalo_isolFrac = p_taudetails->seedCalo_isolFrac(); 
	*m_seedCalo_centFrac = p_taudetails->seedCalo_centFrac(); 
	*m_seedCalo_stripWidth2 = p_taudetails->seedCalo_stripWidth2(); 
	*m_seedCalo_nStrip = p_taudetails->seedCalo_nStrip(); 
	*m_seedCalo_eta = p_taudetails->seedCalo_eta(); 
	*m_seedCalo_phi = p_taudetails->seedCalo_phi(); 
	*m_seedCalo_nIsolLooseTrk = p_taudetails->seedCalo_nIsolLooseTrk(); 
	*m_seedCalo_trkAvgDist = p_taudetails->seedCalo_trkAvgDist(); 
	*m_seedCalo_trkRmsDist = p_taudetails->seedCalo_trkRmsDist();
	*m_seedCalo_caloIso = p_taudetails->seedCalo_caloIso();
	*m_seedCalo_caloIsoCorrected = p_taudetails->seedCalo_caloIsoCorrected();
	*m_seedCalo_dRmax = p_taudetails->seedCalo_dRmax();
	*m_seedCalo_lead2ClusterEOverAllClusterE = p_taudetails->seedCalo_lead2ClusterEOverAllClusterE();
	*m_seedCalo_lead3ClusterEOverAllClusterE = p_taudetails->seedCalo_lead3ClusterEOverAllClusterE();
	*m_seedCalo_etHadCalib = p_taudetails->seedCalo_etHadCalib();
	*m_seedCalo_etEMCalib = p_taudetails->seedCalo_etEMCalib();
	*m_numTopoClusters = p_taudetails->numTopoClusters();
	*m_numEffTopoClusters = p_taudetails->numEffTopoClusters();
	*m_topoInvMass = p_taudetails->topoInvMass();
	*m_effTopoInvMass = p_taudetails->effTopoInvMass();
	*m_topoMeanDeltaR = p_taudetails->topoMeanDeltaR();
	*m_effTopoMeanDeltaR = p_taudetails->effTopoMeanDeltaR();
	*m_numCells = p_taudetails->numCells();
	*m_seedTrk_EMRadius = p_taudetails->seedTrk_EMRadius(); 
	*m_seedTrk_isolFrac = p_taudetails->seedTrk_isolFrac(); 
	*m_seedTrk_etChrgHadOverSumTrkPt = p_taudetails->seedTrk_etChrgHadOverSumTrkPt(); 
	*m_seedTrk_isolFracWide = p_taudetails->seedTrk_isolFracWide(); 
	*m_seedTrk_etHadAtEMScale = p_taudetails->seedTrk_etHadAtEMScale(); 
	*m_seedTrk_etEMAtEMScale = p_taudetails->seedTrk_etEMAtEMScale(); 
	*m_seedTrk_etEMCL = p_taudetails->seedTrk_etEMCL(); 
	*m_seedTrk_etChrgEM = p_taudetails->seedTrk_etChrgEM(); 
	*m_seedTrk_etNeuEM = p_taudetails->seedTrk_etNeuEM(); 
	*m_seedTrk_etResNeuEM = p_taudetails->seedTrk_etResNeuEM(); 
	*m_seedTrk_hadLeakEt = p_taudetails->seedTrk_hadLeakEt(); 
	*m_seedTrk_sumEMCellEtOverLeadTrkPt = p_taudetails->seedTrk_sumEMCellEtOverLeadTrkPt(); 
	*m_seedTrk_secMaxStripEt = p_taudetails->seedTrk_secMaxStripEt(); 
	*m_seedTrk_stripWidth2 = p_taudetails->seedTrk_stripWidth2(); 
	*m_seedTrk_nStrip = p_taudetails->seedTrk_nStrip(); 
	*m_seedTrk_etChrgHad = p_taudetails->seedTrk_etChrgHad(); 
	*m_seedTrk_nOtherCoreTrk = p_taudetails->seedTrk_nOtherCoreTrk(); 
	*m_seedTrk_nIsolTrk = p_taudetails->seedTrk_nIsolTrk(); 
	*m_seedTrk_etIsolEM = p_taudetails->seedTrk_etIsolEM(); 
	*m_seedTrk_etIsolHad = p_taudetails->seedTrk_etIsolHad(); 

	*m_cellBasedEnergyRing1 = p_taudetails->cellBasedEnergyRing1();
	*m_cellBasedEnergyRing2 = p_taudetails->cellBasedEnergyRing2();
	*m_cellBasedEnergyRing3 = p_taudetails->cellBasedEnergyRing3();
	*m_cellBasedEnergyRing4 = p_taudetails->cellBasedEnergyRing4();
	*m_cellBasedEnergyRing5 = p_taudetails->cellBasedEnergyRing5();
	*m_cellBasedEnergyRing6 = p_taudetails->cellBasedEnergyRing6();
	*m_cellBasedEnergyRing7 = p_taudetails->cellBasedEnergyRing7();
	
	return StatusCode::SUCCESS;
    }
    
    
} // namespace D3PD
