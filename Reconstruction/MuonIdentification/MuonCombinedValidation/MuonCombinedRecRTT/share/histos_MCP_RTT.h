#include "MuonParamDefs.h"
#include "physics_MCP_RTT.h"
#include <TH1F.h>
#include <TH2F.h>
#include <list>
#include <string>
#include <map>

#include <TObject.h>
#include <TProfile.h>

using namespace std;
class histos : public physics {

 public:

  map<string,TH1F *> mu_pt, mu_eta_pt20, mu_phi_pt20, chi2;
  map<string, TH2F *> DeltaPtovPt,chi2Match,DeltaEta,DeltaPhi,NhitsEff,NhitsEffSoft;
  map<string, TH2F *> nhit_pt;
  map<string, TH2F *> nhit_eta;
  map<string, TH2F *> nhit_phi;
  map<string, TH2F *> d0_pT,z0_pT,d0_eta,z0_eta;
  map<string, TH1F *> *author, *author_pt_20;
  map<string, TProfile *> nhit_prof;

  histos();
  histos(TTree *tree);
  ~histos();
  void book();
  void fill();
  void print();
  void run();
 


 private:
  TFile *file_out;
  list<string> algorithm_list;
  list<TObject *> histo_to_print, all_histos;
  
  void AddToPrint(TObject *);
  void AddToAll(TObject *);
  
  string id_to_author(int id);     
  inline bool isCombined(string Alg);

  int nmuons;
  vector<double> pt, eta , phi, NhitsEff_value, NhitsEffSoft_value;    
  vector<int> n_outlayers, n_holes;
  map<string,int>  nhit_sub;

  list<string> variables, subdetectors;

  vector<double> d0,z0,pt_ID,pt_MS,eta_ID,eta_MS,phi_ID,phi_MS,Chi2M,n_outliers,chi2_val;

  void FillMuidCollection(string alg);
  void FillStacoCollection(string alg);
  void FillCaloMuonCollection(string alg);

  void FillMuon(string alg);
  void ClearVectors();
  inline int GetAuthorID(string alg);
  void GetPtEta(double qoverp, double theta, double *pt_out, double *eta_out);
  void post_process();
  void Init();


};


bool histos::isCombined(string Alg) {
  return  (Alg=="Staco_CB") || (Alg == "Muid_CB");
}



    

