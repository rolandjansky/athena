/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Name    : TrigTauHistogram.h
/// Package : TrigTauPerformAnalysis
/// Author  : Olga Igonkina (University of Oregon)
/// Created : October 2007
///
/// DESCRIPTION:
///           Class to perform generic selector function, such as histogramming
///
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _TRIGTAUHISTOGRAM_H
#define _TRIGTAUHISTOGRAM_H

#include <vector>
#include "TH1F.h"
#include "TString.h"
#include "TLorentzVector.h"


class TrigTauHistogram {
public :
  enum HISTTYPE{PTHIST=0, ETAHIST=1,  PHIHIST=2, EXTERNAL=3, NONE=4};

  TrigTauHistogram(HISTTYPE  type = PTHIST);
  virtual ~TrigTauHistogram();

  void doHist( HISTTYPE  type = PTHIST); // change histogramming type!
  static void setHistLimits(HISTTYPE type, int nbin, float xmin, float xmax);
  static void setHistLimits(HISTTYPE type, TString baseName, TString xaxistext, int nbin, float xmin, float xmax);
  int  book(const char * name, const char * title);
  void fill(unsigned int index, float var); 
  void fill(unsigned int index, const TLorentzVector & var); 
  void setExternal(float var) { m_externalVar = var; };


  int nHists() const;
  HISTTYPE type() const {return m_type;};

  int plotHist(unsigned int index, const char * opt="");
  int plotEfficiency(unsigned int indexAfterCut, unsigned int indexBeforeCut, const char * opt="");
  int plotAll();
  int plotAllEfficiencies(int opt = 0 ); // draw all histograms divided on the very first one (id=0)

  float externalVar() const {return m_externalVar; }
  

private:

  struct Definition{
    Definition( TrigTauHistogram::HISTTYPE atype,
		const char * baseN, const char * xtext, int nnbin, float xxmin, float xxmax) :
      type(atype), baseName(baseN), xaxistext(xtext), nbin(nnbin), xmin(xxmin), xmax(xxmax) {};

    TrigTauHistogram::HISTTYPE type;
    TString baseName;
    TString xaxistext;
    int nbin;
    float xmin;
    float xmax;
  };

  static Definition histDefinitions[NONE];

  HISTTYPE          m_type;
  std::vector<TH1F*> m_hist;
  float             m_externalVar;
  

};

#endif
