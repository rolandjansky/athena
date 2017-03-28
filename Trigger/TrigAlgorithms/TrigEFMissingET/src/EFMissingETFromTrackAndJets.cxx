/********************************************************************

NAME:     EFMissingETFromTrackAndJets.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Renjie Wang (renjie.wang@cern.ch)
CREATED:  Nov 18, 2016

// using tracks as soft-term of MET instead of low pt jets in mht algorith
// JVT is also implemented to veto pileup jets in central region
 ********************************************************************/
#include "TrigEFMissingET/EFMissingETFromTrackAndJets.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "CxxUtils/sincosf.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
//#include "FourMomUtils/P4DescendingSorters.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/Jet.h"

#include "EventKernel/ISignalState.h"
#include "EventKernel/SignalStateHelper.h"

#include <cmath>
#include <string>
using namespace std;

EFMissingETFromTrackAndJets::EFMissingETFromTrackAndJets(const std::string& type,
        const std::string& name,
        const IInterface* parent) :
    EFMissingETBaseTool(type, name, parent),
    trackselTool("InDet::InDetTrackSelectionTool/TrackSelectionTool", this ),
    muontrackselTool("InDet::InDetTrackSelectionTool/MuonTrackSelectionTool", this )
{
    declareProperty("EtaSeparation", m_etacut = 2.2 ,"Cut to split into forward and central jets -- needs to be positive");
    declareProperty("CentralpTCut", m_central_ptcut = 0.0 ,"pT Cut for central jets");
    declareProperty("ForwardpTCut", m_forward_ptcut = 0.0 ,"pT Cut for forward jets");
    declareProperty("TrackpTCut", m_track_ptcut = 0.0 ,"pT Cut for online tracks");
    declareProperty("CentralJetJVTCut", m_central_jvtcut = 0.9 ,"Jet JVT Cut for central jets");
    declareProperty("TrackSelectionTool", trackselTool );
    declareProperty("MuonTrackSelectionTool", muontrackselTool );

    m_fextype = FexType::JET;
    m_etacut = fabs(m_etacut);

    m_methelperposition = 8;
}


EFMissingETFromTrackAndJets::~EFMissingETFromTrackAndJets()
{
}


StatusCode EFMissingETFromTrackAndJets::initialize()
{
    if(msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "called EFMissingETFromTrackAndJets::initialize()" << endmsg;

    /// timers
    if( service( "TrigTimerSvc", m_timersvc).isFailure() )
        msg(MSG::WARNING) << name() << ": Unable to locate TrigTimer Service" << endmsg;

    if (m_timersvc) {
        // global time
        std::string basename=name()+".TotalTime";
        m_glob_timer = m_timersvc->addItem(basename);
    } // if timing service

    // JVT likelihood histogram
    TString jvtFile = "JVTlikelihood_20140805.root";
    TString jvtName = "JVTRootCore_kNN100trim_pt20to50_Likelihood";
    m_jvtLikelihood = (TH2F *) getHistogramFromFile(jvtName, jvtFile);
    if (m_jvtLikelihood == nullptr) {
        msg(MSG::ERROR) << "Failed to retrieve JVT likelihood file, exiting." << endmsg;
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}


StatusCode EFMissingETFromTrackAndJets::execute()
{

    msg(MSG::DEBUG) << name() << ": Executing Jet algorithm for ETMiss" << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromTrackAndJets::finalize()
{
    if(msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "called EFMissingETFromTrackAndJets::finalize()" << endmsg;


    return StatusCode::SUCCESS;
}

StatusCode EFMissingETFromTrackAndJets::execute(xAOD::TrigMissingET *,
        TrigEFMissingEtHelper *metHelper,
        const xAOD::CaloClusterContainer * /* caloCluster */,
        const xAOD::JetContainer *MHTJetContainer,
        const xAOD::TrackParticleContainer *trackContainer,
        const xAOD::VertexContainer *vertexContainer)
{

    if(msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "called EFMissingETFromTrackAndJets::execute()" << endmsg; // EFMissingET_Fex_Jets

    if(m_timersvc)
        m_glob_timer->start(); // total time

    if(msgLvl(MSG::DEBUG))
        msg(MSG::DEBUG) << "started MET jet CPU timer" << endmsg;

    TrigEFMissingEtComponent* metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Jet component

    if (metComp==0) {
        msg(MSG::ERROR) << "cannot fetch Topo. cluster component!" << endmsg;
        return StatusCode::FAILURE;
    }
    if(string(metComp->m_name).substr(0,3)!="JET") {
        msg(MSG::ERROR) << "fetched " << metComp->m_name << " instead of the Jet component!" << endmsg;
        return StatusCode::FAILURE;
    }


    std::vector<const xAOD::Jet*> MHTJetsVec(MHTJetContainer->begin(), MHTJetContainer->end());
    msg(MSG::DEBUG) << "num of jets: " << MHTJetsVec.size() << endmsg;

    std::vector<const xAOD::TrackParticle*> TrackVec(trackContainer->begin(), trackContainer->end());
    msg(MSG::DEBUG) << "num of tracks: " << TrackVec.size() << endmsg;

    std::vector<const xAOD::Vertex*> VertexVec(vertexContainer->begin(), vertexContainer->end());
    msg(MSG::DEBUG) << "num of vertices: " << VertexVec.size() << endmsg;



    //#################################################################
    //bool hasGoodVtx = false;
    const xAOD::Vertex* primaryVertex =  nullptr;
    for (const xAOD::Vertex* vertex : VertexVec) {

        msg(MSG::DEBUG) << "\tx: " << vertex->x() << "\ty: " << vertex->y() << "\tz: " << vertex->z()
                  << "\tntracks: " <<  vertex->nTrackParticles()
                  << "\ttype: " << vertex->vertexType()
                  << "\txAOD::VxType::PriVtx: " << xAOD::VxType::PriVtx
                  << endmsg;

        // ntracks is not working now in rel21
        // if ( vertex->nTrackParticles() < 2 ) continue;
        if ( vertex->vertexType() == xAOD::VxType::PriVtx ) {
            //hasGoodVtx = true;
            primaryVertex = vertex;
            break;
        }

    }


    std::vector<const xAOD::Jet*> goodJets;
    goodJets = MHTJetsVec;
    std::vector<const xAOD::TrackParticle*> tracksSeparatedWithJets;


    if(primaryVertex!=nullptr) {

        int n_putracks = 0;
        for (const xAOD::TrackParticle* itrk : TrackVec) {
            bool isfromPV =  ((!itrk->vertex() && fabs((itrk->z0()+itrk->vz()-primaryVertex->z())*sin(itrk->theta()))<3.) || itrk->vertex()==primaryVertex);
            if(!isfromPV && itrk->pt()<30e3 && trackselTool->accept(*itrk,primaryVertex)) n_putracks++;
        } // end for loop over tracks
        if (!n_putracks) n_putracks++;



        goodJets.clear();
        for (const xAOD::Jet* jet : MHTJetsVec) {

            double ptsum_all = 0;
            double ptsum_pv = 0;
            double ptsum_pileup = 0;

            for (const xAOD::TrackParticle* itrk : TrackVec) {

                float deltaR_trackj = jet->p4().DeltaR(itrk->p4());
                if(deltaR_trackj>0.4) continue;

                bool accept = (itrk->pt()>500 && trackselTool->accept(*itrk, primaryVertex));
                if (accept) ptsum_all += itrk->pt();
                if (accept && ((!itrk->vertex() && fabs((itrk->z0()+itrk->vz()-primaryVertex->z())*sin(itrk->theta()))<3.) || itrk->vertex()==primaryVertex)) ptsum_pv += itrk->pt();
                if (accept && !(!itrk->vertex() && fabs((itrk->z0()+itrk->vz()-primaryVertex->z())*sin(itrk->theta()))<3.)) ptsum_pileup += itrk->pt();
            }
            //double JVF = ptsum_all>0 ? ptsum_pv/ptsum_all : -1;
            double Rpt = ptsum_pv/jet->pt();
            double corrJVF = ptsum_pv+ptsum_pileup>0 ? ptsum_pv/(ptsum_pv+100*ptsum_pileup/n_putracks) : -1;
            double JVT = corrJVF>=0 ? m_jvtLikelihood->Interpolate(corrJVF,std::min(Rpt,1.0)) : -0.1;

            if(jet->pt()<50e3 && jet->pt()>20e3 && fabs(jet->eta())<2.4 && JVT<m_central_jvtcut) continue;
            goodJets.push_back(jet);

        }




        for (const xAOD::TrackParticle* track : TrackVec) {

            //checking the track coming from PV
            bool isfromPV =  ((!track->vertex() && fabs((track->z0()+track->vz()-primaryVertex->z())*sin(track->theta()))<3.) || track->vertex()==primaryVertex);
            if(!isfromPV) continue;
            if(fabs(track->eta())>2.4 || track->pt()/1000. < m_track_ptcut) continue;
            if(!trackselTool->accept(*track,primaryVertex)) continue;
            if(muontrackselTool->accept(*track,primaryVertex)) continue;

            msg(MSG::DEBUG) << "\ttrack pt: " << track->pt()/1000. << "\teta: " << track->eta() << "\tphi: " << track->phi()
                      << "\tvertex: " << track->vertex()
                      << "\tz0: " << track->z0()
                      << "\tvz: " << track->vz()
                      << "\ttheta: " << track->theta()
                      << "\tdZ: " << fabs((track->z0()+track->vz()-primaryVertex->z())*sin(track->theta()))
                      << "\tisfromPV: " << isfromPV //(track->vertex()==primaryVertex)
                      << endmsg;

            bool findWithinjets(false);
            for (const xAOD::Jet* jet : goodJets) {
                if( fabs(jet->eta())<2.4 && jet->pt()/1000. < m_central_ptcut) continue;
                float deltaR_trackj = track->p4().DeltaR(jet->p4());
                if(deltaR_trackj<0.4) {
                    findWithinjets = true;
                    break;
                }
            }

            if(!findWithinjets) {
                tracksSeparatedWithJets.push_back(track);
            }

        }

    }
    //##################################################################

    //--- fetching the topo. cluster component
    float upperlim[4] = {m_etacut,0,5,-m_etacut};
    float lowerlim[4] = {0,-m_etacut,m_etacut,-5};

    for(int i = 0; i < 5; i++) {

        metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition + i); // fetch Cluster component

        if(i ==0) {
            for (const xAOD::TrackParticle* track : tracksSeparatedWithJets) {
                metComp->m_ex -= track->p4().Px();
                metComp->m_ey -= track->p4().Py();
                metComp->m_ez -= track->p4().Pz();
                metComp->m_sumEt += track->pt();
                metComp->m_sumE  += track->e();
            }
        }

        for (const xAOD::Jet* aJet : goodJets) {

            if(i == 0) {

                if( fabs(aJet->eta())<2.4 && aJet->pt()/1000 < m_central_ptcut ) continue;

                metComp->m_ex -= aJet->px();
                metComp->m_ey -= aJet->py();
                metComp->m_ez -= aJet->pz();
                metComp->m_sumEt += aJet->pt();
                metComp->m_sumE  += aJet->e();
                metComp->m_usedChannels += 1;
                metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,aJet->pt())+0.5));



            } else if (i > 0) {

                float eta = aJet->eta();
                float ptCut = 0.;

                // Set pT cut depending on region
                if(i == 1 || i == 2) ptCut = m_central_ptcut;
                else ptCut = m_forward_ptcut;

                // only sum jets that have a pt above the cut value
                if(aJet->pt() < ptCut) continue;

                if( eta >= lowerlim[i-1] && eta <= upperlim[i-1]) {
                    metComp->m_ex -= aJet->px();
                    metComp->m_ey -= aJet->py();
                    metComp->m_ez -= aJet->pz();
                    metComp->m_sumEt += aJet->pt();
                    metComp->m_sumE  += aJet->e();
                    metComp->m_usedChannels += 1;
                    metComp->m_sumOfSigns += static_cast<short int>(floor(copysign(1.0,aJet->pt())+0.5));
                }

            }

        } // End loop over all jets

        // move from "processing" to "processed" state
        metComp->m_status ^= m_maskProcessing; // switch off bit
        metComp->m_status |= m_maskProcessed;  // switch on bit

    }

    metComp = metHelper->GetComponent(metHelper->GetElements() - m_methelperposition); // fetch Cluster component

    msg(MSG::DEBUG) << " calculated MET: " << sqrt((metComp->m_ex)*(metComp->m_ex)+(metComp->m_ey)*(metComp->m_ey)) << endmsg;


    if(m_timersvc)
        m_glob_timer->stop(); // total time

    return StatusCode::SUCCESS;
}

TH1* EFMissingETFromTrackAndJets::getHistogramFromFile(TString hname, TString fname)
{
    fname = PathResolverFindCalibFile(fname.Data());
    TFile *file = TFile::Open(fname.Data(), "READ");

    if (file == nullptr) {
        msg(MSG::DEBUG) << "getHistogramFromFile() : Couldn't open file "
                        << fname.Data() << ", returning nullptr." << endmsg;
        return nullptr;
    }

    TH1 *temp = (TH1*)file->Get(hname.Data());

    if (temp == nullptr) {
        msg(MSG::DEBUG) << "getHistogramFromFile() : Couldn't find histogram "
                        << hname.Data() << " in file "
                        << fname.Data() << ", returning nullptr." << endmsg;
        return nullptr;
    }

    bool status = TH1::AddDirectoryStatus();
    TH1::AddDirectory(false);
    hname = "cloned_" + hname;
    TH1 *hist = (TH1*)temp->Clone(hname.Data());
    SafeDelete(file);
    TH1::AddDirectory(status);

    return hist;
}
