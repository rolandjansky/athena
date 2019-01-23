/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#ifndef __EGSELECTOR_SAFETH1L__
#define __EGSELECTOR_SAFETH1L__

#include "TFile.h"
#include "TH1.h"

namespace Root{
namespace EGSelectors{
class SafeTH1{

public :
  SafeTH1(TH1F* hist){
    const int nbins = hist->GetNbinsX();
    m_binContent.resize(nbins,0); // Note that the PDF over/underflows are unused and thus unrepresented here!
    for(int i = 0; i < nbins; ++i){
      m_binContent[i] = hist->GetBinContent(i+1);
    }
    m_firstBinLowEdge = hist->GetBinLowEdge(1);
    m_lastBinLowEdge  = hist->GetBinLowEdge(nbins);
    m_binWidth        = (m_lastBinLowEdge - m_firstBinLowEdge) / (GetNbinsX() - 1);
    m_integral        = hist->Integral(1,nbins);
  }
  ~SafeTH1(){};

  int GetNbinsX() const  {
    int n = m_binContent.size();
    return n;
  }

  int FindBin(double value) const{

    if(value < m_firstBinLowEdge){
      return 0; // first bin of m_binContent
    }
    if(value > m_lastBinLowEdge){
      return GetNbinsX() - 1; // last bin of m_binContent
    }
    // note double rather than float due to incorrect rounding in O(1/10000) cases if float is used
    double bin_double = (value - m_firstBinLowEdge) / m_binWidth; 
    int bin = static_cast<int>(bin_double);
    return bin;
  }

  double GetBinContent(int bin) const {
    int nbins = this->GetNbinsX();
    // since we store the bin content in a vector we need a protection 
    // for cases where we try to access a non-existing bin. In these 
    // cases just go to the last bin
    return (bin>nbins) ? m_binContent[nbins-1] : m_binContent[bin];
  }
  double GetBinLowEdge(int bin) const {
    return m_firstBinLowEdge + m_binWidth*bin;
  }
  double Integral() const{
    return m_integral;
  }

private: 
  std::vector<float> m_binContent;
  double m_firstBinLowEdge;
  double m_lastBinLowEdge;
  double m_binWidth;
  double m_integral;
};
}
}
#endif
