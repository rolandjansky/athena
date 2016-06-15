/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file ISCT_CalibHistoSvc.cxx
 * Implementation file to interface class for histograms in the SCT_CalibAlgs package
 * @author Shaun Roe
**/
#include "SCT_CalibAlgs/ISCT_CalibHistoSvc.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <stdexcept> // out_of_range exception

int ISCT_CalibHistoSvc::m_nLb=0;
int ISCT_CalibHistoSvc::m_nLbMerge=0;

ISCT_CalibHistoSvc::ISCT_CalibHistoSvc():m_numberOfEventsHisto(0),m_thistSvc(0),m_pSCTHelper(0){
  //std::cout<<"Instantiation of ISCT_CalibHistoSvc"<<std::endl;
  //nop
}

int 
ISCT_CalibHistoSvc::getNumberOfEventsInBin(const int nbin){
    if (m_numberOfEventsHisto) return m_numberOfEventsHisto->GetBinContent(nbin);
    std::cout << "ERROR: a histogram pointer, m_numberOfEventsHisto, is zero" << std::endl;
    return 0;  
}
  
int
ISCT_CalibHistoSvc::size(){
  return m_numberOfEventsHisto->GetEntries();
}


int
ISCT_CalibHistoSvc::size(const int histogramIndex){
  TH1F* pHisto=0;
  int result=0;
  try{
    pHisto=m_phistoVector.at(histogramIndex);
    result = pHisto->GetEntries();
  } catch (std::out_of_range outOfRange) {
     //do nothing, result is zero if the vector<>.at() throws
  }
  return result;
}

double 
ISCT_CalibHistoSvc::getBinForHistogramIndex(const int bin, const int histogramIndex){
  TH1F* pHisto=0;
  double result=0.0;
  try{
    pHisto=m_phistoVector.at(histogramIndex);
    result = pHisto->GetBinContent(bin);
  } catch (std::out_of_range outOfRange) {
      //do nothing, result is zero if the vector<>.at() throws
  }
  return result;
}

void
//ISCT_CalibHistoSvc::binHistograms(const int eventsPerWindow){
ISCT_CalibHistoSvc::binHistograms(const int nLbMerged){

  int nLb = numberOfLb();
  int yAxisBins = TMath::Ceil(1.0*nLb/nLbMerged);
  //  int totalEventNumber = m_numberOfEventsHisto->GetEntries();

  //  int yAxisBins = TMath::Ceil(1.0*totalEventNumber/eventsPerWindow);
  for (std::vector<TH2F *>::iterator it = m_phistoVector2D.begin() ; it != m_phistoVector2D.end(); ++it){
    (*it)->SetBins(768, -0.5, 768+0.5 ,yAxisBins,0.5,nLbMerged*yAxisBins+0.5);
  }
  //   std::cout<<eventsPerWindow<<std::endl;

}

void
ISCT_CalibHistoSvc::setNumberOfLb(const int nLb){
  m_nLb=nLb;
}

int
ISCT_CalibHistoSvc::numberOfLb(){
  return m_nLb;
}

void
ISCT_CalibHistoSvc::setLbToMerge(const int nLbMerge){
  m_nLbMerge=nLbMerge;
}

int
ISCT_CalibHistoSvc::LbToMerge(){
  return m_nLbMerge;
}

