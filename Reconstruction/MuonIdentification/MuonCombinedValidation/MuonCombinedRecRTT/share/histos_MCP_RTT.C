#include "histos_MCP_RTT.h"
#include <TCanvas.h>
#include <math.h>
#include <TStyle.h>

histos::histos() {

}

histos::histos(TTree *tree) : physics(tree) {
  cout << "Initializing the constructor" << endl;
  Init();
}

void histos::Init() {
  variables.push_back("pt");  variables.push_back("eta");
  variables.push_back("phi");

  subdetectors.push_back("pix"); subdetectors.push_back("sct");
  subdetectors.push_back("trt"); subdetectors.push_back("csc");
  subdetectors.push_back("mdt"); subdetectors.push_back("tgc");
  subdetectors.push_back("rpc");

}

histos::~histos() { 

  list<TObject *>::iterator iter;
  cout << "in destructor " << endl;
  file_out->cd();
  all_histos.sort(); all_histos.unique();
  cout << "after sorting and uniquing" << endl;
  for(iter=all_histos.begin(); iter!=all_histos.end(); ++iter) {
    (*iter)->Write();
    //    cout << "deleting " << *iter << endl;
    //  cout << "deleting" << (*iter)->GetName() << endl;
  }
  file_out->Close();
}

void histos::run() {
  cout << "running ... " << endl;
   book();
   cout << "looping .... " << endl;
   Loop();
   cout << "after loopinf" << endl;
   cout << "printing" << endl;
   print();

}

void histos::post_process() {
  list<string>::iterator var_iter;
  list<string>::iterator sub_iter;
  list<string>::iterator iter;

  for(iter = algorithm_list.begin();iter !=algorithm_list.end();++iter) {
    for(sub_iter=subdetectors.begin();sub_iter!=subdetectors.end(); ++sub_iter) {
      for(var_iter=variables.begin();var_iter!=variables.end();++var_iter) {
        TH2F *pointer;
        if(*var_iter=="pt") {
	  pointer = nhit_pt[*sub_iter+"_"+(*iter)];
        } else if(*var_iter=="eta") {
	  pointer = nhit_eta[*sub_iter+"_"+(*iter)];
        } else if(*var_iter=="phi") {
	  pointer = nhit_phi[*sub_iter+"_"+(*iter)];
        }

        nhit_prof[*var_iter+"_"+*sub_iter+"_"+*iter] = pointer->ProfileX();

      }  
    }  
  }

}

void histos::print() {
  cout << "Printing histograms" << endl;
  list<TObject *>::iterator iter;
  gStyle->SetPalette(1);
  
  for(iter=histo_to_print.begin(); iter!=histo_to_print.end(); ++iter) {
    TCanvas *canvas = new TCanvas("canvas",(*iter)->GetTitle());
    cout << "String = " << ((string) (*iter)->GetName()).substr(0,4) << endl;
    if(((string) (*iter)->GetName()).substr(0,5)=="Delta") {
      (*iter)->Draw("COLZ");
    } else {
      (*iter)->Draw();
    }
    canvas->Update();
    string name = ((string) (*iter)->GetName())+".gif";
    
    canvas->SaveAs(name.c_str());
    delete canvas;
    
  }
}
void histos::book() {
  algorithm_list.push_back("Staco_CB");
  algorithm_list.push_back("MuonBoy");
  algorithm_list.push_back("MuTag");
  algorithm_list.push_back("MuTagIMO");
  algorithm_list.push_back("Muid_CB");
  algorithm_list.push_back("MuidSA");
  algorithm_list.push_back("MuGirl");
  algorithm_list.push_back("Muid_Tight");
  algorithm_list.push_back("CaloMuon");

  file_out = new TFile("histos_MCP_RTT.root","NEW");

  list<string>::iterator iter;
  for(iter = algorithm_list.begin();iter !=algorithm_list.end();++iter) {

  
    string name;
    name = "mu_pt_"+*iter;
    mu_pt[*iter] = new TH1F(name.c_str(),"Muon pt [GeV]",100,0,100);
    AddToPrint(mu_pt[*iter]);

    name = "mu_eta_pt20_"+*iter;
    mu_eta_pt20[*iter] = new TH1F(name.c_str(),"Muon eta ", 60,-3,3);
    AddToPrint(mu_eta_pt20[*iter]);

    name = "mu_phi_pt20_"+*iter;
    mu_phi_pt20[*iter] = new TH1F(name.c_str(),"Muon phi",60,-3.2,3.2);
    AddToPrint(mu_phi_pt20[*iter]);

    name = "chi2_"+*iter;
    chi2[*iter] = new TH1F(name.c_str(),"Track fit chi2/ndf",100,0,2);
    if(*iter!="CaloMuon") AddToPrint(chi2[*iter]);
    if(isCombined(*iter)) {
      Double_t etabins[8] = {-2.5,-2.1,-1.8,-1.05,1.05,1.8,2.1,2.5};
      name = "DeltaPtovPt_pt20_"+*iter;
      DeltaPtovPt[*iter] = new TH2F(name.c_str(),"DPt/Pt",7,etabins,100,-1,1);
      AddToPrint(DeltaPtovPt[*iter]);

      name = "Chi2Match_"+*iter;
      chi2Match[*iter] = new TH2F(name.c_str(),"Chi2Match/ndf",7,etabins,100,0,40);
      AddToPrint(chi2Match[*iter]);

      name = "DeltaEta_pt20_"+*iter;
      DeltaEta[*iter] = new TH2F(name.c_str(),"DeltaEta",7,etabins,100,-.1,.1);
      AddToPrint(DeltaEta[*iter]);

      name = "DeltaPhi_pt20_"+*iter;
      DeltaPhi[*iter] = new TH2F(name.c_str(),"DeltaPhi",7,etabins,100,-.1,.1);
      AddToPrint(DeltaPhi[*iter]);

    }


    name = "NhitsEff_"+*iter;
    NhitsEff[*iter] = new TH2F(name.c_str(),"NhitsEff versus pT (GeV)",100,0,100,100,0.,1);

    name = "NhitsEffSoft_"+*iter;
    NhitsEffSoft[*iter] = new TH2F(name.c_str(),"NhitsEffsoft versus pT (GeV)",100,0,100,100,0.,1);

    list<string>::iterator sub_iter;
    for(sub_iter=subdetectors.begin(); sub_iter!=subdetectors.end();++sub_iter) {

      name = "nhit_pt_"+(*sub_iter)+"_"+(*iter);
      nhit_pt[*sub_iter+"_"+(*iter)] = new TH2F(name.c_str(),"Fraction of hits versus pT (GeV)",100,0,100,100,0,1);

      name = "nhit_eta_"+(*sub_iter)+"_"+(*iter);
      nhit_eta[*sub_iter+"_"+(*iter)] = new TH2F(name.c_str(),"Fraction of hits versus eta",60,-3.2,3.2,100,0,1);

      name = "nhit_phi_"+(*sub_iter)+"_"+(*iter);
      nhit_phi[*sub_iter+"_"+(*iter)] = new TH2F(name.c_str(),"Fraction of hits versus phi",60,-3,3,100,0,1);
    }

    name = "d0_pT" + (*iter);
    d0_pT[*iter] = new TH2F(name.c_str(),"d0 versus pT (GeV)",100,0,100,100,-10,10);

    name = "z0_pT" + (*iter);
    z0_pT[*iter] = new TH2F(name.c_str(),"z0 versus pT (GeV)",100,0,100,100,-10,10);

    name = "d0_eta" + (*iter);
    d0_eta[*iter] = new TH2F(name.c_str(),"d0 versus pT (GeV)", 30,-3,3,100,-10,10);

    name = "z0_eta" + (*iter);
    z0_eta[*iter] = new TH2F(name.c_str(),"z0 versus eta",30,-3,3,100,-10,10);

  }
}

void histos::ClearVectors() {
  pt.clear(); eta.clear(); phi.clear(); NhitsEff_value.clear();
  NhitsEffSoft_value.clear(); n_holes.clear(); nhit_sub.clear();
  d0.clear(); z0.clear(); pt_ID.clear(); pt_MS.clear(); eta_ID.clear(); eta_MS.clear();
  phi_ID.clear(); phi_MS.clear(); Chi2M.clear(); n_outliers.clear(); chi2_val.clear();
}


void histos::fill() {
  static int counter=0;
  counter++;
  bool dump=(counter<100);

  list<string>::iterator iter;
  if(dump) cout << "now we are in fill()" << endl;

  for(iter=algorithm_list.begin();iter!=algorithm_list.end();++iter) {
    if(dump) cout << "processing algorithm " << *iter << endl;
    FillMuon(*iter);
        cout << "Before filling histogram maps..." << " nmuons = " << nmuons << endl;
    for(int i=0;i<=nmuons;i++) {
      mu_pt[*iter]->Fill(pt[i]);
      cout << "just after mu_pt" << endl;
      NhitsEff[*iter]->Fill(pt[i],NhitsEff_value[i]);
     cout << "just after NhitsEff" << endl;
      NhitsEffSoft[*iter]->Fill(pt[i],NhitsEffSoft_value[i]);
    cout << "just after NhitsEffSoft" << endl;
	 
      if(pt[i]>20)  {
	mu_eta_pt20[*iter]->Fill(eta[i]);
	mu_phi_pt20[*iter]->Fill(phi[i]);
      }
	
     cout << "just after pt > 20" << endl;

      Double_t ntot=0;
      list<string>::iterator subiter;

      for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {
        char num[2];
        sprintf(num,"%s_%d",((*subiter).c_str()),i);
        string tmp = num;

	ntot+=nhit_sub[tmp];
      }

      cout << "ntot = " << ntot << endl;

      for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {

        char num[2];
        sprintf(num,"%s_%d",((*subiter).c_str()),i);
        string tmp = num;

        nhit_pt[*subiter+"_"+(*iter)]->Fill(pt[i],nhit_sub[tmp]/ntot);
        if(pt[i]>20) {
           nhit_eta[*subiter+"_"+(*iter)]->Fill(eta[i],nhit_sub[tmp]/ntot);
           nhit_phi[*subiter+"_"+(*iter)]->Fill(phi[i],nhit_sub[tmp]/ntot);
        }
         
       }

      cout << "just after pt > 20" << endl;
      d0_pT[*iter]->Fill(pt[i],d0[i]);
      z0_pT[*iter]->Fill(pt[i],z0[i]);

       cout << "just after pt vs d0" << endl;
      d0_eta[*iter]->Fill(eta[i],d0[i]);
      z0_eta[*iter]->Fill(eta[i],z0[i]);

      chi2[*iter]->Fill(chi2_val[i]);

       cout << "combined variables " << endl;

      if(isCombined(*iter) && pt[i]>20) {

	DeltaPtovPt[*iter]->Fill(eta[i],(pt_ID[i]-pt_MS[i])/pt_ID[i]);
	chi2Match[*iter]->Fill(eta[i],Chi2M[i]);
	DeltaEta[*iter]->Fill(eta[i],(eta_ID[i]-eta_MS[i]));
	DeltaPhi[*iter]->Fill(eta[i],(eta_ID[i]-eta_MS[i]));

      }
    
      
    } 

  }
}


void histos::FillMuon(string alg) {
  nmuons = -1;
  ClearVectors();

  if(alg=="Staco_CB" || alg=="MuonBoy" || alg =="MuTag") {

    FillStacoCollection(alg);

  } else if(alg=="Muid_CB" ||  alg=="MuGirl" || alg=="Muid_Tight" || alg=="MuidSA" || alg=="MuTagIMO") {

    FillMuidCollection(alg);

  } else if(alg=="CaloMuon") {
    FillCaloMuonCollection(alg);
  }

  for(int i=0;i<=nmuons;i++) {
     
    int nhit = 0;
    list<string>::iterator subiter; 

    for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {
      char num[2];
      sprintf(num,"%s_%d",((*subiter).c_str()),i);
      string tmp = num;



      nhit += nhit_sub[tmp];
    }
	
    NhitsEff_value.push_back(((float) nhit)/(nhit+n_outliers[i]+n_holes[i]));
    NhitsEffSoft_value.push_back(((float) nhit)/(nhit+n_outliers[i]+n_holes[i]));    
    
  }

  

}


void histos::FillStacoCollection(string alg) {
  // cout << "before Get Author ID" << endl;
  int authorID = GetAuthorID(alg);
  // cout << "mu_staco_n" << mu_staco_n << endl;
  
  for(int i=0;i < mu_staco_n; i++) {
    bool process = false;
    if(alg=="Staco_CB") process = mu_staco_isCombinedMuon->at(i);
    else {process = mu_staco_author->at(i)==authorID;}
    // cout << "i = " << i << endl;
    // cout << "process = " << process << endl;

    if(process ) {
       cout << "incrmenting nmuons " << endl;
      nmuons++;
       cout << "nmuons = " << nmuons << " i = " << i << endl;
      pt.push_back(mu_staco_pt->at(i)/1000);
       cout << "after pt" << endl;
      eta.push_back(mu_staco_eta->at(i));
      phi.push_back(mu_staco_phi->at(i));

      list<string>::iterator subiter;      

      for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {

        char num[2];
        sprintf(num,"%s_%d",((*subiter).c_str()),i);
        string tmp = num;


        if(*subiter=="pix") {
           nhit_sub[tmp] = mu_staco_nBLHits->at(i)+mu_staco_nPixHits->at(i);
        } else if(*subiter=="sct") {
           nhit_sub[tmp] = mu_staco_nSCTHits->at(i);
        } else if(*subiter=="trt") {
          nhit_sub[tmp] = mu_staco_nTRTHits->at(i);
        } else if(*subiter=="mdt") {
          nhit_sub[tmp] = mu_staco_nMDTHits->at(i);
        } else if(*subiter=="rpc") {  
          nhit_sub[tmp] = mu_staco_nRPCEtaHits->at(i)+mu_staco_nRPCPhiHits->at(i);
        } else if(*subiter=="tgc") {
          nhit_sub[tmp] = mu_staco_nTGCEtaHits->at(i)+mu_staco_nTGCPhiHits->at(i); 
        } else if(*subiter=="csc") {
          nhit_sub[tmp] = mu_staco_nCSCEtaHits->at(i)+mu_staco_nCSCPhiHits->at(i);
        }
      } 
       cout << "before n_outliers" << endl;

      n_outliers.push_back(mu_staco_nTRTOutliers->at(i));
      n_holes.push_back(mu_staco_nPixHoles->at(i));
      n_holes[nmuons] += mu_staco_nSCTHoles->at(i)+mu_staco_nMDTHoles->at(i);
      n_holes[nmuons] += mu_staco_nCSCPhiHoles->at(i)+mu_staco_nCSCEtaHoles->at(i);
      n_holes[nmuons] += mu_staco_nRPCEtaHoles->at(i)+mu_staco_nRPCPhiHoles->at(i);
      n_holes[nmuons] += mu_staco_nTGCPhiHoles->at(i)+mu_staco_nTGCEtaHoles->at(i);
      n_holes[nmuons] += mu_staco_nMDTHoles->at(i);

       cout << "d0 e z0" << endl;      
      d0.push_back(mu_staco_d0_exPV->at(i));
      z0.push_back(mu_staco_z0_exPV->at(i));
   
      int ndof = mu_staco_trackfitndof->at(i);
      chi2_val.push_back((ndof > 0) ? mu_staco_trackfitchi2->at(i)/ndof : -1);    
      cout << "Chi2M" << endl;
      Chi2M.push_back(mu_staco_matchchi2->at(i)/mu_staco_matchndof->at(i));
      double pt_ID_var,eta_ID_var,pt_MS_var,eta_MS_var;
      GetPtEta(mu_staco_id_qoverp->at(i),mu_staco_id_theta->at(i),&pt_ID_var,&eta_ID_var);
      GetPtEta(mu_staco_me_qoverp->at(i),mu_staco_me_theta->at(i),&pt_MS_var,&eta_MS_var);
      
      pt_ID.push_back(pt_ID_var);  pt_MS.push_back(pt_MS_var); 
      eta_ID.push_back(eta_ID_var);  eta_MS.push_back(eta_MS_var); 
      phi_ID.push_back(mu_staco_id_phi->at(i)); phi_MS.push_back(mu_staco_me_phi->at(i));
      
      cout << "end cicle" << endl; 
    }
    

  }
  cout << " going out from Fill Staco collection" << endl;
 
}



void histos::FillMuidCollection(string alg) {

  cout << "entering Muid" << endl;

  int authorID = GetAuthorID(alg);

  bool process=false;

  cout << "before looping mu_muid_n" << endl;
  for(int i=0;i < mu_muid_n; i++) {
    
    if(alg=="Muid_CB") process = mu_muid_isCombinedMuon->at(i);
    else if(alg=="Muid_Tight") process = mu_muid_tight->at(i);
    else process=(authorID==mu_muid_author->at(i));
    
    if(process) {
      nmuons++; 
      pt.push_back(mu_muid_pt->at(i)/1000);
      eta.push_back(mu_muid_eta->at(i));
      phi.push_back(mu_muid_phi->at(i));
 
      list<string>::iterator subiter;
      cout << "before looping on subiter" << endl;

      for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {
        char num[2];
        sprintf(num,"%s_%d",((*subiter).c_str()),i);
        string tmp = num;
        cout << "string " << tmp << " i = " << i << endl;

        if(*subiter=="pix") {
           nhit_sub[tmp] = mu_muid_nBLHits->at(i)+mu_muid_nPixHits->at(i);
        } else if(*subiter=="sct") {
           nhit_sub[tmp] = mu_muid_nSCTHits->at(i);
        } else if(*subiter=="trt") {
          nhit_sub[tmp] = mu_muid_nTRTHits->at(i);
        } else if(*subiter=="mdt") {
          nhit_sub[tmp] = mu_muid_nMDTHits->at(i);
        } else if(*subiter=="rpc") {
          nhit_sub[tmp] = mu_muid_nRPCEtaHits->at(i)+mu_muid_nRPCPhiHits->at(i);
        } else if(*subiter=="tgc") {
          nhit_sub[tmp] = mu_muid_nTGCEtaHits->at(i)+mu_muid_nTGCPhiHits->at(i);
        } else if(*subiter=="csc") {
          nhit_sub[tmp] = mu_muid_nCSCEtaHits->at(i)+mu_muid_nCSCPhiHits->at(i);
        }
        cout << "after filling nhit" << endl;

      }       
      cout << "after hits " << endl;
      
      n_outliers.push_back( mu_muid_nTRTOutliers->at(i));
      n_holes.push_back(mu_muid_nPixHoles->at(i));
      n_holes[nmuons] += mu_muid_nSCTHoles->at(i)+mu_muid_nMDTHoles->at(i);
      n_holes[nmuons] += mu_muid_nCSCPhiHoles->at(i)+mu_muid_nCSCEtaHoles->at(i);
      n_holes[nmuons] += mu_muid_nRPCEtaHoles->at(i)+mu_muid_nRPCPhiHoles->at(i);
      n_holes[nmuons] += mu_muid_nTGCPhiHoles->at(i)+mu_muid_nTGCEtaHoles->at(i);
      n_holes[nmuons] += mu_muid_nMDTHoles->at(i);
     
      cout << "n holes " << endl;
 
      d0.push_back(mu_muid_d0_exPV->at(i));
      z0.push_back(mu_muid_z0_exPV->at(i));
      int ndof = mu_muid_trackfitndof->at(i);
      chi2_val.push_back((ndof > 0) ? mu_muid_trackfitchi2->at(i)/ndof : -1);    
   
      Chi2M.push_back(mu_muid_matchchi2->at(i)/mu_muid_matchndof->at(i));
      
      double pt_ID_var,eta_ID_var,pt_MS_var,eta_MS_var;
      GetPtEta(mu_muid_id_qoverp->at(i),mu_muid_id_theta->at(i),&pt_ID_var,&eta_ID_var);
      GetPtEta(mu_muid_me_qoverp->at(i),mu_muid_me_theta->at(i),&pt_MS_var,&eta_MS_var);
      
      pt_ID.push_back(pt_ID_var);  pt_MS.push_back(pt_MS_var); 
      eta_ID.push_back(eta_ID_var);  eta_MS.push_back(eta_MS_var); 
      phi_ID.push_back(mu_muid_id_phi->at(i)); phi_MS.push_back(mu_muid_me_phi->at(i));
      cout << "going to exit" << endl;
      
    }
    
  }
  

  
}




void histos::FillCaloMuonCollection(string alg) {
  // cout << "before Get Author ID" << endl;
  //  int authorID = GetAuthorID(alg);
  // cout << "mu_staco_n" << mu_staco_n << endl;
  
  for(int i=0;i < mu_calo_n; i++) {
    bool process = false;
    process = true;

    if(process ) {
      // cout << "incrementing nmuons " << endl;
      nmuons++;
      // cout << "nmuons = " << nmuons << " i = " << i << endl;
      pt.push_back(mu_calo_pt->at(i)/1000);
      // cout << "after pt" << endl;
      eta.push_back(mu_calo_eta->at(i));
      phi.push_back(mu_calo_phi->at(i));

      list<string>::iterator subiter;
      for(subiter=subdetectors.begin();subiter!=subdetectors.end(); ++subiter) {
        char num[2];
        sprintf(num,"%s_%d",((*subiter).c_str()),i);
        string tmp = num;
        nhit_sub[tmp] = 0;
      }
      
      // cout << "before n_outliers" << endl;

      n_outliers.push_back(0);
      n_holes.push_back(0);
      /*
      n_holes[nmuons] += mu_calo_nSCTHoles->at(i)+mu_staco_nMDTHoles->at(i);
      n_holes[nmuons] += mu_calo_nCSCPhiHoles->at(i)+mu_staco_nCSCEtaHoles->at(i);
      n_holes[nmuons] += mu_calo_nRPCEtaHoles->at(i)+mu_staco_nRPCPhiHoles->at(i);
      n_holes[nmuons] += mu_calo_nTGCPhiHoles->at(i)+mu_staco_nTGCEtaHoles->at(i);
      n_holes[nmuons] += mu_calo_nMDTHoles->at(i);
      */

      // cout << "d0 e z0" << endl;      
      d0.push_back(0);
      z0.push_back(0);
   
      //      int ndof = mu_calo_trackfitndof->at(i);
      chi2_val.push_back(0);    


      

      
      
    }
    

  }
}


int histos::GetAuthorID(string alg) {
  if(alg=="MuonBoy") {return MuonParameters::Muonboy;}
  else if(alg=="MuidSA") {return MuonParameters::MuidSA;}
  else if(alg=="MuTag") {return MuonParameters::MuTag;}
  else if(alg=="MuGirl") {return MuonParameters::MuGirl;}
  else if(alg=="Staco_CB") {return MuonParameters::STACO;}
  else if(alg=="Muid_CB") {return MuonParameters::MuidCo;}
  else if(alg=="MuTagIMO") {return MuonParameters::MuTagIMO;}
  return MuonParameters::unknown;
}

void histos::AddToPrint(TObject *object) {
  histo_to_print.push_back(object);
   all_histos.push_back(object);
   
}

void histos::AddToAll(TObject *object) {

  all_histos.push_back(object);
}

void histos::GetPtEta(double qoverp, double theta, double *pt_out, double *eta_out) {
  *pt_out = ((fabs(qoverp) > 0) ?fabs(1/qoverp)*sin(theta) : 0);
  *eta_out = ((theta>0) ? -log(tan(theta/2)) : 0.);
  // cout << "qoverp, theta, pt, eta = " << qoverp << ", " << theta << ", " << *pt << ", " << *eta << endl;
}
