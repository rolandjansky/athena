/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MVAUtils/BDT.h"
#include "MVAUtils/NodeImpl.h"

#include "TFile.h"
#include "TTree.h"

#include <vector>
#include <random>
#include <chrono>
#include <iostream>


TTree* get_tree(const std::string& filename)
{
    TFile* f = TFile::Open(filename.c_str());
    auto *keys = f->GetListOfKeys();
    TTree* tree = nullptr;
    for (int ikey=0; ikey != keys->GetSize(); ++ikey)
    {
        TObject* obj = f->Get(keys->At(ikey)->GetName());
        if (std::string(obj->ClassName()) == "TTree") {
            f->GetObject(obj->GetName(), tree);
        }
    }

    if (!tree) {
        std::cout << "cannot find any ttree in file " << filename << std::endl;
    }
    return tree;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "need to provide a ROOT file with a TTree" << std::endl;
        return 1;
    }

    TTree* tree = get_tree(argv[1]);
    if (!tree) { return 1; }

    MVAUtils::BDT bdt(tree);

    std::cout << "sizeof class NodeTMVA is " << sizeof(MVAUtils::NodeTMVA) << std::endl;
    std::cout << "sizeof class NodeLGBMSimple is " << sizeof(MVAUtils::NodeLGBMSimple) << std::endl;
    std::cout << "sizeof class NodeLGBM is " << sizeof(MVAUtils::NodeLGBM) << std::endl;
    std::cout << "sizeof class NodeXGBoost is " << sizeof(MVAUtils::NodeXGBoost) << std::endl;

    if (sizeof(MVAUtils::NodeTMVA) != 8) { std::cout << "WARNING: NodeTMVA should be 8 bytes" << std::endl; }
    if (sizeof(MVAUtils::NodeLGBMSimple) != 8) { std::cout << "WARNING: NodeLGBMSimple should be 8 bytes" << std::endl; }
    if (sizeof(MVAUtils::NodeLGBM) != 8) { std::cout << "WARNING: NodeLGBM should be 8 bytes" << std::endl; }
    if (sizeof(MVAUtils::NodeXGBoost) != 8) { std::cout << "WARNING: NodeXGBoost should be 8 bytes" << std::endl; }

    std::default_random_engine gen;
    std::uniform_real_distribution<float> rnd_uniform(-100., 100.);

    const unsigned int NTEST = 10000;

    auto t1 = std::chrono::high_resolution_clock::now();

    int nvars = bdt.GetNVars();
    std::vector<float> rnd_precomputed;

    // precompute since we don't want to impact timing
    for (unsigned int itest = 0; itest != NTEST * nvars; ++itest)
    {
        rnd_precomputed.push_back(rnd_uniform(gen));
    }

    auto rnd_it1 = rnd_precomputed.begin();
    auto rnd_it2 = rnd_it1;
    std::advance(rnd_it2, nvars);
    for (unsigned int itest = 0; itest != NTEST; ++itest)
    {
        std::vector<float> input_values(rnd_it1, rnd_it2);
        bdt.GetResponse(input_values);
        std::advance(rnd_it1, nvars);
        std::advance(rnd_it2, nvars);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "timing: "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count() / double(NTEST) / double(bdt.GetNTrees())
              << " ns / events / trees\n";


    return 0;

}
