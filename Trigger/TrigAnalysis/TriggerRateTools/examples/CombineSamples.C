/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// Author: Marc-Andre Dufour
// Date: 08 May 2007
//
// Description: Script that will combine step3 output root files from multiple
//              samples into a single one
////////////////////////////////////////////////////////////////////////////////
{
    gROOT->Reset();
    gROOT->SetStyle("Plain");

    gSystem->Load("root_classes/LinkedArrayD_cxx.so");
    gSystem->Load("root_classes/LinkedArrayS_cxx.so");
    gSystem->Load("root_classes/Signature_cxx.so");
    gSystem->Load("root_classes/CplxSignature_cxx.so");

    Int_t temp_index1 = 0;
    Int_t temp_index2 = 0;
    Int_t total_N_evts = 300000;
    Int_t temp_int = 0;
    Double_t temp_double1 = 0.0;
    Double_t temp_double2 = 0.0;
    Double_t temp_double3 = 0.0;
    
    TChain* s_sigs  = new TChain("SingleSignatures");
    TChain* c_sigs  = new TChain("CplxSignatures");
    TChain* l_sigs  = new TChain("Levels");
    TChain* streams = new TChain("Streams");

    gROOT->ProcessLine(".! rm -f TriggerRates.root");
    
    s_sigs->Add("*.root");
    c_sigs->Add("*.root");
    l_sigs->Add("*.root");
    streams->Add("*.root");
    Long64_t s_sigs_Nentries = s_sigs->GetEntries();
    Long64_t c_sigs_Nentries = c_sigs->GetEntries();
    Long64_t l_sigs_Nentries = l_sigs->GetEntries();
    Long64_t streams_Nentries = streams->GetEntries();
    Long64_t Nentries = 0;
    if ((s_sigs_Nentries == c_sigs_Nentries) && (s_sigs_Nentries == l_sigs_Nentries) && (s_sigs_Nentries == streams_Nentries)) {
            Nentries = s_sigs_Nentries;
    } else {
            cout << "	[!!] Signature containers are not consistent... [!!]" << endl;
    }
    
    if (Nentries == 0) {
        cout << "[!!] No threshold set was found, were there any files in this directory? [!!]" << endl;
        gROOT->ProcessLine(".q");
    }
    
    // BEGIN Assign branch addresses to input collections
    
    Int_t threshold_set_s;
    s_sigs->SetBranchAddress("threshold_set_s" , &threshold_set_s);
    
    TObjArray *s_sigs_branches=s_sigs->GetListOfBranches();
    const Int_t num_sigs = s_sigs_branches->GetEntries()-1;

    if (num_sigs < 1) {
        cout << "[!!] No single signatures found [!!]"
        gROOT->ProcessLine(".q");
    }
    
    TString s_sig_names[num_sigs];
    for(Int_t i=0; i<num_sigs; i++) {
        s_sig_names[i]=NULL;
        s_sig_names[i]=*(((Signature)s_sigs_branches.At(i+1))->getName());
    }

    Signature* sigs[num_sigs];
    for (Int_t sig_i = 0; sig_i < num_sigs; sig_i++) {
        sigs[sig_i] = NULL;
        s_sigs->SetBranchAddress((s_sig_names[sig_i]) , &sigs[sig_i]);
    }
    

    TObjArray *c_sigs_branches=c_sigs->GetListOfBranches();
    const Int_t num_csigs = c_sigs_branches->GetEntries()-1;
    TString c_sig_names[num_csigs];
    for(Int_t i=0; i<num_csigs; i++) {
        c_sig_names[i]=NULL;
        c_sig_names[i]=*(((CplxSignature)c_sigs_branches.At(i+1))->getName());
    }

    CplxSignature* sigs_c[num_csigs];
    for (Int_t sig_i = 0; sig_i < num_csigs; sig_i++) {
        sigs_c[sig_i] = NULL;
        c_sigs->SetBranchAddress((c_sig_names[sig_i]) , &sigs_c[sig_i]);
    }

    
    TObjArray *streams_branches=streams->GetListOfBranches();
    const Int_t num_streams = streams_branches->GetEntries()-1;
    TString stream_names[num_streams];
    for(Int_t i=0; i<num_streams; i++) {
        stream_names[i]=NULL;
        stream_names[i]=*(((CplxSignature)streams_branches.At(i+1))->getName());
    }

    CplxSignature* sigs_streams[num_csigs];
    for (Int_t sig_i = 0; sig_i < num_streams; sig_i++) {
        sigs_streams[sig_i] = NULL;
        streams->SetBranchAddress((stream_names[sig_i]) , &sigs_streams[sig_i]);
    }


    TObjArray *level_branches=l_sigs->GetListOfBranches();
    const Int_t num_levels = level_branches->GetEntries()-1;
    TString level_names[num_levels];
    for(Int_t i=0; i<num_levels; i++) {
        level_names[i]=NULL;
        level_names[i]=*(((CplxSignature)level_branches.At(i+1))->getName());
    }

    CplxSignature* levels[num_levels];
    for (Int_t sig_i = 0; sig_i < num_levels; sig_i++) {
        levels[sig_i] = NULL;
        l_sigs->SetBranchAddress((level_names[sig_i]) , &levels[sig_i]);
    }

    // END Assign branch addresses to input collections
    // BEGIN Create output TTree
 
    /* Count the number of Samples / Thresholds */
    Int_t num_samples = 1;
    s_sigs->GetEntry(0);
    TString curr_sample = ((sigs[0])->getSampleAt(0))->Copy();

    Double_t max_xsec = 0.0;
    Double_t num_evts = 0.0;
    
    for (Int_t entry = 0; (entry < Nentries) && (total_N_evts > 0); entry++) {

        s_sigs->GetEntry(entry);
        //cout << entry << " : " << *((sigs[0])->getSampleAt(0)) << "\t" << curr_sample << endl;
        if (*((sigs[0])->getSampleAt(0)) != curr_sample) {
            curr_sample = ((sigs[0])->getSampleAt(0))->Copy();
            num_samples++;
            if ( (sigs[0])->getXsec() > max_xsec ) {
                max_xsec = (sigs[0])->getXsec();
            }
            num_evts = num_evts + (((sigs[0])->getNumEvts())*((sigs[0])->getXsec()));
        }
    }
    
    Int_t num_thresholds = Nentries/num_samples;
    total_N_evts = (Int_t)(num_evts/max_xsec);
    

    if (num_thresholds*num_samples != Nentries) {
        cout << "[!!] Incompatible number of thresholds per file [!!]" << endl;
        gROOT->ProcessLine(".q");
    }

    cout << "Number of samples: " << num_samples << endl;
    cout << "Number of thresholds: " << num_thresholds << endl;
   
    
    Signature sigs_new[num_sigs];
    CplxSignature sigs_c_new[num_csigs];
    CplxSignature sigs_streams_new[num_csigs];
    CplxSignature levels_new[num_levels];

    s_sigs->GetEntry(0);
    c_sigs->GetEntry(0);
    l_sigs->GetEntry(0);
    streams->GetEntry(0);
    
    for (Int_t i = 0; i < num_sigs; i++) {
        sigs_new[i] = new Signature(*((sigs[i])->getName()), *((sigs[i])->getType()), *((sigs[i])->getLevel()), (sigs[i])->getPrescale());
        
        (sigs_new[i]).setXsec(0.0);
        (sigs_new[i]).setLumi((sigs[i])->getLumi());
        (sigs_new[i]).incNumEvts(total_N_evts);

        temp_index1 = (sigs[i])->getOverlapSize();
        temp_index2 = (sigs[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
            gROOT->ProcessLine(".q");
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_new[i]).addOverlap(*((sigs[i])->getOverlapNameAt(j)), 0.0);
        }

    }

    for (Int_t i = 0; i < num_csigs; i++) {
        sigs_c_new[i] = new CplxSignature(*((sigs_c[i])->getName()), *((sigs_c[i])->getType()));
        
        (sigs_c_new[i]).setXsec(0.0);
        (sigs_c_new[i]).setLumi((sigs_c[i])->getLumi());
        (sigs_c_new[i]).setLevel(*((sigs_c[i])->getLevel()));
        (sigs_c_new[i]).incNumEvts(total_N_evts);
        
        temp_index1 = (sigs_c[i])->getSigIndicesSize();
        temp_index2 = (sigs_c[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_c_new[i]).addSig(*((sigs_c[i])->getSigAt(j)), (sigs_c[i])->getSigIndexAt(j));
        }

        temp_index1 = (sigs_c[i])->getOverlapSize();
        temp_index2 = (sigs_c[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_c_new[i]).addOverlap(*((sigs_c[i])->getOverlapNameAt(j)), 0.0);
        }
    }

    for (Int_t i = 0; i < num_streams; i++) {
        sigs_streams_new[i] = new CplxSignature(*((sigs_streams[i])->getName()), *((sigs_streams[i])->getType()));
        
        (sigs_streams_new[i]).setXsec(0.0);
        (sigs_streams_new[i]).setLumi((sigs_streams[i])->getLumi());
        (sigs_streams_new[i]).setLevel(*((sigs_streams[i])->getLevel()));
        (sigs_streams_new[i]).incNumEvts(total_N_evts);
        
        temp_index1 = (sigs_streams[i])->getSigIndicesSize();
        temp_index2 = (sigs_streams[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_streams_new[i]).addSig(*((sigs_streams[i])->getSigAt(j)), (sigs_streams[i])->getSigIndexAt(j));
        }

        temp_index1 = (sigs_streams[i])->getOverlapSize();
        temp_index2 = (sigs_streams[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_streams_new[i]).addOverlap(*((sigs_streams[i])->getOverlapNameAt(j)), 0.0);
        }
    }

    for (Int_t i = 0; i < num_levels; i++) {
        levels_new[i] = new CplxSignature(*((levels[i])->getName()), *((levels[i])->getType()));
        
        (levels_new[i]).setXsec(0.0);
        (levels_new[i]).setLumi((levels[i])->getLumi());
        (levels_new[i]).setLevel(*((levels[i])->getLevel()));
        (levels_new[i]).incNumEvts(total_N_evts);
        
        temp_index1 = (levels[i])->getSigIndicesSize();
        temp_index2 = (levels[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (levels_new[i]).addSig(*((levels[i])->getSigAt(j)), (levels[i])->getSigIndexAt(j));
        }

        temp_index1 = (levels[i])->getOverlapSize();
        temp_index2 = (levels[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (levels_new[i]).addOverlap(*((levels[i])->getOverlapNameAt(j)), 0.0);
        }
    }


    Int_t threshold_set_s_new;
    Int_t threshold_set_c_new;
    Int_t threshold_set_l_new;
    Int_t threshold_set_streams_new;
    Long64_t N_entries = 0;
    TFile *f = new TFile("TriggerRates.root","RECREATE");

    TTree *ntuple_ssigs   = new TTree("SingleSignatures","Trigger rates (single signatures)");
    TTree *ntuple_csigs   = new TTree("CplxSignatures","Trigger rates (complex signatures)");
    TTree *ntuple_streams = new TTree("Streams","Trigger rates (stream signatures)");
    TTree *ntuple_levels  = new TTree("Levels","Overall trigger rates (per level)");
    TBranch *branch_thr_s = ntuple_ssigs ->Branch("threshold_set_s", &threshold_set_s_new, "threshold_set_s/I");
    TBranch *branch_thr_c = ntuple_csigs ->Branch("threshold_set_c", &threshold_set_c_new, "threshold_set_c/I");
    TBranch *branch_thr_streams = ntuple_streams ->Branch("threshold_set_streams", &threshold_set_streams_new, "threshold_set_streams/I");
    TBranch *branch_thr_l = ntuple_levels->Branch("threshold_set_l", &threshold_set_l_new, "threshold_set_l/I");

    
    TBranch *branch_sigs[num_sigs]; 
    Signature *sigs_new_P[num_sigs];
    for (Int_t i = 0; i < num_sigs; i++) {
        branch_sigs[i] = NULL;
        sigs_new_P[i] = &sigs_new[i];
        branch_sigs[i] = ntuple_ssigs->Branch(*(sigs_new[i].getName()), "Signature", &sigs_new_P[i], 128000, 0);
    }

    TBranch *branch_csigs[num_csigs]; 
    CplxSignature *sigs_c_new_P[num_csigs];
    for (Int_t i = 0; i < num_csigs; i++) {
        branch_csigs[i] = NULL;
        sigs_c_new_P[i] = &sigs_c_new[i];
        branch_csigs[i] = ntuple_csigs->Branch(*(sigs_c_new[i].getName()), "CplxSignature", &sigs_c_new_P[i], 128000, 0);
    }

    TBranch *branch_streams[num_streams]; 
    CplxSignature *sigs_streams_new_P[num_streams];
    for (Int_t i = 0; i < num_streams; i++) {
        branch_streams[i] = NULL;
        sigs_streams_new_P[i] = &sigs_streams_new[i];
        branch_streams[i] = ntuple_streams->Branch(*(sigs_streams_new[i].getName()), "CplxSignature", &sigs_streams_new_P[i], 128000, 0);
    }

    TBranch *branch_levels[num_levels]; 
    CplxSignature *levels_new_P[num_levels];
    for (Int_t i = 0; i < num_levels; i++) {
        branch_levels[i] = NULL;
        levels_new_P[i] = &levels_new[i];
        branch_levels[i] = ntuple_levels->Branch(*(levels_new[i].getName()), "CplxSignature", &levels_new_P[i], 128000, 0);
    }
    
    // END Create output TTree
    // BEGIN Filling TTree
    

    
    /* Start filling the TTree */
    for (Int_t thr = 0; thr < num_thresholds; thr++) {
        
        for (Int_t i = 0; i < num_sigs; i++) {
            (sigs_new[i]).setXsec(0.0);
            (sigs_new[i])->setNumPassed(0.0);
            (sigs_new[i])->setNumUnique(0.0);
            (sigs_new[i])->setIndep(0.0);
            (sigs_new[i])->clearSamples();
            (sigs_new[i])->clearFileNames();
            (sigs_new[i])->clearThresholds();
            temp_index1 = (sigs_new[i])->getOverlapSize();
        
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_new[i]).setOverlap(j, 0.0);
            }

        }
        
        for (Int_t i = 0; i < num_csigs; i++) {
            (sigs_c_new[i]).setXsec(0.0);
            (sigs_c_new[i])->setPnumPassed(0.0);
            (sigs_c_new[i])->setNumPassed(0.0);
            (sigs_c_new[i])->clearSamples();
            (sigs_c_new[i])->clearFileNames();
            temp_index1 = (sigs_c_new[i])->getOverlapSize();
        
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_c_new[i]).setOverlap(j, 0.0);
            }

        }
        
        for (Int_t i = 0; i < num_streams; i++) {
            (sigs_streams_new[i]).setXsec(0.0);
            (sigs_streams_new[i])->setPnumPassed(0.0);
            (sigs_streams_new[i])->setNumPassed(0.0);
            (sigs_streams_new[i])->clearSamples();
            (sigs_streams_new[i])->clearFileNames();
            temp_index1 = (sigs_streams_new[i])->getOverlapSize();
        
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_streams_new[i]).setOverlap(j, 0.0);
            }

        }
        
        for (Int_t i = 0; i < num_levels; i++) {
            (levels_new[i]).setXsec(0.0);
            (levels_new[i])->setPnumPassed(0.0);
            (levels_new[i])->setNumPassed(0.0);
            (levels_new[i])->clearSamples();
            (levels_new[i])->clearFileNames();
            temp_index1 = (levels_new[i])->getOverlapSize();
        
            for (Int_t j = 0; j < temp_index1; j++) {
                (levels_new[i]).setOverlap(j, 0.0);
            }

        }
        
        
        for (Int_t sample = 0; sample < num_samples; sample++) {
            
            s_sigs->GetEntry(thr+num_thresholds*sample);
            c_sigs->GetEntry(thr+num_thresholds*sample);
            l_sigs->GetEntry(thr+num_thresholds*sample);
            streams->GetEntry(thr+num_thresholds*sample);

        
            for (Int_t i = 0; i < num_sigs; i++) {

                /* Number of events Passed */
                temp_double1 = (sigs_new[i])->getNumPassed();
                temp_double2 = (sigs_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_new[i])->setNumPassed( (temp_double3 + ( ((sigs[i]).getNumPassed())/((sigs[i]).getNumEvts()) )*total_N_evts*((sigs[i]).getXsec()))/(temp_double2+(sigs[i]).getXsec()) );

                /* Number of Unique Events */
                temp_double1 = (sigs_new[i])->getNumUnique();
                temp_double2 = (sigs_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_new[i])->setNumUnique( (temp_double3 + ( ((sigs[i]).getNumUnique())/((sigs[i]).getNumEvts()) )*total_N_evts*((sigs[i]).getXsec()))/(temp_double2+(sigs[i]).getXsec()) );
                

                /* Independent Number */
                temp_double1 = (sigs_new[i])->getIndep();
                temp_double2 = (sigs_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_new[i])->setIndep( (temp_double3 + ((sigs[i]).getIndepFrac())*((sigs[i]).getXsec()))/(temp_double2+(sigs[i]).getXsec()) );
                

                /* Overlap */
                temp_index1 = (sigs[i])->getOverlapSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    temp_double1 = ( ((sigs_new[i])->getOverlapAt(j))*((sigs_new[i])->getNumPassed()) )/((sigs_new[i])->getPrescale());
                    temp_double2 = (sigs_new[i])->getXsec();
                    temp_double3 = temp_double1*temp_double2;
                    
                    (sigs_new[i])->setOverlap(j, (temp_double3 + ((sigs[i])->getOverlapAt(j))*((sigs[i]).getXsec()))/(temp_double2+(sigs[i]).getXsec()) );
                }

                /* Filenames */
                temp_index1 = (sigs[i])->getFileNamesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_new[i]).addFileName(*((sigs[i])->getFileNameAt(j)));
                }

                /* Samples */
                temp_index1 = (sigs[i])->getSamplesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_new[i]).addSample( *((sigs[i])->getSampleAt(j)) );
                }

                /* Xsec */
                
                temp_double1 = (sigs_new[i])->getXsec();
                
                (sigs_new[i])->setXsec(temp_double1+(sigs[i]).getXsec());
                
            }

            for (Int_t i = 0; i < num_csigs; i++) {
                
                /* Number of events Passed */
                temp_double1 = (sigs_c_new[i])->getNumPassed();
                temp_double2 = (sigs_c_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_c_new[i])->setNumPassed( (temp_double3 + ( ((sigs_c[i]).getNumPassed())/((sigs_c[i]).getNumEvts()) )*total_N_evts*((sigs_c[i]).getXsec()))/(temp_double2+(sigs_c[i]).getXsec()) );

                /* Prescaled Number of events Passed */
                temp_double1 = (sigs_c_new[i])->getPnumPassed();
                temp_double2 = (sigs_c_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_c_new[i])->setPnumPassed( (temp_double3 + ( ((sigs_c[i]).getPnumPassed())/((sigs_c[i]).getNumEvts()) )*total_N_evts*((sigs_c[i]).getXsec()))/(temp_double2+(sigs_c[i]).getXsec()) );

                
                /* Overlap */
                temp_index1 = (sigs_c[i])->getOverlapSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    temp_double1 = ((sigs_c_new[i])->getOverlapAt(j))*((sigs_c_new[i])->getPnumPassed());
                    temp_double2 = (sigs_c_new[i])->getXsec();
                    temp_double3 = temp_double1*temp_double2;
                    
                    (sigs_c_new[i])->setOverlap(j, (temp_double3 + ((sigs_c[i])->getOverlapAt(j))*((sigs_c[i]).getXsec()))/(temp_double2+(sigs_c[i]).getXsec()) );
                }


                /* Filenames */
                temp_index1 = (sigs_c[i])->getFileNamesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_c_new[i]).addFileName(*((sigs_c[i])->getFileNameAt(j)));
                }

                /* Samples */
                temp_index1 = (sigs_c[i])->getSamplesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_c_new[i]).addSample( *((sigs_c[i])->getSampleAt(j)) );
                }

                /* Xsec */
                
                temp_double1 = (sigs_c_new[i])->getXsec();
                
                (sigs_c_new[i])->setXsec(temp_double1+(sigs_c[i]).getXsec());

            }

            for (Int_t i = 0; i < num_streams; i++) {
                
                /* Number of events Passed */
                temp_double1 = (sigs_streams_new[i])->getNumPassed();
                temp_double2 = (sigs_streams_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_streams_new[i])->setNumPassed( (temp_double3 + ( ((sigs_streams[i]).getNumPassed())/((sigs_streams[i]).getNumEvts()) )*total_N_evts*((sigs_streams[i]).getXsec()))/(temp_double2+(sigs_streams[i]).getXsec()) );

                /* Prescaled Number of events Passed */
                temp_double1 = (sigs_streams_new[i])->getPnumPassed();
                temp_double2 = (sigs_streams_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_streams_new[i])->setPnumPassed( (temp_double3 + ( ((sigs_streams[i]).getPnumPassed())/((sigs_streams[i]).getNumEvts()) )*total_N_evts*((sigs_streams[i]).getXsec()))/(temp_double2+(sigs_streams[i]).getXsec()) );

                
                /* Prescaled Number of events Passed */
                temp_double1 = (sigs_streams_new[i])->getPnumUnique();
                temp_double2 = (sigs_streams_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (sigs_streams_new[i])->setPnumUnique( (temp_double3 + ( ((sigs_streams[i]).getPnumUnique())/((sigs_streams[i]).getNumEvts()) )*total_N_evts*((sigs_streams[i]).getXsec()))/(temp_double2+(sigs_streams[i]).getXsec()) );


                 /* Overlap */
                temp_index1 = (sigs_streams[i])->getOverlapSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    temp_double1 = ((sigs_streams_new[i])->getOverlapAt(j))*((sigs_streams_new[i])->getPnumPassed());
                    temp_double2 = (sigs_streams_new[i])->getXsec();
                    temp_double3 = temp_double1*temp_double2;
                    
                    (sigs_streams_new[i])->setOverlap(j, (temp_double3 + ((sigs_streams[i])->getOverlapAt(j))*((sigs_streams[i]).getXsec()))/(temp_double2+(sigs_streams[i]).getXsec()) );
                }


                /* Filenames */
                temp_index1 = (sigs_streams[i])->getFileNamesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_streams_new[i]).addFileName(*((sigs_streams[i])->getFileNameAt(j)));
                }

                /* Samples */
                temp_index1 = (sigs_streams[i])->getSamplesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (sigs_streams_new[i]).addSample( *((sigs_streams[i])->getSampleAt(j)) );
                }

                /* Xsec */
                
                temp_double1 = (sigs_streams_new[i])->getXsec();
                
                (sigs_streams_new[i])->setXsec(temp_double1+(sigs_streams[i]).getXsec());

            }
            
            for (Int_t i = 0; i < num_levels; i++) {

                /* Number of events Passed */
                temp_double1 = (levels_new[i])->getNumPassed();
                temp_double2 = (levels_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (levels_new[i])->setNumPassed( (temp_double3 + ( ((levels[i]).getNumPassed())/((levels[i]).getNumEvts()) )*total_N_evts*((levels[i]).getXsec()))/(temp_double2+(levels[i]).getXsec()) );

                /* Prescaled Number of events Passed */
                temp_double1 = (levels_new[i])->getPnumPassed();
                temp_double2 = (levels_new[i])->getXsec();

                temp_double3 = temp_double1*temp_double2;

                (levels_new[i])->setPnumPassed( (temp_double3 + ( ((levels[i]).getPnumPassed())/((levels[i]).getNumEvts()) )*total_N_evts*((levels[i]).getXsec()))/(temp_double2+(levels[i]).getXsec()) );

                
                /* Overlap */
                temp_index1 = (levels[i])->getOverlapSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    temp_double1 = ((levels_new[i])->getOverlapAt(j))*((levels_new[i])->getPnumPassed());
                    temp_double2 = (levels_new[i])->getXsec();
                    temp_double3 = temp_double1*temp_double2;
                    
                    (levels_new[i])->setOverlap(j, (temp_double3 + ((levels[i])->getOverlapAt(j))*((levels[i]).getXsec()))/(temp_double2+(levels[i]).getXsec()) );
                }
                

                /* Filenames */
                temp_index1 = (levels[i])->getFileNamesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (levels_new[i]).addFileName(*((levels[i])->getFileNameAt(j)));
                }

                /* Samples */
                temp_index1 = (levels[i])->getSamplesSize();
                for (Int_t j = 0; j < temp_index1; j++) {
                    (levels_new[i]).addSample( *((levels[i])->getSampleAt(j)) );
                }

                /* Xsec */
                
                temp_double1 = (levels_new[i])->getXsec();
                
                (levels_new[i])->setXsec(temp_double1+(levels[i]).getXsec());

            }

        } // end loop over samples

        
        for (Int_t i = 0; i < num_sigs; i++) {
            /* Thresholds */
            temp_index1 = (sigs[i])->getThresholdsSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_new[i]).addThreshold(*((sigs[i])->getCutAlgAt(j)), *((sigs[i])->getCutParameterAt(j)), *((sigs[i])->getThresholdAt(j)));
            }

            /* Independent Number Fix */
            temp_double1 = (sigs_new[i])->getIndep();
            temp_double2 = (sigs_new[i])->getNumPassed();
            
            (sigs_new[i])->setIndep( temp_double1*temp_double2 );

            /* Overlap Fix */
            temp_index1 = (sigs_new[i])->getOverlapSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                temp_double1 = (sigs_new[i])->getOverlapAt(j);
                temp_double2 = ((sigs_new[i])->getNumPassed())/((sigs_new[i])->getPrescale());
                
                (sigs_new[i])->setOverlap(j, temp_double1*temp_double2*temp_double2 );
            }
        }

        for (Int_t i = 0; i < num_csigs; i++) {
            /* Overlap Fix */
            temp_index1 = (sigs_c_new[i])->getOverlapSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                temp_double1 = (sigs_c_new[i])->getOverlapAt(j);
                temp_double2 = (sigs_c_new[i])->getPnumPassed();
                
                (sigs_c_new[i])->setOverlap(j, temp_double1*temp_double2*temp_double2 );
            }
        }

        for (Int_t i = 0; i < num_streams; i++) {
            /* Overlap Fix */
            temp_index1 = (sigs_streams_new[i])->getOverlapSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                temp_double1 = (sigs_streams_new[i])->getOverlapAt(j);
                temp_double2 = (sigs_streams_new[i])->getPnumPassed();
                
                (sigs_streams_new[i])->setOverlap(j, temp_double1*temp_double2*temp_double2 );
            }
        }

        for (Int_t i = 0; i < num_levels; i++) {
            /* Overlap Fix */
            temp_index1 = (levels_new[i])->getOverlapSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                temp_double1 = (levels_new[i])->getOverlapAt(j);
                temp_double2 = (levels_new[i])->getPnumPassed();
                
                (levels_new[i])->setOverlap(j, temp_double1*temp_double2*temp_double2 );
            }
        }
                
        threshold_set_s_new = threshold_set_s;
        threshold_set_c_new = threshold_set_s;
        threshold_set_streams_new = threshold_set_s;
        threshold_set_l_new = threshold_set_s;

        ntuple_ssigs-> Fill();
        ntuple_csigs-> Fill();
        ntuple_streams-> Fill();
        ntuple_levels->Fill();


    } // end loop over thresholds
    


    // END Filling TTree
     
    f->Write();

}
