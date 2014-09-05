/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"

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
#include "TString.h"
#include "TDirectory.h"

#include "TileMonitoring/TilePaterMonTool.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"

/*---------------------------------------------------------*/
// Methods registering historgrams
// Ownership passed to Gaudi
template <typename T>
void TilePaterMonTool::regHist(const std::string path, T * hist)
{
  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regHist(m_stem+path, hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register histogram : " 
          << m_stem+path << endreq;
    }
  }
}

/*---------------------------------------------------------*/
TilePaterMonTool::TilePaterMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : MonitorToolBase(type, name, parent)
  , m_eventStore("StoreGateSvc", name)
  , m_detStore("DetectorStore", name)
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cabling(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  // the same variable is property THistSvc_OutStream in MonitorToolBase
  declareProperty("histoStreamName",m_THistSvc_streamname = "/SHIFT");
  declareProperty("savePng",m_savePng=false);
  declareProperty("savePs",m_savePs=false);
  declareProperty("saveSvg",m_saveSvg=false);

  // property histoPathBase in MonitorToolBase
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
  MsgStream log(msgSvc(), name());
  
  m_stem=m_THistSvc_streamname+m_path;

  CHECK( m_eventStore.retrieve() );
  CHECK( m_detStore.retrieve() );

  CHECK( m_detStore->retrieve(m_tileID) );
  CHECK( m_detStore->retrieve(m_tileHWID) );

  m_cabling = TileCablingService::getInstance();

  ToolRootHistSvc();

  SetBookStatus(false);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TilePaterMonTool::bookHists()
/*---------------------------------------------------------*/
{ 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in bookHists()" << endreq;
  log << MSG::DEBUG << "Using base path " << m_stem << endreq;
  
  SetBookStatus(true);
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TilePaterMonTool::fillHists()
/*---------------------------------------------------------*/
{ 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in fillHists()" << endreq;
    
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TilePaterMonTool::finalHists()
/*---------------------------------------------------------*/
{ 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in finalHists()" << endreq;
    
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
StatusCode TilePaterMonTool::checkHists(bool /* fromFinalize */)
/*---------------------------------------------------------*/
{ 
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "in checkHists()" << endreq;
    
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
// Method booking 1D Histograms and storing them in THistSvc
// The method return the pointer to the new histogram
TH1D * TilePaterMonTool::book1D(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax)
{
  TH1D *hist = new TH1D(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TH1F * TilePaterMonTool::book1F(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax)
{
  TH1F *hist = new TH1F(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TH1S * TilePaterMonTool::book1S(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax)
{
  TH1S *hist = new TH1S(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

/*---------------------------------------------------------*/
// Method booking 2D Histograms and storing them in THistSvc
// The method return the pointer to the new histogram
TH2D * TilePaterMonTool::book2D(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax,
				 int ny,
				 double ymin,
				 double ymax)
{		
  TH2D *hist = new TH2D(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TH2F * TilePaterMonTool::book2F(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax,
				 int ny,
				 double ymin,
				 double ymax)
{		
  TH2F *hist = new TH2F(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TH2S * TilePaterMonTool::book2S(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax,
				 int ny,
				 double ymin,
				 double ymax)
{		
  TH2S *hist = new TH2S(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TH2C * TilePaterMonTool::book2C(std::string subdir,
                                 std::string nam, 
                                 std::string tit,
                                 int nx,
                                 double xmin, 
                                 double xmax,
				 int ny,
				 double ymin,
				 double ymax)
{		
  TH2C *hist = new TH2C(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

TProfile * TilePaterMonTool::bookProfile(std::string subdir,
                                          std::string nam, 
                                          std::string tit,
                                          int nx,
                                          double xmin, 
                                          double xmax,
                                          double ymin,
                                          double ymax)
{		
  TProfile *hist = new TProfile(TString(nam), TString(tit), nx, xmin, xmax, ymin, ymax);
  regHist(subdir+"/"+nam,hist);
  return hist;
}

/*-----------------------------------------------------------*/
// Method booking TTree and TGraph and storing them in THistSvc
// The method return the pointer to the new histogram
TTree * TilePaterMonTool::bookTree(std::string subdir,
                                    std::string nam, 
                                    std::string tit)
{		
  TTree *hist = new TTree(TString(nam), TString(tit));
  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regTree(m_stem+subdir+"/"+nam, hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : " 
          << m_stem+subdir+"/"+nam << endreq;
    }
  }
  return hist;
}

//
// Terrible hack to register TGraph in THistSvc
//

//#define private public
//#define GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.h"
//#undef GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.icc"
//#undef private

class TGraph1 : public TGraph 
{
public:
  TGraph1(int N, float * X, float * Y) : TGraph(N, X, Y), fDirectory(0) {}
  TDirectory * GetDirectory () { return fDirectory; }
  void SetDirectory(TDirectory *dir) {
    if (fDirectory == dir) return;
    if (fDirectory) fDirectory->GetList()->Remove(this);
    fDirectory = dir;
    if (fDirectory) fDirectory->GetList()->Add(this);
  }
private:
  TDirectory * fDirectory;
};

TGraph * TilePaterMonTool::bookGraph(std::string subdir,
                                      std::string nam, 
                                      std::string tit, int N, float * X, float * Y)
{
  TGraph1 *hist = new TGraph1(N, X, Y);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regGraph(m_stem+subdir+"/"+nam, hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : " 
          << m_stem+subdir+"/"+nam << endreq;
    }
  }
  return (TGraph *)hist;
}

class TGraphErrors1 : public TGraphErrors
{
public:
  TGraphErrors1(int N, float * X, float * Y, float * X_errors, float * Y_errors) : TGraphErrors(N, X, Y, X_errors, Y_errors), fDirectory(0) {}
  TDirectory * GetDirectory () { return fDirectory; }
  void SetDirectory(TDirectory *dir) {
    if (fDirectory == dir) return;
    if (fDirectory) fDirectory->GetList()->Remove(this);
    fDirectory = dir;
    if (fDirectory) fDirectory->GetList()->Add(this);
  }
private:
  TDirectory * fDirectory;
};

TGraphErrors * TilePaterMonTool::bookGraphErrors(std::string subdir, std::string nam, std::string tit, int N, float * X, float * Y, float * X_errors, float * Y_errors)
{
  TGraphErrors *hist = new TGraphErrors(N, X, Y, X_errors, Y_errors);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regGraph(m_stem+subdir+"/"+nam, hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : "
          << m_stem+subdir+"/"+nam << endreq;
    }
  }
  return (TGraphErrors *)hist;
}

StatusCode TilePaterMonTool::removeTObj(TObject *obj)
{
  if (obj!=0) {
    if(m_rootsvc->deReg(obj) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : "
          << obj->GetName() << endreq;
      return  StatusCode::FAILURE;
    }
    else {delete obj;}
  } else {
    MsgStream log(msgSvc(), name());
    log << MSG::WARNING << "Asked to remove NULL pointer" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

class TGraphAsymmErrors1 : public TGraphAsymmErrors
{
public:
  TGraphAsymmErrors1(int N, float * X, float * Y, float * X_errors1, float * X_errors2, float * Y_errors1, float * Y_errors2) : 
    TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2), fDirectory(0) {}
  TDirectory * GetDirectory () { return fDirectory; }
  void SetDirectory(TDirectory *dir) {
    if (fDirectory == dir) return;
    if (fDirectory) fDirectory->GetList()->Remove(this);
    fDirectory = dir;
    if (fDirectory) fDirectory->GetList()->Add(this);
  }
private:
  TDirectory * fDirectory;
};

TGraphAsymmErrors * TilePaterMonTool::bookGraphAsymmErrors(std::string subdir, std::string nam, std::string tit, int N, 
                                                            float * X, float * Y, float * X_errors1, float * X_errors2, 
                                                            float * Y_errors1, float * Y_errors2)
{
  TGraphAsymmErrors *hist = new TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regGraph(m_stem+subdir+"/"+nam, hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : "
          << m_stem+subdir+"/"+nam << endreq;
    }
  }
  return (TGraphAsymmErrors *)hist;
}

class TMultiGraph1 : public TMultiGraph
{
public:
  TMultiGraph1() : TMultiGraph(), fDirectory(0) {}
  TDirectory * GetDirectory () { return fDirectory; }
  void SetDirectory(TDirectory *dir) {
    if (fDirectory == dir) return;
    if (fDirectory) fDirectory->GetList()->Remove(this);
    fDirectory = dir;
    if (fDirectory) fDirectory->GetList()->Add(this);
  }
private:
  TDirectory * fDirectory;
};

TMultiGraph * TilePaterMonTool::bookMultiGraph(std::string subdir, std::string nam, std::string tit)
{
  TMultiGraph1 *hist = new TMultiGraph1();
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  if (m_THistSvc_streamname.size()>0) {
    if(m_rootsvc->regGraph(m_stem+subdir+"/"+nam, (TGraph*)hist) != StatusCode::SUCCESS) {
      MsgStream log(msgSvc(), name());
      log << MSG::WARNING << "Could not register object : "
          << m_stem+subdir+"/"+nam << endreq;
    }
  }
  return (TMultiGraph *)hist;
}


