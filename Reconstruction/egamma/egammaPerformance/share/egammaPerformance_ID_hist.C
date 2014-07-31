 
 //////////////////////////////////////////////////////
 //
 // Base class. Leaves booking and filling to derived classes
 // virtual doesnt seem to work, so have to cut and paste
 // calling of the various methods to derived classes
 //
 //////////////////////////////////////////////////////
#include <vector>

 class Hists{
 public:
  // initialise with input file name
  Hists(char* ntupFileName, char* histFileName)
  {
    cout<<"Hists construction"<<endl;
     m_file    = new TFile(ntupFileName);
     m_tree    = (TTree*) m_file->Get("CollectionTree");
     m_hlist   = new TObjArray(0);
  }
 
   //opens a disk file and writes out the hists
   void writeHistsToDisk(char* ofname){
    cout<<"writing..."<<endl;
     ofile = new TFile(ofname, "recreate");
     m_hlist->Write();
     ofile->Close();
   }
 
   // books histograms. Adds pointers of newly booked hists to an
   // TObjArray list.
   virtual void bookHists(){cout<<"Hists::booking()"<<endl;}
   // uses the ntuple to fill the hists which are then normalised 
   virtual void fillHists(){}
   // normalise the histograms
   void normH(TH1F* m_hist)
   {
     double scale=m_hist->GetEntries();
     if ( m_hist->GetEntries() != 0 )
       scale = 1/m_hist->GetEntries();
     else
       scale=1.;
     int nb = m_hist->GetNdivisions();
     double error = 0;
     double value = 0;
     for (int i=1;i<=nb;i++) {
       error = m_hist-> GetBinError(i);
       value = m_hist-> GetBinContent(i);
       error = error*scale;
       value = value*scale;
       m_hist->SetBinError(i,error);   
       m_hist->SetBinContent(i,value);   
     } 
       //     m_hist->Scale(scale);
   }

   void normHE(TH1F* m_hist, TH1F* m_histN)
   {
     int nd1 = m_hist->GetNdivisions();
     int nd2 = m_histN->GetNdivisions();
     if (nd1 != nd2) cout<<"ERROR : wrong set of histograms"<<endl;
     double error = 0;
     double value = 0;
     double scale = 0;
     for (int i=1;i<=nd1;i++) {
       error = m_hist-> GetBinError(i);
       value = m_hist-> GetBinContent(i);
       scale = m_histN-> GetBinContent(i);
       if (scale != 0) scale = 1./scale;
       //       else scale = 1.
       error = error*scale;
       value = value*scale;
       m_hist->SetBinError(i,error);   
       m_hist->SetBinContent(i,value);   
     } 
       //     m_hist->Scale(scale);
   }

 protected:
   TObjArray* m_hlist;
   // pointer to tree read from  file
   TTree* m_tree;
 
 
 
 };

 class EgammaIDHists:public Hists{
 public:
   EgammaIDHists(char* ntupFileName, char* histFileName):Hists(ntupFileName, histFileName){
     cout<<"EgammaIdHists construction"<<endl;
 
     this->bookHists();
     this->fillHists();
     this->writeHistsToDisk(histFileName);
 
   }
   void bookHists(){
     cout<<"EgammaIDHists booking..."<<endl;
     
       m_h100 = new TH1F("m_h100","Number of tracks", 50,0.,500);
       m_h101 = new TH1F("m_h101","Number of clusters", 15,0.,15.);
       m_h102 = new TH1F("m_h102","Fake rate vs cluster Et", 100,0.,110000.);
       m_h103 = new TH1F("m_h103","Fake rate vs #eta", 25,0.,2.5);

       m_h1 = new TH1F("m_h1","Eta of e/gamma object", 25,0.,2.5);
       m_h2 = new TH1F("m_h2","Eta of e/gamma object", 21,0.,2.1);

       m_h3 = new TH1F("m_h3","TRT 'zeros' w/o Outliers", 21,0.,2.1);
       m_h4 = new TH1F("m_h4","TRT 'zeros' w/ Outliers", 21,0.,2.1);
       m_h5 = new TH1F("m_h5","TRT hits w/o Outliers", 50,0.,50.);
       m_h6 = new TH1F("m_h6","TRT hits w/ Outliers", 50,0.,50.);
       m_h7 = new TH1F("m_h7","TR hits w/o Outliers", 20,0.,20.);
       m_h8 = new TH1F("m_h8","TR hits w/ Outliers", 20,0.,20.);
       m_h9 = new TH1F("m_h9","TR ratio w/o Outliers", 50,0.,1.);
       m_h10 = new TH1F("m_h10","TR ratio w/ Outliers", 50,0.,1.);

       m_h11 = new TH1F("m_h11","BLayer 'zeros'", 25,0.,2.5);
       m_h12 = new TH1F("m_h12","Pixel 'zeros'", 25,0.,2.5);
       m_h13 = new TH1F("m_h13","SCT 'zeros'", 25,0.,2.5);
       m_h14 = new TH1F("m_h14","Silicons 'zeros'", 25,0.,2.5);
       m_h15 = new TH1F("m_h15","BLayer hits", 5,0.,5.);
       m_h16 = new TH1F("m_h16","Pixel hits", 10,0.,10.);
       m_h17 = new TH1F("m_h17","SCT hits", 20,0.,20.);
       m_h18 = new TH1F("m_h18","Silicons hits",20,0.,20.);

       m_h19 = new TH1F("m_h19","Et of e/gamma cluster",100,0.,110000.);
       m_h104 = new TH1F("m_h104","Et of e/gamma cluster",100,0.,110000.);
       m_h20 = new TH1F("m_h20","E/P of e/gamma object",100,0.,4.5);
       m_h21 = new TH1F("m_h21","A0 of e/gamma track",100,-4.,4.);
       m_h22 = new TH1F("m_h22","dPhi of e/gamma object",100,-0.05,0.05);
       m_h23 = new TH1F("m_h23","dEta of e/gamma object",100,-0.05,0.05);

       m_h24 = new TH1F("m_h24","IsEM efficiencies", 30,0.,30.);
       m_h25 = new TH1F("m_h25","IsEM efficiency vs Eta", 25,0.,2.5);
       m_h26 = new TH1F("m_h26","Pt/PtTruth", 100,0.,2.);
 
       m_h30 = new TH1F("m_h30","E237", 150,0.,150000.);
       m_h31 = new TH1F("m_h31","E277", 150,0.,150000.);
       m_h32 = new TH1F("m_h32","Reta37", 110,0.,1.1);

       m_h33 = new TH1F("m_h33","Wtot", 400,0.,10.);
       m_h34 = new TH1F("m_h34","Weta2c", 100,0.,0.05);
       m_h35 = new TH1F("m_h35","Weta1c", 100,0.,1.);

       m_h36 = new TH1F("m_h36","Emax2", 220,-200.,2000.);
       m_h37 = new TH1F("m_h37","Emin", 220,-200.,2000.);
       m_h38 = new TH1F("m_h38","DeltaEmax2", 110,-0.1,1.);
       m_h39 = new TH1F("m_h39","DeltaE", 110,-50.,500.);

       m_h40 = new TH1F("m_h40","Fracm", 100,0.,1.);
       m_h41 = new TH1F("m_h41","EtCone20", 200,-5000.,15000.);
       m_h42 = new TH1F("m_h42","EtConeIso", 110,-0.1,1.);

     m_hlist->Add(m_h1);
     m_hlist->Add(m_h2);
     m_hlist->Add(m_h3);
     m_hlist->Add(m_h4);
     m_hlist->Add(m_h5);
     m_hlist->Add(m_h6);
     m_hlist->Add(m_h7);
     m_hlist->Add(m_h8);
     m_hlist->Add(m_h9);
     m_hlist->Add(m_h10);
     m_hlist->Add(m_h11);
     m_hlist->Add(m_h12);
     m_hlist->Add(m_h13);
     m_hlist->Add(m_h14);
     m_hlist->Add(m_h15);
     m_hlist->Add(m_h16);
     m_hlist->Add(m_h17);
     m_hlist->Add(m_h18);
     m_hlist->Add(m_h19);
     m_hlist->Add(m_h20);
     m_hlist->Add(m_h21);
     m_hlist->Add(m_h22);
     m_hlist->Add(m_h23);
     m_hlist->Add(m_h24);
     m_hlist->Add(m_h25);
     m_hlist->Add(m_h26);
     m_hlist->Add(m_h30);
     m_hlist->Add(m_h31);
     m_hlist->Add(m_h32);
     m_hlist->Add(m_h33);
     m_hlist->Add(m_h34);
     m_hlist->Add(m_h35);
     m_hlist->Add(m_h36);
     m_hlist->Add(m_h37);
     m_hlist->Add(m_h38);
     m_hlist->Add(m_h39);
     m_hlist->Add(m_h40);
     m_hlist->Add(m_h41);
     m_hlist->Add(m_h42);
     m_hlist->Add(m_h101);
     m_hlist->Add(m_h102);
     m_hlist->Add(m_h103);
     m_hlist->Add(m_h104);
     
   }

   // uses the ntuple to fill the hists which are then normalised 
   void fillHists(){
   if (m_tree == 0) return;
   // Declaration of leave types
   UInt_t          NPar;
   //   vector<long>    *Type;
   vector<float>   *PtGen;
   vector<float>   *PhiGen;
   vector<float>   *EtaGen;
   UInt_t           Trk_totalNumTracks;
   vector<float>   *Trk_d0;
   vector<float>   *Trk_pt;
   vector<long>    *Trk_truthNt;
   vector<long>    *Trk_numberOfBLayerHits;
   vector<long>    *Trk_numberOfPixelHits;
   vector<long>    *Trk_numberOfSCTHits;
   vector<long>    *Trk_numberOfTRTHits;
   vector<long>    *Trk_numberOfTRTHighThresholdHits;
   vector<long>    *Trk_numberOfTRTOutliers;
   vector<long>    *Trk_numberOfTRTHighThresholdOutliers;
   vector<long>    *Trk_ParticlePdg;
   Int_t            Ele_nc;
   vector<float>   *Ele_e;
   vector<float>   *Ele_eta2;
   vector<float>   *Ele_phi;
   vector<float>   *Ele_cl_et;
   vector<float>   *Ele_cl_eta;
   vector<long>    *Ele_IsEM;
   vector<long>    *Ele_trkmatchnt;
   vector<long>    *Ele_numtracks;
   vector<float>   *Ele_eoverp;
   vector<float>   *Ele_deta1;
   vector<float>   *Ele_dphi1;
   vector<float>   *Ele_deta2;
   vector<float>   *Ele_dphi2;
   vector<float>   *Ele_Etha1;
   vector<float>   *Ele_Etha;
   vector<float>   *Ele_Eha1;
   vector<float>   *Ele_F1;
   vector<float>   *Ele_F3;
   vector<float>   *Ele_E233;
   vector<float>   *Ele_E237;
   vector<float>   *Ele_E277;
   vector<float>   *Ele_Weta1;
   vector<float>   *Ele_Weta2;
   vector<float>   *Ele_E2ts1;
   vector<float>   *Ele_E2tsts1;
   vector<float>   *Ele_Wtots1;
   vector<float>   *Ele_Emins1;
   vector<float>   *Ele_Emaxs1;
   vector<float>   *Ele_Fracs1;
   vector<float>   *Ele_EtCone20;
 
   // List of branches
   TBranch        *b_NPar;   //!
   //   TBranch        *b_Type;   //!
   TBranch        *b_PtGen;   //!
   TBranch        *b_PhiGen;   //!
   TBranch        *b_EtaGen;   //!
   TBranch        *b_Trk_totalNumTracks;   //!
   TBranch        *b_Trk_truthNt;   //!
   TBranch        *b_Trk_d0;   //!
   TBranch        *b_Trk_pt;   //!
   TBranch        *b_Trk_numberOfBLayerHits;   //!
   TBranch        *b_Trk_numberOfPixelHits;   //!
   TBranch        *b_Trk_numberOfSCTHits;   //!
   TBranch        *b_Trk_numberOfTRTHits;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdHits;   //!
   TBranch        *b_Trk_numberOfTRTOutliers;   //!
   TBranch        *b_Trk_numberOfTRTHighThresholdOutliers;   //!
   TBranch        *b_Trk_ParticlePdg;   //!
   TBranch        *b_eg_nc;   //!
   TBranch        *b_Ele_e;   //!
   TBranch        *b_Ele_eta2;   //!
   TBranch        *b_Ele_phi;   //!
   TBranch        *b_Ele_cl_et;   //!
   TBranch        *b_Ele_cl_eta;   //!
   TBranch        *b_Ele_IsEM;   //!
   TBranch        *b_Ele_trkmatchnt;   //!
   TBranch        *b_Ele_numtracks;   //!
   TBranch        *b_Ele_eoverp;   //!
   TBranch        *b_Ele_deta1;   //!
   TBranch        *b_Ele_dphi1;   //!
   TBranch        *b_Ele_deta2;   //!
   TBranch        *b_Ele_dphi2;   //!
   TBranch        *b_Ele_Etha1;   //!
   TBranch        *b_Ele_Etha;   //!
   TBranch        *b_Ele_Eha1;   //!
   TBranch        *b_Ele_F1;   //!
   TBranch        *b_Ele_F3;   //!
   TBranch        *b_Ele_E233;   //!
   TBranch        *b_Ele_E237;   //!
   TBranch        *b_Ele_E277;   //!
   TBranch        *b_Ele_Weta1;   //!
   TBranch        *b_Ele_Weta2;   //!
   TBranch        *b_Ele_E2ts1;   //!
   TBranch        *b_Ele_E2tsts1;   //!
   TBranch        *b_Ele_Wtots1;   //!
   TBranch        *b_Ele_Emins1;   //!
   TBranch        *b_Ele_Emaxs1;   //!
   TBranch        *b_Ele_Fracs1;   //!
   TBranch        *b_Ele_EtCone20;   //!

   // Set object pointer
   PtGen = 0;
   Trk_d0 = 0;
   Trk_pt = 0;
   Trk_truthNt = 0;
   Trk_numberOfBLayerHits = 0;
   Trk_numberOfPixelHits = 0;
   Trk_numberOfSCTHits = 0;
   Trk_numberOfTRTHits = 0;
   Trk_numberOfTRTHighThresholdHits = 0;
   Trk_numberOfTRTOutliers = 0;
   Trk_numberOfTRTHighThresholdOutliers = 0;
   Trk_ParticlePdg = 0;
   Ele_e = 0;
   Ele_eta2 = 0;
   Ele_phi = 0;
   Ele_cl_et = 0;
   Ele_cl_eta = 0;
   Ele_IsEM = 0;
   Ele_trkmatchnt = 0;
   Ele_numtracks = 0;
   Ele_eoverp = 0;
   Ele_deta1 = 0;
   Ele_dphi1 = 0;
   Ele_deta2 = 0;
   Ele_dphi2 = 0;
   Ele_Etha1 = 0;
   Ele_Etha = 0;
   Ele_Eha1 = 0;
   Ele_F1 = 0;
   Ele_F3 = 0;
   Ele_E233 = 0;
   Ele_E237 = 0;
   Ele_E277 = 0;
   Ele_Weta1 = 0;
   Ele_Weta2 = 0;
   Ele_E2ts1 = 0;
   Ele_E2tsts1 = 0;
   Ele_Wtots1 = 0;
   Ele_Emins1 = 0;
   Ele_Emaxs1 = 0;
   Ele_Fracs1 = 0;
   // Set branch addresses and branch pointers
   if (!m_tree) return;

   m_tree->SetBranchAddress("NPar", &NPar, &b_NPar);
   m_tree->SetBranchAddress("Trk_totalNumTracks", &Trk_totalNumTracks, &b_Trk_totalNumTracks);
   m_tree->SetBranchAddress("Trk_truthNt", &Trk_truthNt, &b_Trk_truthNt);
   m_tree->SetBranchAddress("Trk_d0", &Trk_d0, &b_Trk_d0);
   m_tree->SetBranchAddress("Trk_pt", &Trk_pt, &b_Trk_pt);
   m_tree->SetBranchAddress("PtGen", &PtGen, &b_PtGen);
   m_tree->SetBranchAddress("Trk_numberOfBLayerHits", &Trk_numberOfBLayerHits, &b_Trk_numberOfBLayerHits);
   m_tree->SetBranchAddress("Trk_numberOfPixelHits", &Trk_numberOfPixelHits, &b_Trk_numberOfPixelHits);
   m_tree->SetBranchAddress("Trk_numberOfSCTHits", &Trk_numberOfSCTHits, &b_Trk_numberOfSCTHits);
   m_tree->SetBranchAddress("Trk_numberOfTRTHits", &Trk_numberOfTRTHits, &b_Trk_numberOfTRTHits);
   m_tree->SetBranchAddress("Trk_numberOfTRTHighThresholdHits", &Trk_numberOfTRTHighThresholdHits, &b_Trk_numberOfTRTHighThresholdHits);
   m_tree->SetBranchAddress("Trk_numberOfTRTOutliers", &Trk_numberOfTRTOutliers, &b_Trk_numberOfTRTOutliers);
   m_tree->SetBranchAddress("Trk_numberOfTRTHighThresholdOutliers", &Trk_numberOfTRTHighThresholdOutliers, &b_Trk_numberOfTRTHighThresholdOutliers);
   m_tree->SetBranchAddress("Trk_ParticlePdg", &Trk_ParticlePdg, &b_Trk_ParticlePdg);

   m_tree->SetBranchAddress("Ele_nc", &Ele_nc, &b_eg_nc);
   m_tree->SetBranchAddress("Ele_e", &Ele_e, &b_Ele_e);
   m_tree->SetBranchAddress("Ele_eta2", &Ele_eta2, &b_Ele_eta2);
   m_tree->SetBranchAddress("Ele_phi", &Ele_phi, &b_Ele_phi);
   m_tree->SetBranchAddress("Ele_cl_et", &Ele_cl_et, &b_Ele_cl_et);
   m_tree->SetBranchAddress("Ele_cl_eta", &Ele_cl_eta, &b_Ele_cl_eta);
   m_tree->SetBranchAddress("Ele_IsEM", &Ele_IsEM, &b_Ele_IsEM);
   m_tree->SetBranchAddress("Ele_trkmatchnt", &Ele_trkmatchnt, &b_Ele_trkmatchnt);
   m_tree->SetBranchAddress("Ele_numtracks", &Ele_numtracks, &b_Ele_numtracks);
   m_tree->SetBranchAddress("Ele_eoverp", &Ele_eoverp, &b_Ele_eoverp);
   m_tree->SetBranchAddress("Ele_deta1", &Ele_deta1, &b_Ele_deta1);
   m_tree->SetBranchAddress("Ele_dphi1", &Ele_dphi1, &b_Ele_dphi1);
   m_tree->SetBranchAddress("Ele_deta2", &Ele_deta2, &b_Ele_deta2);
   m_tree->SetBranchAddress("Ele_dphi2", &Ele_dphi2, &b_Ele_dphi2);
   m_tree->SetBranchAddress("Ele_Etha1", &Ele_Etha1, &b_Ele_Etha1);
   m_tree->SetBranchAddress("Ele_Etha", &Ele_Etha, &b_Ele_Etha);
   m_tree->SetBranchAddress("Ele_Eha1", &Ele_Eha1, &b_Ele_Eha1);
   m_tree->SetBranchAddress("Ele_F1", &Ele_F1, &b_Ele_F1);
   m_tree->SetBranchAddress("Ele_F3", &Ele_F3, &b_Ele_F3);
   m_tree->SetBranchAddress("Ele_E233", &Ele_E233, &b_Ele_E233);
   m_tree->SetBranchAddress("Ele_E237", &Ele_E237, &b_Ele_E237);
   m_tree->SetBranchAddress("Ele_E277", &Ele_E277, &b_Ele_E277);
   m_tree->SetBranchAddress("Ele_Weta1", &Ele_Weta1, &b_Ele_Weta1);
   m_tree->SetBranchAddress("Ele_Weta2", &Ele_Weta2, &b_Ele_Weta2);
   m_tree->SetBranchAddress("Ele_E2ts1", &Ele_E2ts1, &b_Ele_E2ts1);
   m_tree->SetBranchAddress("Ele_E2tsts1", &Ele_E2tsts1, &b_Ele_E2tsts1);
   m_tree->SetBranchAddress("Ele_Wtots1", &Ele_Wtots1, &b_Ele_Wtots1);
   m_tree->SetBranchAddress("Ele_Emins1", &Ele_Emins1, &b_Ele_Emins1);
   m_tree->SetBranchAddress("Ele_Emaxs1", &Ele_Emaxs1, &b_Ele_Emaxs1);
   m_tree->SetBranchAddress("Ele_Fracs1", &Ele_Fracs1, &b_Ele_Fracs1);
   m_tree->SetBranchAddress("Ele_EtCone20", &Ele_EtCone20, &b_Ele_EtCone20);


      Float_t eta=0;
      Long64_t IsEM=0;
      Int_t NumTRT=0;
      Int_t NumTRTOut=0;
      Int_t NumTRTTotal=0;
      Int_t Ntrack=0;
      
      Int_t Counter = 0;  

   Int_t nentries = m_tree->GetEntriesFast();
   cout<<nentries<<endl;

   Long64_t nbytes = 0, nb = 0;
   for (Int_t jentry=0; jentry<nentries;jentry++) {
     //      Long64_t ientry = LoadTree(jentry);
     //      if (ientry < 0) break;
      //      cout<<"ientry  "<<ientry<<endl;
      Int_t nc=Ele_nc;
            m_h101->Fill(nc);
      Int_t numTracks=Trk_totalNumTracks;
            m_h100->Fill(numTracks);
      //      cout<<"number of egamma objects "<<nc<<endl;
      for (Int_t inc=0;inc<nc;inc++) {
       	Int_t istrack = (*Ele_numtracks)[inc];
        if (istrack<1) continue;  
        eta =fabs( (*Ele_eta2)[inc]);
             Float_t Et=(*Ele_cl_et)[inc];
	     if (Et<17000.) continue;
	Counter++;
	  if (eta<2.47) {
            m_h1->Fill(eta);


	//-----------------IsEM efficiencies filling ------------
	  IsEM = (*Ele_IsEM)[inc];
	  //	  cout<<"IsEM = "<<IsEM<<endl;
          if ((IsEM&0x1)==0)       m_h24->Fill(1,1.);
          if ((IsEM&0x10)==0)      m_h24->Fill(4,1.);
          if ((IsEM&0x20)==0)      m_h24->Fill(5,1.);
          if ((IsEM&0x40)==0)      m_h24->Fill(6,1.);
          if ((IsEM&0x80)==0)      m_h24->Fill(7,1.);
          if ((IsEM&0x100)==0)     m_h24->Fill(8,1.);
          if ((IsEM&0x200)==0)     m_h24->Fill(9,1.);
          if ((IsEM&0x400)==0)     m_h24->Fill(10,1.);
          if ((IsEM&0x800)==0)     m_h24->Fill(11,1.);
          if ((IsEM&0x1000)==0)    m_h24->Fill(12,1.);
          if ((IsEM&0x2000)==0)    m_h24->Fill(13,1.);
          if ((IsEM&0x4000)==0)    m_h24->Fill(14,1.);
          if ((IsEM&0x10000)==0)   m_h24->Fill(16,1.);
          if ((IsEM&0x20000)==0)   m_h24->Fill(17,1.);
          if ((IsEM&0x40000)==0)   m_h24->Fill(18,1.);
          if ((IsEM&0x80000)==0)   m_h24->Fill(19,1.);
          if ((IsEM&0x100000)==0)  m_h24->Fill(20,1.);
          if ((IsEM&0x200000)==0)  m_h24->Fill(21,1.);
          if ((IsEM&0x400000)==0)  m_h24->Fill(22,1.);
	  //          if ((IsEM&0x1000000)==0) m_h24->Fill(24,1.);
          if ((IsEM&0x2000000)==0) m_h24->Fill(25,1.);
          if (IsEM==0)             m_h24->Fill(29,1.);
	  //-----------------------------------------------------
           


            if (IsEM == 0) m_h25->Fill(eta);
             Ntrack = (*Ele_trkmatchnt)[inc];


   Float_t E237 = (*Ele_E237)[inc];
   Float_t E277 = (*Ele_E277)[inc];
   Float_t Weta1c = (*Ele_Weta1)[inc];
   Float_t Weta2c = (*Ele_Weta2)[inc];
   Float_t Emax2 = (*Ele_E2tsts1)[inc];
   Float_t Wtot = (*Ele_Wtots1)[inc];
   m_h33->Fill(Wtot);
   m_h34->Fill(Weta2c);
   m_h35->Fill(Weta1c);
   Float_t Emin = (*Ele_Emins1)[inc];
   Float_t Fracm = (*Ele_Fracs1)[inc];
   m_h40->Fill(Fracm);
   m_h36->Fill(Emax2);
   m_h37->Fill(Emin);

   Float_t EtCone20 = (*Ele_EtCone20)[inc];

   Float_t EtConeIso = 0;
   if (Et != 0) EtConeIso = EtCone20/Et;
   m_h41->Fill(EtCone20);
   m_h42->Fill(EtConeIso);
   Float_t Reta37 = 0;

   Int_t NTtruth=(*Trk_truthNt)[Ntrack];
   Bool_t Electron = true;
   if (abs((*Trk_ParticlePdg)[Ntrack])!=11) Electron = false;
   if (!Electron) {
     m_h102->Fill(Et);
     m_h103->Fill(eta);
   }
   if (E277 !=0) Reta37 = E237/E277;
   m_h30->Fill(E237);
   m_h31->Fill(E277);
   m_h32->Fill(Reta37);
   if (Emax2 > 0) {
     Float_t DeltaEmax2 = Emax2/(1000. + 0.009*Et);
     Float_t DE = Emax2 - Emin;
     m_h38->Fill(DeltaEmax2);
     m_h39->Fill(DE);
   }
             Float_t EoverP=(*Ele_eoverp)[inc];
             Float_t dPhi=(*Ele_dphi2)[inc];
             Float_t dEta=(*Ele_deta1)[inc];
             Float_t A0=(*Trk_d0)[Ntrack];
             Float_t Pt=(*Trk_pt)[Ntrack];
             Float_t PtTruth=(*PtGen)[NTtruth];
             Float_t PtRes = 0;
             if (PtTruth != 0) PtRes = Pt/PtTruth;
             m_h20->Fill(EoverP);
             m_h19->Fill(Et);
             m_h104->Fill(Et);
             m_h21->Fill(A0);
             m_h22->Fill(dPhi);
             m_h23->Fill(dEta);
             m_h26->Fill(PtRes);



             Int_t NumBL =(*Trk_numberOfBLayerHits)[Ntrack];
             Int_t NumPix =(*Trk_numberOfPixelHits)[Ntrack];
             Int_t NumSCT =(*Trk_numberOfSCTHits)[Ntrack];
             Int_t NumSi = NumPix + NumSCT;
	     if (NumBL == 0)  m_h11->Fill(eta);          
	     if (NumPix == 0) m_h12->Fill(eta);          
	     if (NumSCT == 0) m_h13->Fill(eta);          
	     if (NumSi == 0)  m_h14->Fill(eta);          
             m_h15->Fill(NumBL);
             m_h16->Fill(NumPix);
             m_h17->Fill(NumSCT);
             m_h18->Fill(NumSi);
	  }
	  if (eta<2.){
            m_h2->Fill(eta);
            NumTRT =(*Trk_numberOfTRTHits)[Ntrack];
            NumTRTOut =(*Trk_numberOfTRTOutliers)[Ntrack];
            NumTRTTotal = NumTRT + NumTRTOut;
            Int_t NumTR = (*Trk_numberOfTRTHighThresholdHits)[Ntrack];
            Int_t NumTROut = (*Trk_numberOfTRTHighThresholdOutliers)[Ntrack];
            Int_t NumTRTotal = NumTR + NumTROut;
            Float_t TRTratio = 0;
            Float_t TRTTotalRatio = 0;
            if (NumTRT > 0.) TRTratio = (float)NumTR/NumTRT;
            if (NumTRTTotal > 0.) TRTTotalRatio = (float)NumTRTotal/NumTRTTotal;

            if (NumTRT == 0) m_h3->Fill(eta);
            if (NumTRTTotal == 0) m_h4->Fill(eta);

            m_h5->Fill(NumTRT);
            m_h6->Fill(NumTRTTotal);
            m_h7->Fill(NumTR);
            m_h8->Fill(NumTRTotal);
            m_h9->Fill(TRTratio);
            m_h10->Fill(TRTTotalRatio);
	  }
      }
      nb = m_tree->GetEntry(jentry);   nbytes += nb; // m_tree <-> fChain
      // if (Cut(ientry) < 0) continue;
   }
   //------------normalisation of IsEM efficiencies---------
        Double_t scale1=m_h1->GetEntries();
        Double_t scale2=m_h2->GetEntries();
     if ( m_h1->GetEntries() != 0 )
       scale1 = 1/m_h1->GetEntries();
     else
       scale=1.;
     if ( m_h2->GetEntries() != 0 )
       scale2 = 1/m_h2->GetEntries();
     else
       scale=2.;
     Int_t nd = m_h24->GetNdivisions();
     Double_t error = 0;
     Double_t value = 0;
     for (Int_t i=1;i<=nd;i++) {
       Double_t scale = scale1;
       if (i==25) scale = scale2;
       error = m_h24-> GetBinError(i);
       value = m_h24-> GetBinContent(i);
       error = error*scale;
       value = value*scale;
       m_h24->SetBinError(i,error);   
       m_h24->SetBinContent(i,value);   
     } 
     //-------------------------------------------------------
   normHE(m_h3,m_h2);
   normHE(m_h4,m_h2);
   normHE(m_h11,m_h1);
   normHE(m_h12,m_h1);
   normHE(m_h13,m_h1);
   normHE(m_h14,m_h1);
   normHE(m_h25,m_h1);
   normH(m_h5);
   normH(m_h6);
   normH(m_h7);
   normH(m_h8);
   normH(m_h9);
   normH(m_h10);
   normH(m_h15);
   normH(m_h16);
   normH(m_h17);
   normH(m_h18);
   normH(m_h19);
   normH(m_h20);
   normH(m_h21);
   normH(m_h22);
   normH(m_h23);
   normH(m_h26);
   normH(m_h30);
   normH(m_h31);
   normH(m_h32);
   normH(m_h33);
   normH(m_h34);
   normH(m_h35);
   normH(m_h36);
   normH(m_h37);
   normH(m_h38);
   normH(m_h39);
   normH(m_h40);
   normH(m_h41);
   normH(m_h42);
   normH(m_h100);
   normH(m_h101);
   normHE(m_h102,m_h104);
   normHE(m_h103,m_h1);
   cout<<"Processed  "<<Counter<<" egamma objects"<<endl;
   }

 private:
   // pointers to histogram objects
   TH1F* m_h1;
   TH1F* m_h2;
   TH1F* m_h3;
   TH1F* m_h4;
 
   TH1F* m_h5;
   TH1F* m_h6;
   TH1F* m_h7; 
   TH1F* m_h8;
 
   TH1F* m_h9;
   TH1F* m_h10;
   TH1F* m_h11; 
   TH1F* m_h12;
 
   TH1F* m_h13;
   TH1F* m_h14;
   TH1F* m_h15; 
   TH1F* m_h16;
 
   TH1F* m_h17; 
   TH1F* m_h18;
   TH1F* m_h19; 
   TH1F* m_h20;
   TH1F* m_h26;
 
   TH1F* m_h21; 
   TH1F* m_h22; 
   TH1F* m_h23; 
   TH1F* m_h24; 
   TH1F* m_h25; 

   TH1F* m_h30; 
   TH1F* m_h31; 
   TH1F* m_h32; 
   TH1F* m_h33; 
   TH1F* m_h34; 

   TH1F* m_h35; 
   TH1F* m_h36; 
   TH1F* m_h37; 
   TH1F* m_h38; 
   TH1F* m_h39; 

   TH1F* m_h40; 
   TH1F* m_h41; 
   TH1F* m_h42; 
 
   TH1F* m_h100; 
   TH1F* m_h101; 
   TH1F* m_h102; 
   TH1F* m_h103; 
   TH1F* m_h104; 
 };
 // Entrance point to the macro. 
 // Expects to find ntuple.root in current directory.
 // Outputs histograms to egammaID_validation.hist
 int egammaPerformance_ID_hist(char* ntupFile)
 {
 
   EgammaIDHists  eg(ntupFile, "egammaID_validation.hist");
 
   return 0;
 }
