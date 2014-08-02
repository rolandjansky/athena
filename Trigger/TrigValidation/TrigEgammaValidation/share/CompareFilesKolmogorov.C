#include "TROOT.h"
#include "TDirectory.h"
#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TKey.h"
#include "TSystem.h"

#include <iostream>
#include <vector>
#include <time.h>
#include <fstream>
#include <string>

Double_t KolmogorovTest(TH1F *hnew, TH1F *href);
Double_t KullbackLeiblerSymTest(TH1F *hnew, TH1F *href);
Double_t JensenShannonTest(TH1F *hnew, TH1F *href);
Double_t QuadraticNegentropyTest(TH1F *hnew, TH1F *href);

void Recurse(TDirectory *File, int &ret, TFile *Ref, ofstream &results, bool completeFile)
{
  // Makes tests between histograms and save results on branches of a TTree
  Double_t kolmogorov;
  Double_t kullback;
  Double_t kl_metric;
  Double_t qn;
  
  TH1F *tmp;
  TH1F *tmp_ref;
  TIter next(File->GetListOfKeys());
  TKey *key;
  TDirectory *kDir;
  while ((key = (TKey *) next())) {
    TString keyname = key->GetName();
    if (std::string(key->GetClassName()).find("TDirectory", 0) != std::string::npos)
      {
	Recurse((TDirectory *) key->ReadObj(), ret, Ref, results, completeFile);
	continue;
      }
    else if (std::string(key->GetClassName()) != std::string("TH1F"))
      { // TODO: Isn't comparing other TH1's!
	continue;
      }

    tmp = (TH1F *) key->ReadObj();

    kDir = key->GetMotherDir();
    if (kDir == 0) {
      cout << "ERROR: Unexpected failure. Can't get mother directory for key." << endl;
      cout << "       Not comparing this histogram." << endl;
      continue;
    }

    TString ThisPath(kDir->GetPath());
    ThisPath += "/";
    ThisPath += key->GetName();

    if (ThisPath.Contains(":")) {
      Ssiz_t loc = ThisPath.Index(":", 0);
      ThisPath.Remove(0, loc+2);
    }

    tmp_ref = (TH1F *) Ref->Get(ThisPath);

    if ( ( tmp == 0 ) || ( tmp_ref == 0 ) ) {
      cout << "ERROR: It wasn't possible to get histogram " << (const char *) ThisPath << endl;
      cout << "       Not comparing this histogram." << endl;
      continue;
    }
    
    if ((tmp->GetEntries() != 0) && (tmp_ref->GetEntries() == 0) ) {
      cout << "WARNING: Found reference histogram with zero entries (" << (const char *) ThisPath << endl;
      continue;
    }
    
    if (tmp->GetEntries() == 0) {
      kolmogorov = 0;
      kullback = 0;
      kl_metric = 999999;
      qn = 99999;
    } else {
      kolmogorov = KolmogorovTest(tmp, tmp_ref);
      kullback = JensenShannonTest(tmp, tmp_ref);
      kl_metric = KullbackLeiblerSymTest(tmp, tmp_ref);
      qn = QuadraticNegentropyTest(tmp, tmp_ref);
    }

    if (completeFile)
      results << "Comparision of histogram named " << ThisPath << " with reference: ";
    else
      results << "Comparision of histogram named " << ((char *) (key->GetName())) << " with reference: ";
    results << kolmogorov*100 << "%" << "   " << kullback*100 << "%" << "   " << kl_metric << "   " << qn << endl;

    if ( (kolmogorov < 0.90) || (kullback < 0.90) )
      ret++;
  }

}

void CompareFilesKolmogorov(TString FilePath, TString RefPath, int &ret, ofstream &results, bool completeFile)
{
  TFile File(FilePath);
  TFile Ref(RefPath);
  if ( (!File.IsZombie()) && (!Ref.IsZombie()) )
    Recurse(&File, ret, &Ref, results, completeFile);

  if (File.IsOpen())
    File.Close();
  if (Ref.IsOpen())
    Ref.Close();

}

Double_t KolmogorovTest(TH1F *hnew, TH1F *href) {
  if ( (hnew->GetEntries() == 0) && (href->GetEntries() == 0) ) {
    return 1.0;
  } else if (href->GetEntries() > 1) {
    return href->KolmogorovTest(hnew, "D");
  }
  return -1.0;
}

Double_t KullbackLeiblerSymTest(TH1F *hnew, TH1F *href) {
  if ( (hnew->GetEntries() == 0) && (href->GetEntries() == 0) ) {
    return 1.0;
  }
  
  // Normal case:
  TH1F *norm_hnew = hnew;
  TH1F *norm_href = href;
  
  Double_t ret = 0.0;
  Double_t D1 = 0.0;
  Double_t D2 = 0.0;
  
  Double_t hnew_int = norm_hnew->Integral();
  Double_t href_int = norm_href->Integral();
  for (unsigned int i = 0; i <= norm_href->GetNbinsX(); ++i) {
    // This should be interpreted as to give in each bin the probability DENSITY for that bin.
    // PS: Should it be implemented?! Maybe we shouldn't divide by the bin width, in order to get a probability per bin.
    //Double_t bin_hnew_i = norm_hnew->GetBinContent(i)/(norm_hnew->GetBinWidth(i)*hnew_int) + 2.0/1.0e52;
    //Double_t bin_href_i = norm_href->GetBinContent(i)/(norm_href->GetBinWidth(i)*href_int) + 2.0/1.0e52;
    Double_t bin_hnew_i = norm_hnew->GetBinContent(i)/(hnew_int) + 2.0/1.0e52;
    Double_t bin_href_i = norm_href->GetBinContent(i)/(href_int) + 2.0/1.0e52;

    // First D[hnew, href] = sum(p_hnew(x)*log(p_hnew(x)/p_href(x)))
    D1 += bin_hnew_i*(log(bin_hnew_i) - log(bin_href_i))/log(2);
    // Then D[href, hnew] = sum(p_href(x)*log(p_href(x)/p_hnew(x)))
    D2 += bin_href_i*(log(bin_href_i) - log(bin_hnew_i))/log(2);
  }

  // Symmetrize it
  ret = (D1 + D2)/2.0;

  // ret is the Symmetrized Kullback-Leibler Divergence
  // It is zero if the distributions are equal (zero distance) or 
  // greater than zero depending on how far the distributions are

  return ret;
}

Double_t JensenShannonTest(TH1F *hnew, TH1F *href) {
  if ( (hnew->GetEntries() == 0) && (href->GetEntries() == 0) ) {
    return 1.0;
  }
  
  // Normal case:
  TH1F *norm_hnew = hnew;
  TH1F *norm_href = href;
  
  Double_t ret = 0.0;
  Double_t D1 = 0.0;
  Double_t D2 = 0.0;
  
  Double_t hnew_int = norm_hnew->Integral();
  Double_t href_int = norm_href->Integral();
  for (unsigned int i = 0; i <= norm_href->GetNbinsX(); ++i) {
    Double_t bin_hnew_i = norm_hnew->GetBinContent(i)/(hnew_int) + 2.0/1.0e52;
    Double_t bin_href_i = norm_href->GetBinContent(i)/(href_int) + 2.0/1.0e52;
    
    Double_t bin_hmean_i = ((norm_hnew->GetBinContent(i)/hnew_int) + (norm_href->GetBinContent(i)/href_int))/2.0 + 2.0/1.0e52;

    // First K[hnew, hmean] = sum(p_hnew(x)*log(p_hnew(x)/p_hmean(x)))
    D1 += bin_hnew_i*(log(bin_hnew_i) - log(bin_hmean_i))/log(2);
    // Then K[href, hmean] = sum(p_href(x)*log(p_href(x)/p_hmean(x)))
    D2 += bin_href_i*(log(bin_href_i) - log(bin_hmean_i))/log(2);
  }

  // Symmetrize it
  ret = (D1 + D2)/2.0;
  ret = 1.0 - ret; // returns 1 for equal distributions and 0 for completely different distributions

  return ret;
}

Double_t QuadraticNegentropyTest(TH1F *hnew, TH1F *href) {
  if ( (hnew->GetEntries() == 0) && (href->GetEntries() == 0) ) {
    return 1.0;
  }
  
  // Normal case:
  TH1F *norm_hnew = hnew;
  TH1F *norm_href = href;
  
  Double_t ret = 0.0;
  
  Double_t hnew_int = norm_hnew->Integral();
  Double_t href_int = norm_href->Integral();
  for (unsigned int i = 0; i <= norm_href->GetNbinsX(); ++i) {
    // This should be interpreted as to give in each bin the probability DENSITY for that bin.
    Double_t bin_hnew_i = norm_hnew->GetBinContent(i)/(hnew_int) + 2.0/1.0e52;
    Double_t bin_href_i = norm_href->GetBinContent(i)/(href_int) + 2.0/1.0e52;

    ret += (bin_hnew_i - bin_href_i)*(bin_hnew_i - bin_href_i);
  }

  // ret is the Quadratic Negentropy
  // It is zero if the distributions are equal (zero distance) or 
  // greater than zero depending on how far the distributions are

  return ret;
}
