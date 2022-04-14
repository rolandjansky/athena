#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "nlohmann/json.hpp"
#include "lwtnn/generic/FastGraph.hh"
#include "lwtnn/parse_json.hh"
#include "lwtnn/Stack.hh"
#include "PathResolver/PathResolver.h"

int main() {

    // Use the Path Resolver to find the jvt file and retrieve the likelihood histogram
    std::string NNConfigDir = "JetPileupTag/NNJvt/2022-03-22/";
    std::string NNParamFileName = "NNJVT.Network.graph.Offline.Nonprompt_All_MaxWeight.json";
    std::string NNCutFileName = "NNJVT.Cuts.FixedEffPt.Offline.Nonprompt_All_MaxW.json";

    std::string configPath = PathResolverFindCalibFile(NNConfigDir+"/"+NNParamFileName);
    std::cout << "Reading JVT NN file from:\n    " << NNConfigDir << "/" << NNParamFileName << std::endl;
    std::cout << "  resolved in  :\n    " << configPath  << std::endl;

    std::ifstream fconfig( configPath.c_str() );

    if ( !fconfig.is_open() ) {
        std::cerr <<  "Error opening config file: "  << NNConfigDir << "/" << NNParamFileName << std::endl;
        std::cerr <<  "Are you sure that the file exists at this path?" << std::endl;
    return 1;
    }

    std::string cutsPath = PathResolverFindCalibFile(NNConfigDir+"/"+NNCutFileName);
    std::cout << "  Reading JVT NN cut file from:\n    "  << NNConfigDir << "/" << NNCutFileName << std::endl;
    std::cout << "                     resolved in  :\n    " << cutsPath << std::endl;
    std::ifstream fcuts( cutsPath.c_str() );
    if ( !fconfig.is_open() ) {
        std::cerr <<  "Error opening cuts file: "  << NNConfigDir << "/" << NNCutFileName << std::endl;
        std::cerr <<  "Are you sure that the file exists at this path?" << std::endl;
    return 2;
    }

    nlohmann::json cut_j;
    fcuts >> cut_j;
    std::vector<float> ptbin_edges = cut_j["ptbin_edges"].get<std::vector<float> >();
    std::vector<float> etabin_edges = cut_j["etabin_edges"].get<std::vector<float> >();
    std::map<std::string,float> cut_map_raw = cut_j["cuts"].get<std::map<std::string,float> >();
    // Initialise 2D vector with cuts per bin
    // Edge vectors have size Nbins+1
    std::vector<std::vector<float> > cut_map(ptbin_edges.size()-1);
    for(std::vector<float>& cuts_vs_eta : cut_map) {
        cuts_vs_eta.resize(etabin_edges.size()-1,0.);
    }
    std::regex binre("\\((\\d+),\\s*(\\d+)\\)");
    for(const std::pair<const std::string,float>& bins_to_cut_str : cut_map_raw) {
        std::cout << bins_to_cut_str.first << " --> " << bins_to_cut_str.second << std::endl;
        std::smatch sm;
        if(std::regex_match(bins_to_cut_str.first,sm,binre) && sm.size()==3) {
            // First entry is full match, followed by sub-matches
            size_t ptbin = std::stoi(sm[1]);
            size_t etabin = std::stoi(sm[2]);
            cut_map[ptbin][etabin] = bins_to_cut_str.second;
        } else {
            std::cerr << "Regex match of pt/eta bins failed! Received string " << bins_to_cut_str.first << std::endl;
            std::cerr << "Match size " << sm.size() << std::endl;
            return 3;
        }
    }

    lwt::GraphConfig cfg = lwt::parse_json_graph( fconfig );
    lwt::InputOrder order;
    lwt::order_t node_order;
    std::vector<std::string> inputs = {"Rpt","JVFCorr","ptbin","etabin"};
    // Single input block
    node_order.emplace_back(cfg.inputs[0].name,inputs);
    order.scalar = node_order;

    std::cout << "Reading JVT likelihood histogram from: " << configPath << std::endl;
    std::cout << "Network NLayers: " << cfg.layers.size() << std::endl;
    lwt::generic::FastGraph<double> lwnn(cfg, order);

    std::cout << "Computation for test values:" << std::endl;
    // A few jet test cases
    for(size_t ptbin=0; ptbin<5; ++ptbin) {
        for(size_t etabin=0; etabin<5; ++etabin) {
            std::cout << "  pt bin[" << ptbin_edges[ptbin] << "," << ptbin_edges[ptbin+1] << "]: " << ptbin << ", eta bin [" << etabin_edges[etabin] << "," << etabin_edges[etabin+1] << "]: " << etabin <<std::endl;
            lwt::VectorX<double> inputvals_HS = lwt::build_vector({1.0,1.0,static_cast<double>(ptbin),static_cast<double>(etabin)});
            std::vector<lwt::VectorX<double> > scalars_HS{inputvals_HS};
            lwt::VectorX<double> output_HS = lwnn.compute(scalars_HS);
            std::cout << "    HS jet --> " << output_HS(0) << std::endl;

            lwt::VectorX<double> inputvals_AMB = lwt::build_vector({0.2,0.5,static_cast<double>(ptbin),static_cast<double>(etabin)});
            std::vector<lwt::VectorX<double> > scalars_AMB{inputvals_AMB};
            lwt::VectorX<double> output_AMB = lwnn.compute(scalars_AMB);
            std::cout << "    Ambiguous jet --> " << output_AMB(0) << std::endl;

            lwt::VectorX<double> inputvals_PU = lwt::build_vector({0.0,-1.0,static_cast<double>(ptbin),static_cast<double>(etabin)});
            std::vector<lwt::VectorX<double> > scalars_PU{inputvals_PU};
            lwt::VectorX<double> output_PU = lwnn.compute(scalars_PU);
            std::cout << "    PU jet --> " << output_PU(0) << std::endl;

            std::cout << "  Cut for this bin: " << cut_map[ptbin][etabin] << std::endl;
        }
    }

    return 0;
}