struct TriggerL1MetaObject {
    TriggerL1MetaObject (std::string in) {
        TBP = new float(0);
        TAP = new float(0);
        TAV = new float(0);
        PS = new float(0);
        DT = new float(0);

        tbp = new float(0);
        tap = new float(0);
        tav = new float(0);
        psv = new float(0);
        dtv = new float(0);
        xbp = new float(0);
        trigger_name = in;
        //Info("TriggerL1MetaObject()", "Registered new trigger with chain name = [%s]",trigger_name.c_str() );
    }
    // Read In Variables
    float * TBP;
    float * TAP;
    float * TAV;
    float * PS;
    float * DT;

    // Write Out Varialbes
    float * tbp;
    float * tap;
    float * tav;
    float * psv;
    float * dtv;
    float * xbp;

    std::string trigger_name;
    std::string getReadName (std::string postfix) {
        return (trigger_name + "_" + postfix);
    }
    std::string getWriteName (std::string prefix) {
        return (prefix + "__" +trigger_name);
    }
    std::string getKeyName (std::string prefix) {
        return (prefix + "__" + trigger_name + "/F");
    }

    void Update (float lumi) {
        *tbp = (*TBP);
        *tap = (*TAP);
        *tav = (*TAV);
        *psv = (*PS);
        *dtv = (*DT);
        if (lumi > 50) *xbp = (*TBP)/lumi;
        else *xbp = 0;
    }

};



struct TriggerHLTMetaObject {
    TriggerHLTMetaObject (std::string in) {
        input = new float(0);
        prescaled = new float(0);
        raw = new float(0);
        output = new float(0);

        ipt = new float(0);
        tap = new float(0);
        tav = new float(0);
        psd = new float(0);
        xbp = new float(0);
        trigger_name = in;
        //Info("TriggerL1MetaObject()", "Registered new trigger with chain name = [%s]",trigger_name.c_str() );
    }
    // Read In Variables
    float * input;
    float * prescaled;
    float * raw;
    float * output;

    // Write Out Varialbes
    float * ipt;
    float * tap;
    float * tav;
    float * psd;
    float * xbp;

    std::string trigger_name;
    std::string getReadName (std::string postfix) {
        return (trigger_name + "_" + postfix);
    }
    std::string getWriteName (std::string prefix) {
        return (prefix + "__" +trigger_name);
    }
    std::string getKeyName (std::string prefix) {
        return (prefix + "__" + trigger_name + "/F");
    }

    void Update (float lumi) {
        *ipt = (*input);
        *psd = (*prescaled);
        *tap = (*raw);
        *tav = (*output);
        if (lumi > 50) *xbp = (*output)/lumi;
        else *xbp = 0;
    }

};


void createNtupleFast (int run_number) {
    Info("createNtupleFast()", "Reading runcard");
    std::string input_path;
    std::string rules_path;
    std::string output_path;
    std::string line;
    Info("createNtupleFast()", "Assuming a properly formatted runcard...");
    ifstream myfile ("./runcard_" + std::to_string(run_number) + ".xmon");
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
          if (line.find("INPUT") != std::string::npos) {
              input_path = line.erase(0,6);
              Info("createNtupleFast()", "Got input path : %s", input_path.c_str());
          }
          if (line.find("RULES") != std::string::npos) {
              rules_path = line.erase(0,6);
              Info("createNtupleFast()", "Got rules path : %s", rules_path.c_str());

          }
          if (line.find("OUTPUT") != std::string::npos) {
              output_path  = line.erase(0,7);
              Info("createNtupleFast()", "Got output path : %s", output_path.c_str());

          }
        }
        myfile.close();
      }

    std::vector<Int_t> rulebook__lumi;
    std::vector<Int_t> rulebook__l1;
    std::vector<Int_t> rulebook__hlt;

    std::vector<TriggerL1MetaObject> l1_triggers;
    std::vector<TriggerHLTMetaObject> hlt_triggers;

    // Get Rules
    TFile * f_rates = new TFile(input_path.c_str());
    TFile * f_s = new TFile(rules_path.c_str());
    TFile * f_out = new TFile(output_path.c_str(), "RECREATE");

    TTree * t1 = (TTree*)f_s->Get("Rules");
    t1->Print("toponly");

    Int_t rules__lumi = 0;
    Int_t rules__l1 = 0;
    Int_t rules__hlt = 0;

    t1->SetBranchAddress("rules__lumi",  &rules__lumi);
    t1->SetBranchAddress("rules__l1", &rules__l1);
    t1->SetBranchAddress("rules__hlt", &rules__hlt);
    Int_t nentries = 0;
    Int_t count = 0;
    // all entries and fill the histograms from signal tree
    nentries = (Int_t)t1->GetEntries();
    Info("createNuple()","Total entries to process %d",nentries);
    count = 0;
    for (Int_t i = 0; i < nentries; i++) {
        // Info("NtupleDeco()","Processing entry %d",i);
        t1->GetEntry(i);
        rulebook__lumi.push_back(rules__lumi);
        rulebook__l1.push_back(rules__l1);
        rulebook__hlt.push_back(rules__hlt);
    }
    Info("createNtupleFast()", "Got IOV match rulebook!");
    Info("createNtupleFast()", "...");

    TTree * lumi_tree = (TTree*)f_rates->Get("Lu_Rate_Luminosity");
    TTree * l1_tree = (TTree*)f_rates->Get("L1_Rate");
    TTree * hlt_tree = (TTree*)f_rates->Get("HLT_Rate");

    Info("createNtupleFast()", "Constructing trigger metaobject lists...");
    for (auto s : *l1_tree->GetListOfBranches()) {
        // Look for any TBP chain
        if (string(s->GetName()).find("_TBP") != std::string::npos) {
            l1_triggers.push_back(TriggerL1MetaObject(string(s->GetName()).substr(0,string(s->GetName()).find("_TBP"))));
        }
    }

    for (auto s : *hlt_tree->GetListOfBranches()) {
        // Look for any TBP chain
        if (string(s->GetName()).find("_output") != std::string::npos) {
            hlt_triggers.push_back(TriggerHLTMetaObject(string(s->GetName()).substr(0,string(s->GetName()).find("_output"))));
        }
    }
    Info("createNtupleFast()", "Done.");


    TTree * t_out = new TTree( "ntp", "Xmon Flattened Rate Info" );

    float Lumi;
    float Mu;
    double Time;
    int Run;
    int LB;

    lumi_tree->SetBranchAddress("ATLAS_PREFERRED_Inst_CalibLumi_Val", &Lumi);
    lumi_tree->SetBranchAddress("ATLAS_PREFERRED_Inst_Mu", &Mu);
    l1_tree->SetBranchAddress("TimeStamp", &Time);
    l1_tree->SetBranchAddress("Run", &Run);
    l1_tree->SetBranchAddress("LumiBlock", &LB);

    t_out->Branch("evt__lumi", &Lumi, "evt__lumi/F");
    t_out->Branch("evt__mu", &Mu, "evt__mu/F");
    t_out->Branch("evt__lb", &LB, "evt__lb/I");
    t_out->Branch("evt__run", &Run, "evt__run/I");
    t_out->Branch("evt__time", &Time, "evt__time/D");


    Info("createNtupleFast()", "Setting branch address in read-in and write-out trees");


    for (unsigned int i = 0; i < l1_triggers.size(); i++) {
        l1_tree->SetBranchAddress(l1_triggers[i].getReadName("TBP").c_str(),  l1_triggers[i].TBP);
        l1_tree->SetBranchAddress(l1_triggers[i].getReadName("TAP").c_str(),  l1_triggers[i].TAP);
        l1_tree->SetBranchAddress(l1_triggers[i].getReadName("TAV").c_str(),  l1_triggers[i].TAV);
        l1_tree->SetBranchAddress(l1_triggers[i].getReadName("PS").c_str(),  l1_triggers[i].PS);
        l1_tree->SetBranchAddress(l1_triggers[i].getReadName("DT").c_str(),  l1_triggers[i].DT);
        t_out->Branch(l1_triggers[i].getWriteName("tbp").c_str(), l1_triggers[i].tbp, l1_triggers[i].getKeyName("tbp").c_str());
        t_out->Branch(l1_triggers[i].getWriteName("tap").c_str(), l1_triggers[i].tap, l1_triggers[i].getKeyName("tap").c_str());
        t_out->Branch(l1_triggers[i].getWriteName("tav").c_str(), l1_triggers[i].tav, l1_triggers[i].getKeyName("tav").c_str());
        t_out->Branch(l1_triggers[i].getWriteName("psv").c_str(), l1_triggers[i].psv, l1_triggers[i].getKeyName("psv").c_str());
        t_out->Branch(l1_triggers[i].getWriteName("dtv").c_str(), l1_triggers[i].dtv, l1_triggers[i].getKeyName("dtv").c_str());
        t_out->Branch(l1_triggers[i].getWriteName("xbp").c_str(), l1_triggers[i].xbp, l1_triggers[i].getKeyName("xbp").c_str());
    }

    for (unsigned int i = 0; i < hlt_triggers.size(); i++) {
        hlt_tree->SetBranchAddress(hlt_triggers[i].getReadName("input").c_str(),  hlt_triggers[i].input);
        hlt_tree->SetBranchAddress(hlt_triggers[i].getReadName("prescaled").c_str(),  hlt_triggers[i].prescaled);
        hlt_tree->SetBranchAddress(hlt_triggers[i].getReadName("raw").c_str(),  hlt_triggers[i].raw);
        hlt_tree->SetBranchAddress(hlt_triggers[i].getReadName("output").c_str(),  hlt_triggers[i].output);
        t_out->Branch(hlt_triggers[i].getWriteName("ipt").c_str(), hlt_triggers[i].ipt, hlt_triggers[i].getKeyName("ipt").c_str());
        t_out->Branch(hlt_triggers[i].getWriteName("tap").c_str(), hlt_triggers[i].tap, hlt_triggers[i].getKeyName("tap").c_str());
        t_out->Branch(hlt_triggers[i].getWriteName("tav").c_str(), hlt_triggers[i].tav, hlt_triggers[i].getKeyName("tav").c_str());
        t_out->Branch(hlt_triggers[i].getWriteName("psd").c_str(), hlt_triggers[i].psd, hlt_triggers[i].getKeyName("psd").c_str());
        t_out->Branch(hlt_triggers[i].getWriteName("xbp").c_str(), hlt_triggers[i].xbp, hlt_triggers[i].getKeyName("xbp").c_str());
    }
    Info("createNtupleFast()", "Done.");


    Int_t rates_read_nentries = rulebook__lumi.size();
    Info("createNtupleFast()", "Going to process %d entries!", rates_read_nentries);
    Info("createNtupleFast()", "Here we go....");

    for (Int_t i = 0; i < rates_read_nentries; i++) {
        // Info("NtupleDeco()","Processing entry %d",i);
        lumi_tree->GetEntry(rulebook__lumi[i]);
        l1_tree->GetEntry(rulebook__l1[i]);
        hlt_tree->GetEntry(rulebook__hlt[i]);
        Info("createNtupleFast()", "Currently updating master point %d, with rulebook set [%d,%d,%d] at LB = %d", i, rulebook__lumi[i],rulebook__l1[i],rulebook__hlt[i],LB);
        for (auto p : l1_triggers)  p.Update(Lumi);
        for (auto p : hlt_triggers) p.Update(Lumi);
        t_out->Fill();

    }
    f_s->Close();
    f_rates->Close();
    // Write out to planned ntuple
    Info("createNtupleFast()", "Now writing out ntuple");
    Info("createNtupleFast()", "Saved rates to --> %s", output_path.c_str());
    f_out->Write();
    f_out->Close();

}
