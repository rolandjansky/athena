/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/TopoClusterSelectionTool.h"
#include <PathResolver/PathResolver.h>
#include <TFile.h>
#include <TH1F.h>

namespace HI
{
  TopoClusterSelectionTool::TopoClusterSelectionTool(const std::string& name) : 
    asg::AsgTool(name),
    m_isInitialized(false),
    m_calibFileName(""),
    m_calibHistoName("h1_TC_sig_cuts"),
    m_minPt(200.),
    m_minEnergy(-1),
    m_minEta(-1),
    m_maxEta(4.9),
    m_doAbsEta(true),
    m_doSigCuts(true),
    m_h1TCSigCut(nullptr),
    m_minPtCutIndex(999),
    m_minEnergyCutIndex(999),
    m_minEtaCutIndex(999),
    m_maxEtaCutIndex(999),
    m_minCellSignificanceCutIndex(999)
  {

#ifdef ASGTOOL_ATHENA
    declareInterface<ITopoClusterSelectionTool>(this);
#endif

    declareProperty("MinPt", m_minPt, "Minimum transverse momentum");
    declareProperty("MinEnergy", m_minEnergy, "Minimum energy");
    declareProperty("MaxEta", m_maxEta, "Maximum eta");
    declareProperty("MinEta", m_minEta, "Minimum eta");
    declareProperty("ApplySignificanceCuts", m_doSigCuts, "Significance requirements");
    declareProperty("DoAbsEta", m_doAbsEta, "If true, eta cuts are on absolute value");
    declareProperty("CalibFile", m_calibFileName, "Calibration file name");
    declareProperty("CalibHisto", m_calibHistoName, "Calibration histogram name");


  }

  StatusCode TopoClusterSelectionTool::initialize()
  {
    ATH_MSG_INFO("Initializing TopoClusterSelectionTool");
    m_isInitialized=true;
    if(m_doSigCuts)
    {
      std::unique_ptr<TFile> calibFilePtr(TFile::Open(PathResolverFindCalibFile(m_calibFileName).c_str()));
      if(!calibFilePtr || calibFilePtr->IsZombie())
      {
	ATH_MSG_ERROR("Could not find TC significance file: " << m_calibFileName);
	return StatusCode::FAILURE;
      }
      m_h1TCSigCut=static_cast<TH1F*>(calibFilePtr->Get(m_calibHistoName.c_str()));
      if(!m_h1TCSigCut)
      {
	ATH_MSG_ERROR("Error getting significance cut histogram: " << m_calibHistoName << " in file: " << m_calibFileName);
	return StatusCode::FAILURE;
      }
      m_h1TCSigCut->SetName(std::string(name()+"_"+m_calibHistoName).c_str());
      m_h1TCSigCut->SetDirectory(0);
      calibFilePtr->Close();
    }
    //
    m_minPtCutIndex=m_accept.addCut("MinPt","Minimum transverse momentum");
    m_minEnergyCutIndex=m_accept.addCut("MinEnergy","Minimum energy");
    m_minEtaCutIndex=m_accept.addCut("MinEta","Minimum eta");
    m_maxEtaCutIndex=m_accept.addCut("MaxEta","Maximum eta");
    m_minCellSignificanceCutIndex=m_accept.addCut("MinCellSignificance","significance of cells");
    //
    ATH_MSG_INFO("TopoClusterSelectionTool configured with the following cuts:");
    ATH_MSG_INFO("MinPt = " << m_minPt);
    ATH_MSG_INFO("MinEnergy = " << m_minEnergy);
    if(m_doAbsEta) 
    {
      ATH_MSG_INFO("Eta selections applied to |eta|");
      if(m_minEta < 0) ATH_MSG_INFO( "|eta| <= " << m_maxEta);
      else ATH_MSG_INFO(m_minEta << "< |eta| <= " << m_maxEta);
    }
    else
    {
      ATH_MSG_INFO("Eta selections applied to signed eta");
      ATH_MSG_INFO(m_minEta << "< eta <= " << m_maxEta);
    }
    if(m_doSigCuts)
    {
      ATH_MSG_INFO("Significance cuts taken from " << m_calibFileName);
      if(msgLvl(MSG::VERBOSE) && m_h1TCSigCut) m_h1TCSigCut->Print("ALL");
    }
    else ATH_MSG_INFO("No significance cuts applied");

    return StatusCode::SUCCESS;
  }

  float TopoClusterSelectionTool::lookupCellSignificanceCut(float eta, int clCellSigSamp) const
  {
    float clCellSigCut=5.;
    if(m_h1TCSigCut)
    {
      int etabin=m_h1TCSigCut->GetXaxis()->FindBin(eta);
      if(etabin> 0  && etabin<= m_h1TCSigCut->GetNbinsX()) clCellSigCut=m_h1TCSigCut->GetBinContent(etabin);
    }
    if(clCellSigSamp>=CaloSampling::TileBar0 && clCellSigSamp<=CaloSampling::TileExt2) clCellSigCut+=2.;
    return clCellSigCut;
  }

  const Root::TAccept& TopoClusterSelectionTool::getTAccept() const
  {
    return m_accept;
  }
  const Root::TAccept& TopoClusterSelectionTool::accept(const xAOD::IParticle* p) const
  {
    m_accept.clear();
    // Check if this is a cluster:
    if( p->type() != xAOD::Type::CaloCluster ) {
      ATH_MSG_ERROR( "accept(...) Function received a non-cluster" );
      return m_accept;
    }
    const xAOD::CaloCluster* cluster=static_cast<const xAOD::CaloCluster*>(p);
    return accept(cluster);
  }
  const Root::TAccept& TopoClusterSelectionTool::accept(const xAOD::CaloCluster& cl) const
  {
    m_accept.clear();
    m_accept.setCutResult(m_minPtCutIndex,cl.pt() > m_minPt);
    m_accept.setCutResult(m_minEnergyCutIndex,cl.e() > m_minEnergy);
    float etaSigned=cl.eta();
    float eta=etaSigned;
    if(m_doAbsEta) eta=std::abs(etaSigned);
    m_accept.setCutResult(m_minEtaCutIndex,eta > m_minEta);
    m_accept.setCutResult(m_maxEtaCutIndex,eta <= m_maxEta);

    if(m_doSigCuts)
    {
      double clCellSig=cl.getMomentValue(xAOD::CaloCluster::CELL_SIGNIFICANCE);
      int clCellSigSamp=static_cast<int>(cl.getMomentValue(xAOD::CaloCluster::CELL_SIG_SAMPLING));
      float sigCut=lookupCellSignificanceCut(eta,clCellSigSamp);
      m_accept.setCutResult(m_minCellSignificanceCutIndex,clCellSig > sigCut);
    }
    else m_accept.setCutResult(m_minCellSignificanceCutIndex,true);

    return m_accept;
  }


}
