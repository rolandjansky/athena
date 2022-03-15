/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TilePaterMonTool.cxx
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
// 
// July 2006	
// ********************************************************************

#include "TilePaterMonTool.h"

#include "CaloIdentifier/TileID.h"
#include "CaloIdentifier/TileTBID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"

#include "TH1C.h"
#include "TH2C.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TDirectory.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"


#include <sstream>
#include <algorithm>

/*---------------------------------------------------------*/
// Methods registering historgrams
// Ownership passed to Gaudi
template <typename T>
void TilePaterMonTool::regHist(const std::string& subDir, T* hist, Interval_t interval, MgmtAttr_t attribute,  std::string trigChain, std::string mergeAlgo)
{

  std::string path(m_path);
  if (!subDir.empty()) path += ("/" + subDir);

  if(ManagedMonitorToolBase::regHist(hist, path, interval, attribute, trigChain, mergeAlgo).isFailure()) {
    ATH_MSG_WARNING( "Could not register histogram : "  << path + "/" + hist->GetName() );
  }
 
}

template <typename T>
void TilePaterMonTool::regGraph(const std::string& subDir, T* graph, Interval_t interval, MgmtAttr_t attribute,  std::string trigChain, std::string mergeAlgo)
{

  std::string path(m_path);
  if (!subDir.empty()) path += ("/" + subDir);

  if(ManagedMonitorToolBase::regGraph(graph, path, interval, attribute, trigChain, mergeAlgo).isFailure()) {
    ATH_MSG_WARNING( "Could not register Graph : " << path + "/" + graph->GetName() );
  }	 
}


/*---------------------------------------------------------*/
TilePaterMonTool::TilePaterMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_tileTBID(0)
  , m_cabling(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("savePng",m_savePng=false);
  declareProperty("savePs",m_savePs=false);
  declareProperty("saveSvg",m_saveSvg=false);

  declareProperty("FragIDsToIgnoreDMUErrors", m_fragIDsToIgnoreDMUerrors, "List of Tile frag IDs for which ignore DMU errors");

  m_path = "/Tile";

}

/*---------------------------------------------------------*/
TilePaterMonTool::~TilePaterMonTool()
/*---------------------------------------------------------*/
{
}

/*---------------------------------------------------------*/
StatusCode TilePaterMonTool:: initialize()
/*---------------------------------------------------------*/
{

  CHECK( detStore()->retrieve(m_tileID) );
  CHECK( detStore()->retrieve(m_tileHWID) );
  CHECK( detStore()->retrieve(m_tileTBID) );

  std::sort(m_fragIDsToIgnoreDMUerrors.begin(), m_fragIDsToIgnoreDMUerrors.end());

  m_cabling = TileCablingService::getInstance();

  std::ostringstream os;
  for (int fragID : m_fragIDsToIgnoreDMUerrors) {
    os << " 0x" << std::hex << fragID << std::dec;
  }

  ATH_MSG_INFO("Tile DMU errors will be ignored in drawers (frag IDs):" << os.str());

  //ToolRootHistSvc();
  //SetBookStatus(false);
  CHECK(ManagedMonitorToolBase::initialize());


  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
// Method booking 1D Histograms and storing them in THistSvc
// The method return the pointer to the new histogram
TH1D * TilePaterMonTool::book1D(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1D* hist = new TH1D(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1F * TilePaterMonTool::book1F(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1F* hist = new TH1F(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1C* TilePaterMonTool::book1C(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{

  TH1C* hist = new TH1C(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1S * TilePaterMonTool::book1S(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1S* hist = new TH1S(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1S * TilePaterMonTool::book1Sx(std::string subdir, std::string nam, std::string tit,
                                int nx, const Double_t *xlgbins,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1S* hist = new TH1S(TString(nam), TString(tit), nx, xlgbins);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1I* TilePaterMonTool::book1I(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{

  TH1I* hist = new TH1I(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

/*---------------------------------------------------------*/
// Method booking 2D Histograms and storing them in THistSvc
// The method return the pointer to the new histogram
TH2D* TilePaterMonTool::book2D(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, double ymin, double ymax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2D* hist = new TH2D(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2F* TilePaterMonTool::book2F(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                int ny, double ymin, double ymax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{
  TH2F* hist = new TH2F(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2F* TilePaterMonTool::book2F(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, const double* ybins,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2F* hist = new TH2F(TString(nam), TString(tit), nx, xmin, xmax, ny, ybins);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


TH2I* TilePaterMonTool::book2I(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, double ymin, double ymax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2I* hist = new TH2I(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


TH2S * TilePaterMonTool::book2S(std::string subdir, std::string nam, std::string tit,
                                  int nx, double xmin, double xmax,
                                  int ny, double ymin, double ymax,
                                  Interval_t interval, MgmtAttr_t attribute,
                                  std::string trigChain, std::string mergeAlgo)
{
  TH2S *hist = new TH2S(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2C* TilePaterMonTool::book2C(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                int ny, double ymin, double ymax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{
  TH2C* hist = new TH2C(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile* TilePaterMonTool::bookProfile(std::string subdir, std::string nam, std::string tit,
                                          int nx, double xmin, double xmax,
                                          Interval_t interval, MgmtAttr_t attribute,
                                          std::string trigChain, std::string mergeAlgo)
{
  TProfile* hist = new TProfile(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile* TilePaterMonTool::bookProfile(std::string subdir, std::string nam, std::string tit,
                                          int nx, double xmin, double xmax,
                                          double ymin, double ymax,
                                          Interval_t interval, MgmtAttr_t attribute,
                                          std::string trigChain, std::string mergeAlgo)
{
  TProfile* hist = new TProfile(TString(nam), TString(tit), nx, xmin, xmax, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile* TilePaterMonTool::bookProfile(std::string subdir, std::string nam, std::string tit,
                                          int nx, const float* xbins,
                                          Interval_t interval, MgmtAttr_t attribute,
                                          std::string trigChain, std::string mergeAlgo)
{
  TProfile* hist = new TProfile(TString(nam), TString(tit), nx, xbins);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile2D* TilePaterMonTool::bookProfile2D(std::string subdir, std::string nam, std::string tit,
                                             int nx, double xmin, double xmax,
                                             int ny, double ymin, double ymax,
                                             double zmin, double zmax,
                                             Interval_t interval, MgmtAttr_t attribute,
                                             std::string trigChain, std::string mergeAlgo)
{

  TProfile2D* hist = new TProfile2D(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax, zmin, zmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


TH1I_LW* TilePaterMonTool::book1ILW(std::string subdir, std::string nam, std::string tit,
                                    int nx, double xmin, double xmax,
                                    Interval_t interval, MgmtAttr_t attribute,
                                    std::string trigChain, std::string mergeAlgo)
{
  TH1I_LW* hist =  TH1I_LW::create(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2I_LW* TilePaterMonTool::book2ILW(std::string subdir, std::string nam, std::string tit,
                                    int nx, double xmin, double xmax,
                                    int ny, double ymin, double ymax,
                                    Interval_t interval, MgmtAttr_t attribute,
                                    std::string trigChain, std::string mergeAlgo)
{
  TH2I_LW* hist =  TH2I_LW::create(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


TProfile_LW* TilePaterMonTool::bookProfileLW(std::string subdir, std::string nam, std::string tit,
                                             int nx, double xmin, double xmax,
                                             double ymin, double ymax,
                                             Interval_t interval, MgmtAttr_t attribute,
                                             std::string trigChain, std::string mergeAlgo)
{
  TProfile_LW* hist = TProfile_LW::create(TString(nam), TString(tit), nx, xmin, xmax, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


/*-----------------------------------------------------------*/
// Method booking TTree and TGraph and storing them in THistSvc
// The method return the pointer to the new histogram

/*
TTree* TilePaterMonTool::bookTree(std::string subdir, std::string nam, std::string tit) {
  TTree* hist = new TTree(TString(nam), TString(tit));
  if (m_THistSvc_streamname.size() > 0) {
    if (m_THistSvc->regTree(m_stem + subdir + "/" + nam, hist).isFailure()) {
      ATH_MSG_WARNING( "Could not register object : " << m_stem + subdir + "/" + nam );
    }
  }
  return hist;
}

*/


//
// Terrible hack to register TGraph in THistSvc
//

//#define private public
//#define GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.h"
//#undef GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.icc"
//#undef private

class TGraph1: public TGraph {
  public:
    TGraph1(int N, float * X, float * Y)
        : TGraph(N, X, Y), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TGraph* TilePaterMonTool::bookGraph(std::string subdir, std::string nam, std::string tit, int N, float * X, float * Y) {

  TGraph1 *hist = new TGraph1(N, X, Y);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(subdir, hist);

  return (TGraph*) hist;
}

class TGraphErrors1: public TGraphErrors {
  public:
    TGraphErrors1(int N, float * X, float * Y, float * X_errors, float * Y_errors)
        : TGraphErrors(N, X, Y, X_errors, Y_errors), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory * fDirectory;
};

TGraphErrors * TilePaterMonTool::bookGraphErrors(std::string subdir, std::string nam, std::string tit, int N, float * X, float * Y, float * X_errors, float * Y_errors) {

  TGraphErrors *hist = new TGraphErrors(N, X, Y, X_errors, Y_errors);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(subdir, hist);
  return (TGraphErrors *) hist;
}

StatusCode TilePaterMonTool::removeTObj(TObject *obj) {
  if (obj != 0) {
    if (obj->IsA()->InheritsFrom("TH1")) {
      if (deregHist((TH1*) obj).isFailure()) {
        ATH_MSG_WARNING( "Could not dereg Histogram : " << obj->GetName() );
        return StatusCode::FAILURE;
      } else {
        delete obj;
      }
    } else if (obj->IsA()->InheritsFrom("TGraph")) {
      if (deregGraph((TGraph*) obj) != StatusCode::SUCCESS) {
        ATH_MSG_WARNING( "Could not dereg Graph : " << obj->GetName() );
        return StatusCode::FAILURE;
      } else {
        delete obj;
      }
    } else {
      ATH_MSG_WARNING( "Asked to remove object " << obj->GetName() << "of unsupported type " << obj->IsA() );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_WARNING( "Asked to remove NULL pointer" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


class TGraphAsymmErrors1: public TGraphAsymmErrors {
  public:
    TGraphAsymmErrors1(int N, float * X, float * Y, float * X_errors1, float * X_errors2, float * Y_errors1, float * Y_errors2)
        : TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TGraphAsymmErrors* TilePaterMonTool::bookGraphAsymmErrors(std::string subdir, std::string nam, std::string tit, int N,
                                                           float* X, float* Y, float* X_errors1, float* X_errors2,
                                                           float* Y_errors1, float* Y_errors2)
{

  TGraphAsymmErrors *hist = new TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(subdir, hist);
  return (TGraphAsymmErrors*) hist;
}

class TMultiGraph1: public TMultiGraph {
  public:
    TMultiGraph1()
        : TMultiGraph(), fDirectory(0) {
    }

    TDirectory* GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TMultiGraph* TilePaterMonTool::bookMultiGraph(std::string subdir, std::string nam, std::string tit) {

  TMultiGraph1* hist = new TMultiGraph1();
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(subdir, (TGraph*) hist);
  return (TMultiGraph*) hist;
}



