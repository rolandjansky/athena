#include "TFile.h"
#include "TH2I.h"
#include "TString.h"

#include "TROOT.h"
#include <iostream>
#include <iomanip>
using namespace std;

void getMeanEff(const TString &file = "expert-monitoring.root") {
  TFile *f = new TFile(file);
  TH2I *h = (TH2I *) f->Get("TrigSteer_EF/signatureAcceptance_EF_runsummary");
  cout.setf(ios::right, ios::adjustfield);
  std::cout << setw(30) << "Signature" << setw(20) << "Input count of EF" << setw(23) << "Fraction thru step 1" << setw(23) << "Fraction thru step 2" << setw(23) << "Fraction thru step 3" << setw(23) << "Fraction thru PS" << setw(23) << "Fraction thru PT" << std::endl;
  for(int i = 0; i < h->GetNbinsX()+1; i++) {
    Double_t input = h->GetBinContent(i, 1);
    Double_t step1 = h->GetBinContent(i, 2);
    Double_t step2 = h->GetBinContent(i, 3);
    Double_t step3 = h->GetBinContent(i, 4);
    Double_t ps = h->GetBinContent(i,5);
    Double_t pt = h->GetBinContent(i,6);
    std::string name(h->GetXaxis()->GetBinLabel(i));
     
    Double_t err_input = (input == 0) ? 0 : (1.0/((Double_t) sqrt(input)));
    Double_t err_step1 = (step1 == 0) ? 0 : (1.0/((Double_t) sqrt(step1)));
    Double_t err_step2 = (step2 == 0) ? 0 : (1.0/((Double_t) sqrt(step2)));
    Double_t err_step3 = (step3 == 0) ? 0 : (1.0/((Double_t) sqrt(step3)));
    Double_t err_ps    = (ps == 0)    ? 0 : (1.0/((Double_t) sqrt(ps)));
    Double_t err_pt    = (pt == 0)    ? 0 : (1.0/((Double_t) sqrt(pt)));
    Double_t err_f1    = (input == 0) ? 0 : (sqrt(((step1*step1/(input*input))*err_input*err_input) + (err_step1*err_step1))/input);
    Double_t err_f2    = (input == 0) ? 0 : (sqrt(((step2*step2/(input*input))*err_input*err_input) + (err_step2*err_step2))/input);
    Double_t err_f3    = (input == 0) ? 0 : (sqrt(((step3*step3/(input*input))*err_input*err_input) + (err_step3*err_step3))/input);
    Double_t err_fps   = (input == 0) ? 0 : (sqrt(((ps*ps/(input*input))*err_input*err_input) + (err_ps*err_ps))/input);
    Double_t err_fpt   = (input == 0) ? 0 : (sqrt(((pt*pt/(input*input))*err_input*err_input) + (err_pt*err_pt))/input);

    if (input) {
    cout.setf(ios::right, ios::adjustfield);
    cout.setf(ios::fixed, ios::floatfield);
    cout << 
    setw(30)  << name << " " << 
    setw(10)  << setprecision(0) << input       << "(" << setw(7) << setprecision(5) << err_input << ") " << 
    setw(10)  << setprecision(5) << ((input == 0) ? step1 : step1) << "(" << setw(10) << setprecision(8) << err_step1    << ") " <<
    setw(10)  << setprecision(5) << ((input == 0) ? step2 : step2) << "(" << setw(10) << setprecision(8) << err_step2    << ") " << 
    setw(10)  << setprecision(5) << ((input == 0) ? step3 : step3) << "(" << setw(10) << setprecision(8) << err_step3    << ") " <<
    setw(10)  << setprecision(5) << ((input == 0) ? ps    : ps)    << "(" << setw(10) << setprecision(8) << err_ps   << ") " <<
    setw(10)  << setprecision(5) << ((input == 0) ? pt    : pt)    << "(" << setw(10) << setprecision(8) << err_pt   << ")"  << 
    endl;
    }
  }
  f->Close();
}

