/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** 
 * NAME : 	TrigT1CaloTauFex.cxx
 * PACKAGE : 	Trigger/L1CaloUpgrade/TrigT1CaloEFex
 *
 * AUTHOR : 	Denis Oliveira Damazio
 *
 *
 * **/

#include "TrigT1CaloBaseFex.h"
#include "TrigT1CaloTauFex.h"
#include "CaloEvent/CaloCellContainer.h"
#include <math.h>
#include <numeric>
#include <string>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

/* +++++++++++++++++++++++++++ */
// sorting utils

template <typename T, typename Compare>
std::vector<std::size_t> TrigT1CaloTauFex::sort_permutation(
    const std::vector<T>& vec,
    Compare compare)
{
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
        [&](std::size_t i, std::size_t j){ return compare(vec[i], vec[j]); });
    return p;
}

template <typename T>
std::vector<T> TrigT1CaloTauFex::apply_permutation(
    const std::vector<T>& vec,
    const std::vector<std::size_t>& p)
{
    std::vector<T> sorted_vec(p.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(),
        [&](std::size_t i){ return vec[i]; });
    return sorted_vec;
}

/* ++++++++++++++++++++++++++ */

TrigT1CaloTauFex::TrigT1CaloTauFex( const std::string& name, ISvcLocator* pSvcLocator ) : TrigT1CaloBaseFex (name, pSvcLocator) {
	declareProperty("EnableMonitoring", 		m_enableMon=false);
	declareProperty("DoTruth", 			m_doTruth=false);
	declareProperty("TruthMaxEta",                  m_truth_maxEta_thr=2.5);
	declareProperty("TruthMinPtGeV",                m_truth_minPt_thr=20.);
        declareProperty("CellsEtThreholdGeV",           m_cellsEtThrehold=3.);
	declareProperty("NoiseSignificanceCore", 	m_NoiseSignificanceCore=3);
	declareProperty("NoiseSignificanceIso",         m_NoiseSignificanceIso=0);
	declareProperty("OutputClusterName", 		m_outputClusterName  = "SClusterTau" );
}

TrigT1CaloTauFex::~TrigT1CaloTauFex(){
	// finish base class
}

StatusCode TrigT1CaloTauFex::initialize(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::INFO << "initialing TrigT1CaloTauFex" << endmsg;
	if ( TrigT1CaloBaseFex::initialize().isFailure() ) return StatusCode::FAILURE;
	if ( m_enableMon ){
		std::string histoName(name());
		histoName+="Algo.root";
		m_histFile = new TFile(histoName.c_str(),"RECREATE");
		m_EtSTau = new TH1F("EtSTau","Et of Super Cell based Taus",100,0,100);
		// TODO: add here initialisation of other histos ...
	}

	msg << MSG::INFO << " REGTEST: TrigT1CaloTauFex configurations: " <<endmsg;
	msg << MSG::INFO << " REGTEST: EnableMonitoring			" << m_enableMon  <<endmsg;
        msg << MSG::INFO << " REGTEST: CellsEtThreholdGeV        	" << m_cellsEtThrehold  << " GeV" << endmsg;
	msg << MSG::INFO << " REGTEST: DoTruth	        		" << m_doTruth  <<endmsg;
	msg << MSG::INFO << " REGTEST: TruthMaxEta          		" << m_truth_maxEta_thr  <<endmsg;
        msg << MSG::INFO << " REGTEST: TruthMinPtGeV                    " << m_truth_minPt_thr << " GeV"  <<endmsg;
	msg << MSG::INFO << " REGTEST: NoiseSignificanceCore  " << m_NoiseSignificanceCore <<endmsg;
	msg << MSG::INFO << " REGTEST: NoiseSignificanceIso   " << m_NoiseSignificanceIso <<endmsg;
	msg << MSG::INFO << " REGTEST: OutputClusterName      " << m_outputClusterName <<endmsg;

	if(m_doTruth){
	        m_true_nprong.reserve(200);
        	m_true_pt.reserve(200);
        	m_true_neutrals.reserve(200);
        	m_true_vistau.reserve(200);
	}

	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloTauFex::finalize(){
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "finalizing TrigT1CaloTauFex" << endmsg;
	if ( TrigT1CaloBaseFex::finalize().isFailure() ) return StatusCode::FAILURE;
	if ( m_enableMon ) {
		m_histFile->Write();
		m_histFile->Close();
	}

	if(m_doTruth){
	        m_true_nprong.clear();
        	m_true_pt.clear();
        	m_true_neutrals.clear();
        	m_true_vistau.clear();
	}

	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloTauFex::execute(){
	
        MsgStream msg(msgSvc(), name());
	msg << MSG::DEBUG << "execute TrigT1CaloTauFex" << endmsg;

        std::vector<const CaloCell*> scells;
	const xAOD::TriggerTowerContainer* TTs(0);
	const xAOD::TruthParticleContainer *truthContainer(0);

	if ( getContainers(scells, TTs, m_cellsEtThrehold).isFailure() || (TTs==0) ) {
		msg << MSG::WARNING << " Could not get scell or TT containers" << endmsg;
		return StatusCode::SUCCESS;
	}

	if(m_doTruth){ 
		if ( getContainers(truthContainer).isFailure() || (truthContainer==0) ) {
                	msg << MSG::WARNING << " Could not get truth containers" << endmsg;
                	return StatusCode::SUCCESS;
		}	
        	Truth(truthContainer);
  		if(m_true_vistau.size()!=2){
        		msg << MSG::WARNING << "Found " << m_true_vistau.size() << " instead of 2, skipping event" << endmsg;
        		return StatusCode::SUCCESS;
  		}
        }

	// TODO: debugged up to here

	// Simply check cells which are above a given threshold (3GeV)
	///findCellsAbove(scells,3e3,m_cellsAboveThr);
	
	// TODO: scells is the container of scells above CellsEtThreholdGeV and passing m_provenance quality selection. Other quality cuts to be applied?
	// TODO: use m_NoiseSignificanceCore and m_NoiseSignificanceIso to make this selection configurable  
	// TODO: add here and also in the folloing histograms of the #scell multiplicities for monitoring/debugging
	NoiseThreshold(scells,2,m_cellsAboveThr); // loop E/sigm>2

	// Loop over seed cells, this should give us
	for( auto cellAbove : m_cellsAboveThr ) {
		// builds a vector with all the cells around the seed cell
		// with the size (deta,dphi)=(0.08,0.21)
		///findCellsAround(scells, cellAbove, m_cellsAround,0.08,0.21); // large window
		///float etaCluster, phiCluster; // Cluster baricenter
		// Find cluster center (eta/phiCluster) based on the
		// energy weighted scell position
		///findCluster(m_cellsAround, etaCluster, phiCluster);
		// if find the cluster position fails, etaCluster=999.0
		///if ( etaCluster > 998.0 ) continue;
		// include TT (for Tile region only)
		///findTTsAround(TTs, etaCluster, phiCluster, m_TTsAround);
	        IsolationCore(scells, cellAbove, m_CoreCells,m_IsoCells,m_RoI);
		sumEMIsoCells(m_IsoCells, m_IsoEnergy);
		sumEMCoreCells(m_CoreCells, m_RoICells, m_CoreEnergy);
		// check if seed cell is a local maximum (maybe could
		// do this check before)
		///if ( ! isCellEmMaximum ( m_cellsAround, cellAbove ) ) continue;
		///float et = sumEmCells( m_cellsAround )/TMath::CosH(cellAbove->eta());
		///msg << MSG::INFO << "Tau found at (eta,phi)=(" << etaCluster << "," << phiCluster << ") with ET of : " << et << endmsg;
	}
	for (unsigned int i=0; i<m_RoI.size(); i++) {
	//for( auto RoI : m_RoI ) {
	  msg << MSG::INFO << "RoI in the loop=" << m_RoI.at(0).at(0) << endmsg;
	}
	return StatusCode::SUCCESS;
}

void TrigT1CaloTauFex::ExamineTruthTau(const xAOD::TruthParticle& xTruthParticle) const {

    xTruthParticle.auxdecor<double>("pt_vis") = 0;
    xTruthParticle.auxdecor<bool>("IsLeptonicTau") = false;

    TLorentzVector VisSumTLV;
    xTruthParticle.auxdecor<double>("pt_vis") = 0;
    xTruthParticle.auxdecor<double>("eta_vis") = 0;
    xTruthParticle.auxdecor<double>("phi_vis") = 0;
    xTruthParticle.auxdecor<double>("m_vis") = 0;
    xTruthParticle.auxdecor<int>("childChargeSum") = 0;
    xTruthParticle.auxdecor<int>("nTracks") = 0;
    xTruthParticle.auxdecor<bool>("neutrals") = false;

    if(!xTruthParticle.hasDecayVtx()) return;

    const xAOD::TruthVertex* decayvtx = xTruthParticle.decayVtx();
    if(decayvtx)
    {
      const std::size_t nChildren = decayvtx->nOutgoingParticles();
      for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
        {
          const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
          if((abs(child->pdgId()) == 12 || abs(child->pdgId()) == 14 || abs(child->pdgId()) == 16)) continue;
          if(child->status()==3) continue;
            
            if ((abs(child->pdgId()) == 11 || abs(child->pdgId()) == 13 || abs(child->pdgId()) == 15)) xTruthParticle.auxdecor<bool>("IsLeptonicTau") = true;
            VisSumTLV += child->p4();
            xTruthParticle.auxdecor<int>("childChargeSum") += child->charge();
            xTruthParticle.auxdecor<int>("nTracks") += abs(child->charge());
            if(child->charge()==0) xTruthParticle.auxdecor<bool>("neutrals") = true;
        }
    }

    xTruthParticle.auxdecor<double>("pt_vis")  = VisSumTLV.Pt();
    xTruthParticle.auxdecor<double>("eta_vis") = VisSumTLV.Eta();
    xTruthParticle.auxdecor<double>("phi_vis") = VisSumTLV.Phi();
    xTruthParticle.auxdecor<double>("m_vis")   = VisSumTLV.M();
    if(xTruthParticle.auxdecor<int>("childChargeSum")!=xTruthParticle.charge() || xTruthParticle.auxdecor<int>("nTracks")%2==0)
      {
        Info("event loop", "strange tau: charge %i  and %i tracks",xTruthParticle.auxdecor<int>("childChargeSum"),xTruthParticle.auxdecor<int>("nTracks") );
        const std::size_t nChildren = decayvtx->nOutgoingParticles();
        for ( std::size_t iChild = 0; iChild != nChildren; ++iChild )
          {
            const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
            Info("event loop", "  child pdgId %i, status %i, charge %f",child->pdgId(),child->status(),child->charge());
          }
      }
}

void TrigT1CaloTauFex::Truth(const xAOD::TruthParticleContainer *truthContainer) {

  MsgStream msg(msgSvc(), name());
 
  m_true_vistau.clear();
  m_true_nprong.clear();
  m_true_pt.clear();
  m_true_neutrals.clear();
 
  xAOD::TruthParticleContainer::const_iterator tru_itr2 = truthContainer->begin();
  xAOD::TruthParticleContainer::const_iterator tru_end2 = truthContainer->end();
  for( ; tru_itr2 != tru_end2; ++tru_itr2 ) {
    if(abs((*tru_itr2)->pdgId()) == 15  && (*tru_itr2)->status() == 2){
 
      ExamineTruthTau(**tru_itr2);     
      double pt  = (*tru_itr2)->auxdata<double>("pt_vis");
      double eta = (*tru_itr2)->auxdata<double>("eta_vis");
      double phi = (*tru_itr2)->auxdata<double>("phi_vis");
      double m   = (*tru_itr2)->auxdata<double>("m_vis");
      bool lep = (*tru_itr2)->auxdata<bool>("IsLeptonicTau");
      int ntracks = (*tru_itr2)->auxdata<int>("nTracks");
      bool neutral = (*tru_itr2)->auxdata<bool>("neutrals");
     
      if(pt < m_truth_minPt_thr*1e3 || lep || fabs(eta) > m_truth_maxEta_thr ) continue;
                                
      TLorentzVector TruthTauTLV;
      TruthTauTLV.SetPtEtaPhiM(pt,eta,phi,m);
      m_true_vistau.push_back(TruthTauTLV);
      m_true_nprong.push_back(ntracks);
      m_true_pt.push_back(TruthTauTLV.Pt());
      m_true_neutrals.push_back(neutral);
    }
  }

  //sort(m_true_pt.begin(),m_true_pt.end()); reverse(m_true_pt.begin(),m_true_pt.end());
    
  // TODO: test pt sorting 
  //for (auto a : m_true_vistau) std::cout << a.Pt() << " "; std::cout << '\n';
  //for (auto a : m_true_nprong) std::cout << a << " "; std::cout << '\n';
  //for (auto a : m_true_pt) std::cout << a << " "; std::cout << '\n';
  //for (auto a : m_true_neutrals) std::cout << a << " "; std::cout << '\n';

  auto p = sort_permutation(m_true_pt,
    [](float const& a, float const& b){ return b < a; });
  m_true_vistau		= apply_permutation(m_true_vistau, p);
  m_true_nprong 	= apply_permutation(m_true_nprong, p);
  m_true_pt 		= apply_permutation(m_true_pt, p);
  m_true_neutrals 	= apply_permutation(m_true_neutrals, p);
        
  //for (auto a : m_true_vistau) std::cout << a.Pt() << " "; std::cout << '\n';
  //for (auto a : m_true_nprong) std::cout << a << " "; std::cout << '\n';
  //for (auto a : m_true_pt) std::cout << a << " "; std::cout << '\n';
  //for (auto a : m_true_neutrals) std::cout << a << " "; std::cout << '\n';

  return;
}


void TrigT1CaloTauFex::IsolationCorett(const std::vector<const CaloCell*>& scells,
                                       const CaloCell* cell,
                                       std::vector<const CaloCell*>& out1,
                                       std::vector<const CaloCell*>& out2) {
        out1.clear();
        out2.clear();
        if ( !cell ) return;
        float etacell = cell->eta();
        float phicell = cell->phi();
        bool maps = false;
        bool pres = true;
        // use maps (only barrel)
        if (maps) {
          int m_phiBin(64), m_etaBin(54);
          std::vector<int> m_map;
          for(int i=0;i<(m_etaBin*m_phiBin);i++) m_map.push_back(-1);
          float m_phiBin_size(2.*TMath::Pi()/m_phiBin), m_etaBin_size(0.1);
          etacell +=2.5;
          phicell +=TMath::Pi();
          int m_eta_bin = (int) (etacell/m_etaBin_size);
          int m_phi_bin = (int) (phicell/m_phiBin_size);
          if(m_eta_bin*m_phiBin+m_phi_bin>(m_etaBin*m_phiBin)) {std::cout << "bin outside map!! "<< m_eta_bin << "," << m_phi_bin<<std::endl;return;}
          if(m_map.at(m_eta_bin*m_phiBin+m_phi_bin)==-1) {m_map.at(m_eta_bin*m_phiBin+m_phi_bin) = 2;}
          else return;
        }
        // use coordinates of pre-sampler
        if (pres) {
          if (cell->caloDDE()->getSampling()!=1 && cell->caloDDE()->getSampling()!=5) return; // assuming endcap geometry like barrel
        }
        float deltaetaOuter1 = 0.05;
        float deltaetaOuter2 = 0.15;
        float deltaphiOuter = 0.15;
        float deltaetaInner = 0.05;
        float deltaphiInner = 0.05;
        for(auto scell : scells) {
                if ( fabsf( scell->eta() - etacell) > deltaetaOuter2 ) continue;
                if (scell->caloDDE()->getSampling()!=1 && scell->caloDDE()->getSampling()!=5 && cell->caloDDE()->getSampling()!=4 && cell->caloDDE()->getSampling()!=8) deltaetaOuter1 = 1.0; // tighter isolation in 1° and 2° layers
                float dphi = fabsf( scell->phi() - phicell);
                dphi = fabsf( M_PI - dphi );
                dphi = fabsf( M_PI - dphi );
                if ( fabsf( dphi ) > deltaphiOuter ) continue;
                if ( fabsf( scell->eta() - etacell) < deltaetaInner && fabsf( dphi ) < deltaphiInner ) out1.push_back(scell); // core
                else if ( fabsf( scell->eta() - etacell) > deltaetaOuter1 ) out2.push_back(scell); // isolation
                else if ( scell->caloDDE()->getSampling()!=2 && scell->caloDDE()->getSampling()!=3 && cell->caloDDE()->getSampling()!=6 && cell->caloDDE()->getSampling()!=7 && fabsf( scell->eta() - etacell) < deltaetaInner && fabsf( dphi ) > deltaphiInner  ) out2.push_back(scell); // isolation
        }
        return;
}


void TrigT1CaloTauFex::IsolationCore(const std::vector<const CaloCell*>& scells,
                                     const CaloCell* cell,
                                     std::vector<const CaloCell*>& out1,
                                     std::vector<const CaloCell*>& out2,
                                     std::vector<std::vector<float>>& out) {
        out1.clear();
        out2.clear();
        out.clear();
        if ( !cell ) return;
        float etacell = cell->eta();
        float phicell = cell->phi();
        float totalEMCoreSum = 0.0;
        float totalEMIsoSum = 0.0;
        if (cell->caloDDE()->getSampling()!=3 && cell->caloDDE()->getSampling()!=7) return; // take the second layer (most energetic)
        float deltaetaOuter1 = 0.085;
        float deltaetaOuter2 = 0.13;
        float deltaphiOuter = 0.15;
        float deltaetaInner = 0.03;
        float deltaphiInner = 0.05;
        float AvEta = 0;
        float AvPhi = 0;
        std::vector<float> RoIVar;
        for(auto scell : scells) {
          float eta = scell->eta();
          float phi = scell->phi();
          if ( fabsf( scell->eta() - etacell) > deltaetaOuter2 ) continue;
          if (scell->caloDDE()->getSampling()!=1 && scell->caloDDE()->getSampling()!=5 && cell->caloDDE()->getSampling()!=4 && cell->caloDDE()->getSampling()!=8) deltaetaInner = 0.5; // looser isolation in layers 0 and 3
          float dphi = fabsf( scell->phi() - phicell);
          dphi = fabsf( M_PI - dphi );
          dphi = fabsf( M_PI - dphi );
          if ( fabsf( dphi ) > deltaphiOuter ) continue;
          if ( fabsf( scell->eta() - etacell) < deltaetaInner && fabsf( dphi ) < deltaphiInner ) {out1.push_back(scell); totalEMCoreSum += scell->energy(); AvEta += eta*(scell->energy()); AvPhi += phi*(scell->energy());} // core
          else if ( fabsf( scell->eta() - etacell) > deltaetaOuter1 ) {out2.push_back(scell); totalEMIsoSum += scell->energy();} // isolation
          else if ( scell->caloDDE()->getSampling()!=2 && scell->caloDDE()->getSampling()!=3 && cell->caloDDE()->getSampling()!=6 && cell->caloDDE()->getSampling()!=7 && fabsf( scell->eta() - etacell) < deltaetaInner && fabsf( dphi ) > deltaphiInner  ) {out2.push_back(scell); totalEMIsoSum = scell->energy();} // isolation
        }
        AvEta = AvEta/totalEMCoreSum;
        AvPhi = AvPhi/totalEMCoreSum;
        if (totalEMCoreSum > 10) {
          RoIVar.push_back(AvEta);
          RoIVar.push_back(AvPhi);
          RoIVar.push_back(totalEMCoreSum);
          out.push_back(RoIVar);
        }
        return;
}


void TrigT1CaloTauFex::sumEMCoreCells(const std::vector<const CaloCell*>& scells, std::vector<const CaloCell*>& out1, std::vector<float>& out2) {
  out1.clear();
  out2.clear();
  float totalEMCoreSum = 0.0;
  for(auto scell : scells) {totalEMCoreSum+= scell->energy(); if (scell->caloDDE()->getSampling()!=2 && scell->caloDDE()->getSampling()!=3 && scell->caloDDE()->getSampling()!=6 && scell->caloDDE()->getSampling()!=7 && scell->caloDDE()->getSampling()!=4 && scell->caloDDE()->getSampling()!=8) out1.push_back(scell);}
      out2.push_back(totalEMCoreSum);
      return;
}

void TrigT1CaloTauFex::sumEMIsoCells(const std::vector<const CaloCell*>& scells, std::vector<float>& out) {
  out.clear();
  float totalEMIsoSum = 0.0;
  for(auto scell : scells) totalEMIsoSum+= scell->energy();
  out.push_back(totalEMIsoSum);
  return;
}
