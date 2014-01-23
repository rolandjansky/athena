/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// HistogramHolder class
///---------------------------------------------------
/// Holder for some standard histograms
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#ifndef HistogramHolder_H
#define HistogramHolder_H

#include <map>
#include "Log.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TLine.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TCanvas.h"
#include "TDirectory.h"

#include "CutsBase.h"
#include "Utils.h"

//instance counter
static int iPlots = 0;  // this no. is included in the histograms name so they don't overwrite when created multiple times

enum HistType { HIST_1D, HIST_2D };

// struct strCmp {
//   bool operator()( std::string s1, std::string s2 ) const {
//     return s1 < s2;
//   }
// };

struct HistInfo {

  //constructr
  HistInfo() : name(""), title(""), legend(""), disp(""), cut(""), optStat(1110), hist(NULL), valX(NULL), valY(NULL), valXVect(NULL), valYVect(NULL), weight(NULL), weightVect(NULL) {  } ;

  void create(HistType typeIn, std::string nameIn, std::string titleIn,
              std::string legendIn, std::string dispIn, std::string cutIn,
              int optStatIn,
              int nBinsXIn, double minXIn, double maxXIn,
              int nBinsYIn = 0, double minYIn = 0, double maxYIn = 0);

  //public attributes
  HistType                type;     // histogram type
  std::string             name;     // histogram name
  std::string             title;    // histogram title
  std::string             legend;   // legend entry
  std::string             disp;     // legend display option
  std::string             cut;      // name of the corresponding cut variable
  int                     optStat; // options of statistics frame

  Int_t                   nBinsX;   // n of bins
  Double_t                minX;     // X-axis minimum
  Double_t                maxX;     // X-axis maximum

  Int_t                   nBinsY;   // n of bins
  Double_t                minY;     // Y-axis minimum
  Double_t                maxY;     // Y-axis maximum

  TH1*                    hist;     // histogram itself

  Double_t*               valX;     // single fill variable
  Double_t*               valY;     // single fill variable

  std::vector<Double_t>*  valXVect; // vector of fill variables
  std::vector<Double_t>*  valYVect; // vector of fill variables

  Double_t*               weight;     // weight of the candidate
  std::vector<Double_t>*  weightVect; // weights of the candidates

};

class HistogramHolder {

  public:
    //constructor
    HistogramHolder() : m_ol(INFO), m_cuts(NULL) { ++iPlots; m_histDir = new TDirectory(); }
    HistogramHolder(OutputLevel ol) : m_ol(ol), m_cuts(NULL) { ++iPlots;  m_histDir = new TDirectory();  }

    //public methods
    bool      isDefined(std::string name);                                      // check whether histogram is defined

    bool      addHistogram1D(std::string nameIn,
                             std::string titleIn,
                             std::string legendIn,
                             std::string dispIn,
                             std::string cutIn,
                             int optStatIn,
                             int nBinsXIn, double minXIn, double maxXIn);       // add TH1D histogram

    bool      addHistogram1D(std::string definition);                           // add TH1D histogram defined by single string

    bool      addHistogram2D(std::string nameIn,
                             std::string titleIn,
                             std::string legendIn,
                             std::string dispIn,
                             std::string cutIn,
                             int optStatIn,
                             int nBinsXIn, double minXIn, double maxXIn,
                             int nBinsYIn, double minYIn, double maxYIn);       // add TH2F histogram

    bool      addHistogram2D(std::string definition);                           // add TH2F histogram defined by single string

    bool      importHistFromFile(std::string fileName);                             // reads histogram definitions from the text file

    HistInfo* getHistInfo(std::string name);                                    // retrieve HistInfo. Null is returned if histogram doesn't exist
    TH1D*     getHist(std::string name);                                        // retrieve 1D histogram. NULL is returned if histogram doesn't exist or has wrong type
    TH2F*     getHist2D(std::string name);                                      // retrieve 2D histogram. NULL is returned if histogram doesn't exist or has wrong type

    void      resetHistVar();                                                   // clear pointers to the histogram variables

    bool      setHistVar(std::string name, Double_t* p, Double_t* weight=NULL); // set fill variable pointer
    bool      setHistVarY(std::string name, Double_t* p);                       // set fill variable pointer

    bool      setHistVar(std::string name, std::vector<Double_t>* p);           // set fill variable pointer
    bool      setHistVarY(std::string name, std::vector<Double_t>* p);          // set fill variable pointer

    bool      setHistVar(std::string name, std::vector<Double_t>* p,
                         Double_t* weight);                                     // set fill variable pointer with weight
    bool      setHistVar(std::string name, std::vector<Double_t>* p,
                         std::vector<Double_t>* weight);                        // set fill variable pointer with weight

    void      fillAll();                                                        // fill all histograms with defined fill variables
    void      saveHist(TFile* file);                                            // save histograms to the file

    void      setTitle(std::string name, std::string title);                    // set histogram title

    void      setMarkerStyle(std::string name,
                             int style,
                             int color=kBlack,
                             double size=1.2,
                             bool doSumw2=true);                                // set histogram marker style

    void      setFillStyle(std::string name, int color);                        // set histogram fill style

    void      drawHist(std::string name, std::string opt="");                   // draws histogram and it's cuts if defined
    void      drawHist(std::vector<std::string> names, std::string opt="");     // draws histograms defined by names
    void      drawHist(NameList names, std::string opt="");                     // draws histograms defined by NameList

    void      drawMoreHist(std::vector<std::string> names, int markerStyle=20); // draws histograms in separate windows
    void      drawMoreHist(NameList names, int markerStyle=20);                 // draws histograms in separate windows

    void      drawLegend(std::vector<std::string> names,
                         double x1, double y1, double x2, double y2,
                         std::vector<TF1*> fun,
                         std::vector<std::string> funName);                     // draws legend
    void      drawLegend(NameList names,
                         double x1, double y1, double x2, double y2,
                         std::vector<TF1*> fun,
                         std::vector<std::string> funName);                     // draws legend
    void      drawLegend(std::vector<std::string> names,
                         double x1, double y1, double x2, double y2);           // draws legend
    void      drawLegend(NameList names,
                         double x1, double y1, double x2, double y2);           // draws legend

    void      drawCut(double cut, double min, double max);                      // draws lines indicating single cut
    void      drawCuts(double cut1, double cut2, double min, double max);       // draws lines indicating cut range
    void      drawCut(double cut, std::string name);                            // draws lines indicating single cut
    void      drawCuts(double cut1, double cut2, std::string name);             // draws lines indicating cut range


    // inline methods

    void setCutsForHist(CutsBase* cuts)  { m_cuts = cuts; }                      // set pointer to the used cuts
    TDirectory* getHistDir() { return m_histDir; }                               // return histogram directory

    std::vector<std::string> getActiveHists() { return m_activeHists; }          // return list of active histograms
    void resetActiveHists() { m_activeHists.clear(); }                           // clear list of active histograms

  private:
    // private attributes
    OutputLevel                     m_ol;           // output level
    std::map<std::string, HistInfo> m_hists;        // histograms
    std::vector<std::string>        m_activeHists;  // list of histograms that heve set histograming variable

    CutsBase*                       m_cuts;         // pointer to the cuts used to create histograms
    TDirectory*                     m_histDir;      // ROOT directory where histograms will be stored

};

///===================================================
/// Begin of definitions
///===================================================
void HistInfo::create(HistType typeIn, std::string nameIn, std::string titleIn,
                      std::string legendIn, std::string dispIn, std::string cutIn,
                      int optStatIn,
                      int nBinsXIn, double minXIn, double maxXIn,
                      int nBinsYIn, double minYIn, double maxYIn)
{
  type      = typeIn;
  name      = nameIn;
  title     = titleIn;
  legend    = legendIn;
  disp      = dispIn;
  cut       = cutIn;
  optStat  = optStatIn;
  nBinsX    = nBinsXIn;
  minX      = minXIn;
  maxX      = maxXIn;
  nBinsY    = nBinsYIn;
  minY      = minYIn;
  maxY      = maxYIn;

  if(type == HIST_1D) {
    hist = new TH1D((name+"_"+iToStr(iPlots)).c_str(), title.c_str(), nBinsX, minX, maxX);
  } else {
    hist = new TH2F((name+"_"+iToStr(iPlots)).c_str(), title.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
  }
}
///===================================================
bool HistogramHolder::isDefined(std::string name) {
  Log log("HistogramHolder::isDefined", m_ol);
  return getHistInfo(name) != NULL;
}
///===================================================
bool HistogramHolder::addHistogram1D(std::string nameIn, std::string titleIn, std::string legendIn,
                                     std::string dispIn, std::string cutIn,
                                     int optStatIn,
                                     int nBinsXIn, double minXIn, double maxXIn)
{
  Log log("HistogramHolder::addHistogram1D", m_ol);

  if( m_hists.find(nameIn) != m_hists.end() ) {
    log.warning("Histogram with name " + nameIn + " already exists. It will be reset");
    getHist(nameIn)->Reset();
    return true;
  }

  //create histogram
  HistInfo hist;
  hist.create(HIST_1D, nameIn, titleIn, legendIn, dispIn, cutIn, optStatIn, nBinsXIn, minXIn, maxXIn);
  m_hists[nameIn] = hist;

  log.debug("1D histogram " + nameIn + " added to the collection");
  return true;

}
///===================================================
bool HistogramHolder::addHistogram1D(std::string definition) {
  Log log("HistogramHolder::addHistogram1D", m_ol);

  //get histogram name
  std::string name;
  if( !getValue(definition, "name=", name) ) {
    //name wasn't defined. Create a default name
    name = "Hist0";
    for(int i=0; m_hists.find(name) != m_hists.end(); name = std::string("Hist") + iToStr(++i) );
  }

  //get histogram title
  std::string title = name;
  getValue(definition, "title=", title);

  //get histogram legend
  std::string legend = name;
  getValue(definition, "legend=", legend);
  std::string disp = "p";
  getValue(definition, "disp=", disp);

  //get cut variable
  std::string cut = "";
  getValue(definition, "cut=", cut);

  //get optStat variable
  int optStat = 1110;
  getValue(definition, "optStat=", optStat);

  //get no of bins, minX and maxX
  int    nBinsX = 100;
  double minX = 0;
  double maxX = 1;
  getValue(definition, "nBinsX=", nBinsX);
  getValue(definition, "minX=", minX);
  getValue(definition, "maxX=", maxX);

  log.info("Histogram: name=" + name + ", title = " + title + ", legend = " + legend + ", cut = " + cut + ", optStat = " + iToStr(optStat) +
      ", nBinsX = " + iToStr(nBinsX) + ", minX = " + fToStr(minX) + ", maxX = " + fToStr(maxX));

  //adding histogram
  return addHistogram1D(name, title, legend, disp, cut, optStat, nBinsX, minX, maxX);

}
///===================================================
bool HistogramHolder::addHistogram2D(std::string nameIn, std::string titleIn, std::string legendIn,
                                     std::string dispIn, std::string cutIn,
                                     int optStatIn,
                                     int nBinsXIn, double minXIn, double maxXIn,
                                     int nBinsYIn, double minYIn, double maxYIn)
{
  Log log("HistogramHolder::addHistogram2D", m_ol);

  if( m_hists.find(nameIn) != m_hists.end() ) {
    log.warning("Histogram with name " + nameIn + " already exists. It will be reset");
    getHist2D(nameIn)->Reset();
    return true;
  }

  HistInfo hist;
  hist.create(HIST_2D, nameIn, titleIn, legendIn, dispIn, cutIn, optStatIn, nBinsXIn, minXIn, maxXIn, nBinsYIn, minYIn, maxYIn);
  m_hists[nameIn] = hist;

  log.debug("2D histogram " + nameIn + " added to the collection");
  return true;

}
///===================================================
bool HistogramHolder::addHistogram2D(std::string definition) {
  Log log("HistogramHolder::addHistogram2D", m_ol);

  //get histogram name
  std::string name;
  if( !getValue(definition, "name=", name) ) {
    //name wasn't defined. Create a default name
    name = "Hist0";
    for(int i=0; m_hists.find(name) != m_hists.end(); name = std::string("Hist") + iToStr(++i) );
  }

  //get histogram title
  std::string title = name;
  getValue(definition, "title=", title);

  //get histogram legend
  std::string legend = name;
  getValue(definition, "legend=", legend);
  std::string disp = "p";
  getValue(definition, "disp=", disp);

  //get cut variable
  std::string cut = "";
  getValue(definition, "cut=", cut);

  //get optStat variable
  int optStat = 1110;
  getValue(definition, "optStat=", optStat);

  //get no of bins, minX and maxX
  int    nBinsX = 100;
  double minX = 0;
  double maxX = 1;
  getValue(definition, "nBinsX=", nBinsX);
  getValue(definition, "minX=", minX);
  getValue(definition, "maxX=", maxX);

  //get no of bins, minY and maxY
  int    nBinsY = 100;
  double minY = 0;
  double maxY = 1;
  getValue(definition, "nBinsY=", nBinsY);
  getValue(definition, "minY=", minY);
  getValue(definition, "maxY=", maxY);

  log.info("Histogram: name=" + name + ", title = " + title + ", legend = " + legend + ", cut = " + cut + ", optStat = " + iToStr(optStat) +
      ", nBinsX = " + iToStr(nBinsX) + ", minX = " + fToStr(minX) + ", maxX = " + fToStr(maxX)+
      ", nBinsY = " + iToStr(nBinsY) + ", minY = " + fToStr(minY) + ", maxY = " + fToStr(maxY));

  //adding histogram
  return addHistogram2D(name, title, legend, disp, cut, optStat, nBinsX, minX, maxX, nBinsY, minY, maxY);

}
///===================================================
HistInfo* HistogramHolder::getHistInfo(std::string name) {
  Log log("HistogramHolder::getHistInfo", m_ol);

  map<std::string, HistInfo>::iterator iter = m_hists.find(name);
  if(iter!=m_hists.end())
    return &( iter->second );

  log.debug("Histogram "+name+" doesn't exist");

  return NULL;
}
///===================================================
TH1D* HistogramHolder::getHist(std::string name) {
  Log log("HistogramHolder::getHist", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo!=NULL && histInfo->type == HIST_1D)
    return dynamic_cast<TH1D*>(histInfo->hist);

  return NULL;

}
///===================================================
TH2F* HistogramHolder::getHist2D(std::string name) {
  Log log("HistogramHolder::getHist2D", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo!=NULL && histInfo->type == HIST_2D)
    return dynamic_cast<TH2F*>(histInfo->hist);

  return NULL;

}
///===================================================
bool HistogramHolder::importHistFromFile(std::string fileName) {
  Log log("HistogramHolder::importHistFromFile", m_ol);

  // backup the current directory
  TDirectory* currentDir = gDirectory;

  // change to the histogram directory
  m_histDir->cd();

  const int maxLength = 1000;
  char buffer[maxLength];

  ifstream fin( fileName.c_str() );

  while( fin.getline(buffer, maxLength) ) {
    string line(buffer);

    // resolve comments: everything after % will be removed
    line = line.substr(0,line.find_first_of("%"));

    log.debug(line);

    // create 1D histograms
    if(line.find("HIST_1D")!=std::string::npos && line.find("name=")!=std::string::npos) {
      if( !addHistogram1D(line) ) return false;
    }

    if(line.find("HIST_2D")!=std::string::npos && line.find("name=")!=std::string::npos) {
      if( !addHistogram2D(line) ) return false;
    }

  }

  // change to the previous directory
  currentDir->cd();

  return true;

}
///===================================================
void HistogramHolder::resetHistVar() {
  Log log("HistogramHolder::resetHistVar", m_ol);

  std::map<std::string, HistInfo>::iterator histsItr = m_hists.begin();
  for(; histsItr!=m_hists.end(); ++histsItr) {
    histsItr->second.valX = NULL;
    histsItr->second.valY = NULL;
    histsItr->second.valXVect = NULL;
    histsItr->second.valYVect = NULL;
  }

}
///===================================================
bool HistogramHolder::setHistVar(std::string name, Double_t* p, Double_t* weight) {
  Log log("HistogramHolder::setHistVar", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo == NULL) {
    log.debug("Histogram " + name + " doesn't exist");
    return false;
  }

  //set fill variable pointer
  histInfo->valX = p;
  if(weight!=NULL) histInfo->weight = weight;

  // update list of active histograms
  if(find(m_activeHists.begin(), m_activeHists.end(), name) == m_activeHists.end())
    m_activeHists.push_back(name);

  return true;
}
///===================================================
bool HistogramHolder::setHistVarY(std::string name, Double_t* p) {
  Log log("HistogramHolder::setHistVarY", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo == NULL) {
    log.debug("Histogram " + name + " doesn't exist");
    return false;
  }

  //set fill variable pointer
  histInfo->valY = p;

  // update list of active histograms
  if(find(m_activeHists.begin(), m_activeHists.end(), name) == m_activeHists.end())
    m_activeHists.push_back(name);

  return true;
}
///===================================================
bool HistogramHolder::setHistVar(std::string name, std::vector<Double_t>* p) {
  Log log("HistogramHolder::setHistVar", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo == NULL) {
    log.debug("Histogram " + name + " doesn't exist");
    return false;
  }

  //set fill variable pointer
  histInfo->valXVect = p;

  // update list of active histograms
  if(find(m_activeHists.begin(), m_activeHists.end(), name) == m_activeHists.end())
    m_activeHists.push_back(name);


  return true;

}
///===================================================
bool HistogramHolder::setHistVarY(std::string name, std::vector<Double_t>* p) {
  Log log("HistogramHolder::setHistVarY", m_ol);

  HistInfo* histInfo = getHistInfo(name);
  if(histInfo == NULL) {
    log.debug("Histogram " + name + " doesn't exist");
    return false;
  }

  //set fill variable pointer
  histInfo->valYVect = p;


  // update list of active histograms
  if(find(m_activeHists.begin(), m_activeHists.end(), name) == m_activeHists.end())
    m_activeHists.push_back(name);


  return true;

}
///===================================================
bool HistogramHolder::setHistVar(std::string name, std::vector<Double_t>* p, Double_t* weight) {
  Log log("HistogramHolder::setHistVar", m_ol);

  if( !setHistVar(name, p) ) return false;
  getHistInfo(name)->weight = weight;
  return true;
}
///===================================================
bool HistogramHolder::setHistVar(std::string name, std::vector<Double_t>* p, std::vector<Double_t>* weight) {
  Log log("HistogramHolder::setHistVar", m_ol);

  if( !setHistVar(name, p) ) return false;
  getHistInfo(name)->weightVect = weight;
  return true;
}
///===================================================
void HistogramHolder::fillAll() {
  Log log("HistogramHolder::fillAll", m_ol);

  std::map<std::string, HistInfo>::iterator histsItr = m_hists.begin();
  for(; histsItr!=m_hists.end(); ++histsItr) {
    log.debug( "Histogram " + histsItr->first);
    //1D histogram
    if(histsItr->second.type == HIST_1D && histsItr->second.hist != NULL) {
      // cast according to hist. type
      TH1D* hist = dynamic_cast<TH1D*>(histsItr->second.hist);
      if(hist==NULL) {
        log.error("1D Histogram " + histsItr->first + " cannot be cast");
        continue;
      }

      log.debug(" ... is 1D histogram");

      //fill single variables
      if(histsItr->second.valX != NULL) {
        log.debug( "Filling single variable for " + histsItr->first +" : " + fToStr(*(histsItr->second.valX)) );
        // get weight
        double weight = 1;
        if(histsItr->second.weight != NULL) weight = *(histsItr->second.weight);

        // fill
        hist->Fill( *(histsItr->second.valX) , weight );
      }

      //fill vector
      if(histsItr->second.valXVect != NULL) {
        log.debug( "Filling vector variable for " + histsItr->first );
        std::vector<Double_t>::iterator valXItr = histsItr->second.valXVect->begin();
        for(int i=0; valXItr != histsItr->second.valXVect->end(); ++valXItr, ++i) {
          // get weight
          double weight = 1;
          if( histsItr->second.weightVect !=NULL && i< (int)histsItr->second.weightVect->size())
            weight = histsItr->second.weightVect->at(i);
          else if(histsItr->second.weight != NULL)
            weight = *(histsItr->second.weight);

          // fill
          hist->Fill(*valXItr, weight);
        }
      }
    }

    //2D histogram
    if(histsItr->second.type == HIST_2D && histsItr->second.hist != NULL) {
      // cast according to hist. type
      TH2F* hist = dynamic_cast<TH2F*>(histsItr->second.hist);
      if(hist==NULL) {
        log.error("2D Histogram " + histsItr->first + " cannot be cast");
        continue;
      }

      //fill single variables
      if(histsItr->second.valX != NULL && histsItr->second.valY != NULL)
        hist->Fill( *(histsItr->second.valX),  *(histsItr->second.valY) );

      //fill vector
      //log.info("Going to fill 2D hist: " + iToStr(histsItr->second.valXVect->size()) + ", " +iToStr(histsItr->second.valYVect->size()) );
      if(histsItr->second.valXVect != NULL && histsItr->second.valYVect != NULL &&
         histsItr->second.valXVect->size() == histsItr->second.valYVect->size() )
      {
        std::vector<Double_t>::iterator valXItr = histsItr->second.valXVect->begin();
        std::vector<Double_t>::iterator valYItr = histsItr->second.valYVect->begin();
        for(int i=0; valXItr != histsItr->second.valXVect->end(); ++valXItr, ++valYItr, ++i) {
          // get weight
          double weight = 1;
          if( histsItr->second.weightVect !=NULL && i< (int)histsItr->second.weightVect->size())
            weight = histsItr->second.weightVect->at(i);
          else if(histsItr->second.weight != NULL)
            weight = *(histsItr->second.weight);

          // fill
          hist->Fill(*valXItr, *valYItr, weight);
        }
      }
    }
  }

  // cleanup
  histsItr = m_hists.begin();
  for(; histsItr!=m_hists.end(); ++histsItr) {
    //1D histogram
    if(histsItr->second.valXVect != NULL) {
      //cleanup for next run
      histsItr->second.valXVect->clear();
    }

    //2D histogram
    if(histsItr->second.type == HIST_2D && histsItr->second.hist != NULL) {
      //fill vector
      if(histsItr->second.valXVect != NULL && histsItr->second.valYVect != NULL &&
         histsItr->second.valXVect->size() == histsItr->second.valYVect->size() )
      {
        //cleanup for next run
        histsItr->second.valXVect->clear();
        histsItr->second.valYVect->clear();
      }
    }

    // clean weights
    if(histsItr->second.weightVect != NULL) {
      //cleanup for next run
      histsItr->second.weightVect->clear();
    }
  }

}
///===================================================
void HistogramHolder::saveHist(TFile* file) {
  Log log("HistogramHolder::save", m_ol);

  file->cd();
  std::map<std::string, HistInfo>::iterator histsItr = m_hists.begin();
  for(; histsItr!=m_hists.end(); ++histsItr) {
    (*histsItr).second.hist->Write();
  }
}
///===================================================
void HistogramHolder::setTitle(std::string name, std::string title) {
  Log log("HistogramHolder::setMarkerStyle", m_ol);

  if(isDefined(name)) {
    getHistInfo(name)->hist->SetTitle(title.c_str());
  }

}
///===================================================
void HistogramHolder::setMarkerStyle(std::string name, int style, int color, double size, bool doSumw2) {
  Log log("HistogramHolder::setMarkerStyle", m_ol);

  if(isDefined(name)) {
    HistInfo* histInfo = getHistInfo(name);
    histInfo->hist->SetMarkerStyle(style);
    histInfo->hist->SetMarkerColor(color);
    histInfo->hist->SetMarkerSize(size);
    if(doSumw2) histInfo->hist->Sumw2();
  }
}
///===================================================
void HistogramHolder::setFillStyle(std::string name, int color) {
  Log log("HistogramHolder::setFillStyle", m_ol);

  if(isDefined(name)) {
    HistInfo* histInfo = getHistInfo(name);
    histInfo->hist->SetFillColor(color);
  }
}
///===================================================
void HistogramHolder::drawHist(std::string name, std::string opt) {
  Log log("HistogramHolder::drawHist", m_ol);

  if(isDefined(name)) {
    HistInfo* histInfo = getHistInfo(name);

    if(histInfo->optStat == 0)
      histInfo->hist->SetStats(0);

    //set histogram range
    //histInfo->hist->SetMinimum(0);
    histInfo->hist->SetMaximum(1.5*histInfo->hist->GetMaximum());

    //draw histogram
    histInfo->hist->Draw(opt.c_str());

    // set optStat
    if(histInfo->optStat != 0 && gStyle->GetOptStat() != histInfo->optStat) {
      log.debug("Different optStat requested");

      gPad->Update();
      TPaveStats *st = (TPaveStats*)histInfo->hist->FindObject("stats");

      if(st!=NULL) {
        log.info("Setting optStat = "+iToStr(histInfo->optStat));
        st->SetOptStat(histInfo->optStat);
        st->Draw();
      }else{
        log.warning("Stats were not found");
      }

    }


    //try to find a corresponding cut variable if requested
    if(histInfo->cut != "" && m_cuts!=NULL) {
      std::vector<CutBase*>::iterator cutsItr = m_cuts->getCuts().begin();
      for(; cutsItr!=m_cuts->getCuts().end(); ++cutsItr) {
        if((*cutsItr)->name == histInfo->cut) {
          // DoubleRangeCut
          if((*cutsItr)->type == CUT_DOUBLE_RANGE) {
            drawCuts(((DoubleRangeCut*)(*cutsItr))->min, ((DoubleRangeCut*)(*cutsItr))->max, name);
          }

          //DoubleCut
          if((*cutsItr)->type == CUT_DOUBLE) {
            drawCut(((DoubleCut*)(*cutsItr))->val, name);
          }

          //IntCut
          if((*cutsItr)->type == CUT_INT) {
            drawCut(((IntCut*)(*cutsItr))->val, name);
          }

        }
      } // end of loop over cut variables
    }
  }

}
///===================================================
void HistogramHolder::drawHist(std::vector<std::string> names, std::string opt) {
  Log log("HistogramHolder::drawHist", m_ol);

  std::vector<std::string>::iterator namesItr = names.begin();
  for(int i=0; namesItr!=names.end(); ++namesItr, ++i) {
    if(isDefined(*namesItr)) {
      if(i == 0 || opt!="") {
        drawHist(*namesItr, opt.c_str());
      }else{
        drawHist(*namesItr, (std::string("same")).c_str());
      }
    }
  }

}
///===================================================
void HistogramHolder::drawHist(NameList names, std::string opt) {
  Log log("HistogramHolder::drawHist", m_ol);

  drawHist(names.names,opt);

}
///===================================================
void HistogramHolder::drawMoreHist(std::vector<std::string> names, int markerStyle) {
  Log log("HistogramHolder::drawMoreHist", m_ol);

  std::vector<std::string>::iterator namesItr = names.begin();
  for(int i=0; namesItr!=names.end(); ++namesItr, ++i) {
    if(isDefined(*namesItr)) {
      new TCanvas((std::string("c_")+(*namesItr)).c_str(), (*namesItr).c_str(),0,0,800,600);
      HistInfo* histInfo = getHistInfo(*namesItr);
      if(histInfo->disp == "f") {
        setFillStyle(*namesItr, kYellow);
      }else{
        setMarkerStyle(*namesItr, markerStyle);
      }

      drawHist(*namesItr);
    }
  }
}
///===================================================
void HistogramHolder::drawMoreHist(NameList names, int markerStyle) {
  Log log("HistogramHolder::drawHist", m_ol);

  drawMoreHist(names.names, markerStyle);

}
///===================================================
void HistogramHolder::drawLegend(std::vector<std::string> names, double x1, double y1, double x2, double y2,
                                 std::vector<TF1*> fun, std::vector<std::string> funName)
{
  Log log("HistogramHolder::drawLegend", m_ol);

  TLegend* leg = new TLegend(x1,y1,x2,y2);

  std::vector<std::string>::iterator namesItr = names.begin();
  for(int i=0; namesItr!=names.end(); ++namesItr, ++i) {
    if(isDefined(*namesItr)) {
      HistInfo* histInfo = getHistInfo(*namesItr);
      leg->AddEntry(histInfo->hist, histInfo->legend.c_str(), histInfo->disp.c_str());
    }
  }

  std::vector<TF1*>::iterator funItr = fun.begin();
  std::vector<std::string>::iterator funNameItr = funName.begin();
  for(; funItr!=fun.end() && funNameItr!=funName.end(); ++funNameItr) {
    leg->AddEntry(*funItr, (*funNameItr).c_str(), "l");
  }

  // draw legend
  leg->SetFillColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
  leg->Draw("same");

}
///===================================================
void HistogramHolder::drawLegend(NameList names, double x1, double y1, double x2, double y2,
                                 std::vector<TF1*> fun, std::vector<std::string> funName)
{
  Log log("HistogramHolder::drawLegend", m_ol);

  drawLegend(names.names, x1, y1, x2, y2, fun, funName);
}
///===================================================
void HistogramHolder::drawLegend(std::vector<std::string> names, double x1, double y1, double x2, double y2) {
  Log log("HistogramHolder::drawLegend", m_ol);

  TLegend* leg = new TLegend(x1,y1,x2,y2);

  std::vector<std::string>::iterator namesItr = names.begin();
  for(int i=0; namesItr!=names.end(); ++namesItr, ++i) {
    if(isDefined(*namesItr)) {
      HistInfo* histInfo = getHistInfo(*namesItr);
      leg->AddEntry(histInfo->hist, histInfo->legend.c_str(), histInfo->disp.c_str());
    }
  }

  // draw legend
  leg->SetFillColor(kWhite);
  leg->SetBorderSize(0);
  leg->Draw("same");

}
///===================================================
void HistogramHolder::drawLegend(NameList names, double x1, double y1, double x2, double y2){
  Log log("HistogramHolder::drawLegend", m_ol);

  drawLegend(names.names, x1, y1, x2, y2);
}
///===================================================
void HistogramHolder::drawCut(double cut, double min, double max) {
  Log log("HistogramHolder::drawCut", m_ol);

  TLine* l = new TLine(cut, min, cut, max);
  l->SetLineStyle(2);
  l->Draw();

}
///===================================================
void HistogramHolder::drawCuts(double cut1, double cut2, double min, double max) {
  Log log("HistogramHolder::drawCuts", m_ol);

  drawCut(cut1, min, max);
  drawCut(cut2, min, max);
}
///===================================================
void HistogramHolder::drawCut(double cut, std::string name) {
  Log log("HistogramHolder::drawCut", m_ol);

  if(isDefined(name)) {
    double max = 0.6*getHistInfo(name)->hist->GetMaximum();
    double min = getHistInfo(name)->hist->GetMinimum();

    drawCut(cut, min, max);
  }
}
///===================================================
void HistogramHolder::drawCuts(double cut1, double cut2, std::string name) {
  Log log("HistogramHolder::drawCuts", m_ol);

  drawCut(cut1, name);
  drawCut(cut2, name);
}
///===================================================

#endif

///===================================================
///      . .
///====o==V==o========================================
