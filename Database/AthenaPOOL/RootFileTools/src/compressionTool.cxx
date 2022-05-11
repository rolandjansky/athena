/*
 *   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

// Use CloneTree(0) to get the structure of the tree, then copy events one event at a time from old to new tree 
// Successfully changes compression algorithm, compression level, and auto flush 
// Output basket (when controlling for other variables) is slightly smaller than original
//
// Macro Options:
// comp_alg_input: String denoting desired compression algorithm
//  Options: -1, 1, 2, 4
//  if -1 is given, new file is compressed with the same comp alg as the old file
//
// comp_level:
//  Options: -1, 0-9 
//  if -1 is given, and comp_alg=-1, then comp_level will be copied from the input file
//  else it will be set to the default comp_levl for that alg
//
//  auto_flush:
//  Options: -1, any int
//  if -1 is given, auto_flush for the CollectionTree will be set to the auto flush of the input CollectionTree

#include "TFile.h"
#include "TTree.h"
#include "TKey.h" 

#include <iostream>

using namespace std;

void compression_tool(const char* input_filename, const char* output_filename, int comp_alg=-1, int comp_level=-1, int auto_flush=-1) {
    
    // Create output file, open input file
    TFile* output_file = new TFile(output_filename, "recreate");
    TFile* input_file = TFile::Open(input_filename);

    // Set comp_alg, comp_level, auto_flush if any were left -1
    // comp_alg gets set to whatever the input file's algorithm is
    // comp_level gets set to the default compression level for the given algorithm is
    // unless comp_alg == -1, in which case it gets set to the input file's comp level
    // auto_flush gets set to the auto flush value of the input file's CollectionTree
    if (comp_alg == -1) {
        comp_alg = input_file->GetCompressionAlgorithm();
        if (comp_level == -1) {
            comp_level = input_file->GetCompressionLevel();
        }
    }

    if (comp_level == -1) {
        if (comp_alg == ROOT::kZLIB) comp_level = ROOT::RCompressionSetting::ELevel::kDefaultZLIB;
        else if (comp_alg == ROOT::kLZMA) comp_level = ROOT::RCompressionSetting::ELevel::kDefaultLZMA;
        else if (comp_alg == ROOT::kLZ4) comp_level = ROOT::RCompressionSetting::ELevel::kDefaultLZ4;
        else comp_level = ROOT::RCompressionSetting::ELevel::kUseMin;
    }

    if (auto_flush == -1) {
        auto_flush = (static_cast<TTree*>(input_file->Get("CollectionTree")))->GetAutoFlush();
    }

    // Set comp_alg and comp_level in the output file
    output_file->SetCompressionAlgorithm(comp_alg);
    output_file->SetCompressionLevel(comp_level);

    // Copy all the TTrees of the input file to the output file
    // Set the auto flush of the CollectionTree to the variable auto_flush
    TIter nextTree(input_file->GetListOfKeys());
    for ( TKey* tree_key = static_cast<TKey*>(nextTree()); tree_key; tree_key = static_cast<TKey*>(nextTree()) ) {
        if (strcmp(tree_key->GetClassName(), "TTree")) continue;
        input_file->cd();
        TTree* input_tree = static_cast<TTree*>(input_file->Get(tree_key->GetName()));
        output_file->cd();
        TTree* output_tree = static_cast<TTree*>(output_file->Get(tree_key->GetName()));
        output_tree = input_tree->CloneTree(0);
        
        // Set auto flush in the new CollectionTree
        if (!strcmp(tree_key->GetName(), "CollectionTree")) output_tree->SetAutoFlush(auto_flush);

        // Copy addresses to output_tree
        input_tree->CopyAddresses(output_tree);

        // iterate over all the entries of the input tree, copying them to the output tree
        int nentries = input_tree->GetEntriesFast();
        for (int i=0; i < nentries; i++) {
            input_tree->GetEntry(i);
            output_tree->Fill();
        }    
    }
    
    // write and close output file
    output_file->Write();
    output_file->Close();
}

int usage() {
    cout << "Tool to change compression settings of a ROOT file.\n"
            "Usage: \n"
            "-i <input filename>\n"
            "-o <output filename>\n"
            "-c <compression algorithm>   1==ZLIB, 2==LZMA, 4==LZ4, -1==Match input file\n"
            "-l <compression level>       Possible values: 1-9, -1==Match input file or default\n"
            "-a <auto flush>              Possible values: any int, -1==Match input file\n"
            "-h                           Return usage details\n";
    return 1;
}

int main(int argc, char** argv) {
    const char* input_filename = "";
    const char* output_filename = "";
    int comp_alg = -1;
    int comp_level = -1;
    int auto_flush = -1;

    // Read input arguments
    for (int i=1; i<argc; ++i) {
        if (*argv[i] == '-') {
            switch(::toupper(*(argv[i]+1))) {
                case 'I':
                    if (i+1<argc) input_filename = argv[i+1];
                    ++i;
                    break;
                case 'O':
                    if (i+1<argc) output_filename = argv[i+1];
                    ++i;
                    break;
                case 'C':
                    if (i+1<argc) comp_alg = atoi(argv[i+1]);
                    ++i;
                    break;
                case 'L':
                    if (i+1<argc) comp_level = atoi(argv[i+1]);
                    ++i;
                    break;
                case 'A':
                    if (i+1<argc) auto_flush = atoi(argv[i+1]);
                    ++i;
                    break;
                case 'H':
                    return usage();
                default:
                    return usage();
            }
        }
    }

    // Verify valid input arguments
    if (input_filename == nullptr) {
        cout << "No input filename received" << endl;
        return usage();
    }
    if (output_filename == nullptr) {
        cout << "No output filename received" << endl;
        return usage();
    }

    compression_tool(input_filename, output_filename, comp_alg, comp_level, auto_flush);
 
    return 0;
}
