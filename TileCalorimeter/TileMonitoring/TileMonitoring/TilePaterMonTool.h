/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TilePaterMonTool.h
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//
// July 2006	
// ********************************************************************
#ifndef TILEPATERMONTOOL_H
#define TILEPATERMONTOOL_H

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaMonitoring/MonitorToolBase.h"

class TH1C;
class TH2C;
class TH1S;
class TH2S;
class TH1F;
class TH2F;
class TH1D;
class TH2D;
class TTree;
class TGraph;
class TProfile;
class TGraphErrors;
class TGraphAsymmErrors;
class TMultiGraph;
class StoreGateSvc;
class TileID;
class TileHWID;
class TileCablingService;

/** @class TilePaterMonTool
 *  @brief Base class for tilecal calibration monitoring tools
 */ 

class TilePaterMonTool: public MonitorToolBase
{

 public:
  
  TilePaterMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TilePaterMonTool();

  StatusCode initialize();
    
  //pure virtual methods
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  virtual StatusCode finalHists();
  StatusCode checkHists(bool fromFinalize);

protected:

  TH1D* book1D(std::string nam, std::string tit, 
               int nx, double xmin, double xmax) 
    { return book1D("",nam,tit,nx,xmin,xmax); }

  TH2D* book2D(std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax)
    { return book2D("",nam,tit,nx,xmin,xmax,ny,ymin,ymax); }


  TH1D* book1D(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax);
  TH1F* book1F(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax);
  TH1S* book1S(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax);
  TH2D* book2D(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax);
  TH2F* book2F(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax);
  TH2S* book2S(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax);
  TH2C* book2C(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax);

  TProfile* bookProfile(std::string dir, std::string nam, std::string tit, 
                        int nx, double xmin, double xmax, 
                        double ymin, double ymax);

  TTree  * bookTree  (std::string dir, std::string nam, std::string tit);

  TGraph * bookGraph (std::string dir, std::string nam, std::string tit, 
                      int N, float * X, float * Y);
  TGraphErrors * bookGraphErrors (std::string dir, std::string nam, std::string tit,
                                  int N, float * X, float * Y, float * X_errors, float * Y_errors);
  TGraphAsymmErrors * bookGraphAsymmErrors (std::string dir, std::string nam, std::string tit,
                                            int N, float * X, float * Y, float * X_errors1, 
                                            float * X_errors2, float * Y_errors1, float * Y_errors2);
  TMultiGraph * bookMultiGraph (std::string dir, std::string nam, std::string tit);

  StatusCode removeTObj(TObject* obj);

  template <typename T>
  void regHist(const std::string path, T * hist);
  
  std::string m_stem;

  ServiceHandle<StoreGateSvc> m_eventStore;
  ServiceHandle<StoreGateSvc> m_detStore;
 
  const TileID* m_tileID;
  const TileHWID* m_tileHWID;
  const TileCablingService* m_cabling;

  bool m_savePng;
  bool m_savePs;
  bool m_saveSvg;

};

#endif
