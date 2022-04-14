/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMomentTools/JetVertexNNTagger.h"

#include <fstream>
#include <regex>
#include <map>

#include "AsgDataHandles/ReadHandle.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowCopy.h"
#include "PathResolver/PathResolver.h"

#include "AsgDataHandles/ReadDecorHandle.h"
#include "AsgDataHandles/WriteDecorHandle.h"

#include "nlohmann/json.hpp"
#include "lwtnn/generic/FastGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Stack.hh"

using xAOD::JetContainer;

namespace JetPileupTag {
    StatusCode JetVertexNNTagger::initialize()
    {

        ATH_MSG_DEBUG("Initializing...");

        if(m_jetContainerName.empty()){
            ATH_MSG_ERROR("JetVertexTaggerTool needs to have its input jet container configured!");
            return StatusCode::FAILURE;
        }

        // Use the Path Resolver to find the jvt file and retrieve the likelihood histogram
        std::string configPath = PathResolverFindCalibFile(m_NNConfigDir+"/"+m_NNParamFileName);
        ATH_MSG_INFO("  Reading JVT NN file from:\n    " << m_NNParamFileName << "\n");
        ATH_MSG_INFO("                     resolved in  :\n    " << configPath << "\n\n");

        std::ifstream fconfig( configPath.c_str() );
        if ( !fconfig.is_open() ) {
        ATH_MSG_ERROR( "Error opening config file: " << m_NNParamFileName );
        ATH_MSG_ERROR( "Are you sure that the file exists at this path?" );
        return StatusCode::FAILURE;
        }

        ATH_MSG_INFO("\n Reading JVT likelihood histogram from:\n    " << configPath << "\n\n");
        lwt::GraphConfig cfg = lwt::parse_json_graph( fconfig );
        // FastGraph is initialised with the order in which inputs will be
        // provided, to avoid map lookup
        lwt::InputOrder order;
        lwt::order_t node_order;
        std::vector<std::string> inputs = {"Rpt","JVFCorr","ptbin","etabin"};
        // Single input block
        node_order.emplace_back(cfg.inputs[0].name,inputs);
        order.scalar = node_order;
        ATH_MSG_DEBUG( "Network NLayers: " << cfg.layers.size() );

        // Primarily using double to take advantage of build_vector later
        m_lwnn = std::make_unique<lwt::generic::FastGraph<double> >(cfg,order);

        std::string cutsPath = PathResolverFindCalibFile(m_NNConfigDir+"/"+m_NNCutFileName);
        ATH_MSG_INFO("  Reading JVT NN cut file from:\n    " << m_NNCutFileName << "\n");
        ATH_MSG_INFO("                     resolved in  :\n    " << cutsPath << "\n\n");
        std::ifstream fcuts( cutsPath.c_str() );
        if ( !fcuts.is_open() ) {
            ATH_MSG_ERROR( "Error opening cuts file: " << m_NNCutFileName );
            ATH_MSG_ERROR( "Are you sure that the file exists at this path?" );
            return StatusCode::FAILURE;
        }
        nlohmann::json cut_j;
        fcuts >> cut_j;
        // Bin edges are read in GeV
        m_ptbin_edges = cut_j["ptbin_edges"].get<std::vector<float> >();
        m_etabin_edges = cut_j["etabin_edges"].get<std::vector<float> >();
        std::map<std::string,float> cut_map_raw = cut_j["cuts"].get<std::map<std::string,float> >();
        // Initialise 2D vector with cuts per bin
        // Edge vectors have size Nbins+1
        m_cut_map.resize(m_ptbin_edges.size()-1);
        for(std::vector<float>& cuts_vs_eta : m_cut_map) {
            cuts_vs_eta.resize(m_etabin_edges.size()-1,0.);
        }
        ATH_MSG_DEBUG("Extracting cut map");
        std::regex binre("\\((\\d+),\\s*(\\d+)\\)");
        for(const std::pair<const std::string,float>& bins_to_cut_str : cut_map_raw) {
            ATH_MSG_DEBUG( bins_to_cut_str.first << " --> " << bins_to_cut_str.second );
            std::smatch sm;
            if(std::regex_match(bins_to_cut_str.first,sm,binre) && sm.size()==3) {
                // First entry is full match, followed by sub-matches
                size_t ptbin = std::stoi(sm[1]);
                size_t etabin = std::stoi(sm[2]);
                m_cut_map[ptbin][etabin] = bins_to_cut_str.second;
            } else {
                ATH_MSG_ERROR( "Regex match of pt/eta bins failed! Received string " << bins_to_cut_str.first );
                ATH_MSG_ERROR( "Match size " << sm.size() );
                return StatusCode::FAILURE;
            }
        }

        m_jvfCorrKey = m_jetContainerName + "." + m_jvfCorrKey.key();
        m_sumPtTrkKey = m_jetContainerName + "." + m_sumPtTrkKey.key();
        m_jvtKey = m_jetContainerName + "." + m_jvtKey.key();
        m_rptKey = m_jetContainerName + "." + m_rptKey.key();
        m_passJvtKey = m_jetContainerName + "." + m_passJvtKey.key();

        ATH_CHECK(m_vertexContainer_key.initialize());
        ATH_CHECK(m_jvfCorrKey.initialize(m_suppressInputDeps));
        ATH_CHECK(m_sumPtTrkKey.initialize(m_suppressInputDeps));
        ATH_CHECK(m_jvtKey.initialize());
        ATH_CHECK(m_rptKey.initialize());
        ATH_CHECK(m_passJvtKey.initialize());

        return StatusCode::SUCCESS;
    }

    // Assign jet to kinematic bins in pt,eta
    // The network output depends on the bins, which can also be used
    // for varying cuts to achieve flatter efficiency/rejection profiles
    std::pair<size_t,size_t> JetVertexNNTagger::get_kinematic_bin(const xAOD::Jet& jet) const
    {
        // Initialise to an invalid value, check elsewhere that we are in range
        size_t ptbin{m_ptbin_edges.size()}, etabin{m_etabin_edges.size()};
        for(size_t iptbin{0}; iptbin<m_ptbin_edges.size()-1; ++iptbin) {
            if(m_ptbin_edges[iptbin]*GeV < jet.pt() && jet.pt() < m_ptbin_edges[iptbin+1]*GeV) {
                ptbin = iptbin;
                break;
            }
        }
        // For now this is a signed check
        // Reformulating as abs eta would require retraining
        for(size_t ietabin{0}; ietabin<m_etabin_edges.size()-1; ++ietabin) {
            if(m_etabin_edges[ietabin] < jet.eta() && jet.eta() < m_etabin_edges[ietabin+1]) {
                etabin = ietabin;
                break;
            }
        }

        return std::make_pair(ptbin,etabin);
    }

    float JetVertexNNTagger::evaluateJvt(float rpt, float jvfcorr, size_t ptbin, size_t etabin) const
    {

    lwt::VectorX<double> inputvals = lwt::build_vector({rpt,jvfcorr,static_cast<double>(ptbin),static_cast<double>(etabin)});
    std::vector<lwt::VectorX<double> > scalars{inputvals};
    lwt::VectorX<double> output = m_lwnn->compute(scalars);

    return output(0);

    }

    float JetVertexNNTagger::getJvtCut(size_t ptbin, size_t etabin) const
    {
        return m_cut_map[ptbin][etabin];
    }


    const xAOD::Vertex* JetVertexNNTagger::findHSVertex() const
    {
        // Get input vertex collection
        SG::ReadHandle<xAOD::VertexContainer> vertexHandle = SG::makeHandle (m_vertexContainer_key);
        if (!vertexHandle.isValid()){
            return nullptr;
            ATH_MSG_ERROR("Invalid VertexContainer datahandle: " << m_vertexContainer_key.key());
        }
        const xAOD::VertexContainer* vertices = vertexHandle.cptr();
        ATH_MSG_DEBUG("Successfully retrieved VertexContainer: " << m_vertexContainer_key.key());

        if (vertices->size() == 0 ) {
            ATH_MSG_WARNING("There are no vertices in the container. Exiting");
            return nullptr;
        }

        for ( size_t iVertex = 0; iVertex < vertices->size(); ++iVertex ) {
        if(vertices->at(iVertex)->vertexType() == xAOD::VxType::PriVtx) {

            ATH_MSG_VERBOSE("JetVertexTaggerTool " << name() << " Found HS vertex at index: "<< iVertex);
            return vertices->at(iVertex);
            }
        }
        ATH_MSG_VERBOSE("There is no vertex of type PriVx. Taking default vertex.");
        return vertices->at(0);
    }

    StatusCode JetVertexNNTagger::decorate(const xAOD::JetContainer& jetCont) const
    {
        const xAOD::Vertex* HSvertex = findHSVertex();
        if(HSvertex==nullptr) {
            ATH_MSG_WARNING("Invalid primary vertex found, will not continue decorating with JVT.");
            return StatusCode::FAILURE;
        }

        // Grab vertices for index bookkeeping
        SG::ReadHandle<xAOD::VertexContainer> vertexHandle = SG::makeHandle (m_vertexContainer_key);
        const xAOD::VertexContainer* vertices = vertexHandle.cptr();

        SG::ReadDecorHandle<xAOD::JetContainer, float> jvfCorrHandle(m_jvfCorrKey);
        SG::ReadDecorHandle<xAOD::JetContainer, std::vector<float> > sumPtTrkHandle(m_sumPtTrkKey);
        SG::WriteDecorHandle<xAOD::JetContainer, float> jvtHandle(m_jvtKey);
        SG::WriteDecorHandle<xAOD::JetContainer, float> rptHandle(m_rptKey);
        SG::WriteDecorHandle<xAOD::JetContainer, char>  passJvtHandle(m_passJvtKey);

        for(const xAOD::Jet* jet : jetCont) {
            // Calculate RpT and JVFCorr
            // Default JVFcorr to -1 when no tracks are associated.
            float jvfcorr = jvfCorrHandle(*jet);
            std::vector<float> sumpttrk = sumPtTrkHandle(*jet);
            float rpt = sumpttrk[HSvertex->index() - (*vertices)[0]->index()]/jet->pt();

            size_t ptbin, etabin;
            std::tie(ptbin,etabin) = get_kinematic_bin(*jet);
            ATH_MSG_VERBOSE("Jet with pt " << jet->pt() << ", eta " << jet->eta() );
            ATH_MSG_VERBOSE("  --> ptbin " << ptbin << ", etabin " << etabin);
            ATH_MSG_VERBOSE("  --> inputs: corrJVF " << jvfcorr << ", rpt " << rpt );

            // Default variable values, in case of invalid kinematic ranges
            float jvt = -1.;
            char jet_passjvt = false;
            // If desired, can combine with explicit max pt, abs(eta) values
            // Left off for now
            if(ptbin==m_ptbin_edges.size() || etabin==m_etabin_edges.size() || jet->pt()>m_maxpt_for_cut) {
                ATH_MSG_VERBOSE("Jet outside kinematic ranges, setting default values and automatic pass.");
                jet_passjvt = true;
            } else {
                jvt = evaluateJvt(rpt, jvfcorr, ptbin, etabin);
                float jvtcut = getJvtCut(ptbin, etabin);
                ATH_MSG_VERBOSE("JVT cut for ptbin " << ptbin << ", etabin " << etabin << " = " << jvtcut);
                jet_passjvt = jvt > jvtcut;
                ATH_MSG_VERBOSE("Evaluated JVT = " << jvt << ", jet " << (jet_passjvt ? "passes" :"fails") << " working point" );
            }

            // Decorate jet
            rptHandle(*jet) = rpt;
            jvtHandle(*jet) = jvt;
            passJvtHandle(*jet) = jet_passjvt;
        }

        return StatusCode::SUCCESS;
    }
}