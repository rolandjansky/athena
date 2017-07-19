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

#include <cstdint> //for uint32_t
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1C;
class TH2C;
class TH1I;
class TH2I;
class TH1S;
class TH2S;
class TH1F;
class TH2F;
class TH1D;
class TH2D;
class TTree;
class TGraph;
class TProfile;
class TProfile2D;
class TGraphErrors;
class TGraphAsymmErrors;
class TMultiGraph;
class TAxis;
class StoreGateSvc;
class TileID;
class TileTBID;
class TileHWID;
class TileCablingService;

class TH2I_LW;
class TProfile_LW;

/** @class TilePaterMonTool
 *  @brief Base class for tilecal calibration monitoring tools
 */ 

class TilePaterMonTool: public ManagedMonitorToolBase
{

 public:
  
  TilePaterMonTool(const std::string & type, const std::string & name,
		  const IInterface* parent);
    

  ~TilePaterMonTool();

  StatusCode initialize();

  enum TileFragStatus {ALL_OK=0, CRC_ERR=1, ALL_FF=0x10, ALL_00=0x20, NO_FRAG=0x40, NO_ROB=0x80};

protected:
/// Implicit version of book1D 
  TH1D* book1D(std::string nam, std::string tit, 
               int nx, double xmin, double xmax) 
  { 
    return book1D(m_path, nam, tit, nx, xmin, xmax); 
  }

/// Implicit version of book2D 
  TH2D* book2D(std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax)
 {
    return book2D(m_path, nam, tit, nx, xmin, xmax, ny, ymin, ymax);
 }

  TH1D* book1D(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH1F* book1F(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH1I* book1I(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH1S* book1S(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH1C* book1C(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH2D* book2D(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH2F* book2F(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH2F* book2F(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, const double* ybins, 
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH2I* book2I(std::string dir, std::string nam, std::string tit,
               int nx, double xmin, double xmax,
               int ny, double ymin, double ymax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");



  TH2S* book2S(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");

  TH2C* book2C(std::string dir, std::string nam, std::string tit, 
               int nx, double xmin, double xmax, 
               int ny, double ymin, double ymax,
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
               std::string trigChain = "", std::string mergeAlgo = "");


  TProfile* bookProfile(std::string dir, std::string nam, std::string tit, 
                        int nx, double xmin, double xmax, 
                        Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                        std::string trigChain = "", std::string mergeAlgo = "");

  TProfile* bookProfile(std::string dir, std::string nam, std::string tit, 
                        int nx, double xmin, double xmax, 
                        double ymin, double ymax,
                        Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                        std::string trigChain = "", std::string mergeAlgo = "");

  TProfile* bookProfile(std::string dir, std::string nam, std::string tit, 
                        int nx, const float* xbins, 
                        Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                        std::string trigChain = "", std::string mergeAlgo = "");
  TProfile2D* bookProfile2D(std::string dir, std::string nam, std::string tit, 
                            int nx, double xmin, double xmax,
                            int ny, double ymin, double ymax,
                            double zmin, double zmax,
                            Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                            std::string trigChain = "", std::string mergeAlgo = "");

 // TTree* bookTree  (std::string dir, std::string nam, std::string tit);

  TGraph* bookGraph (std::string dir, std::string nam, std::string tit, int N, float* X, float* Y);

  TGraphErrors* bookGraphErrors (std::string dir, std::string nam, std::string tit,
                                  int N, float* X, float* Y, float* X_errors, float* Y_errors);

  TGraphAsymmErrors* bookGraphAsymmErrors (std::string dir, std::string nam, std::string tit,
                                           int N, float* X, float* Y, float* X_errors1,
                                           float* X_errors2, float* Y_errors1, float* Y_errors2);

  TMultiGraph* bookMultiGraph (std::string dir, std::string nam, std::string tit);


  TH2I_LW* book2ILW(std::string dir, std::string nam, std::string tit, 
                    int nx, double xmin, double xmax, 
                    int ny, double ymin, double ymax,
                    Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                    std::string trigChain = "", std::string mergeAlgo = "");

    
  TProfile_LW* bookProfileLW(std::string dir, std::string nam, std::string tit,
                             int nx, double xmin, double xmax,
                             double ymin, double ymax,
                             Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED,
                             std::string trigChain = "", std::string mergeAlgo = "");


  StatusCode removeTObj(TObject* obj);

  using ManagedMonitorToolBase::regHist;
  using ManagedMonitorToolBase::regGraph;

  template <typename T>
  void regHist(const std::string subDir, T* hist, 
               Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED, 
               std::string trigChain = "", std::string mergeAlgo = "" );

  template <typename T>
  void regGraph(const std::string subDir, T* graph, 
                Interval_t interval = run, MgmtAttr_t attribute = ATTRIB_MANAGED, 
                std::string trigChain = "", std::string mergeAlgo = "" );
  

  const TileID* m_tileID;
  const TileHWID* m_tileHWID;
  const TileTBID* m_tileTBID;
  const TileCablingService* m_cabling;

  bool m_savePng;
  bool m_savePs;
  bool m_saveSvg;


  std::string m_EBcellName[48] = { "E3", "E4", "D4", "D4", "C10", "C10", "A12", "A12", "B11", "B11", "A13", "A13"
                                 , "E1", "E2", "B12", "B12", "D5", "D5", "E3*", "E4*", "A14", "A14", "B13", "B13"
                                 ,   "",   "",    "",    "",   "",   "", "B14", "A15", "A15",    "",    "", "B14"
                                 , "B15", "D6", "D6", "B15", "A16", "A16",  "",    "",    "",    "",    "", "" };

  std::string m_LBcellName[48] = { "D0", "A1", "B1", "B1", "A1", "A2", "B2", "B2", "A2",  "A3",  "A3", "B3"
                                 , "B3", "D1", "D1", "A4", "B4", "B4", "A4", "A5", "A5",  "B5",  "B5", "A6"
                                 , "D2", "D2", "A6", "B6", "B6", "A7",   "",   "", "A7",  "B7",  "B7", "A8"
                                 , "A9", "A9", "A8", "B8", "B8", "D3", "B9",   "", "D3", "A10", "A10", "B9" };


  inline std::string getCellName(unsigned int ros, unsigned int channel) {
    if (ros < 3) return m_LBcellName[channel];
    else return m_EBcellName[channel];
  };


  std::string m_TMDB_LB_cell_names[8] = {"D0", "D1L", "D1R", "D2R", "D2L", "D3L", "D3R", ""}; // should be corrected at some time
  std::string m_TMDB_EB_cell_names[4] = {"D5L", "D5R", "D6L", "D6R"};

  inline std::string getTMDBCellName(unsigned int ros, unsigned int channel) {
    if (ros < 3) return m_TMDB_LB_cell_names[channel];
    else return m_TMDB_EB_cell_names[channel];
  };


  // Tells if a channel is disconnected or not
  // Special modules are considered too.
  // NB Input is channel number (0-47)
  // BEWARE: ugly code below this line!
  int m_chMapLB[48] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                      , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                      , 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
                      , 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };

  int m_chMapEB[48] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                      , 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0
                      , 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0
                      , 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };

  int m_chMapEBsp[48] = { 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
                        , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
                        , 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0
                        , 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1 };


  inline bool isDisconnected(int ros, int drawer, int ch) {

    if (ros < 3) { //LB, all standard. Channels 30,31,43 are disconnected
      return m_chMapLB[ch];
    } else {
      if (((ros == 3) && (drawer == 14)) || ((ros == 4) && (drawer == 17))) {//EB, EBA15 and EBC18 are special
        return m_chMapEBsp[ch];
      } else  {//EB standard module
        return m_chMapEB[ch];
      }
    } //end if LB else EB

  }


};

#endif
