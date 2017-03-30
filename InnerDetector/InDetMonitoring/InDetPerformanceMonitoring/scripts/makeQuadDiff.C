#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <iostream>
#include <vector>
#include <string>
#include "AtlasStyle.C"
//macro made by Federico Sforza
using namespace std;
const  int debug = 1;

void   BuildPalettes (Int_t colorstyle=0);

struct sortFirstPair{
  sortFirstPair(double val1in=0., double val2in=0. ): val1(val1in), val2(val2in){};
  double val1;
  double val2;
  inline bool operator< (const sortFirstPair&) const;
};
inline bool sortFirstPair::operator< (const sortFirstPair& input) const
{
  return val1 < input.val1;
}
//Function that applies a 3 bin (or 3x3) MEDIAN FILTER on H1
// if H0 is present, then  calculates the ratio (Hr) between H0 and H1,
// and then works on Hr. At the end Hr is multiplied again for H0.
// Result replaces the H1 pointer.
// The filter is applied only to the X axis
void MedianFilter3_1D(TH1* H1, TH1* H0 = nullptr, const bool verb = false){
  if(H0) H1->Divide(H0);
  int nbinsX = H1->GetNbinsX();
  int nbinsY = H1->GetNbinsY();
  if(nbinsX<2){
    cerr<<"you need at least 2 X bins to implement the algorithm"<<endl;
    exit(-1);
  }
  sortFirstPair matrix[3][1];
  int edgex=1, edgey=0;
  int MatrixLimitY=1;  //**** LIMITS ALONG Y FOR THE MATRIX *****
  //Scan the histo
  for(int i=(1+edgex); i <= (nbinsX-edgex); ++i){
    for(int j=(1+edgey); j <= (nbinsY-edgey); ++j){
      //fill the matrix
      for(int x=0; x<3; ++x)
        for(int y=0; y<MatrixLimitY; ++y){
          matrix[y][x] = sortFirstPair(H1->GetBinContent( (i-edgex+x), (j-edgey+y) ),
                                       H1->GetBinError( (i-edgex+x), (j-edgey+y) ) );
          if(verb)       
            cout<<" H1->GetBinContent("<< (i-edgex+x)<<","<< (j-edgey+y)<<"): "<<H1->GetBinContent( (i-edgex+x), (j-edgey+y))<<
              " H1->GetBinError("<< (i-edgex+x)<<","<< (j-edgey+y)<<"): "<<H1->GetBinError( (i-edgex+x), (j-edgey+y))<<
              " M["<<x<<"]["<<y<<"]:"<<matrix[x][y].val1 <<endl;
        }
      sort(&matrix[0][0], &matrix[3][MatrixLimitY-1]); //sorting the elements of the matrix
      if(verb) for(int x=0; x<3; ++x) for(int y=0; y<MatrixLimitY; ++y)
                                        cout<<" matrix["<<x<<"]["<<y<<"]: "<<matrix[x][y].val1<<" "<<matrix[x][y].val2<<endl;
      //setting the bin content and error to the median value of the matrix.
      if(verb) cout<<"i: "<<i<<", j: "<<j<<" ,matrix[1][(MatrixLimitY-1)/2]: "<<matrix[1][(MatrixLimitY-1)/2].val1<<endl;
      H1->SetBinContent(i,j,matrix[1][(MatrixLimitY-1)/2].val1);
      H1->SetBinError(i,j,matrix[1][(MatrixLimitY-1)/2].val2);
    }
  }
  if(H0)  H1->Multiply(H0);
  return;
}
void prepareQuadDiff(string ip, string baseStr, string WPStr,
                     TH2F* &ip_quadDiff, TH2F* &ip_quadDiff_sys_dw, TH2F*& ip_quadDiff_sys_up){
  TExec *PaletteForSmearing = new TExec("PaletteForSmearing","BuildPalettes(0);");
  PaletteForSmearing->Exec();

  TFile* fData = new TFile("ZmumuValidationExampleOutput_test_pretsi.root");
  TFile* fMC = new TFile("ZmumuValidationExampleOutput_test_mc.root");
  //additional input file used to have one more syst uncertainty w.r.t. default data
  //TFile* fDataSys = new TFile("data_2015MBrw_iter1_v20/out.root");
  TH2F* ip_data = (TH2F*) fData->Get(("IPresolutions_"+ip+"/"+ip+baseStr+"_"+WPStr).c_str());
  TH2F* ip_mc = (TH2F*) fMC->Get(("IPresolutions_"+ip+"/"+ip+baseStr+"_"+WPStr).c_str());
  //TH2F* ip_dataSys = (TH2F*) fDataSys->Get(("IPresolutions_"+ip+"/"+ip+baseStr+"_"+WPStr).c_str());
  ip_quadDiff = (TH2F*)ip_data->Clone((ip+"quaddiff_comb_Pt_Eta").c_str());
  ip_quadDiff->Reset(); ip_quadDiff->SetTitle((ip+"quaddiff_comb_Pt_Eta").c_str());
  ip_quadDiff_sys_up = (TH2F*)ip_data->Clone((ip+"quaddiff_comb_Pt_Eta_sys_up").c_str());
  ip_quadDiff_sys_up->Reset(); ip_quadDiff_sys_up->SetTitle((ip+"quaddiff_comb_Pt_Eta_sys_up").c_str());
  ip_quadDiff_sys_dw = (TH2F*)ip_data->Clone((ip+"quaddiff_comb_Pt_Eta_sys_dw").c_str());
  ip_quadDiff_sys_dw->Reset(); ip_quadDiff_sys_dw->SetTitle((ip+"quaddiff_comb_Pt_Eta_sys_dw").c_str());
  vector<TH2F*> ip_sys_data, ip_sys_mc, ip_sys_effect;
  //vector<string> sys_v = {"kIterErUp", "kIterErDw", "kPVErUp", "kPVErDw"};
  vector<string> sys_v = {"kIterErUp", "kIterErDw", "kPVErUp", "kPVErDw", "widerFitErr"};

  for(uint i=0; i<sys_v.size(); ++i){
    string getStr = "IPresolutions_"+ip+"/"+ip+baseStr+"_"+sys_v[i]+"_"+WPStr;
    cout<<"Get: "<< getStr<<endl;
    //PATCH!!! +++++++++
    /*if(getStr.find("z0") != string::npos && getStr.find("widerFitErr") != string::npos )
      getStr =  "IPresolutions_"+ip+"/"+ip+baseStr+"_"+sys_v[i]+WPStr;*/
    //++++++++++++++
    TH2F* ip_data_tmp = (TH2F*) fData->Get(getStr.c_str());
    ip_sys_data.push_back(ip_data_tmp);
    TH2F* ip_mc_tmp = (TH2F*) fMC->Get(getStr.c_str());
    ip_sys_mc.push_back(ip_mc_tmp);
    TH2F* ip_sys_tmp = (TH2F*) ip_mc_tmp->Clone((ip+"_sys_effect_"+sys_v[i]+"_"+WPStr).c_str());
    ip_sys_tmp->SetTitle(ip_sys_tmp->GetName()); ip_sys_tmp->Reset();
    ip_sys_effect.push_back(ip_sys_tmp);
    //ip_sys_effect.at(i);

  }
  //Add also the syst corresponding to the data2015 vs data2016, it will be the last in the vector
  //TH2F* ip_sys_data_tmp = (TH2F*) ip_dataSys->Clone((ip+"_sys_effect_data2015_"+WPStr).c_str());
  //ip_sys_data_tmp->SetTitle(ip_sys_data_tmp->GetName()); ip_sys_data_tmp->Reset();
  //ip_sys_effect.push_back(ip_sys_data_tmp);
  for(int ix=1; ix < ip_quadDiff->GetNbinsX()+1; ++ix ){
    for(int iy=1; iy < ip_quadDiff->GetNbinsY()+1; ++iy ){
      if(debug) cout<<"ix = "<<ix<<" , iy = "<<iy<<endl;
      float data_ip = ip_data->GetBinContent(ix, iy);
      float mc_ip = ip_mc->GetBinContent(ix, iy);
      //Smearing should not go negative but I will enforce it later
      // so that I can use this calculated quantity for stat. error propagation
      float  smear_ip_sq = (data_ip*data_ip - mc_ip*mc_ip);
      if(debug) cout<<"smear_ip_sq = "<<smear_ip_sq<<endl;
      float smear_ip = (smear_ip_sq > 0. ) ? sqrt(smear_ip_sq) : 0.; //-sqrt(-smear_ip_sq);//0. ;
      ip_quadDiff->SetBinContent(ix, iy, smear_ip);
      if(debug) cout<<"smear_"+ip+" = "<<smear_ip<<endl;
      ////////   Stat error  ////////
      //Add uncorrelated with error propagation (if I did the math right):
      // sigma_smear_ip^2 = data_ip^2/smear_ip^2 *data_err_ip^2 + mc_ip^2/smear_ip^2 *mc_err_ip^2
      float data_ip_err = ip_data->GetBinError(ix, iy);
      float mc_ip_err = ip_mc->GetBinError(ix, iy);
      if(debug) cout<<"data_"+ip+"_err = "<<data_ip_err<<" mc_"+ip+"_err = "<<mc_ip_err<<endl;
   
      float smear_ip_err_sq = (smear_ip_sq == 0.) ? 0. :
        pow(data_ip,2) / fabs(smear_ip_sq) * pow(data_ip_err,2)   
        + pow(mc_ip,2) / fabs(smear_ip_sq) * pow(mc_ip_err,2);
      ip_quadDiff->SetBinError(ix, iy, sqrt(smear_ip_err_sq));
      if(debug) cout<<"smear_"<<ip<<"_stat_err = "<<sqrt(smear_ip_err_sq)<<endl;
      //////   Sys errors ///////
      //the data and MC sources I know are correlated => do the quad
      // subtraction with simultaneous ipiations, add in quadrature
      // and then feed to the up/down total hist (where also stat uncertainty is added)
      float sys_ip_up_sq =0., sys_ip_dw_sq = 0.;
      for(uint i=0; i<sys_v.size(); ++i){
        float data_sys_ip = ip_sys_data[i]->GetBinContent(ix, iy);
        float mc_sys_ip = ip_sys_mc[i]->GetBinContent(ix, iy);
       
        float smear_sys_ip = (data_sys_ip > mc_sys_ip) ?
          sqrt(data_sys_ip*data_sys_ip - mc_sys_ip*mc_sys_ip) : 0.; // -sqrt(-data_sys_ip*data_sys_ip + mc_sys_ip*mc_sys_ip); //0.;
        if(smear_sys_ip > smear_ip) sys_ip_up_sq += pow((smear_sys_ip - smear_ip), 2);
        else  sys_ip_dw_sq += pow((smear_ip - smear_sys_ip), 2);
        if(debug) cout<<sys_v[i]<<endl
                      <<"smear_sys_"+ip+" = "<<smear_sys_ip<<" , sys_"+ip+"_up = "<<sqrt(sys_ip_up_sq)
                      <<", sys_"+ip+"_dw = "<<sqrt(sys_ip_dw_sq)<<endl;

        if(i == 4){
          float subs = ip_sys_effect[i]->GetBinContent(ix,iy)-ip_data->GetBinContent(ix,iy);
          float subs_err = sqrt(pow(ip_sys_effect[i]->GetBinError(ix,iy),2)+pow(ip_data->GetBinError(ix,iy),2));
          if (fabs(subs) > 2*subs_err) ip_sys_effect[i]->SetBinContent(ix, iy, (smear_sys_ip - smear_ip) );
          else{ 
             ip_sys_effect[i]->SetBinContent(ix, iy, 0.);
          }
        }
        else{
        ip_sys_effect[i]->SetBinContent(ix, iy, (smear_sys_ip - smear_ip) );
        }

        if(i == 4) cout << "  *** (SMEAR SYS,SMEAR) CONTENT: ****  " <<  smear_sys_ip << " , " << smear_ip << endl;

      }
      //////   Sys from different data file (e.g. period dependence) //////
      /*float dataSys_ip = 0.;//ip_dataSys->GetBinContent(ix, iy);
      float smearing_dataSys_ip_sq = pow( (data_ip - dataSys_ip), 2.0);
      if(debug) cout<<"smearing_dataSys_"+ip+" = "<<sqrt(smearing_dataSys_ip_sq)<<endl;     
      ip_sys_effect[sys_v.size()]->SetBinContent(ix, iy, (data_ip - dataSys_ip));*/
      //Add stat  and dataSys errors in quadrature
      ip_quadDiff_sys_up->SetBinContent(ix, iy, smear_ip +
                                        sqrt(sys_ip_up_sq+smear_ip_err_sq));// + smearing_dataSys_ip_sq));
      float smearing_down = (smear_ip - sqrt(sys_ip_dw_sq+smear_ip_err_sq));// + smearing_dataSys_ip_sq));
      if( smearing_down > 0)  ip_quadDiff_sys_dw->SetBinContent(ix, iy, smearing_down);
      else   ip_quadDiff_sys_dw->SetBinContent(ix, iy, 0);
      if(debug) cout<<ip+"_quadDiff_sys_up = "<<ip_quadDiff_sys_up->GetBinContent(ix,iy)
                    <<" "+ip+"_quadDiff_sys_dw = "<<ip_quadDiff_sys_dw->GetBinContent(ix, iy)<<endl;
    }

   }
    //Plot of the smearing (sqrt(sigma(data)^2 - sigma(MC)^2)
    TCanvas * c_profile = new TCanvas ("c_profile", "c_profile", 900, 600);
    TH1D *ip_quadDiff_profx = ip_quadDiff->ProjectionX("ip_quadDiff_profx",0,-1,"e");
    ip_quadDiff_profx->Scale(1./ip_quadDiff->GetNbinsY());
    ip_quadDiff_profx->SetMarkerStyle(20);	
    TH1D *ip_quadDiff_sys_up_profx = ip_quadDiff_sys_up->ProjectionX("ip_quadDiff_sys_up_profx",0,-1,"e");
    ip_quadDiff_sys_up_profx->Scale(1./ip_quadDiff->GetNbinsY());
    ip_quadDiff_sys_up_profx->SetMarkerStyle(20);
    TH1D *ip_quadDiff_sys_dw_profx = ip_quadDiff_sys_dw->ProjectionX("ip_quadDiff_sys_dw_profx",0,-1,"e");
    ip_quadDiff_sys_dw_profx->Scale(1./ip_quadDiff->GetNbinsY());
    ip_quadDiff_sys_dw_profx->SetMarkerStyle(20);
    //TH1D *ip_quadDiff_unc = new TH1D ("ip_quadDiff_unc", "ip_quadDiff_unc", ip_quadDiff_profx->GetNbinsX(), ip_quadDiff_profx->GetMinimum(), ip_quadDiff_profx->GetMaximum());
    for (int nbin = 1; nbin <= ip_quadDiff_profx->GetNbinsX(); nbin++) {
       cout << " ** " << ip << " BIN: " << nbin << "   ip_quadDiff:" << ip_quadDiff_profx->GetBinContent(nbin) 
            << " stat. " << ip_quadDiff_profx->GetBinError(nbin)
            << " sys. up " << ip_quadDiff_sys_up_profx->GetBinContent(nbin)
            << " sys. down " << ip_quadDiff_sys_dw_profx->GetBinContent(nbin)
            << endl;
           
    } 
    ip_quadDiff_sys_up_profx->SetTitle(Form("%s%s", ip.c_str(), " smearing p_{T} projection")); 
    ip_quadDiff_sys_up_profx->GetYaxis()->SetTitle("#sqrt{#sigma^{2}_{data} - #sigma^{2}_{MC}} [mm]");
    ip_quadDiff_sys_up_profx->GetYaxis()->SetTitleOffset(1.2);
    ip_quadDiff_sys_up_profx->SetMinimum(0.);
    ip_quadDiff_sys_up_profx->Draw();
    ip_quadDiff_profx->Draw("samepe");
    ip_quadDiff_sys_dw_profx->Draw("same");
    c_profile->Print(Form("%s_%s",ip.c_str(),"quadProfile.png"));
    for (int nbin = 1; nbin <= ip_quadDiff_profx->GetNbinsX(); nbin++) {
 
       float value = 0;
       for (int eta = 1; eta <= ip_quadDiff->GetNbinsY(); eta++) {
  
        value = value + ip_quadDiff_profx->GetBinContent(nbin);
       }
     cout << "BIN : " <<  nbin << " value = " << value/ip_quadDiff->GetNbinsY() << endl;
    }
    

  
  

  //Save systematics effects in file
  bool save_sys_eff = true;
  if(save_sys_eff){
    TFile f(("sys_eff"+ip+"_"+WPStr+".root").c_str(),"recreate");
    for(uint i=0; i< ip_sys_effect.size(); ++i){
      cout << "****Writing: ****" << ip_sys_effect[i]->GetName() << endl;      
      ip_sys_effect[i]->Write();
     }
    f.Close();
  }
}
void makeQuadDiff(string WPStr="Tight", string addStr = "", bool applyFilter = true){
  //SetAtlasStyle();
  TExec *PaletteForSmearing = new TExec("PaletteForSmearing","BuildPalettes(0);");
  PaletteForSmearing->Exec();
  gStyle->SetOptStat(0);
  gStyle->SetPadRightMargin(0.15);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);
  gStyle->SetPadLeftMargin(0.1);
  Double_t txsize=0.04;
  gStyle->SetLabelSize(txsize,"x");
  gStyle->SetTitleSize(txsize,"x");
  gStyle->SetLabelSize(txsize,"y");
  gStyle->SetTitleSize(txsize,"y");
  gStyle->SetLabelSize(txsize,"z");
  gStyle->SetTitleSize(txsize,"z");
  string baseStr = "unf_Pt_Eta";
  TH2F* d0unf_quadDiff = nullptr, *d0unf_quadDiff_sys_dw = nullptr, * d0unf_quadDiff_sys_up = nullptr;
  prepareQuadDiff("d0", baseStr, WPStr, d0unf_quadDiff, d0unf_quadDiff_sys_dw, d0unf_quadDiff_sys_up);
  TH2F* z0unf_quadDiff = nullptr, *z0unf_quadDiff_sys_dw = nullptr, * z0unf_quadDiff_sys_up = nullptr;
  prepareQuadDiff("z0", baseStr, WPStr, z0unf_quadDiff, z0unf_quadDiff_sys_dw, z0unf_quadDiff_sys_up);
  TCanvas c_d0("c_d0", "c_d0", 900, 600);   
  //c_d0.SetLogx();
  //gStyle->SetPalette(1);
  d0unf_quadDiff->SetMarkerSize(0.7);
  //if(WPStr.find("_p") != string::npos) d0unf_quadDiff->SetMaximum(0.1);
  //else d0unf_quadDiff->SetMaximum(0.18);
  d0unf_quadDiff->SetMinimum(0.0);
  d0unf_quadDiff->SetMaximum(0.015);
  d0unf_quadDiff->SetAxisRange(0., 250., "X");
  //d0unf_quadDiff->GetXaxis()->SetLimits(100,250);
  d0unf_quadDiff->SetTitle("d0 smearing #eta - p_{T} map");
  d0unf_quadDiff->GetZaxis()->SetTitle("#sqrt{#sigma^{2}_{data} - #sigma^{2}_{MC}} [mm]");
  d0unf_quadDiff->GetZaxis()->SetTitleOffset(1.4);
  //c_d0.SetLogx();
  d0unf_quadDiff->Draw("colz");
  c_d0.Print(("c_d0_quadDiff_"+WPStr+addStr+".png").c_str());
  if(applyFilter){
    MedianFilter3_1D(d0unf_quadDiff);
    MedianFilter3_1D(d0unf_quadDiff_sys_up);
    MedianFilter3_1D(d0unf_quadDiff_sys_dw);
    d0unf_quadDiff->Draw("colz");
    c_d0.Print(("c_d0_medianFilter_"+WPStr+addStr+".png").c_str());
  }
  TCanvas c_z0("c_z0", "c_z0", 900, 600); //c_z0.SetLogx();
  z0unf_quadDiff->SetMarkerSize(0.7);
  //if(WPStr.find("_p") != string::npos) z0unf_quadDiff->SetMaximum(0.2);
  //else z0unf_quadDiff->SetMaximum(0.9);
  z0unf_quadDiff->SetMinimum(0.0);
  z0unf_quadDiff->SetMaximum(0.3);
  z0unf_quadDiff->SetTitle("z0 smearing #eta - p_{T} map");
  z0unf_quadDiff->SetAxisRange(0., 250., "X");
  z0unf_quadDiff->GetZaxis()->SetTitle("#sqrt{#sigma^{2}_{data} - #sigma^{2}_{MC}} [mm]");
  z0unf_quadDiff->GetZaxis()->SetTitleOffset(1.4);
  z0unf_quadDiff->Draw("colz");
  c_z0.Print(("c_z0_quadDiff_"+WPStr+addStr+".png").c_str());
  if(applyFilter){
    MedianFilter3_1D(z0unf_quadDiff);
    MedianFilter3_1D(z0unf_quadDiff_sys_up);
    MedianFilter3_1D(z0unf_quadDiff_sys_dw);
    z0unf_quadDiff->Draw("colz");
    c_z0.Print(("c_z0_medianFilter_"+WPStr+addStr+".png").c_str());
  }
  TFile* fOut = new TFile(("trackIPAlign"+WPStr+addStr+".root").c_str(), "recreate");
  fOut->mkdir("quad_diff");
  fOut->cd("quad_diff");
  d0unf_quadDiff->Write();
  z0unf_quadDiff->Write();
  d0unf_quadDiff_sys_up->Write();
  d0unf_quadDiff_sys_dw->Write(); 
  z0unf_quadDiff_sys_up->Write();
  z0unf_quadDiff_sys_dw->Write();
  fOut->Close();
}

////////////////////////////////////////
void BuildPalettes(Int_t colorstyle)
{
  if (colorstyle == 0) {
    const Int_t NCont = 100;
    const Int_t NRGBs = 6;
    Double_t stops[NRGBs] = {0.09, 0.24, 0.35, 0.60, 0.80, 1.00};
    Double_t red[NRGBs] =   {0.00, 0.00, 1.00, 0.99, 0.55, 0.20};
    Double_t green[NRGBs] = {0.10, 1.00, 1.00, 0.10, 0.15, 0.04};
    Double_t blue[NRGBs] =  {0.00, 0.00, 0.10, 0.10, 0.99, 0.40};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
    gStyle->SetNumberContours(99);
    std::cout << " -- Palette 0 built -- " << std::endl;
  }
  if (colorstyle == 1) {
    const Int_t NCont = 100;
    const Int_t NRGBs = 6;
    Double_t stops[NRGBs] = {0.10, 0.45, 0.55, 0.65, 0.90, 1.00};
    Double_t red[NRGBs] =   {0.00, 0.00, 0.00, 1.00, 1.00, 0.10};
    Double_t green[NRGBs] = {0.00, 1.00, 1.00, 1.00, 0.00, 0.00};
    Double_t blue[NRGBs] =  {0.10, 1.00, 0.00, 0.00, 0.00, 0.00};
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
    gStyle->SetNumberContours(99);
    std::cout << " -- Palette 1 built -- " << std::endl;
  }
  return;
}
