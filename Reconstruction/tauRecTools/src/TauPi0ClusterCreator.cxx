/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS
//-----------------------------------------------------------------------------
// file:        TauPi0ClusterCreator.cxx
// package:     Reconstruction/tauEvent
// authors:     Benedict Winter, Will Davey, Stephanie Yuen
// date:        2012-10-09
//-----------------------------------------------------------------------------

#include "CaloUtils/CaloClusterStoreHelper.h"
#include "FourMomUtils/P4Helpers.h"
#include "xAODJet/Jet.h"

#include "TauPi0ClusterCreator.h"
#include "tauRecTools/HelperFunctions.h"

//-------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------

TauPi0ClusterCreator::TauPi0ClusterCreator(const std::string& name) :
    TauRecToolBase(name) {
}

//-------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------

TauPi0ClusterCreator::~TauPi0ClusterCreator() 
{
}


StatusCode TauPi0ClusterCreator::initialize() {
  ATH_CHECK(m_tauVertexCorrection.retrieve()); 
  return StatusCode::SUCCESS;
}

//______________________________________________________________________________
StatusCode TauPi0ClusterCreator::executePi0ClusterCreator(xAOD::TauJet& pTau, xAOD::PFOContainer& neutralPFOContainer,
							  xAOD::PFOContainer& hadronicClusterPFOContainer,
							  const xAOD::CaloClusterContainer& pi0ClusterContainer) const
{
    // Any tau needs to have PFO vectors. Set empty vectors before nTrack cut
    std::vector<ElementLink<xAOD::PFOContainer> > empty;
    pTau.setProtoChargedPFOLinks(empty);
    pTau.setProtoNeutralPFOLinks(empty);
    pTau.setProtoPi0PFOLinks(empty);
    pTau.setHadronicPFOLinks(empty);

    // only run shower subtraction on 1-5 prong taus 
    if (pTau.nTracks() == 0 || pTau.nTracks() >5) {
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("ClusterCreator: new tau. \tpt = " << pTau.pt() << "\teta = " << pTau.eta() << "\tphi = " << pTau.phi() << "\tnprongs = " << pTau.nTracks());

    // Retrieve Ecal1 shots and match them to clusters
    std::vector<const xAOD::PFO*> shotVector;
    unsigned nShots = pTau.nShotPFOs();
    for(unsigned iShot=0;iShot<nShots;++iShot){
        const xAOD::PFO* thisShot = pTau.shotPFO(iShot);
        shotVector.push_back( thisShot );
    }
    std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap = getClusterToShotMap(shotVector, pi0ClusterContainer, pTau);
 
    if (! pTau.jetLink().isValid()) {
      ATH_MSG_ERROR("Tau jet link is invalid.");
      return StatusCode::FAILURE;
    }
    const xAOD::Jet *jetSeed = pTau.jet();
    
    const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
    
    const xAOD::Vertex* tauVertex = nullptr;
    if (pTau.vertexLink().isValid()) tauVertex = pTau.vertex();
    
    TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(pTau);

    for (const xAOD::CaloCluster* cluster: pi0ClusterContainer){
        TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
        
        // selection
        if (clusterP4.Pt() < m_clusterEtCut)   continue;
        // Cluster container has clusters for all taus.
        // Only run on clusters that belong to this tau
        if (clusterP4.DeltaR(tauAxis) > 0.4) continue;

        // Get shots in this cluster. Need to use (CaloCluster*) (*clusterItr) 
        // (not a copy!) since the pointer will otherwise be different than in clusterToShotMap
        std::vector<unsigned> shotsInCluster = getShotsMatchedToCluster( shotVector, clusterToShotMap, cluster);

        // Calculate input variables for fake supression. 
        // Do this before applying the vertex correction, 
        // since the position of the cluster in the 
        // calorimeter is required.
        float EM1CoreFrac = getEM1CoreFrac(cluster);
        int NHitsInEM1 = getNPhotons(shotVector, shotsInCluster);
	std::vector<int> NPosECellsInLayer = getNPosECells(cluster);
	std::vector<float> firstEtaWRTClusterPositionInLayer = get1stEtaMomWRTCluster(cluster);
	std::vector<float> secondEtaWRTClusterPositionInLayer = get2ndEtaMomWRTCluster(cluster);

        // Retrieve cluster moments that are used for fake supression and that are not stored in AOD
        // for every cluster. Do this after applying the vertex correction, since the moments 
        // (especcially DELTA_PHI and DELTA_THETA) must be calculated WRT the tau vertex
        double CENTER_MAG = 0.0;
        double FIRST_ETA = 0.0;
        double SECOND_R = 0.0;
        double SECOND_LAMBDA = 0.0;
        double DELTA_PHI = 0.0;
        double DELTA_THETA = 0.0;
        double CENTER_LAMBDA = 0.0;
        double LATERAL = 0.0;
        double LONGITUDINAL = 0.0;
        double ENG_FRAC_EM = 0.0;
        double ENG_FRAC_MAX = 0.0;
        double ENG_FRAC_CORE = 0.0;
        double SECOND_ENG_DENS = 0.0;

        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_MAG, CENTER_MAG) ) ATH_MSG_WARNING("Couldn't retrieve CENTER_MAG moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::FIRST_ETA, FIRST_ETA) ) ATH_MSG_WARNING("Couldn't retrieve FIRST_ETA moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_R, SECOND_R) ) ATH_MSG_WARNING("Couldn't retrieve SECOND_R moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_LAMBDA, SECOND_LAMBDA) ) ATH_MSG_WARNING("Couldn't retrieve SECOND_LAMBDA moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::DELTA_PHI, DELTA_PHI) ) ATH_MSG_WARNING("Couldn't retrieve DELTA_PHI moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::DELTA_THETA, DELTA_THETA) ) ATH_MSG_WARNING("Couldn't retrieve DELTA_THETA moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::CENTER_LAMBDA, CENTER_LAMBDA) ) ATH_MSG_WARNING("Couldn't retrieve CENTER_LAMBDA moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::LATERAL, LATERAL) ) ATH_MSG_WARNING("Couldn't retrieve LATERAL moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::LONGITUDINAL, LONGITUDINAL) ) ATH_MSG_WARNING("Couldn't retrieve LONGITUDINAL moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::ENG_FRAC_EM, ENG_FRAC_EM) ) ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_EM moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::ENG_FRAC_MAX, ENG_FRAC_MAX) ) ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_MAX moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::ENG_FRAC_CORE, ENG_FRAC_CORE) ) ATH_MSG_WARNING("Couldn't retrieve ENG_FRAC_CORE moment. Set it to 0.");
        if( !cluster->retrieveMoment(xAOD::CaloCluster::MomentType::SECOND_ENG_DENS, SECOND_ENG_DENS) ) ATH_MSG_WARNING("Couldn't retrieve SECOND_ENG_DENS moment. Set it to 0.");

       	float E_EM1 = cluster->eSample(CaloSampling::EMB1) + cluster->eSample(CaloSampling::EME1);
	      float E_EM2 = cluster->eSample(CaloSampling::EMB2) + cluster->eSample(CaloSampling::EME2);
        
        // create neutral PFO. Set BDTScore to dummy value <-1. The BDT score is calculated within TauPi0Selector.cxx.
        xAOD::PFO* neutralPFO = new xAOD::PFO();
        neutralPFOContainer.push_back( neutralPFO );

        // Create element link from tau to neutral PFO
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( neutralPFOContainer, neutralPFO );
        pTau.addProtoNeutralPFOLink( PFOElementLink );

        // Set PFO variables
        ElementLink<xAOD::CaloClusterContainer> clusElementLink;
        clusElementLink.toContainedElement( pi0ClusterContainer, cluster );
        neutralPFO->setClusterLink( clusElementLink );
        
        neutralPFO->setP4( (float) cluster->pt(), (float) cluster->eta(), (float) cluster->phi(), (float) cluster->m());
        neutralPFO->setBDTPi0Score( (float) -9999. );
        neutralPFO->setCharge( 0. );
        neutralPFO->setCenterMag( (float) CENTER_MAG);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::nPi0Proto, -1);

        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_FIRST_ETA,       (float) FIRST_ETA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_R,        (float) SECOND_R);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_LAMBDA,   (float) SECOND_LAMBDA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_DELTA_PHI,       (float) DELTA_PHI);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_DELTA_THETA,     (float) DELTA_THETA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_CENTER_LAMBDA,   (float) CENTER_LAMBDA);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_LATERAL,         (float) LATERAL);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_LONGITUDINAL,    (float) LONGITUDINAL);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_EM,     (float) ENG_FRAC_EM);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_MAX,    (float) ENG_FRAC_MAX);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_ENG_FRAC_CORE,   (float) ENG_FRAC_CORE);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_SECOND_ENG_DENS, (float) SECOND_ENG_DENS);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM1,      (float) E_EM1);
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_energy_EM2,      (float) E_EM2);


        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_EM1CoreFrac, EM1CoreFrac);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NHitsInEM1, NHitsInEM1);
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_PS,  NPosECellsInLayer.at(0));
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM1, NPosECellsInLayer.at(1));
        neutralPFO->setAttribute<int>(xAOD::PFODetails::PFOAttributes::cellBased_NPosECells_EM2, NPosECellsInLayer.at(2));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM1, firstEtaWRTClusterPositionInLayer.at(1));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_firstEtaWRTClusterPosition_EM2, firstEtaWRTClusterPositionInLayer.at(2));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM1, secondEtaWRTClusterPositionInLayer.at(1));
        neutralPFO->setAttribute<float>(xAOD::PFODetails::PFOAttributes::cellBased_secondEtaWRTClusterPosition_EM2, secondEtaWRTClusterPositionInLayer.at(2));

        // Store shot element links in neutral PFO
        std::vector<ElementLink<xAOD::IParticleContainer> > shotlinks;
        for(unsigned iShot = 0;iShot<shotsInCluster.size();++iShot){
            ElementLink<xAOD::PFOContainer> shotPFOElementLink = pTau.shotPFOLinks().at(shotsInCluster.at(iShot));
            ElementLink<xAOD::IParticleContainer> shotElementLink;
            shotPFOElementLink.toPersistent();
            shotElementLink.resetWithKeyAndIndex( shotPFOElementLink.persKey(), shotPFOElementLink.persIndex() ); 
            if (!shotElementLink.isValid()) ATH_MSG_WARNING("Created an invalid element link to xAOD::PFO");
            shotlinks.push_back(shotElementLink);
        }
        if(!neutralPFO->setAssociatedParticleLinks( xAOD::PFODetails::TauShot,shotlinks)) 
            ATH_MSG_WARNING("Couldn't add shot links to neutral PFO!");
    }

    // Create hadronic PFOs, put them in output container and store links to tau
    if(!setHadronicClusterPFOs(pTau, hadronicClusterPFOContainer)){
        ATH_MSG_ERROR("Could not set hadronic PFOs");
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

//______________________________________________________________________________
// Functions used to calculate BDT variables other than those provided by the CaloClusterMomentsMaker
float TauPi0ClusterCreator::getEM1CoreFrac(const xAOD::CaloCluster* pi0Candidate) const
{
    float coreEnergy=0.;
    float sumEPosCellsEM1=0.;

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();
    for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        if(sampling!=1 && sampling!=5) continue;
        float cellE = cellInCluster->e() * cellInClusterItr.weight();
        if(cellE<=0) continue;
        sumEPosCellsEM1 += cellE;
        float cellEtaWRTCluster = cellInCluster->eta()-pi0Candidate->eta();
        float cellPhiWRTCluster = P4Helpers::deltaPhi(cellInCluster->phi(), pi0Candidate->phi());
        if(std::abs(cellPhiWRTCluster) > 0.05 || std::abs(cellEtaWRTCluster) > 2 * 0.025/8.) continue;
        coreEnergy+=cellE;
    }
    if(sumEPosCellsEM1<=0.) return 0.;
    return coreEnergy/sumEPosCellsEM1;
}

//______________________________________________________________________________
// Do cluster to shot matching. 
// A cluster is matched to a shot if the seed cell of the shot is in the cluster
std::map<unsigned, xAOD::CaloCluster*> TauPi0ClusterCreator::getClusterToShotMap(const std::vector<const xAOD::PFO*>& shotVector,
										 const xAOD::CaloClusterContainer& pi0ClusterContainer,
										 const xAOD::TauJet &pTau) const
{
    std::map<unsigned, xAOD::CaloCluster*> clusterToShotMap;
    for(unsigned iShot = 0;iShot<shotVector.size();++iShot){
        int seedHash_int = -1;
        if( shotVector.at(iShot)->attribute(xAOD::PFODetails::PFOAttributes::tauShots_seedHash, seedHash_int) == false) {
            std::cout << "WARNING: Couldn't find seed hash. Set it to -1, no cluster will be associated to shot." << std::endl;
        }
        const IdentifierHash seedHash = (const IdentifierHash) seedHash_int; 
        xAOD::CaloClusterContainer::const_iterator clusterItr   (pi0ClusterContainer.begin()),
                                                   clusterItrEnd(pi0ClusterContainer.end());
        float weightInCluster=-1.;
        float weightInPreviousCluster=-1;
    
        const xAOD::Jet *jetSeed = pTau.jet();
        if (!jetSeed) {
          ATH_MSG_ERROR("Tau jet link is invalid.");
          return clusterToShotMap;
        }
        const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
    
        const xAOD::Vertex* tauVertex = nullptr;
        if (pTau.vertexLink().isValid()) tauVertex = pTau.vertex();
      
        TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(pTau);

        for (; clusterItr != clusterItrEnd; ++clusterItr){
            xAOD::CaloCluster* cluster = (xAOD::CaloCluster*) (*clusterItr);
            TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
            
            weightInCluster=-1.;
            if (clusterP4.Et() < m_clusterEtCut) continue; // Not interested in clusters that fail the Et cut
            // Cluster container has clusters for all taus.
            // Only run on clusters that belong to this tau
            if (clusterP4.DeltaR(tauAxis) > 0.4)  continue;
            const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
            CaloClusterCellLink::const_iterator cellItr  = theCellLink->begin();
            CaloClusterCellLink::const_iterator cellItrE = theCellLink->end();
            for(;cellItr!=cellItrE; ++cellItr){
                CaloCell* cellInCluster = (CaloCell*) *cellItr;
                // Check if seed cell is in cluster.
                if(cellInCluster->caloDDE()->calo_hash()!=seedHash) continue;
                weightInCluster = cellItr.weight();
                // found cell, no need to loop over other cells
                break;
            }
            if(weightInCluster<0) continue;
            // Check if cell was already found in a previous cluster
            if(weightInPreviousCluster<0){
                // Cell not found in a previous cluster. 
                // Have to check whether cell is shared with other cluster
                clusterToShotMap[iShot] = cluster;
                weightInPreviousCluster = weightInCluster;
            }
            else{
                // Cell has been found in a previous cluster
                // assign shot to this cluster if it has larger weight for the cell
                // otherwise the shots keeps assigned to the previous cluster
                if(weightInCluster>weightInPreviousCluster){
                    clusterToShotMap[iShot] = cluster;
                }
                // No need to loop over other clusters as cells can not be shared by more than two clusters
                break;
            }
        }
    }
    return clusterToShotMap;
}

//______________________________________________________________________________
std::vector<unsigned> TauPi0ClusterCreator::getShotsMatchedToCluster(const std::vector<const xAOD::PFO*>& shotVector,
								     const std::map<unsigned, xAOD::CaloCluster*>& clusterToShotMap, 
								     const xAOD::CaloCluster* pi0Cluster) const
{
    std::vector<unsigned> shotsMatchedToCluster;
    for(unsigned iShot = 0;iShot<shotVector.size();++iShot){
        auto itr = clusterToShotMap.find(iShot);
        if(itr==clusterToShotMap.end()) continue;
        if(itr->second!=pi0Cluster) continue;
        shotsMatchedToCluster.push_back(iShot);
    }
    return shotsMatchedToCluster;
}

//______________________________________________________________________________
int TauPi0ClusterCreator::getNPhotons(const std::vector<const xAOD::PFO*>& shotVector,
				      const std::vector<unsigned>& shotsInCluster ) const
{
    int nPhotons = 0;
    for(unsigned iShot = 0;iShot<shotsInCluster.size();++iShot){
        int curNPhotons=0;
        if(shotVector.at(shotsInCluster.at(iShot))->attribute(xAOD::PFODetails::PFOAttributes::tauShots_nPhotons,curNPhotons) == false)
            ATH_MSG_WARNING("Can't find NHitsInEM1. Set it to 0.");
        nPhotons+=curNPhotons;
    }
    return nPhotons;
}

//______________________________________________________________________________
std::vector<int> TauPi0ClusterCreator::getNPosECells(const xAOD::CaloCluster* pi0Candidate) const
{
    std::vector<int> nPosECellsInLayer(3,0); // 3 layers initialised with 0 +ve cells

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

    for(;cellInClusterItr!=cellInClusterItrE; ++cellInClusterItr){
        const CaloCell* cellInCluster = static_cast<const CaloCell*>( *cellInClusterItr);
        int sampling = cellInCluster->caloDDE()->getSampling();
        // Get cell layer: PSB and PSE belong to layer 0,  
        // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
        int cellLayer = sampling%4;  
        if(cellLayer < 3 && cellInCluster->e() > 0) nPosECellsInLayer[cellLayer]++;
    }
    return nPosECellsInLayer;
}

//______________________________________________________________________________
std::vector<float> TauPi0ClusterCreator::get1stEtaMomWRTCluster(const xAOD::CaloCluster* pi0Candidate) const
{
    std::vector<float> firstEtaWRTClusterPositionInLayer (4, 0.);  //init with 0. for 0-3 layers
    std::vector<float> sumEInLayer (4, 0.); //init with 0. for 0-3 layers

    const CaloClusterCellLink* theCellLink = pi0Candidate->getCellLinks();
    CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
    CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

    for(;cellInClusterItr!=cellInClusterItrE;++cellInClusterItr){
        CaloCell* cellInCluster = (CaloCell*) *cellInClusterItr;
        int sampling = cellInCluster->caloDDE()->getSampling();
        // Get cell layer: PSB and PSE belong to layer 0,  
        // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
        int cellLayer = sampling%4;
        
        float cellEtaWRTClusterPos=cellInCluster->eta()-pi0Candidate->eta();
        float cellE=cellInCluster->e();
        if(cellE<=0  || cellLayer>=3) continue;
        firstEtaWRTClusterPositionInLayer[cellLayer]+=cellEtaWRTClusterPos*cellE;
        sumEInLayer[cellLayer]+=cellE;
    }

    for(int iLayer=0;iLayer<4;++iLayer){
        if(sumEInLayer[iLayer]!=0) 
            firstEtaWRTClusterPositionInLayer[iLayer]/=std::abs(sumEInLayer[iLayer]);
        else firstEtaWRTClusterPositionInLayer[iLayer]=0.;
    }
    return firstEtaWRTClusterPositionInLayer;
}

//______________________________________________________________________________
std::vector<float> TauPi0ClusterCreator::get2ndEtaMomWRTCluster( const xAOD::CaloCluster* pi0Candidate) const
{
      std::vector<float> secondEtaWRTClusterPositionInLayer (4, 0.); //init with 0. for 0-3 layers
      std::vector<float> sumEInLayer (4, 0.); //init with 0. for 0-3 layers

      const CaloClusterCellLink* theCellLinks = pi0Candidate->getCellLinks();

      for(const CaloCell* cellInCluster: *theCellLinks){
            int sampling = cellInCluster->caloDDE()->getSampling();
            // Get cell layer: PSB and PSE belong to layer 0,  
            // EMB1 and EME1 to layer 1, EMB2 and EME2 to layer 2. 
            int cellLayer = sampling%4;

            float cellEtaWRTClusterPos=cellInCluster->eta()-pi0Candidate->eta();
            float cellE=cellInCluster->e();
            if(cellE<=0  || cellLayer>=3) continue;
            secondEtaWRTClusterPositionInLayer[cellLayer]+=cellEtaWRTClusterPos*cellEtaWRTClusterPos*cellE;
            sumEInLayer[cellLayer]+=cellE;
      }

      for(int iLayer=0;iLayer<4;++iLayer){
            if(sumEInLayer[iLayer]!=0) 
                secondEtaWRTClusterPositionInLayer[iLayer]/=std::abs(sumEInLayer[iLayer]);
            else secondEtaWRTClusterPositionInLayer[iLayer]=0.;
      }
      return secondEtaWRTClusterPositionInLayer;
}

//______________________________________________________________________________
bool TauPi0ClusterCreator::setHadronicClusterPFOs(xAOD::TauJet& pTau, xAOD::PFOContainer& pHadronPFOContainer) const
{
    if (! pTau.jetLink().isValid()) {
      ATH_MSG_ERROR("Tau jet link is invalid.");
      return false;
    }
    const xAOD::Jet *jetSeed = pTau.jet();
    
    const xAOD::Vertex* jetVertex = m_tauVertexCorrection->getJetVertex(*jetSeed);
    
    const xAOD::Vertex* tauVertex = nullptr;
    if (pTau.vertexLink().isValid()) tauVertex = pTau.vertex();
    
    TLorentzVector tauAxis = m_tauVertexCorrection->getTauAxis(pTau);
    
    std::vector<const xAOD::CaloCluster*> clusterList;
    StatusCode sc = tauRecTools::GetJetClusterList(jetSeed, clusterList, m_useSubtractedCluster);
    if (!sc) return false;

    for (const xAOD::CaloCluster* cluster : clusterList){
        // Procedure: 
        // - Calculate cluster energy in Hcal. This is to treat -ve energy cells correctly
        // - Then set 4momentum via setP4(E/cosh(eta), eta, phi, m). This forces the PFO to have the correct energy and mass
        // - Ignore clusters outside 0.2 cone and those with overall negative energy or negative energy in Hcal

        // Don't create PFOs for clusters with overall (Ecal+Hcal) negative energy (noise)
        TLorentzVector clusterP4 = m_tauVertexCorrection->getVertexCorrectedP4(*cluster, tauVertex, jetVertex);
        
        if(clusterP4.E()<=0.) continue;

        // Only need clusters in core cone. Others are not needed for subtraction
        if(tauAxis.DeltaR(clusterP4) > 0.2) continue;

        // Loop over cells to calculate cluster energy in Hcal
        double clusterE_Hcal=0.;
	const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
	CaloClusterCellLink::const_iterator cellInClusterItr  = theCellLink->begin();
	CaloClusterCellLink::const_iterator cellInClusterItrE = theCellLink->end();

	for(; cellInClusterItr != cellInClusterItrE; ++cellInClusterItr){
	   const CaloCell* cellInCluster = static_cast<const CaloCell*> (*cellInClusterItr);

            //Get only HCAL cells
            int sampling = cellInCluster->caloDDE()->getSampling();
            if (sampling < 8) continue;

            double cellE = cellInCluster->e()*cellInClusterItr.weight();
            clusterE_Hcal+=cellE;
        }
        // Don't save PFOs for clusters with negative energy in Hcal 
        if(clusterE_Hcal<=0.) continue;

        // Create hadronic PFO
        xAOD::PFO* hadronicPFO = new xAOD::PFO();
        pHadronPFOContainer.push_back( hadronicPFO );

        // Set 4mom. Eta and phi are taken from cluster
        double cluster_Pt_Hcal = clusterE_Hcal/std::cosh(cluster->eta());
        hadronicPFO->setP4( (float) cluster_Pt_Hcal, (float) cluster->eta(), (float) cluster->phi(), (float) 0.);

        // TODO: May want to set element link to the cluster the PFO is originating from
        // ElementLink<xAOD::CaloClusterContainer> clusElementLink;
        // clusElementLink.toContainedElement( CLUSTERCONTAINER, cluster );
        // hadronicPFO->setClusterLink( clusElementLink );

        // Create element link from tau to hadronic PFO
        ElementLink<xAOD::PFOContainer> PFOElementLink;
        PFOElementLink.toContainedElement( pHadronPFOContainer, hadronicPFO );
        pTau.addHadronicPFOLink( PFOElementLink );
    }
    return true;
}

#endif
