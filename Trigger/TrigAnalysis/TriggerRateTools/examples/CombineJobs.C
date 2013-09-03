/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
// Author: Marc-Andre Dufour, Miika Klemetti
// Date: April 22, 2009
//
// Description: Script that will combine output root files from multiple
//              jobs into a single one
////////////////////////////////////////////////////////////////////////////////
{
    gROOT->Reset();
    gROOT->SetStyle("Plain");

    #include <vector>
    
    gSystem->Load("$TrigNtDecision_PATH/$CMTCONFIG/libTrigNtDecision.so");
    gSystem->Load("$TriggerRateSlices_PATH/$CMTCONFIG/libTriggerRateSlices.so");
   
    //multiply the number of events by the factor below
    //use this ONLY IF you REALLY know what you are doing!
    bool do_scale_num_evts = false;
    Double_t scale_num_evts = 5.1;

    //set the number of events to the factor below
    bool do_override_num_evts = false;
    Int_t override_num_evts = 7000000;
    
    Int_t temp_index1 = 0;
    Int_t temp_index2 = 0;
    Int_t total_N_evts = 0;
    
    TChain* s_sigs = new TChain("SingleSignatures");
    TChain* c_sigs = new TChain("CplxSignatures");
    TChain* l_sigs = new TChain("Levels");
    TChain* raw_ps = new TChain("raw_PS");
    TChain* raw_td = new TChain("raw_TD");
    TChain* slices_info = new TChain("SlicesInfo");
    TChain* streams = new TChain("Streams");
    TChain* menus = new TChain("Menus");

    gROOT->ProcessLine(".! rm -f TriggerRates.root");
    
    s_sigs->Add("*.root");
    c_sigs->Add("*.root");
    l_sigs->Add("*.root");
    raw_ps->Add("*.root");
    raw_td->Add("*.root");
    slices_info->Add("*.root");
    streams->Add("*.root");
    menus->Add("*.root");
    Long64_t s_sigs_Nentries = s_sigs->GetEntries();
    Long64_t c_sigs_Nentries = c_sigs->GetEntries();
    Long64_t l_sigs_Nentries = l_sigs->GetEntries();
    Long64_t total_evts = raw_td->GetEntries();
    Long64_t streams_Nentries = streams->GetEntries();
    Long64_t menus_Nentries = menus->GetEntries();
    Long64_t Nentries = 0;
    if ((s_sigs_Nentries == c_sigs_Nentries) && (s_sigs_Nentries == l_sigs_Nentries)) {
            Nentries = s_sigs_Nentries;
    } else {
            cout << "	[!!] Signature containers are not consistent... [!!]" << endl;
    }
    
    if (Nentries == 0) {
        cout << "[!!] No results found, were there any files in this directory? [!!]" << endl;
        gROOT->ProcessLine(".q");
    }
    
    // BEGIN Assign branch addresses to input collections
    
    TObjArray *s_sigs_branches=s_sigs->GetListOfBranches();
    const Int_t num_sigs = s_sigs_branches->GetEntries();
    TString s_sig_names[num_sigs];
    for(Int_t i=0; i<num_sigs; i++) {
        s_sig_names[i]=NULL;
        s_sig_names[i]=*(((TD_Signature)s_sigs_branches.At(i))->getName());
    }

    TD_Signature* sigs[num_sigs];
    for (Int_t sig_i = 0; sig_i < num_sigs; sig_i++) {
        sigs[sig_i] = NULL;
        s_sigs->SetBranchAddress((s_sig_names[sig_i]) , &sigs[sig_i]);
    }
    

    TObjArray *c_sigs_branches=c_sigs->GetListOfBranches();
    const Int_t num_csigs = c_sigs_branches->GetEntries();
    TString c_sig_names[num_csigs];
    for(Int_t i=0; i<num_csigs; i++) {
        c_sig_names[i]=NULL;
        c_sig_names[i]=*(((TD_CplxSignature)c_sigs_branches.At(i))->getName());
    }

    TD_CplxSignature* sigs_c[num_csigs];
    for (Int_t sig_i = 0; sig_i < num_csigs; sig_i++) {
        sigs_c[sig_i] = NULL;
        c_sigs->SetBranchAddress((c_sig_names[sig_i]) , &sigs_c[sig_i]);
    }

    
    TObjArray *level_branches=l_sigs->GetListOfBranches();
    const Int_t num_levels = level_branches->GetEntries();
    TString level_names[num_levels];
    for(Int_t i=0; i<num_levels; i++) {
        level_names[i]=NULL;
        level_names[i]=*(((TD_CplxSignature)level_branches.At(i))->getName());
    }

    TD_CplxSignature* levels[num_levels];
    for (Int_t sig_i = 0; sig_i < num_levels; sig_i++) {
        levels[sig_i] = NULL;
        l_sigs->SetBranchAddress((level_names[sig_i]) , &levels[sig_i]);
    }

    TObjArray *stream_branches=streams->GetListOfBranches();
    const Int_t num_streams = stream_branches->GetEntries();
    TString stream_names[num_streams];
    for(Int_t i=0; i<num_streams; i++) {
        stream_names[i]=NULL;
        stream_names[i]=*(((TD_CplxSignature)stream_branches.At(i))->getName());
    }

    TD_CplxSignature* sigs_streams[num_streams];
    for (Int_t sig_i = 0; sig_i < num_streams; sig_i++) {
        sigs_streams[sig_i] = NULL;
        streams->SetBranchAddress((stream_names[sig_i]) , &sigs_streams[sig_i]);
    }

    TObjArray *menu_branches=menus->GetListOfBranches();
    const Int_t num_menus = menu_branches->GetEntries();
    TString menu_names[num_menus];
    for(Int_t i=0; i<num_menus; i++) {
        menu_names[i]=NULL;
        menu_names[i]=*(((TD_CplxSignature)menu_branches.At(i))->getName());
    }

    TD_CplxSignature* sigs_menus[num_menus];
    for (Int_t sig_i = 0; sig_i < num_menus; sig_i++) {
        sigs_menus[sig_i] = NULL;
        menus->SetBranchAddress((menu_names[sig_i]) , &sigs_menus[sig_i]);
    }

    TObjArray *raw_ps_branches=raw_ps->GetListOfBranches();
    const Int_t num_ps = raw_ps_branches->GetEntries();
    TString ps_names [num_ps];
    Int_t ps [num_ps];
    for(Int_t i=0; i<num_ps; i++) {
        ps_names[i]=NULL;
        ps_names[i]=TString((raw_ps_branches.At(i))->GetName());
        raw_ps->SetBranchAddress(ps_names[i], &ps[i]);
    }
    
    TObjArray *raw_td_branches=raw_td->GetListOfBranches();
    const Int_t num_td = raw_td_branches->GetEntries();
    TString td_names [num_td];
    Int_t td [num_td];
    for(Int_t i=0; i<num_td; i++) {
        td_names[i]=NULL;
        td_names[i]=TString((raw_td_branches.At(i))->GetName());
        raw_td->SetBranchAddress(td_names[i], &td[i]);
    }

    const Int_t num_slices_info = 5;

    Int_t evt_num = 0;
    TrigL1MissingEtObject* l1_met = NULL;
    TrigL2MissingEtObject* l2_met = NULL;
    TrigEFMissingEtObject* ef_met = NULL;
    TrigBCMObject* bcm = NULL;

    slices_info->SetBranchAddress("EventNumber", &evt_num);
    slices_info->SetBranchAddress("L1MissingEt", &l1_met);
    slices_info->SetBranchAddress("L2MissingEt", &l2_met);
    slices_info->SetBranchAddress("EFMissingEt", &ef_met);
    slices_info->SetBranchAddress("BCM", &bcm);

    // END Assign branch addresses to input collections
    // BEGIN Create output TTree
    s_sigs->GetEntry(0);
    c_sigs->GetEntry(0);
    l_sigs->GetEntry(0);
    streams->GetEntry(0);
    menus->GetEntry(0);
    
    TD_Signature sigs_new[num_sigs];
    TD_CplxSignature sigs_c_new[num_csigs];
    TD_CplxSignature sigs_menus_new[num_menus];
    TD_CplxSignature levels_new[num_levels];
    TD_CplxSignature sigs_streams_new[num_csigs];
    TD_CplxSignature sigs_menus_new[num_csigs];

    for (Int_t i = 0; i < num_sigs; i++) {
        sigs_new[i] = new TD_Signature(*((sigs[i])->getName()), *((sigs[i])->getLevel()), (sigs[i])->getPrescale(), (sigs[i])->getPassthrough());
        
        (sigs_new[i]).setXsec((sigs[i])->getXsec());
        (sigs_new[i]).setLumi((sigs[i])->getLumi());
	(sigs_new[i]).setL1Parent(*((sigs[i])->getL1Parent()));
	(sigs_new[i]).setL2Parent(*((sigs[i])->getL2Parent()));
	(sigs_new[i]).setNColls((sigs[i])->getNColls());

        temp_index1 = (sigs[i])->getOverlapNumSize();
        temp_index2 = (sigs[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_new[i]).addOverlap(*((sigs[i])->getOverlapNameAt(j)));
        }
    }

    for (Int_t i = 0; i < num_csigs; i++) {
        sigs_c_new[i] = new TD_CplxSignature(*((sigs_c[i])->getName()), *((sigs_c[i])->getType()), *((sigs_c[i])->getLevel()));
        
        (sigs_c_new[i]).setXsec((sigs_c[i])->getXsec());
        (sigs_c_new[i]).setLumi((sigs_c[i])->getLumi());
	(sigs_c_new[i]).setL1Parent(*((sigs_c[i])->getL1Parent()));
	(sigs_c_new[i]).setL2Parent(*((sigs_c[i])->getL2Parent()));
	(sigs_c_new[i]).setNColls((sigs_c[i])->getNColls()); 

        temp_index1 = (sigs_c[i])->getSigIndicesSize();
        temp_index2 = (sigs_c[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_c_new[i]).addSig(*((sigs_c[i])->getSigAt(j)), (sigs_c[i])->getSigIndexAt(j), 1.0);
        }

        temp_index1 = (sigs_c[i])->getOverlapNumSize();
        temp_index2 = (sigs_c[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_c_new[i]).addOverlap(*((sigs_c[i])->getOverlapNameAt(j)));
        }
    }

    for (Int_t i = 0; i < num_streams; i++) {
        sigs_streams_new[i] = new TD_CplxSignature(*((sigs_streams[i])->getName()), *((sigs_streams[i])->getType()), *((sigs_streams[i])->getLevel()));
        
        (sigs_streams_new[i]).setXsec((sigs_streams[i])->getXsec());
        (sigs_streams_new[i]).setLumi((sigs_streams[i])->getLumi());
        (sigs_streams_new[i]).setL1Parent(*((sigs_streams[i])->getL1Parent()));
        (sigs_streams_new[i]).setL2Parent(*((sigs_streams[i])->getL2Parent()));
        (sigs_streams_new[i]).setNColls((sigs_streams[i])->getNColls());
       
 
        temp_index1 = (sigs_streams[i])->getSigIndicesSize();
        temp_index2 = (sigs_streams[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_streams_new[i]).addSig(*((sigs_streams[i])->getSigAt(j)), (sigs_streams[i])->getSigIndexAt(j), (sigs_streams[i])->getSigPrescaleAt(j));
        }

        temp_index1 = (sigs_streams[i])->getOverlapNumSize();
        temp_index2 = (sigs_streams[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_streams_new[i]).addOverlap(*((sigs_streams[i])->getOverlapNameAt(j)));
        }
    }

    for (Int_t i = 0; i < num_menus; i++) {
        sigs_menus_new[i] = new TD_CplxSignature(*((sigs_menus[i])->getName()), *((sigs_menus[i])->getType()), *((sigs_menus[i])->getLevel()));
        
        (sigs_menus_new[i]).setXsec((sigs_menus[i])->getXsec());
        (sigs_menus_new[i]).setLumi((sigs_menus[i])->getLumi());
        (sigs_menus_new[i]).setL1Parent(*((sigs_menus[i])->getL1Parent()));
        (sigs_menus_new[i]).setL2Parent(*((sigs_menus[i])->getL2Parent()));
        (sigs_menus_new[i]).setNColls((sigs_menus[i])->getNColls());
 
        temp_index1 = (sigs_menus[i])->getSigIndicesSize();
        temp_index2 = (sigs_menus[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_menus_new[i]).addSig(*((sigs_menus[i])->getSigAt(j)), (sigs_menus[i])->getSigIndexAt(j), (sigs_menus[i])->getSigPrescaleAt(j));
        }

        temp_index1 = (sigs_menus[i])->getOverlapNumSize();
        temp_index2 = (sigs_menus[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (sigs_menus_new[i]).addOverlap(*((sigs_menus[i])->getOverlapNameAt(j)));
        }
    }

    for (Int_t i = 0; i < num_levels; i++) {
        levels_new[i] = new TD_CplxSignature(*((levels[i])->getName()), *((levels[i])->getType()), *((levels[i])->getLevel()));
        
        (levels_new[i]).setXsec((levels[i])->getXsec());
        (levels_new[i]).setLumi((levels[i])->getLumi());
        (levels_new[i]).setL1Parent(*((levels[i])->getL1Parent()));
        (levels_new[i]).setL2Parent(*((levels[i])->getL2Parent()));
        (levels_new[i]).setNColls((levels[i])->getNColls());
  
        temp_index1 = (levels[i])->getSigIndicesSize();
        temp_index2 = (levels[i])->getSigNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Sig components) size mismatch, files could be corrupted..." << endl;
        }

        for (Int_t j = 0; j < temp_index1; j++) {
            (levels_new[i]).addSig(*((levels[i])->getSigAt(j)), (levels[i])->getSigIndexAt(j), 1.0);
        }

        temp_index1 = (levels[i])->getOverlapNumSize();
        temp_index2 = (levels[i])->getOverlapNamesSize();
        if (temp_index1 != temp_index2) {
            cout << "Container (Overlaps) size mismatch, files could be corrupted..." << endl;
        }
        
        for (Int_t j = 0; j < temp_index1; j++) {
            (levels_new[i]).addOverlap(*((levels[i])->getOverlapNameAt(j)));
        }
    }

    Long64_t N_entries = 0;
    TFile *f = new TFile("TriggerRates.root","RECREATE");

    TTree *ntuple_ssigs   = new TTree("SingleSignatures","Trigger rates (single signatures)");
    TTree *ntuple_csigs   = new TTree("CplxSignatures","Trigger rates (complex signatures)");
    TTree *ntuple_streams = new TTree("Streams","Trigger rates (stream signatures)");
    TTree *ntuple_menus   = new TTree("Menus","Trigger rates (menu signatures)");
    TTree *ntuple_levels  = new TTree("Levels","Overall trigger rates (per level)");
    
    TTree *ntuple_raw_ps   = new TTree("raw_PS","Raw Trigger isPrescaled decision");
    TTree *ntuple_raw_td   = new TTree("raw_TD","Raw Trigger isPassedRaw||isPassedThrough decision");
    TTree *ntuple_slices_info = new TTree("SlicesInfo","Slice-specific information");
    
    TBranch *branch_sigs[num_sigs]; 
    TD_Signature *sigs_new_P[num_sigs];
    for (Int_t i = 0; i < num_sigs; i++) {
        branch_sigs[i] = NULL;
        sigs_new_P[i] = &sigs_new[i];
        branch_sigs[i] = ntuple_ssigs->Branch(*(sigs_new[i].getName()), "TD_Signature", &sigs_new_P[i], 128000, 0);
    }

    TBranch *branch_csigs[num_csigs]; 
    TD_CplxSignature *sigs_c_new_P[num_csigs];
    for (Int_t i = 0; i < num_csigs; i++) {
        branch_csigs[i] = NULL;
        sigs_c_new_P[i] = &sigs_c_new[i];
        branch_csigs[i] = ntuple_csigs->Branch(*(sigs_c_new[i].getName()), "TD_CplxSignature", &sigs_c_new_P[i], 128000, 0);
    }

    TBranch *branch_streams[num_streams]; 
    TD_CplxSignature *sigs_streams_new_P[num_streams];
    for (Int_t i = 0; i < num_streams; i++) {
        branch_streams[i] = NULL;
        sigs_streams_new_P[i] = &sigs_streams_new[i];
        branch_streams[i] = ntuple_streams->Branch(*(sigs_streams_new[i].getName()), "TD_CplxSignature", &sigs_streams_new_P[i], 128000, 0);
    }

    TBranch *branch_menus[num_menus]; 
    TD_CplxSignature *sigs_menus_new_P[num_menus];
    for (Int_t i = 0; i < num_menus; i++) {
        branch_menus[i] = NULL;
        sigs_menus_new_P[i] = &sigs_menus_new[i];
        branch_menus[i] = ntuple_menus->Branch(*(sigs_menus_new[i].getName()), "TD_CplxSignature", &sigs_menus_new_P[i], 128000, 0);
    }

    TBranch *branch_levels[num_levels]; 
    TD_CplxSignature *levels_new_P[num_levels];
    for (Int_t i = 0; i < num_levels; i++) {
        branch_levels[i] = NULL;
        levels_new_P[i] = &levels_new[i];
        branch_levels[i] = ntuple_levels->Branch(*(levels_new[i].getName()), "TD_CplxSignature", &levels_new_P[i], 128000, 0);
    }
    
    TBranch *branch_ps[num_ps];
    Int_t new_ps[num_ps];
    for (Int_t i = 0; i < num_ps; i++) {
        branch_ps[i] = NULL;
        branch_ps[i] = ntuple_raw_ps->Branch(ps_names[i], &new_ps[i], "new_ps/I");
    }
    
    TBranch *branch_td[num_td];
    Int_t new_td[num_td];
    for (Int_t i = 0; i < num_td; i++) {
        branch_td[i] = NULL;
        branch_td[i] = ntuple_raw_td->Branch(td_names[i], &new_td[i], "new_td/I");
    }

    TBranch *branch_slices_info[num_slices_info];
    Int_t evt_num_new = 0;
    TrigL1MissingEtObject* l1_met_new = new TrigL1MissingEtObject();
    TrigL2MissingEtObject* l2_met_new = new TrigL2MissingEtObject();
    TrigEFMissingEtObject* ef_met_new = new TrigEFMissingEtObject();
    TrigBCMObject* bcm_new = new TrigBCMObject();

    for (Int_t i = 0; i < num_slices_info; i++) {
        branch_slices_info[i] = NULL;
    }

    branch_slices_info[0] = ntuple_slices_info->Branch("EventNumber", &evt_num, "evt_num/I");
    branch_slices_info[1] = ntuple_slices_info->Branch("L1MissingEt", "TrigL1MissingEtObject", &l1_met, 128000, 0);
    branch_slices_info[2] = ntuple_slices_info->Branch("L2MissingEt", "TrigL2MissingEtObject", &l2_met, 128000, 0);
    branch_slices_info[3] = ntuple_slices_info->Branch("EFMissingEt", "TrigEFMissingEtObject", &ef_met, 128000, 0);
    branch_slices_info[4] = ntuple_slices_info->Branch("BCM", "TrigBCMObject", &bcm, 128000, 0);
                            
    // END Create output TTree
    // BEGIN Filling TTree
    
    for (Int_t entry = 0; (entry < Nentries)&&(num_sigs>0); entry++) {

        s_sigs->GetEntry(entry);
        total_N_evts += (sigs[0])->getNumEvts();
        
    }

    for (Int_t entry = 0; (entry < Nentries) && (total_N_evts > 0); entry++) {

        s_sigs->GetEntry(entry);
        c_sigs->GetEntry(entry);
        l_sigs->GetEntry(entry);
        streams->GetEntry(entry);
        menus->GetEntry(entry);

        
        for (Int_t i = 0; i < num_sigs; i++) {

            (sigs_new[i]).incNumEvts((sigs[i])->getNumEvts());
            (sigs_new[i]).incNumPassed((sigs[i])->getNumPassed());
            (sigs_new[i]).incPnumUnique((sigs[i])->getPnumUnique());
            (sigs_new[i]).incIndep((sigs[i])->getIndep());
            
            temp_index1 = (sigs[i])->getOverlapNumSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_new[i]).incOverlapNum(j, (sigs[i])->getOverlapNumAt(j));
            }
        }

        for (Int_t i = 0; i < num_csigs; i++) {

            (sigs_c_new[i]).incNumEvts((sigs_c[i])->getNumEvts());
            (sigs_c_new[i]).incNumPassed((sigs_c[i])->getNumPassed());
            (sigs_c_new[i]).incPnumPassed((sigs_c[i])->getPnumPassed());
            (sigs_c_new[i]).incPnumUnique((sigs_c[i])->getPnumUnique());
            
            temp_index1 = (sigs_c[i])->getOverlapNumSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_c_new[i]).incOverlapNum(j, (sigs_c[i])->getOverlapNumAt(j));
            }
        }

        for (Int_t i = 0; i < num_streams; i++) {

            (sigs_streams_new[i]).incNumEvts((sigs_streams[i])->getNumEvts());
            (sigs_streams_new[i]).incNumPassed((sigs_streams[i])->getNumPassed());
            (sigs_streams_new[i]).incPnumPassed((sigs_streams[i])->getPnumPassed());
            (sigs_streams_new[i]).incPnumUnique((sigs_streams[i])->getPnumUnique());
            
            temp_index1 = (sigs_streams[i])->getOverlapNumSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_streams_new[i]).incOverlapNum(j, (sigs_streams[i])->getOverlapNumAt(j));
            }
        }

        for (Int_t i = 0; i < num_menus; i++) {

            (sigs_menus_new[i]).incNumEvts((sigs_menus[i])->getNumEvts());
            (sigs_menus_new[i]).incNumPassed((sigs_menus[i])->getNumPassed());
            (sigs_menus_new[i]).incPnumPassed((sigs_menus[i])->getPnumPassed());
            (sigs_menus_new[i]).incPnumUnique((sigs_menus[i])->getPnumUnique());
            
            temp_index1 = (sigs_menus[i])->getOverlapNumSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (sigs_menus_new[i]).incOverlapNum(j, (sigs_menus[i])->getOverlapNumAt(j));
            }
        }

        
        for (Int_t i = 0; i < num_levels; i++) {

            (levels_new[i]).incNumEvts((levels[i])->getNumEvts());
            (levels_new[i]).incNumPassed((levels[i])->getNumPassed());
            (levels_new[i]).incPnumPassed((levels[i])->getPnumPassed());
            
            temp_index1 = (levels[i])->getOverlapNumSize();
            for (Int_t j = 0; j < temp_index1; j++) {
                (levels_new[i]).incOverlapNum(j, (levels[i])->getOverlapNumAt(j));
            }
        }

    }
   
    if (do_scale_num_evts) {
        for (Int_t i = 0; i < num_sigs; i++) {
            (sigs_new[i]).setNumEvts( (sigs_new[i]).getNumEvts()*scale_num_evts );
        }
        for (Int_t i = 0; i < num_csigs; i++) {
            (sigs_c_new[i]).setNumEvts( (sigs_c_new[i]).getNumEvts()*scale_num_evts );
        }
        for (Int_t i = 0; i < num_streams; i++) {
            (sigs_streams_new[i]).setNumEvts( (sigs_streams_new[i]).getNumEvts()*scale_num_evts );
        }
        for (Int_t i = 0; i < num_menus; i++) {
            (sigs_menus_new[i]).setNumEvts( (sigs_menus_new[i]).getNumEvts()*scale_num_evts );
        }
        for (Int_t i = 0; i < num_levels; i++) {
            (levels_new[i]).setNumEvts( (levels_new[i]).getNumEvts()*scale_num_evts );
        }
    }
 
    if (do_override_num_evts) {
        for (Int_t i = 0; i < num_sigs; i++) {
            (sigs_new[i]).setNumEvts(override_num_evts);
        }
        for (Int_t i = 0; i < num_csigs; i++) {
            (sigs_c_new[i]).setNumEvts(override_num_evts);
        }
        for (Int_t i = 0; i < num_streams; i++) {
            (sigs_streams_new[i]).setNumEvts(override_num_evts);
        }
        for (Int_t i = 0; i < num_menus; i++) {
            (sigs_menus_new[i]).setNumEvts(override_num_evts);
        }
        for (Int_t i = 0; i < num_levels; i++) {
            (levels_new[i]).setNumEvts(override_num_evts);
        }
    }

    ntuple_ssigs-> Fill();
    ntuple_csigs-> Fill();
    ntuple_levels->Fill();
    ntuple_streams-> Fill();
    ntuple_menus-> Fill();

    for (Int_t entry = 0; (entry < total_evts) && (total_evts > 0); entry++) {
        raw_ps->GetEntry(entry);
        raw_td->GetEntry(entry);
        slices_info->GetEntry(entry);

        for (Int_t j = 0; j < num_ps; j++) {
            new_ps[j] = ps[j];
        }
        for (Int_t j = 0; j < num_ps; j++) {
            new_td[j] = td[j];
        }
        evt_num_new = evt_num;
        l1_met_new = l1_met;
        l2_met_new = l2_met;
        ef_met_new = ef_met;
        bcm_new = bcm;

        ntuple_raw_ps->Fill();
        ntuple_raw_td->Fill();
        ntuple_slices_info->Fill();
    }

    // END Filling TTree
     
    f->Write();

}
