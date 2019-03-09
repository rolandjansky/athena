/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFromTrackAndClusters.h
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Renjie Wang (renjie.wang@cern.ch)
	  Use tracks and vertices to remove pile-up clusters by Constituent-level pile-up mitigation techniques
	  (cluster vertex fraction, Voronoi Subtraction, SofterKiller )
	  the track-based soft term is also added to MET

CREATED:  Aug 05, 2018


 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromTrackAndClusters.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include <cmath>
#include <string>
using namespace std;


namespace SortHelper {
//
// For Sorting
//
struct pt_sort {

    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs) {
        return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs) {
        return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs) {
        return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs) {
        return (lhs->pt() > rhs->pt());
    }
};


template<typename T>
T sort_container_pt(T* inCont)
{
    T sortedCont(SG::VIEW_ELEMENTS);
    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return sortedCont;
}

template<typename T>
const T sort_container_pt(const T* inCont)
{
    ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return *sortedCont.asDataVector();
}

struct PJcomp {
    bool operator() (const std::pair<fastjet::PseudoJet, std::vector<float> >& lhsp, const std::pair<fastjet::PseudoJet, std::vector<float> >& rhsp) {
        fastjet::PseudoJet lhs = lhsp.first;
        fastjet::PseudoJet rhs = rhsp.first;
        return lhs.pt()>rhs.pt();
        //The comparator must be a strict weak ordering.
    }
};


} // end SortHelper



EFMissingETFromTrackAndClusters::EFMissingETFromTrackAndClusters(const std::string& type,
        const std::string& name,
        const IInterface* parent) :
    EFMissingETBaseTool(type, name, parent)
{
    declareProperty("SaveUncalibrated", m_saveUncalibrated = false ,"save uncalibrated topo. clusters");
    // standalone module.

    // declare configurables
    m_fextype = FexType::TOPO;
    m_metHelperPosition = 18;

    // Properties are as follows:
    m_lambdaCalDivide = 317;
    m_isCaloSplit = false; //If false, SK is run the same on all clusters. If true, SK is run separately for clusters in the ECal and the  HCal.
    m_gridSpacing = 0.6;   // The grid size that should be applied for the SK algorithm. Suggested values between 0.3 and 0.6
    m_eCalGrid = 0.5;    //Only necessary if isCaloSplit == true. The SK grid spacing used for the ECal.
    m_hCalGrid = 0.5;    // Only necessary if isCaloSplit == true. The SK grid spacing used for the HCal.
    m_rapmin = 0.0;      //The minimum (absolute) rapidity over which to calculate SK
    m_rapmax = 2.5;      //The maximum (absolute) rapidity over which to calculate SK
    m_rapminApplied = 0;  //The minimum (absolute) rapidity over which to apply SK
    m_rapmaxApplied = 10; //The maximum (absolute) rapidity over which to apply SK


    m_deltaR = 0.01;
    //initialization to make coverity happy:
    m_clusterstate = xAOD::CaloCluster_v1::UNCALIBRATED;

}


EFMissingETFromTrackAndClusters::~EFMissingETFromTrackAndClusters()
{
}


StatusCode EFMissingETFromTrackAndClusters::initialize()
{

    ATH_MSG_DEBUG( "called EFMissingETFromTrackAndClusters::initialize()" );

    /// timers
    if( service( "TrigTimerSvc", m_timersvc).isFailure() )
        ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );

    if (m_timersvc) {
        // global time
        std::string basename=name()+".TotalTime";
        m_glob_timer = m_timersvc->addItem(basename);
    } // if timing service

    if(m_saveUncalibrated) m_metHelperPosition = 13;


    m_clusterstate = (m_saveUncalibrated) ? xAOD::CaloCluster_v1::UNCALIBRATED : xAOD::CaloCluster_v1::CALIBRATED;



    return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromTrackAndClusters::execute()
{
    return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromTrackAndClusters::finalize()
{
    ATH_MSG_DEBUG( "called EFMissingETFromTrackAndClusters::finalize()" );

    return StatusCode::SUCCESS;

}

StatusCode EFMissingETFromTrackAndClusters::execute(xAOD::TrigMissingET * /* met */ ,
        TrigEFMissingEtHelper *metHelper ,
        const xAOD::CaloClusterContainer *caloCluster, const xAOD::JetContainer * /* jets */,
        const xAOD::TrackParticleContainer * trackContainer,
        const xAOD::VertexContainer * vertexContainer,
        const xAOD::MuonContainer * muonContainer )
{

    ATH_MSG_DEBUG( "called EFMissingETFromTrackAndClusters::execute()" );

    if(m_timersvc)
        m_glob_timer->start(); // total time

    /// fetching the topo. cluster component
    TrigEFMissingEtComponent* metComp = nullptr;
    metComp = metHelper->GetComponent(metHelper->GetElements() - m_metHelperPosition); // fetch Cluster component
    if (metComp==0) {
        ATH_MSG_ERROR( "cannot fetch Topo. cluster component!" );
        return StatusCode::FAILURE;
    }
    if(string(metComp->m_name).substr(0,2)!="TC") {
        ATH_MSG_ERROR( "fetched " << metComp->m_name << " instead of the Clusters component!" );
        return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG( "fetched metHelper component \"" << metComp->m_name << "\"" );


    if ( (metComp->m_status & m_maskProcessed)==0 ) { // not yet processed
        metComp->Reset();  // reset component...
    } else { // skip if processed
        if(m_timersvc) m_glob_timer->stop(); // total time
        return StatusCode::SUCCESS;
    }

    // set status to Processing
    metComp->m_status |= m_maskProcessing;

    //######################################################################


    ATH_MSG_DEBUG( "num of tracks: " << trackContainer->size() );
    ATH_MSG_DEBUG( "num of topoclusters: " << caloCluster->size() );
    ATH_MSG_DEBUG( "num of vertices: " << vertexContainer->size() );

    std::vector<const xAOD::Muon*> MuonVec;
    if(muonContainer!=nullptr) {
        for (auto muon : *muonContainer) {
            MuonVec.push_back(muon);
        }
    }
    ATH_MSG_DEBUG( "num of muons: " << MuonVec.size() );


    // HLT Muons
    std::vector<const xAOD::TrackParticle*> vecOfMuonTrk;
    for( const xAOD::Muon* muon : MuonVec ) {
        const xAOD::Muon::MuonType muontype = muon->muonType();
        // combined or segment tagged muon
        if(muontype == xAOD::Muon::MuonType::Combined || muontype == xAOD::Muon::MuonType::SegmentTagged ) {
            const xAOD::TrackParticle* idtrk = muon->trackParticle( xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle );
            if(idtrk==0) continue;
            if(fabs(muon->pt())<5000) continue;


            bool find_duplicate(false);
            for(std::vector<const xAOD::TrackParticle*>::size_type idx=0; idx<vecOfMuonTrk.size(); idx++) {
                float deltaR_ = idtrk->p4().DeltaR(vecOfMuonTrk[idx]->p4());
                if(deltaR_< m_deltaR) {
                    find_duplicate = true;
                    break;
                }
            } //end of loop over stored muon tracks

            ATH_MSG_DEBUG( "Found muon track --> pt = " << idtrk->pt()/1000.
                          << " eta= " << idtrk->eta()
                          << " phi= " << idtrk->phi()
                          <<  " Found duplicate=" << find_duplicate
                        );
            if(find_duplicate) continue;

            vecOfMuonTrk.push_back(idtrk);  // store for testing future muon tracks for double counting prevention

        }
    }

    // vertex
    const xAOD::Vertex* primaryVertexFTK = nullptr;
    for (auto vertex: *vertexContainer) {
        //count the number of vertices with at least 2 tracks
        ATH_MSG_DEBUG ( " x: " << vertex->x() << "\ty: " << vertex->y() << "\tz: " << vertex->z()
                       << "\tntracks: " <<  vertex->nTrackParticles()
                       << "\ttype: " << vertex->vertexType()
                       << "\txAOD::VxType::PriVtx: " << xAOD::VxType::PriVtx
                     );
        //if( vertex->nTrackParticles() < 2 ) continue;
        if( vertex->vertexType() == xAOD::VxType::PriVtx ) {
            primaryVertexFTK = vertex;
            break;
        }
    }


    //having FTK vertex, and find the associated tracks
    std::vector<const xAOD::TrackParticle*> TrackVec_FTK_PV;
    TrackVec_FTK_PV.clear();

    std::vector<const xAOD::TrackParticle*> goodTracksnoMuons;;
    goodTracksnoMuons.clear();

    if(primaryVertexFTK!=nullptr) {

        const std::vector< ElementLink< xAOD::TrackParticleContainer> > tpLinks = primaryVertexFTK->trackParticleLinks();
        ATH_MSG_DEBUG ( "  tpLinks size: " << tpLinks.size());
        if(tpLinks.size() != 0) {

            for(const auto& tp_elem : tpLinks ) {
                if (tp_elem != nullptr && tp_elem.isValid()) {
                    const xAOD::TrackParticle* itrk = *tp_elem;

                    ATH_MSG_DEBUG ( "\titrk pt: " << itrk->pt()/1000. << "\teta: " << itrk->eta() << "\tphi: " << itrk->phi()
                                    //<< "\tvertex: " << itrk->vertex()
                                    << "\tz0: " << itrk->z0()
                                    << "\tvz: " << itrk->vz()
                                    << "\ttheta: " << itrk->theta()
                                    //<< "\tdZ: " << fabs((itrk->z0()+itrk->vz()-primaryVertexFTK->z())*sin(itrk->theta()))
                                    //<< "\tisfromPV: " << (itrk->vertex()==primaryVertexFTK)
                                  );

                    TrackVec_FTK_PV.push_back(itrk);


                    //saving tracks for soft-term
                    float t_pt = itrk->pt() * 0.001 ;
                    float t_eta = itrk->eta();
                    //float t_phi = itrk->phi();
                    //float t_en = itrk->e() * 0.001;
                    if(t_pt<0.4 || fabs(t_eta)>2.5) continue;

                    bool findMuonTrack(false);
                    for(std::vector<const xAOD::TrackParticle*>::size_type idx=0; idx<vecOfMuonTrk.size(); idx++) {
                        float deltaR_ = itrk->p4().DeltaR(vecOfMuonTrk[idx]->p4());
                        if(deltaR_<m_deltaR) {
                            findMuonTrack=true;
                            break;
                        }
                    }
                    if(findMuonTrack) continue;

                    goodTracksnoMuons.push_back(itrk);

                }
            }
        }

    } // having FTK vertex



    /////////////////////////////////////////
    // Voronoi Subtraction
    ////////////////////////////////////////




    //CaloClusterChangeSignalStateList stateHelperList;
    std::vector<fastjet::PseudoJet> clusters;
    clusters.reserve(caloCluster->size());
    for(auto clust: *caloCluster) {
        //read in clusters as PseudoJets
        //if(m_doLC) stateHelperList.add(clust,xAOD::CaloCluster::State(1)); //default is calibrated but we can make it explicit anyway
        //else stateHelperList.add(clust,xAOD::CaloCluster::State(0));

        fastjet::PseudoJet test;
        test = fastjet::PseudoJet(clust->p4());
        if(clust->e() >= 0) { //definition of clusters (\label{clust_def})
            clusters.push_back(test);
            if(m_debug) std::cout << clust->pt() << std::endl;
        }
    }

    const char* APP_NAME = "VoronoiWeightTool::process()";
    std::vector< std::pair< fastjet::PseudoJet, std::vector<float> > > ptvec; //vector of pairs of PJs and their corrected pTs
    if(makeVoronoiClusters(clusters, ptvec) != StatusCode::SUCCESS) ATH_MSG_ERROR(APP_NAME << ": Error in makeVoronoiClusters");
    std::sort(ptvec.begin(), ptvec.end(), SortHelper::PJcomp());

    if(m_doSpread && m_nSigma > 0) ATH_MSG_ERROR(APP_NAME << ": Can't combine spreading with nSigma yet");
    int alg;
    if(m_doSpread && m_nSigma == 0) alg = 3;
    if(!m_doSpread && m_nSigma == 0) alg = 1;
    if(!m_doSpread && m_nSigma > 0) alg = 2;

    size_t i=0;
    std::vector<const xAOD::CaloCluster*> SortedAlltopoClusters;
    for(auto el : *caloCluster) SortedAlltopoClusters.push_back( el);
    std::sort(SortedAlltopoClusters.begin(), SortedAlltopoClusters.end(), SortHelper::pt_sort());


    // need new colletion need to change the energy of cluster,
    //and remove the zero-energy clusters
    std::vector<xAOD::CaloCluster> VoronoiWeightedTopoClusters;
    VoronoiWeightedTopoClusters.clear();


    for(auto clust : SortedAlltopoClusters) {

        xAOD::CaloCluster aclust = (*clust);

        float newE;
        //There should be the same number of positive E Clusters in the container as clusters in the ptvec
        bool endContainer = clust->e()<0; //consistent with definition of clusters on L.~\ref{clust_def}
        bool endVec = i>=ptvec.size();
        if(endVec && endContainer) {
            newE = 0;  //remove negative energy clusters
        } else if(endContainer || endVec) {
            ATH_MSG_ERROR(APP_NAME << ": Clusters don't have same number of elements.");
            return StatusCode::FAILURE;
        } else {
            //And the clusters should match
            float Containerpt = clust->pt();
            float PJpt = ptvec[i].first.pt();
            if(m_debug) {
                std::cout << "Container: " << Containerpt << std::endl;
                std::cout << "Ptvec: " << PJpt << std::endl;
            }
            if (fabs(Containerpt-PJpt) > 0.1) {
                if(m_debug) std::cout << fabs(Containerpt-PJpt) << std::endl;
                ATH_MSG_ERROR(APP_NAME << ": Clusters don't match.");
                return StatusCode::FAILURE;
            }
            newE = ptvec[i].second[alg]*cosh(clust->eta());
        }

        //float w = newE/clust->e();
        aclust.setE(newE);
        if(newE>0) VoronoiWeightedTopoClusters.push_back(aclust);

        i++;
    }



    //////////////////////////////////////
    //Soft Killer
    //////////////////////////////////////

    if(m_isCaloSplit == false) RunClusters(VoronoiWeightedTopoClusters);
    else RunSplitClusters(VoronoiWeightedTopoClusters);

    std::vector<xAOD::CaloCluster> SKWeightedTopoClusters;

    for(xAOD::CaloCluster cl : VoronoiWeightedTopoClusters) {
        int w = 1;
        if(m_isCaloSplit == false) w = calculateWeight(cl);
        else w = calculateSplitWeight(cl);
        float clust_e = cl.e(); // Cluster energy
        float newclust_e = clust_e * w;
        //cout << "newclust_e: " << newclust_e << endl;
        cl.setE( newclust_e );
        if(newclust_e>0) SKWeightedTopoClusters.push_back(cl);
    }


    ///////////////////////////////////////
    // Cluster Vertex Fraction
    ///////////////////////////////////////
    std::vector<float> cvfs;
    setCVF(SKWeightedTopoClusters,trackContainer,TrackVec_FTK_PV,cvfs);



    //######################################################################

    size_t iclus=0;
    std::vector<xAOD::CaloCluster> HStopocls;
    for(auto clust: SKWeightedTopoClusters) { //Es are sorted by default order

        float phi = clust.phi();
        float cosPhi, sinPhi;
        sincosf(phi, &sinPhi, &cosPhi);
        float eta = clust.eta();

        float Et  = clust.pt();
        float Ex = Et*cosPhi;
        float Ey = Et*sinPhi;
        float Ez = Et*sinhf(eta);
        float E =  clust.p4().E();

        bool isPU_topocl(false);
        if(cvfs[iclus]>-1 && cvfs[iclus]<=0.1) {
            if( Et < 3000 ) {
                isPU_topocl = true;
            }
        }

        if(!isPU_topocl) {
            metComp->m_ex -= Ex;
            metComp->m_ey -= Ey;
            metComp->m_ez -= Ez;
            metComp->m_sumEt += Et;
            metComp->m_sumE  += E;
            metComp->m_usedChannels += 1;
            metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,Et)+0.5));

            HStopocls.push_back(clust);
        }

        iclus++;
    }

    //adding track-based soft-term
    for( const xAOD::TrackParticle* track : goodTracksnoMuons ) {

        float trketa = track->eta();
        float trkphi = track->phi();
        float trkpt = track->pt();
        float trkcharge = track->charge();

        float deltaphi = ExtrapolationEstimate(trkpt,trketa,trkcharge);
        float newtrkphi = -999.0;
        if(deltaphi>-998.0) newtrkphi = trkphi-deltaphi;

        if(trkpt<1000) continue;

        float dRmin(999);
        for(auto clust: HStopocls) {
            float dR = xAOD::P4Helpers::deltaR(clust,trketa, newtrkphi);
            if(dR<dRmin) dRmin=dR;
        }

        if(dRmin>0.1) {
            metComp->m_ex -= track->p4().Px();
            metComp->m_ey -= track->p4().Py();
            metComp->m_ez -= track->p4().Pz();
            metComp->m_sumEt += track->pt();
            metComp->m_sumE  += track->e();
        }
    }




    // move from "processing" to "processed" state
    metComp->m_status ^= m_maskProcessing; // switch off bit
    metComp->m_status |= m_maskProcessed;  // switch on bit

    if(m_timersvc)
        m_glob_timer->stop(); // total time

    return StatusCode::SUCCESS;
}


void EFMissingETFromTrackAndClusters::setCVF(
    std::vector<xAOD::CaloCluster> clusters,
    const xAOD::TrackParticleContainer* tracks,
    std::vector<const xAOD::TrackParticle*> TrackVec_FTK_PV,
    std::vector<float>& cvfs)
{
    //static SG::AuxElement::Decorator< float > newphi("newphi");
    for(auto clust: clusters) { //cvfs are in default order
        //std::cout << "c: " << clust->pt() << ";" << clust->eta() << ";" << clust->phi() << std::endl;
        float num = 0;
        float den = 0;

        float cleta = clust.eta();

        for(auto track: *tracks) {

            bool isTrackClusterMatched(true);
            //isTrackClusterMatched = xAOD::P4Helpers::isInDeltaR(*clust,*track,0.1,false);

            float trketa = track->eta();
            float trkphi = track->phi();
            float trkpt = track->pt();
            float trkcharge = track->charge();

            float deta = trketa-cleta;
            isTrackClusterMatched &= (fabs(deta)<0.1);

            float deltaphi = ExtrapolationEstimate(trkpt,trketa,trkcharge);
            float newtrkphi = -999.0;
            if(deltaphi>-998.0) newtrkphi = trkphi-deltaphi;

            float dR = xAOD::P4Helpers::deltaR(clust,trketa,newtrkphi);
            isTrackClusterMatched &= (dR<0.1);

            if(isTrackClusterMatched) {
                float trkpt = track->pt();
                //std::cout << "t: " << track->pt() << ";" << track->eta() << ";" << track->phi() << std::endl;

                bool isfromPV(false);
                for( const xAOD::TrackParticle* itrkPV : TrackVec_FTK_PV ) {
                    float deltaR_ = track->p4().DeltaR(itrkPV->p4());
                    if(deltaR_ < m_deltaR) {
                        isfromPV=true;
                        break;
                    }
                }

                if (isfromPV) {
                    //PV
                    num+=trkpt;
                    den+=trkpt;
                } else {
                    //pileup
                    den+=trkpt;
                }
            }
        }
        float cvf;
        if(den==0) cvf = -1;
        else cvf = num/den; //Rpt
        cvfs.push_back(cvf);
        //if(cvf==0 and clust->pt()>2000) std::cout << clust->pt() << std::endl;
        //std::cout << clust->pt() << ";" << cvf << std::endl;
    }
}


float EFMissingETFromTrackAndClusters::ExtrapolationEstimate(float pt, float eta, float charge)
{
    const float params[3] = { -14.6027571, -44.7818374, 540.656643};
    float eptsindeltaphi = params[0]*pow(eta,4)+params[1]*pow(eta,2)+params[2]*pow(eta,0);
    float sindeltaphi = eptsindeltaphi/(pt*charge);
    if (fabs(sindeltaphi)>1.0) return -999.0; //never reaches the detector
    float deltaphi = asin(sindeltaphi);
    return deltaphi;
}



StatusCode EFMissingETFromTrackAndClusters::makeVoronoiClusters(std::vector<fastjet::PseudoJet>& clusters,std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec) const
{
    std::vector<fastjet::PseudoJet> & inputConst = clusters;
    fastjet::Selector jselector = fastjet::SelectorAbsRapRange(0.0,2.1);
    fastjet::JetAlgorithm algo = fastjet::kt_algorithm;
    float jetR = 0.4;
    fastjet::JetDefinition jetDef(algo, jetR,fastjet::E_scheme, fastjet::Best);
    fastjet::AreaDefinition area_def(fastjet::voronoi_area, fastjet::VoronoiAreaSpec(0.9));

    fastjet::ClusterSequenceArea clustSeq(inputConst, jetDef, area_def);
    fastjet::JetMedianBackgroundEstimator bge(jselector,jetDef,area_def);

    bge.set_particles(inputConst);
    std::vector<fastjet::PseudoJet> inclusiveJets = sorted_by_pt(clustSeq.inclusive_jets(0));

    int nsigma = m_nSigma;
    float rho = bge.rho();
    float sigma = bge.sigma();
    for(unsigned int iJet = 0 ; iJet < inclusiveJets.size() ; iJet++) {
        fastjet::PseudoJet jet = inclusiveJets[iJet];
        std::vector<fastjet::PseudoJet> constituents = jet.constituents();
        for(auto cons : constituents) {
            float pt = cons.pt();
            float area = cons.area();
            float subPt = pt-rho*area;
            //std::cout << "Area: " << area << "; Rho: " << bge.rho() << "; pt: " << constituents[iCons].pt() << "; corrected: " << correctedPt << std::endl;
            //std::cout << "Pt: " << cons.pt() << "; Eta: " << cons.eta() <<"; Phi: " << cons.phi() << std::endl;
            //fastjet::PseudoJet constituentP;
            /*if(correctedPt<0.) continue;
            constituentP.reset_PtYPhiM(correctedPt, constituents[iCons].rap(), constituents[iCons].phi(), constituents[iCons].m());
            clusters_voronoi.push_back(constituentP);*/
            //correctedptmap[cons] = correctedPt;
            float voro0pt = subPt * (subPt > 0);
            float voro1pt = subPt * (subPt > sqrt(area)*sigma*(float)nsigma);
            std::vector<float> algopts;
            algopts.push_back(subPt);
            algopts.push_back(voro0pt);
            algopts.push_back(voro1pt);
            algopts.push_back(0);
            std::pair <fastjet::PseudoJet,std::vector<float> > pjcptpair (cons,algopts);
            correctedptvec.push_back(pjcptpair);
        } // end loop over cons
    } // end loop over jets
    //std::cout << "Size: " << correctedptmap.size() << std::endl;

    if(m_doSpread) spreadPt(correctedptvec);

    return StatusCode::SUCCESS;
}



void EFMissingETFromTrackAndClusters::spreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr, float alpha) const
{
    //default alpha = 2
    //Set up neighbors within spreadr:
    int clusters = correctedptvec.size();
    std::vector<float> spreadPT(clusters);
    std::vector<bool> isPositive(clusters);
    for(int iCl = 0; iCl < clusters; iCl++) {
        spreadPT[iCl] = correctedptvec[iCl].second[0];
        isPositive[iCl] = spreadPT[iCl]>0;
    }

    std::vector<std::vector<std::pair<int,float> > > cluster_drs; //for each cluster, list of nearby positive pT clusters and their distances
    for(int iCl = 0; iCl < clusters; iCl++) {
        fastjet::PseudoJet icluster = correctedptvec[iCl].first;
        //float ieta = icluster.eta();
        //float iphi = icluster.phi();
        std::vector<std::pair<int,float> > this_cluster_drs;
        for(int jCl = 0; jCl < clusters; jCl++) {
            if(iCl == jCl) continue;
            if(!isPositive[jCl]) continue;
            fastjet::PseudoJet jcluster = correctedptvec[jCl].first;
            //float jeta = jcluster.eta();
            //float jphi = jcluster.phi();
            float dphi = icluster.delta_phi_to(jcluster);
            float deta = icluster.eta() - jcluster.eta(); //fastjet::pseudojet::delta_R(const PseudoJet& other) gives rap-phi distance
            float dr2 = pow(dphi,2) + pow(deta,2);
            if(dr2 > pow(spreadr,2)) continue;
            std::pair<int,float> jdrpair (jCl,dr2);
            this_cluster_drs.push_back(jdrpair);
        }
        cluster_drs.push_back(this_cluster_drs);
    }


    for(int i = 0; i < clusters; i++) {
        if(!(spreadPT[i]<0)) continue; //only spread from negative pT clusters
        //find closest positive PT cluster:
        float sumdR2 = 0;
        //iterate over nearby positive pT clusters
        for(size_t j=0; j<cluster_drs[i].size(); j++) {
            //cout << "j: " << j << " realid: " << realid << " Eta: " << points[realid][0]<< " Phi: " << points[realid][1] << " Pt:" << spreadPT[realid] << " Dist: " << dists[j] << endl;  // dists[j] = dR^2
            float dr = cluster_drs[i][j].second;
            if(dr>0) sumdR2 += 1./(pow(dr,alpha/2));
        }
        //if at least one neighbor
        if(sumdR2 > 0) {
            float spreadPT_orig = spreadPT[i];
            //std::cout << "orig: " << spreadPT_orig << std::endl;
            for(size_t j=0; j<cluster_drs[i].size(); j++) {
                float dr = cluster_drs[i][j].second;
                float realid = cluster_drs[i][j].first;
                if(dr>0) {
                    float weight = (1./pow(dr,alpha/2))/sumdR2;
                    //std::cout << dr << "; " << weight << std::endl;
                    //std::cout << "Before spreading: " << weight << ";" << weight*spreadPT_orig << ";" << spreadPT[realid] << std::endl;
                    if(fabs(weight*spreadPT_orig)>spreadPT[realid]) {
                        spreadPT[i]+=spreadPT[realid];
                        spreadPT[realid]=0;
                    } else {
                        spreadPT[realid]+=weight*spreadPT_orig;
                        spreadPT[i]-=weight*spreadPT_orig;
                    }
                    //std::cout << "After spreading: " << weight << ";" << weight*spreadPT_orig << ";" << spreadPT[realid] << std::endl;
                }
            }
            //std::cout << "final: "  << spreadPT[i] << std::endl;
        }
        //cout << i << ";" << cluster(i,key).Float("correctedPT") << ";" << spreadPT[i]<< endl;
    }

    /*float totalcorrpt=0, totalspreadpt=0;
      for(int i=0; i<clusters; i++){ totalcorrpt+=cluster(i,key).Float("correctedPT"); totalspreadpt+=spreadPT[i];}
      cout << totalcorrpt << ";" << totalspreadpt << endl; //should be the same*/

    for(int iCl = 0; iCl < clusters; iCl++) {
        correctedptvec[iCl].second[3] = spreadPT[iCl] * (spreadPT[iCl] > 0);
    }
}



//##########
// Softer Killer
// Finds the pT cut for this event based on the SK results
// The clustSK collection contains all clusters that aren't cut, so clusters below
// its min pT are cut
double EFMissingETFromTrackAndClusters::findMinPt(std::vector<fastjet::PseudoJet> *clustSK) const
{
    double minPt = 999999999999;

    for(unsigned int i=0; i < clustSK->size(); i++) {
        if( (*clustSK)[i].pt() < minPt) minPt = (*clustSK)[i].pt();
    }

    // There is a small rounding error which I account for this way
    return (minPt - 1e-12);
}

// Reweights clusters (when calo isn't split)
void EFMissingETFromTrackAndClusters::RunClusters(std::vector<xAOD::CaloCluster> clust)
{
    vector<fastjet::PseudoJet> clustPJ;

    for(xAOD::CaloCluster cl : clust) {
        if(cl.e() > 0) clustPJ.push_back( fastjet::PseudoJet( cl.p4() ));
    }

    fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
    fastjet::RectangularGrid SKgrid(-m_rapmax, m_rapmax, m_gridSpacing, m_gridSpacing, selector);
    fastjet::contrib::SoftKiller softkiller(SKgrid);
    std::vector<fastjet::PseudoJet> clustSK = softkiller(selector(clustPJ));

    m_minPt = findMinPt(&clustSK);
}

void EFMissingETFromTrackAndClusters::RunSplitClusters(std::vector<xAOD::CaloCluster> clust)
{
    vector<fastjet::PseudoJet> clustPJ_ECal;
    vector<fastjet::PseudoJet> clustPJ_HCal;

    for(xAOD::CaloCluster cl : clust) {
        double center_lambda;
        cl.retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda);
        if( center_lambda < m_lambdaCalDivide && cl.e() > 0) clustPJ_ECal.push_back( fastjet::PseudoJet( cl.p4() ));
        if( center_lambda >= m_lambdaCalDivide && cl.e() > 0) clustPJ_HCal.push_back( fastjet::PseudoJet( cl.p4() ));
    }

    fastjet::Selector selector = fastjet::SelectorAbsRapRange(m_rapmin, m_rapmax);
    fastjet::RectangularGrid SKgridECal(-m_rapmax, m_rapmax, m_eCalGrid, m_eCalGrid, selector);
    fastjet::contrib::SoftKiller softkillerECal(SKgridECal);
    std::vector<fastjet::PseudoJet> clustSK_ECal = softkillerECal(selector(clustPJ_ECal));
    m_minPtECal = findMinPt(&clustSK_ECal);

    fastjet::RectangularGrid SKgridHCal(-m_rapmax, m_rapmax, m_hCalGrid, m_hCalGrid, selector);
    fastjet::contrib::SoftKiller softkillerHCal(SKgridHCal);
    std::vector<fastjet::PseudoJet> clustSK_HCal = softkillerHCal(selector(clustPJ_HCal));
    m_minPtHCal = findMinPt(&clustSK_HCal);
}


int EFMissingETFromTrackAndClusters::calculateWeight(xAOD::CaloCluster cl) const
{
    // If the cluster pT is below the SoftKiller pT cut, rescale 4-momentum to 0
    if( abs(cl.eta()) < m_rapminApplied || abs(cl.eta()) > m_rapmaxApplied) return 1;
    if( cl.pt() < m_minPt) return 0;
    return 1;
}


int EFMissingETFromTrackAndClusters::calculateSplitWeight(xAOD::CaloCluster cl) const
{
    if( abs(cl.eta()) < m_rapminApplied || abs(cl.eta()) > m_rapmaxApplied) return 1;
    double center_lambda;

    if(!cl.retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda)) center_lambda = 0;

    //Make a separate pT cut for the ECal and HCal
    if( center_lambda < m_lambdaCalDivide && cl.pt() < m_minPtECal) return 0;
    if( cl.pt() < m_minPtHCal) return 0;
    return 1;
}





