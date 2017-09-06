//********************************
//Macro for EM cluster validation
//
//author: A. Kaczmarska
//date: 27.07.2004
//
// RTT adaption P.S.
// 16.12.2004 - A.K. correction of histograms 
//              ranges, soft e variables, adding
//              some new histos
//********************************

//////////////////////////////////////////////////////
//
// Base class. Leaves booking and filling to derived classes
// virtual doesnt seem to work, so have to cut and paste
// calling of the various methods to derived classes
//
//////////////////////////////////////////////////////
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
    cout<< m_hlist->GetEntriesFast() <<endl;
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

    m_hist->Scale(scale);
  }
protected:
  TObjArray* m_hlist;
  // pointer to tree read from  file
  TTree* m_tree;

};
  
class ClusterHists:public Hists{
public:
  ClusterHists(char* ntupFileName, char* histFileName):Hists(ntupFileName, histFileName){
    cout<<"ClusterHists construction"<<endl;

    this->bookHists();
    this->fillHists();
    this->writeHistsToDisk(histFileName);

  }
  void bookHists(){
    cout<<"ClusterHists booking..."<<endl;
    
    m_h1    = new TH1F("m_h1","Cluster E (GeV)",50,70.,110.);
    m_h2    = new TH1F("m_h2","Cluster E (GeV) - barrel",50,70.,110.);
    m_h3    = new TH1F("m_h3","Cluster E (GeV) - endcap",50,70.,110.);
    m_h4    = new TH1F("m_h4","Cluster eta",40,-2.5,2.5);

    m_h5    = new TH1F("m_h5","Cluster phi",25,-7.,7.);
    m_h6    = new TH1F("m_h6","number of clusters",6,-.5,5.5);
    m_h7    = new TProfile("m_h7","Cluster E (GeV) vs eta",30,-2.5,2.5,80.,110.);
    m_h8    = new TH2F("m_h8","number of clusters vs eta",50,-2.5,2.5,50,-.5,5.5);

    m_h9    = new TH1F("m_h9","E frac. in Presampler - barrel",50,0.,1.);
    m_h10   = new TH1F("m_h10","E frac. in 1st samp. - barrel",50,0.,1.);
    m_h11   = new TH1F("m_h11","E frac. in 2nd samp. - barrel",50,0.,1.);
    m_h12   = new TH1F("m_h12","E frac. in 3rd samp. - barrel",50,0.,.1);

    m_h13   = new TH1F("m_h13","E frac. in Presampler - endcap",50,0.,1.);
    m_h14   = new TH1F("m_h14","E frac. in 1st samp. - endcap",50,0.,1.);
    m_h15   = new TH1F("m_h15","E frac. in 2nd samp. - endcap",50,0.,1.);
    m_h16   = new TH1F("m_h16","E frac. in 3rd samp. - endcap",50,0.,0.1);

    m_h17    = new TH2F("m_h17","E  (GeV) vs phi : barrel+",50,0.,3.3,50,70.,110.);
    m_h18    = new TH2F("m_h18","E (GeV) vs phi : barrel-",50,0.,3.3,50,70.,110.); 
    m_h19    = new TH2F("m_h19","E  (GeV) vs phi : ec+",50,-3.3,0.,50,70.,110.);
    m_h20    = new TH2F("m_h20","E (GeV) vs phi : ec-",50,-3.3,0.,50,70.,110.); 

    m_h21    = new TH1F("m_h21","Cluster Et (GeV)",50,10.,110.);
    m_h22    = new TH1F("m_h22","Cluster Et (GeV) - barrel",50,10.,110.);
    m_h23    = new TH1F("m_h23","Cluster Et (GeV) - endcap",50,10.,110.);
    m_h24    = new TProfile("m_h24","Cluster Et (GeV) vs eta",30,-2.5,2.5,10.,110.);

    m_h25    = new TH2F("m_h25","Et  (GeV) vs phi : barrel+",50,0.,3.3,50,10.,110.);
    m_h26    = new TH2F("m_h26","Et (GeV) vs phi : barrel-",50,0.,3.3,50,10.,110.); 
    m_h27    = new TH2F("m_h27","Et  (GeV) vs phi : ec+",50,-3.3,0.,50,10.,110.);
    m_h28    = new TH2F("m_h28","Et (GeV) vs phi : ec-",50,-3.3,0.,50,10.,110.); 

    m_hlist->Add(m_h1);
    m_hlist->Add(m_h2);
    m_hlist->Add(m_h3);
    m_hlist->Add(m_h4);
    m_hlist->Add(m_h5);
    m_hlist->Add(m_h6);
    //profile m_hlist->Add(m_h7);
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
    //profile m_hlist->Add(m_h24);     
    m_hlist->Add(m_h25);     
    m_hlist->Add(m_h26);     
    m_hlist->Add(m_h27);     
    m_hlist->Add(m_h28);      
    
  }

  // uses the ntuple to fill the hists which are then normalised 
  void fillHists(){
    cout<<"Cluster hist filling..."<<endl;
    
    m_tree->Draw("cl_et*cosh(cl_eta)/1000>>m_h1","","goff");
    normH(m_h1);

    m_tree->Draw("cl_et*cosh(cl_eta)/1000>>m_h2","abs(cl_eta)<1.475","goff");
    normH(m_h2);

    m_tree->Draw("cl_et*cosh(cl_eta)/1000>>m_h3","abs(cl_eta)>=1.475","goff");
    normH(m_h3);

    m_tree->Draw("cl_et/1000>>m_h21","","goff");
    normH(m_h21);

    m_tree->Draw("cl_et/1000>>m_h22","abs(cl_eta)<1.475","goff");
    normH(m_h22);

    m_tree->Draw("cl_et/1000>>m_h23","abs(cl_eta)>=1.475","goff");
    normH(m_h23);

    m_tree->Draw("cl_eta>>m_h4","","goff");
    normH(m_h4);

    m_tree->Draw("cl_phi>>m_h5","","goff");
    normH(m_h5);

    m_tree->Draw("cl_nc>>m_h6","","goff");
    normH(m_h6);

    m_tree->Draw("cl_et*cosh(cl_eta)/1000:cl_eta>>m_h7","","goff");
 
    m_tree->Draw("cl_nc:cl_eta>>m_h8","","goff");


    m_tree->Draw("cl_eemb0/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h9","abs(cl_eta)<1.475","goff");
    normH(m_h9);

    m_tree->Draw("cl_eemb1/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h10","abs(cl_eta)<1.475","goff");
    normH(m_h10);

    m_tree->Draw("cl_eemb2/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h11","abs(cl_eta)<1.475","goff");
    normH(m_h11);

    m_tree->Draw("cl_eemb3/(cl_eemb0+cl_eemb1+cl_eemb2+cl_eemb3)>>m_h12","abs(cl_eta)<1.475","goff");
    normH(m_h12);

       
    m_tree->Draw("cl_eeme0/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h13","abs(cl_eta)>=1.475 && abs(cl_eta)<=1.8","goff");
    normH(m_h13);

    m_tree->Draw("cl_eeme1/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h14","abs(cl_eta)>=1.475","goff");
    normH(m_h14);

    m_tree->Draw("cl_eeme2/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h15","abs(cl_eta)>=1.475","goff");
    normH(m_h15);

    m_tree->Draw("cl_eeme3/(cl_eeme0+cl_eeme1+cl_eeme2+cl_eeme3)>>m_h16","abs(cl_eta)>=1.475","goff");
    normH(m_h16);

    m_tree->Draw("cl_et*cosh(cl_eta)/1000:cl_phi>>m_h17","cl_eta<1.475 && cl_eta>0","goff");
    m_tree->Draw("cl_et*cosh(cl_eta)/1000:cl_phi>>m_h18","cl_eta>-1.475 && cl_eta<0","goff");
    m_tree->Draw("cl_et*cosh(cl_eta)/1000:cl_phi>>m_h19","cl_eta>1.475","goff");
    m_tree->Draw("cl_et*cosh(cl_eta)/1000:cl_phi>>m_h20","cl_eta<-1.475","goff");
    m_tree->Draw("cl_et/1000:cl_eta>>m_h24","","goff");

    m_tree->Draw("cl_et/1000:cl_phi>>m_h25","cl_eta<1.475 && cl_eta>0","goff");
    m_tree->Draw("cl_et/1000:cl_phi>>m_h26","cl_eta>-1.475 && cl_eta<0","goff");
    m_tree->Draw("cl_et/1000:cl_phi>>m_h27","cl_eta>1.475","goff");
    m_tree->Draw("cl_et/1000:cl_phi>>m_h28","cl_eta<-1.475","goff");

  }
private:
  // pointers to histogram objects
  TH1F* m_h1;
  TH1F* m_h2;
  TH1F* m_h3;
  TH1F* m_h4;

  TH1F* m_h5;
  TH1F* m_h6;
  TProfile* m_h7; 
  TH2F* m_h8;

  TH1F* m_h9;
  TH1F* m_h10;
  TH1F* m_h11; 
  TH1F* m_h12;

  TH1F* m_h13;
  TH1F* m_h14;
  TH1F* m_h15; 
  TH1F* m_h16;

  TH2F* m_h17; 
  TH2F* m_h18;
  TH2F* m_h19; 
  TH2F* m_h20;

  TH1F* m_h21;
  TH1F* m_h22;
  TH1F* m_h23;
  TProfile* m_h24;

  TH2F* m_h25; 
  TH2F* m_h26;
  TH2F* m_h27; 
  TH2F* m_h28;

};
class SoftEHists:public Hists{
public:
  SoftEHists(char* ntupFileName, char* histFileName):Hists(ntupFileName, histFileName){
    cout<<"SoftEHists construction"<<endl;

    this->bookHists();
    this->fillHists();
    this->writeHistsToDisk(histFileName);

  }
  void bookHists(){
    cout<<"SoftEHists booking..."<<endl;

    m_h1    = new TH1F("m_h1","nTR Hits",33,0.,33.);
    m_h2    = new TH1F("m_h2","Et/pt",50,0.,3.);
    m_h3    = new TH1F("m_h3","Position match",50,-7.,7.);
    m_h4    = new TH1F("m_h4","Asy",50,0.,1.);

    m_h5    = new TH1F("m_h5","Width",50,-0.2,1.1);
    m_h6    = new TH1F("m_h6","E1/E",50,0.,1.);
    m_h7    = new TH1F("m_h7","E3/E",50,0.,.2);
    m_h8    = new TH1F("m_h8","Isolation",50,0.,1.1);

    m_h9    = new TH1F("m_h9","a0",50,0.,1.);
    m_h10   = new TH1F("m_h10","deta0",50,-4.,4.);
    m_h11   = new TH1F("m_h11","dphi0",50,-4.,4.); 
    m_h12   = new TH1F("m_h12","deta1",50,-0.007,0.007); 

    m_h13   = new TH1F("m_h13","dphi1",50,-0.08,0.08); 
    m_h14   = new TH1F("m_h14","deta2",50,-0.04,0.04); 
    m_h15   = new TH1F("m_h15","dphi2",50,-0.04,0.04); 
    m_h16   = new TH1F("m_h16","deta3",50,-0.09,0.09); 

    m_h17    = new TH1F("m_h17","dphi3",50,-0.015,0.015); 
    m_h18    = new TH1F("m_h18","E131",100,0,30.);
    m_h19    = new TH1F("m_h19","Ecore",100,0,120.); 
    m_h20    = new TH1F("m_h20","number of soft e",6,-.5,5.5); 

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

  }

  // uses the ntuple to fill the hists which are then normalised 
  void fillHists(){
    cout<<"SoftE hist filling..."<<endl;
    
    m_tree->Draw("Trk_numberOfTRTHighThresholdHits>>m_h1","","goff");
    normH(m_h1);

    m_tree->Draw("Softe_eoverp>>m_h2","","goff");
    normH(m_h2);
    
    m_tree->Draw("Softe_Pos7>>m_h3","","goff");
    normH(m_h3);

    m_tree->Draw("Softe_Asy1>>m_h4","","goff");
    normH(m_h4);
    
    m_tree->Draw("Softe_Weta1>>m_h5","","goff");
    normH(m_h5);

    m_tree->Draw("Softe_F1core>>m_h6","","goff");
    normH(m_h6);

    m_tree->Draw("Softe_F3core>>m_h7","","goff");
    normH(m_h7);

    m_tree->Draw("Softe_Isol>>m_h8","","goff");
    normH(m_h8);

    m_tree->Draw("Trk_d0>>m_h9","","goff");
    normH(m_h9);

    m_tree->Draw("Softe_deta0>>m_h10","","goff");
    normH(m_h10);

    m_tree->Draw("Softe_dphi0>>m_h11","","goff");
    normH(m_h11);

    m_tree->Draw("Softe_deta1>>m_h12","","goff");
    normH(m_h12);

    m_tree->Draw("Softe_dphi1>>m_h13","","goff");
    normH(m_h13);

    m_tree->Draw("Softe_deta2>>m_h14","","goff");
    normH(m_h14);

    m_tree->Draw("Softe_dphi2>>m_h15","","goff");
    normH(m_h15);

    m_tree->Draw("Softe_deta3>>m_h16","","goff");
    normH(m_h16);

    m_tree->Draw("Softe_dphi3>>m_h17","","goff");
    normH(m_h17);

    m_tree->Draw("Softe_E131/1000.>>m_h18","","goff");
    normH(m_h18);

    m_tree->Draw("Softe_Ecore/1000.>>m_h19","","goff");
    normH(m_h19);

    m_tree->Draw("Softe_nc>>m_h20","","goff");
    normH(m_h20);

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

};
///////////////////////////////////////////////////////////////
class EgammaHists:public Hists{
public:
  EgammaHists(char* ntupFileName, char* histFileName):Hists(ntupFileName, histFileName){
    cout<<"EgammaHists construction"<<endl;

    this->bookHists();
    this->fillHists();
    this->writeHistsToDisk(histFileName);

  }
  void bookHists(){
    cout<<"EgammaHists booking..."<<endl;

    m_h0    = new TH1F("m_h0","Barrel : non conv elec, conv elec, non conv phot, conv phot",10,0.,10.);
    m_h100    = new TH1F("m_h100","Endcap : non conv elec, conv elec, non conv phot, conv phot ",10,0.,10.);

    m_h1    = new TH1F("m_h1","Electrons E/p - barrel",50,.5,3.);
    m_h2    = new TH1F("m_h2","Electrons E/p - endcap pos",50,0.5,3.);
    m_h3    = new TH1F("m_h3","Electrons E/p - endcap neg",50,.5,3.);
    m_h4    = new TH1F("m_h4","Electrons track match : deta 1st samp",50,-0.006,0.006);

    m_h5    = new TH1F("m_h5","Electrons track match : deta 2nd samp",25,-0.006,0.006);
    m_h6    = new TH1F("m_h6","Electrons E outside core (E(+-7)-E(+-3))/E(+-7)",50,0.,2.);
    m_h7    = new TH1F("m_h7","Electrons track match : dphi 2nd samp",50,-0.02,0.02);
    m_h8    = new TH1F("m_h8","Electrons number of strips in 1st samp.",50,0.,25.);

    m_h9    = new TH1F("m_h9","Electrons uncorr width in 3 strips",50,0.,2.);
    m_h10   = new TH1F("m_h10","Electrons corrected width in 3 strips",100,0.,1.);
    m_h11   = new TH1F("m_h11","Electrons Frac. of E in 1st samp.",100,-0.1,1.);
    m_h12   = new TH1F("m_h12","Electrons Corrected width in 3x5 - 2nd samp.",50,0.,.02); 

    m_h13   = new TH1F("m_h13","Electrons E2(3x7)/E2(7x7)",50,0.4,1.);
    m_h14   = new TH1F("m_h14","Electrons E2(3x3)/E2(3x7)",50,0.4,1.);
    m_h15   = new TH1F("m_h15","Electrons Uncorr.width in 3x5 - 2nd samp.",50,0.,.02);
    m_h16   = new TH1F("m_h16","Electrons Et 3x7",100,-1.,100.); 

    m_h17    = new TH1F("m_h17","Electrons E max2",100,0.,0.5); 
    m_h18    = new TH1F("m_h18","Electrons ET max2",100,0.,0.5);
    m_h19    = new TH1F("m_h19","Electrons E min",100,-0.1,0.5);
    m_h20    = new TH1F("m_h20","Electrons E max2 - E min",100,0.,0.2);

    m_h21    = new TProfile("m_h21","Electrons E in S0 (GeV) vs eta",50,0.,2.5,0.,60.);
    m_h22    = new TProfile("m_h22","Electrons E in S1 (GeV) vs eta",50,0.,2.5,0.,70.);
    m_h23    = new TProfile("m_h23","Electrons E in S2 (GeV) vs eta",50,0.,2.5,1.,110.);
    m_h24    = new TProfile("m_h24","Electrons E in S3 (GeV) vs eta",50,0.,2.5,0.,10.);

    m_h25    = new TH1F("m_h25","Electrons ET leak. in 1st samp. HAD (MeV)",100,-1000.,1000.);
    m_h26    = new TH1F("m_h26","Electrons ET leakage into had calo (Mev)",100,-1000.,1000.);
    m_h27    = new TH2F("m_h27","Electrons Depth vs eta - barrel",50,-1.7,1.7,100,1450.,1700.);
    m_h28    = new TH2F("m_h28","Electrons |Depth| vs eta - endcap",50,-2.7,2.7,100,3600.,4000.);

    m_h29    = new TH1F("m_h29"," ",20,-2.5,2.5);
    m_h30    = new TH1F("m_h30"," ",20,-2.5,2.5);
    m_h31    = new TH1F("m_h31","Electrons Efficiency of e ID vs eta",20,-2.5,2.5);

    m_h32    = new TH1F("m_h32","Electrons Efficiency : per bit ",30,0.,30.);
    m_h37    = new TH1F("m_h37","Electrons Efficiency : marginal ",30,0.,30.);
    m_h1032    = new TH1F("m_h1032","Electrons dummy",1,0.,1.);

    m_h33    = new TH1F("m_h33","Electrons energy of C and ID",110,10.,120.);
    m_h34    = new TH1F("m_h34","Electrons eta of C and ID",52,-2.6,2.6);
    m_h35    = new TH1F("m_h35","Electrons phi of C and ID",66,-3.3,3.3);
    m_h36    = new TH1F("m_h36","Electrons Et of C and ID",100,10.,110.);

    m_h38    = new TH1F("m_h38","Electrons eta truth-calo",100,-0.25,0.25);

    m_h39    = new TH1F("m_h39","Electrons NN",120,-0.1,1.1);
    m_h40    = new TH1F("m_h40","Electrons EM / EM+Pion weight ",120,-0.1,1.1);
    m_h41    = new TH1F("m_h41","Electrons Hmatrix5 ",100,0.,50.);
    m_h42    = new TH1F("m_h42","Electrons Hmatrix10 ",100,0.,50.);
    m_h43    = new TH1F("m_h43","Electrons isol likelihood ",100,0.,50.);
    m_h44    = new TH1F("m_h44","Electrons IsEM",100,0.,1.e+7);

    m_h45    = new TH1F("m_h45","Electrons IsEMse",100,0.,1.e+7);
    m_h46    = new TH1F("m_h46","Electrons NNse",120,-0.1,1.1);
    m_h47    = new TH1F("m_h47","Electrons EMse / EMse+Pionse weight ",120,-0.1,1.1);

    // same for photons :

    m_h101    = new TH1F("m_h101","Photons E/p - barrel",50,.5,3.);
    m_h102    = new TH1F("m_h102","Photons E/p - endcap pos",50,0.5,3.);
    m_h103    = new TH1F("m_h103","Photons E/p - endcap neg",50,.5,3.);
    m_h104    = new TH1F("m_h104","Photons track match : deta 1st samp",50,-0.006,0.006);

    m_h105    = new TH1F("m_h105","Photons track match : deta 2nd samp",25,-0.006,0.006);
    m_h106    = new TH1F("m_h106","Photons E outside core (E(+-7)-E(+-3))/E(+-7)",50,0.,2.);
    m_h107    = new TH1F("m_h107","Photons track match : dphi 2nd samp",50,-0.02,0.02);
    m_h108    = new TH1F("m_h108","Photons number of strips in 1st samp.",50,0.,25.);

    m_h109    = new TH1F("m_h109","Photons uncorr width in 3 strips",50,0.,2.);
    m_h110   = new TH1F("m_h110","Photons corrected width in 3 strips",100,0.,1.);
    m_h111   = new TH1F("m_h111","Photons Frac. of E in 1st samp.",100,-0.1,1.);
    m_h112   = new TH1F("m_h112","Photons Corrected width in 3x5 - 2nd samp.",50,0.,.02); 

    m_h113   = new TH1F("m_h113","Photons E2(3x7)/E2(7x7)",50,0.4,1.);
    m_h114   = new TH1F("m_h114","Photons E2(3x3)/E2(3x7)",50,0.4,1.);
    m_h115   = new TH1F("m_h115","Photons Uncorr. width in 3x5 - 2nd samp.",50,0.,.02);
    m_h116   = new TH1F("m_h116","Photons Et 3x7",100,-1.,100.); 

    m_h117    = new TH1F("m_h117","Photons E max2",100,0.,0.5); 
    m_h118    = new TH1F("m_h118","Photons ET max2",100,0.,0.5);
    m_h119    = new TH1F("m_h119","Photons E min",100,-0.1,0.5);
    m_h120    = new TH1F("m_h120","Photons E max2 -E min",100,0.,0.2);

    m_h121    = new TProfile("m_h121","Photons E in S0 (GeV) vs eta",50,0.,2.5,0.,60.);
    m_h122    = new TProfile("m_h122","Photons E in S1 (GeV) vs eta",50,0.,2.5,0.,70.);
    m_h123    = new TProfile("m_h123","Photons E in S2 (GeV) vs eta",50,0.,2.5,1.,110.);
    m_h124    = new TProfile("m_h124","Photons E in S3 (GeV) vs eta",50,0.,2.5,0.,10.);

    m_h125    = new TH1F("m_h125","Photons ET leak. in 1st samp. HAD (MeV)",100,-1000.,1000.);
    m_h126    = new TH1F("m_h126","Photons ET leakage into had calo (Mev)",100,-1000.,1000.);
    m_h127    = new TH2F("m_h127","Photons Depth vs eta - barrel",50,-1.7,1.7,100,1450.,1700.);
    m_h128    = new TH2F("m_h128","Photons |Depth| vs eta - endcap",50,-2.7,2.7,100,3600.,4000.);

    m_h129    = new TH1F("m_h129"," ",20,-2.5,2.5);
    m_h130    = new TH1F("m_h130"," ",20,-2.5,2.5);
    m_h131    = new TH1F("m_h131","Photons Efficiency of e ID vs eta",20,-2.5,2.5);

    m_h132    = new TH1F("m_h132","Photons Efficiency : per bit ",30,0.,30.);
    m_h137    = new TH1F("m_h137","Photons Efficiency : marginal ",30,0.,30.);
    m_h1132    = new TH1F("m_h1132","Photons dummy",1,0.,1.);

    m_h133    = new TH1F("m_h133","Photons energy of C and ID",110,10.,120.);
    m_h134    = new TH1F("m_h134","Photons eta of C and ID",52,-2.6,2.6);
    m_h135    = new TH1F("m_h135","Photons phi of C and ID",66,-3.3,3.3);
    m_h136    = new TH1F("m_h136","Photons Et of C and ID",100,10.,110.);

    m_h138    = new TH1F("m_h138","Photons eta truth-calo",100,-0.25,0.25);

    m_h139    = new TH1F("m_h139","Photons NN",120,-0.1,1.1);
    m_h140    = new TH1F("m_h140","Photons EM / EM+Pion weight ",120,-0.1,1.1);
    m_h141    = new TH1F("m_h141","Photons Hmatrix5 ",100,0.,50.);
    m_h142    = new TH1F("m_h142","Photons Hmatrix10 ",100,0.,50.);
    m_h143    = new TH1F("m_h143","Photons isol likelihood ",100,0.,50.);
    m_h144    = new TH1F("m_h144","Photons IsEM",100,0.,1.e+7);

    m_h145    = new TH1F("m_h145","Photons IsEMse",100,0.,1.e+7);
    m_h146    = new TH1F("m_h146","Photons NNse",120,-0.1,1.1);
    m_h147    = new TH1F("m_h147","Photons EMse / EMse+Pionse weight ",120,-0.1,1.1);

    // eg_truth :

    m_h200    = new TH1F("m_h200","egtruth eta",70,-3.5,3.5);
    m_h201    = new TH1F("m_h201","egtruth phi",70,-3.5,3.5);
    m_h202    = new TH1F("m_h202","egtruth etaCalo : all ",14,-1200,200.);
    m_h203    = new TH1F("m_h203","egtruth etaCalo: sucessful extrapol.",70,-3.5,3.5);


    // this drives the ordering in the hist and PS files : 

    // overal numbers 
    m_hlist->Add(m_h0);
    m_hlist->Add(m_h100);
    m_hlist->Add(m_h33);
    m_hlist->Add(m_h133);

    //  Electrons 

    // overall eta, phi, energy
    m_hlist->Add(m_h34);
    m_hlist->Add(m_h35);
    m_hlist->Add(m_h31);
    m_hlist->Add(m_h36);

    // Et , e/p
    m_hlist->Add(m_h16);
    m_hlist->Add(m_h1);
    m_hlist->Add(m_h2);
    m_hlist->Add(m_h3);

    // track match
    m_hlist->Add(m_h38);
    m_hlist->Add(m_h4);
    m_hlist->Add(m_h5);
    m_hlist->Add(m_h7);

    // energy per layer 
    //profile m_hlist->Add(m_h21);
    //profile m_hlist->Add(m_h22);
    //profile m_hlist->Add(m_h23);
    //profile m_hlist->Add(m_h24);

    // energy fraction
    m_hlist->Add(m_h11);
    m_hlist->Add(m_h6);
    m_hlist->Add(m_h13);
    m_hlist->Add(m_h14);

    // width in strips
    m_hlist->Add(m_h9);
    m_hlist->Add(m_h10);
    m_hlist->Add(m_h15);
    m_hlist->Add(m_h12);

    // second max
    m_hlist->Add(m_h8);
    m_hlist->Add(m_h17);
    m_hlist->Add(m_h19);
    m_hlist->Add(m_h20);

    // Leakage & eff
    m_hlist->Add(m_h25);
    m_hlist->Add(m_h26);
    //m_hlist->Add(m_h44);
    m_hlist->Add(m_h32);
    m_hlist->Add(m_h37);

    // New tagging variables
    m_hlist->Add(m_h39);
    m_hlist->Add(m_h40);
    m_hlist->Add(m_h41);
    m_hlist->Add(m_h42);

    m_hlist->Add(m_h43);
    m_hlist->Add(m_h45);
    m_hlist->Add(m_h46);
    m_hlist->Add(m_h47);

    //m_hlist->Add(m_h27);
    //m_hlist->Add(m_h28);
    //m_hlist->Add(m_h18);

    // Photons :

    // overall eta, phi, efficiency
    m_hlist->Add(m_h134);
    m_hlist->Add(m_h135);
    m_hlist->Add(m_h131);
    m_hlist->Add(m_h136);

    // energy , e/p
    m_hlist->Add(m_h116);
    m_hlist->Add(m_h101);
    m_hlist->Add(m_h102);
    m_hlist->Add(m_h103);

    // track match
    m_hlist->Add(m_h138);
    m_hlist->Add(m_h104);
    m_hlist->Add(m_h105);
    m_hlist->Add(m_h107);

    // Efficiency & leakage
    m_hlist->Add(m_h125);
    m_hlist->Add(m_h126);
    m_hlist->Add(m_h132);
    m_hlist->Add(m_h137);

    // energy per layer 
    //profile m_hlist->Add(m_h121);
    //profile m_hlist->Add(m_h122);
    //profile  m_hlist->Add(m_h123);
    //profile m_hlist->Add(m_h124);

    // energy fraction
    m_hlist->Add(m_h111);
    m_hlist->Add(m_h106);
    m_hlist->Add(m_h113);
    m_hlist->Add(m_h114);

    // width in strips
    m_hlist->Add(m_h109);
    m_hlist->Add(m_h110);
    m_hlist->Add(m_h115);
    m_hlist->Add(m_h112);

    // second max
    m_hlist->Add(m_h108);
    m_hlist->Add(m_h117);
    m_hlist->Add(m_h119);
    m_hlist->Add(m_h120);

    // New tagging variables
    m_hlist->Add(m_h139);
    m_hlist->Add(m_h140);
    m_hlist->Add(m_h141);
    m_hlist->Add(m_h142);

    m_hlist->Add(m_h143);
    m_hlist->Add(m_h145);
    m_hlist->Add(m_h146);
    m_hlist->Add(m_h147);

    //m_hlist->Add(m_h127);
    //m_hlist->Add(m_h128); 
    //m_hlist->Add(m_h118);

    // truth
    m_hlist->Add(m_h200);
    m_hlist->Add(m_h201);
    m_hlist->Add(m_h202);
    m_hlist->Add(m_h203);

  }

  void fillHists(){
    cout<<"Egamma hist filling..."<<endl;

    // Number of entries - barrel
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_convTrkMatch==0 && abs(Ele_eta)<1.475","goff");
    double ele = m_h1032->GetEntries();
    //cout << " Barrel : number of non converted electrons " << ele << endl;
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_convTrkMatch>0 && abs(Ele_eta)<1.475","goff");
    double eleconv = m_h1032->GetEntries();
    //cout << " Barrel : number of converted electrons " << eleconv << endl;
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_convTrkMatch==0 && abs(Pho_eta)<1.475","goff");
    double pho = m_h1032->GetEntries();
    //cout << " Barrel : number of non converted photons " << pho << endl;
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_convTrkMatch>0 && abs(Pho_eta)<1.475","goff");
    double phoconv = m_h1032->GetEntries();
    //cout << " Barrel : number of converted photons " << phoconv << endl;
    m_h0->Fill(1,ele);
    m_h0->Fill(2,eleconv);
    m_h0->Fill(5,pho);
    m_h0->Fill(6,phoconv);

    // endcap
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_convTrkMatch==0 && abs(Ele_eta)>=1.475","goff");
    ele = m_h1032->GetEntries();
    //cout << " EC : number of non converted electrons " << ele << endl;
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_convTrkMatch>0 && abs(Ele_eta)>=1.475","goff");
    eleconv = m_h1032->GetEntries();
    //cout << " EC : number of converted electrons " << eleconv << endl;
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_convTrkMatch==0 && abs(Pho_eta)<1.475","goff");
    pho = m_h1032->GetEntries();
    //cout << " EC : number of non converted photons " << pho << endl;
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_convTrkMatch>0 && abs(Pho_eta)>=1.475","goff");
    phoconv = m_h1032->GetEntries();
    //cout << " EC : number of converted photons " << phoconv << endl;
    m_h100->Fill(1,ele);
    m_h100->Fill(2,eleconv);
    m_h100->Fill(5,pho);
    m_h100->Fill(6,phoconv);

    m_tree->Draw("Ele_eoverp>>m_h1","abs(Ele_eta)<1.475","goff");
    m_tree->Draw("Pho_eoverp>>m_h101","abs(Pho_eta)<1.475","goff");
    normH(m_h1);
    normH(m_h101);

    m_tree->Draw("Ele_eoverp>>m_h2","Ele_eta>1.475","goff");
    m_tree->Draw("Pho_eoverp>>m_h102","Pho_eta>1.475","goff");
    normH(m_h2);
    normH(m_h102);
    
    m_tree->Draw("Ele_eoverp>>m_h3","Ele_eta<-1.475","goff");
    m_tree->Draw("Pho_eoverp>>m_h103","Pho_eta<-1.475","goff");
    normH(m_h3);
    normH(m_h103);

    m_tree->Draw("Ele_deta1>>m_h4","","goff");
    m_tree->Draw("Pho_deta1>>m_h104","","goff");
    normH(m_h4);
    normH(m_h104);

    m_tree->Draw("Ele_deta2>>m_h5","","goff");
    m_tree->Draw("Pho_deta2>>m_h105","","goff");
    normH(m_h5);
    normH(m_h105);

    m_tree->Draw("Ele_Fracs1>>m_h6","","goff");
    m_tree->Draw("Pho_Fracs1>>m_h106","","goff");
    normH(m_h6);
    normH(m_h106);

    m_tree->Draw("Ele_dphi2>>m_h7","","goff");
    m_tree->Draw("Pho_dphi2>>m_h107","","goff");
    normH(m_h7);
    normH(m_h107);

    m_tree->Draw("Ele_Wtots1>>m_h8","Ele_Wtots1>-9999","goff");
    m_tree->Draw("Pho_Wtots1>>m_h108","Pho_Wtots1>-9999","goff");
    normH(m_h8);
    normH(m_h108);

    m_tree->Draw("Ele_Widths1>>m_h9","","goff");
    m_tree->Draw("Pho_Widths1>>m_h109","","goff");
    normH(m_h9);
    normH(m_h109);

    m_tree->Draw("Ele_Weta1>>m_h10","","goff");
    m_tree->Draw("Pho_Weta1>>m_h110","","goff");
    normH(m_h10);
    normH(m_h110);

    m_tree->Draw("Ele_F1>>m_h11","","goff");
    m_tree->Draw("Pho_F1>>m_h111","","goff");
    normH(m_h11);
    normH(m_h111);

    m_tree->Draw("Ele_Weta2>>m_h12","","goff");
    m_tree->Draw("Pho_Weta2>>m_h112","","goff");
    normH(m_h12);
    normH(m_h112);

    m_tree->Draw("Ele_E237/Ele_E277>>m_h13","Ele_E277>0","goff");
    m_tree->Draw("Pho_E237/Pho_E277>>m_h113","Pho_E277>0","goff");
    normH(m_h13);
    normH(m_h113);

    m_tree->Draw("Ele_E233/Ele_E237>>m_h14","Ele_E237>0","goff");
    m_tree->Draw("Pho_E233/Pho_E237>>m_h114","Pho_E237>0","goff");
    normH(m_h14);
    normH(m_h114);

    m_tree->Draw("Ele_Widths2>>m_h15","","goff");
    m_tree->Draw("Pho_Widths2>>m_h115","","goff");
    normH(m_h15);
    normH(m_h115);

    m_tree->Draw("Ele_E237/(cosh(Ele_eta2)*1000.)>>m_h16","","goff");
    m_tree->Draw("Pho_E237/(cosh(Pho_eta2)*1000.)>>m_h116","","goff");
    normH(m_h16);
    normH(m_h116);

    m_tree->Draw("Ele_E2tsts1/1000.>>m_h17","","goff");
    m_tree->Draw("Pho_E2tsts1/1000.>>m_h117","","goff");
    normH(m_h17);
    normH(m_h117);

    m_tree->Draw("Ele_E2tsts1/(exp(-Ele_eta2)+exp(Ele_eta2))/2./1000.>>m_h18","","goff");
    m_tree->Draw("Pho_E2tsts1/(exp(-Pho_eta2)+exp(Pho_eta2))/2./1000.>>m_h118","","goff");
    normH(m_h18);
    normH(m_h118);

    m_tree->Draw("Ele_Emins1/1000.>>m_h19","","goff");
    m_tree->Draw("Pho_Emins1/1000.>>m_h119","","goff");
    normH(m_h19);
    normH(m_h119);

    m_tree->Draw("(Ele_E2tsts1-Ele_Emins1)/1000.>>m_h20","","goff");
    m_tree->Draw("(Pho_E2tsts1-Pho_Emins1)/1000.>>m_h120","","goff");
    normH(m_h20);
    normH(m_h120);

    m_tree->Draw("Ele_e0/1000:Ele_eta>>m_h21","","goff");
    m_tree->Draw("Pho_e0/1000:Pho_eta>>m_h121","","goff");

    m_tree->Draw("Ele_e1/1000:Ele_eta>>m_h22","","goff");
    m_tree->Draw("Pho_e1/1000:Pho_eta>>m_h122","","goff");

    m_tree->Draw("Ele_e2/1000:Ele_eta>>m_h23","","goff");
    m_tree->Draw("Pho_e2/1000:Pho_eta>>m_h123","","goff");

    m_tree->Draw("Ele_e3/1000:Ele_eta>>m_h24","","goff");
    m_tree->Draw("Pho_e3/1000:Pho_eta>>m_h124","","goff");

    m_tree->Draw("Ele_Etha1>>m_h25","","goff");
    m_tree->Draw("Pho_Etha1>>m_h125","","goff");
    normH(m_h25);
    normH(m_h125);

    m_tree->Draw("Ele_Etha>>m_h26","","goff");
    m_tree->Draw("Pho_Etha>>m_h126","","goff");
    normH(m_h26);
    normH(m_h126);

    m_tree->Draw("Ele_depth:Ele_eta>>m_h27","abs(Ele_eta)<1.475","goff");
    m_tree->Draw("Pho_depth:Pho_eta>>m_h127","abs(Pho_eta)<1.475","goff");

    m_tree->Draw("abs(Ele_depth):Ele_eta>>m_h28","abs(Ele_eta)>=1.475","goff");
    m_tree->Draw("abs(Pho_depth):Pho_eta>>m_h128","abs(Pho_eta)>=1.475","goff");

    m_tree->Draw("Ele_eta>>m_h29","","goff");
    m_tree->Draw("Pho_eta>>m_h129","","goff");
    m_tree->Draw("Ele_eta>>m_h30","Ele_IsEM==0","goff");
    m_tree->Draw("Pho_eta>>m_h130","Pho_IsEM==0","goff");
    m_tree->Draw("Ele_eta>>m_h31","","goff");
    m_tree->Draw("Pho_eta>>m_h131","","goff");
    m_h31->Divide(m_h30,m_h29,1,1);  
    m_h131->Divide(m_h130,m_h129,1,1);  

    m_tree->Draw("Ele_epiNN>>m_h39","","goff");
    m_tree->Draw("Pho_epiNN>>m_h139","","goff");
    m_tree->Draw("Ele_EMWeight/(Ele_EMWeight+Ele_PionWeight)>>m_h40","","goff");
    m_tree->Draw("Pho_EMWeight/(Pho_EMWeight+Pho_PionWeight)>>m_h140","","goff");
    m_tree->Draw("Ele_Hmatrix5>>m_h41","","goff");
    m_tree->Draw("Pho_Hmatrix5>>m_h141","","goff");
    m_tree->Draw("Ele_Hmatrix10>>m_h42","","goff");
    m_tree->Draw("Pho_Hmatrix10>>m_h142","","goff");
    m_tree->Draw("Ele_IsolationLikelihood>>m_h43","","goff");
    m_tree->Draw("Pho_IsolationLikelihood>>m_h143","","goff");
    m_tree->Draw("Ele_IsEM*1.>>m_h44","","goff");
    m_tree->Draw("Pho_IsEM*1.>>m_h144","","goff");

    m_tree->Draw("Ele_IsEM*1.>>m_h45","","goff");
    m_tree->Draw("Pho_IsEM*1.>>m_h145","","goff");
    m_tree->Draw("Ele_epiNN>>m_h46","","goff");
    m_tree->Draw("Pho_epiNN>>m_h46","","goff");
    m_tree->Draw("Ele_EMWeight/(Ele_EMWeight+Ele_PionWeight)>>m_h47","","goff");
    m_tree->Draw("Pho_EMWeight/(Pho_EMWeight+Pho_PionWeight)>>m_h147","","goff");

    double scale = 1.;
    double eff=0.;

    // ------ Electrons efficiency --------

    m_tree->Draw("Ele_IsEM>>m_h1032","","goff");
    if (m_h1032->GetEntries() > 0)
      scale = 1/m_h1032->GetEntries();
    else
      scale = 1.;
    //cout << " electron : total " << m_h1032->GetEntries() << " scale= " << scale << endl;

    /*
    // 12.0.6 definitions : loose
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x7)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(1,eff);
    // 12.0.6 definitions : medium
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x4FF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(2,eff);
    // 12.0.6 definitions : tight
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_IsEM==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(3,eff);
    */

    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x1)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(0,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x2)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(1,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x10)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(2,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x20)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(3,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x40)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(4,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x80)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(5,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x100)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(6,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x200)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(7,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x400)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(8,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x800)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(9,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x1000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(10,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x2000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(11,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x10000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(15,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x20000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(12,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x40000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(13,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x80000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(14,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x100000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(16,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x200000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(17,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x400000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(18,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x800000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(19,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0x1000000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(20,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_IsEM ==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h32->Fill(21,eff);

    // ------- Electrons : "marginal" ----------

    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFFFE)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(0,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFFFD)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(1,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFFEF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(2,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFFDF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(3,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFFBF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(4,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFF7F)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(5,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFEFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(6,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFDFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(7,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFFBFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(8,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFF7FF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(9,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFEFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(10,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFFDFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(11,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFEFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(15,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFDFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(12,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFFBFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(13,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFF7FFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(14,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFEFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(16,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFDFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(17,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFFBFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(18,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFF7FFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(19,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","(Ele_IsEM & 0xFEFFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(20,eff);
    m_tree->Draw("Ele_IsEM>>m_h1032","Ele_IsEM ==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h37->Fill(21,eff);

    // -------- Photons : efficiency ------

    m_tree->Draw("Pho_IsEM>>m_h1032","","goff");
    if (m_h1032->GetEntries() > 0)
      scale = 1/m_h1032->GetEntries();
    else
      scale = 1.;

    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x1)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(0,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x2)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(1,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x10)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(2,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x20)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(3,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x40)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(4,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x80)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(5,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x100)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(6,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x200)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(7,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x400)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(8,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x800)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(9,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x1000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(10,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x2000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(11,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x10000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(15,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x20000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(12,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x40000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(13,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x80000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(14,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x100000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(16,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x200000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(17,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x400000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(18,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x800000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(19,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0x1000000)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(20,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_IsEM ==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h132->Fill(21,eff);

    // ------- Photons : "marginal" ----------

    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFFFE)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(0,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFFFD)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(1,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFFEF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(2,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFFDF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(3,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFFBF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(4,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFF7F)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(5,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFEFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(6,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFDFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(7,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFFBFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(8,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFF7FF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(9,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFEFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(10,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFFDFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(11,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFEFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(15,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFDFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(12,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFFBFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(13,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFF7FFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(14,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFEFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(16,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFDFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(17,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFFBFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(18,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFF7FFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(19,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","(Pho_IsEM & 0xFEFFFFFF)==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(20,eff);
    m_tree->Draw("Pho_IsEM>>m_h1032","Pho_IsEM ==0 ","goff");
    eff = scale*m_h1032->GetEntries();
    m_h137->Fill(21,eff);

    //------------------------------------------

    m_tree->Draw("Ele_e/1000.>>m_h33","","goff");
    normH(m_h33);
    m_tree->Draw("Pho_e/1000.>>m_h133","","goff");
    normH(m_h133);

    m_tree->Draw("Ele_eta>>m_h34","","goff");
    normH(m_h34);
    m_tree->Draw("Pho_eta>>m_h134","","goff");
    normH(m_h134);

    m_tree->Draw("Ele_phi>>m_h35","","goff");
    normH(m_h35);
    m_tree->Draw("Pho_phi>>m_h135","","goff");
    normH(m_h135);

    m_tree->Draw("Ele_e/(cosh(Ele_eta)*1000.)>>m_h36","","goff");
    normH(m_h36);
    m_tree->Draw("Pho_e/(cosh(Pho_eta)*1000.)>>m_h136","","goff");
    normH(m_h136);

    m_tree->Draw("egtruth_etaCalo-Ele_eta>>m_h38","","goff");
    normH(m_h38);
    m_tree->Draw("egtruth_etaCalo-Pho_eta>>m_h138","","goff");
    normH(m_h138);


    // --------- truth ---------------

    m_tree->Draw("egtruth_eta>>m_h200","","goff");
    m_tree->Draw("egtruth_phi>>m_h201","","goff");
    m_tree->Draw("egtruth_etaCalo>>m_h202","","goff");
    m_tree->Draw("egtruth_etaCalo>>m_h203","egtruth_etaCalo>-10.","goff");


  }
private:
  // pointers to histogram objects
  TH1F* m_h0;
  TH1F* m_h100;

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

  TProfile* m_h21; 
  TProfile* m_h22;
  TProfile* m_h23;
  TProfile* m_h24;

  TH1F* m_h25; 
  TH1F* m_h26;

  TH2F* m_h27; 
  TH2F* m_h28;

  TH1F* m_h29;
  TH1F* m_h30;
  TH1F* m_h31; 
  TH1F* m_h32;
  TH1F* m_h1032;
  TH1F* m_h33;
  TH1F* m_h34;
  TH1F* m_h35;
  TH1F* m_h37;
  TH1F* m_h38;

  TH1F* m_h101;
  TH1F* m_h102;
  TH1F* m_h103;
  TH1F* m_h104;

  TH1F* m_h105;
  TH1F* m_h106;
  TH1F* m_h107; 
  TH1F* m_h108;

  TH1F* m_h109;
  TH1F* m_h110;
  TH1F* m_h111; 
  TH1F* m_h112;

  TH1F* m_h113;
  TH1F* m_h114;
  TH1F* m_h115; 
  TH1F* m_h116;

  TH1F* m_h117; 
  TH1F* m_h118;
  TH1F* m_h119; 
  TH1F* m_h120;

  TProfile* m_h121; 
  TProfile* m_h122;
  TProfile* m_h123;
  TProfile* m_h124;

  TH1F* m_h125; 
  TH1F* m_h126;

  TH2F* m_h127; 
  TH2F* m_h128;

  TH1F* m_h129;
  TH1F* m_h130;
  TH1F* m_h131; 
  TH1F* m_h132;
  TH1F* m_h1132;
  TH1F* m_h133;
  TH1F* m_h134;
  TH1F* m_h135;
  TH1F* m_h136;
  TH1F* m_h137;
  TH1F* m_h138;

};

// Entrance point to the macro. 
// Expects to find ntuple.root in current directory.
// Outputs histograms to cluster_validation.hist
int egammaPerformance_hist(char* ntupFile)
{

  ClusterHists ch(ntupFile, "cluster_validation.hist");
  SoftEHists   se(ntupFile, "softE_validation.hist");
  EgammaHists  eg(ntupFile, "eGamma_validation.hist");

  return 0;
}

 
