/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MCAST_MUONSMEARINGTOOL_H
#define MCAST_MUONSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "MuonMomentumCorrections/IMuonCalibrationAndSmearingTool.h"

// C++ include(s)
#include <boost/unordered_map.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <map>

namespace CP {

typedef enum { SMEAR_PT=1, SMEAR_QPT=2 } SMEARTYPE;
typedef enum { SCALE_DEFAULT=1, SCALE_KPKM=2, SCALE_KC=3, SCALE_K=4, SCALE_C=5 } SCALETYPE;
typedef enum { DET_MS=1, DET_ID=2, DET_CB=3 } DETTYPE;
typedef enum { ID_UP=1, ID_LOW=2, MS_UP=3, MS_LOW=4,SCALE_UP=5, SCALE_LOW=6 } SYSTTYPE;
typedef enum { DATA10=1, DATA11=2,DATA12=3 } DATATYPE;
typedef enum { MUID=1, STACO=2, MUONS=3 } ALGOTYPE;
typedef enum { REL16_6=1, REL17=2, REL17_2=3, REL17_2_REPRO=4, REL17_2_SUM13=5 } RELTYPE;
const double m_defaultInit = -999.;

class MuonCalibrationAndSmearingTool : 
  public virtual IMuonCalibrationAndSmearingTool,
  public virtual ISystematicsTool,
  public asg::AsgTool {

  //::: Create a proper constructor for Athena
  ASG_TOOL_CLASS2( MuonCalibrationAndSmearingTool, CP::IMuonCalibrationAndSmearingTool, CP::ISystematicsTool )

public:

  //::: Create a constructor for standalone usage
  MuonCalibrationAndSmearingTool( const std::string& name );

  //::: Regions helpers 
  StatusCode   Regions( std::string inRegionFile, int doMacroRegionsFlag = 0 );
  void         PrintRegions() const;
  unsigned int GetNRegions() const;
  int          GetRegion( const double eta, const double phi ) const;
  float        GetRegionInnerEta( const int r_i ) const; //Return Eta closer to the origin
  std::string  GetRegionName( const int r_i ) const;
  std::string  GetRegionName( const double eta, const double phi ) const;

  virtual StatusCode initialize();

  //::: Apply the correction on a modifyable object
  virtual CorrectionCode applyCorrection( xAOD::Muon& mu );
  //::: Create a corrected copy from a constant muon
  virtual CorrectionCode correctedCopy( const xAOD::Muon& input, xAOD::Muon*& output );

  //::: Is the tool affected by a specific systematic?
  virtual bool isAffectedBySystematic( const SystematicVariation& systematic ) const;
  //::: Which systematics have an effect on the tool's behaviour?
  virtual SystematicSet affectingSystematics() const;
  //::: Systematics to be used for physics analysis
  virtual SystematicSet recommendedSystematics() const;
  //::: Use specific systematic
  virtual SystematicCode applySystematicVariation ( const SystematicSet& systConfig );

  double GetSmearing( int, xAOD::Muon& );
  double GetSystVariation( int DetType, double var );
  int GetScaleRegion( xAOD::Muon& );
  void CalcCBWeights( xAOD::Muon& );
  double CalculatePt( const int DetType, const double inSmearID = m_defaultInit, const double inSmearMS = m_defaultInit, const double scaleVar = 0. ) const;
  StatusCode FillValues();
  void Clean();
  double ScaleApply( const double pt, const double S1, const double S2, double S = 1.0, const double S_EnLoss = 0. ) const;
  void CleanScales();
  void CollectMacroRegionsSL();//Small and large regions are collected together
  void CollectMacroRegionsSL_UpDn();//Small,Large,Up,Down regions are collected together
  void CollectMacroRegionsSL_SplitBAR();//Large,Small sectors split plus Feet(12+14) and 11+15 sector split in Barrel
  double ExpectedResolution( const std::string& DetType, xAOD::Muon& mu, const bool mc = false ) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)
  double ExpectedResolution( const int DetType, xAOD::Muon& mu, const bool mc = false ) const; //!< Expected resolution in data (or unsmeard MC if second argument is true)

  DATATYPE SetData( std::string );
  ALGOTYPE SetAlgorithm( std::string );
  RELTYPE  SetRelease( std::string );

private:
  struct ParameterSet { 
    DETTYPE SubDetectorType; 
    double VarSmearingID; 
    double VarSmearingMS; 
    double Scale; 
  };
  boost::unordered_map< SystematicSet, ParameterSet > m_Parameters;
  ParameterSet *m_currentParameters;
  double m_pTmax;
  double m_smearDeltaMS, m_smearDeltaID, m_smearDeltaCB;
  std::string m_year, m_algo, m_type, m_release, m_path;
  SMEARTYPE m_Tsmear;
  SCALETYPE m_Tscale;
  DETTYPE   m_Tdet;
  SYSTTYPE  m_Tfun;
  DATATYPE  m_Tdata;
  RELTYPE   m_Trel;
  ALGOTYPE  m_Talgo;
  int m_nb_regions;
  int m_detRegion;
  double m_useNsigmaForICombine;
  double m_ptms, m_ptid, m_ptcb, m_eta, m_phi;
  std::vector<double> m_scale_ID, m_enLoss_MS, m_scale_MS, m_scale_CB;
  //sys variations (stat error added in quadrature), one if it's simmetrized, 2 if Up != Dw.
  std::vector<double> m_scaleSyst_ID, m_enLossSyst_MS, m_scaleSyst_MS, m_scaleSyst_CB;
  std::vector<double> m_scaleSystUp_ID, m_enLossSystUp_MS, m_scaleSystUp_MS;
  std::vector<double> m_scaleSystDw_ID, m_enLossSystDw_MS, m_scaleSystDw_MS;

  std::vector<double> m_scaleBins;
  std::vector<double> m_S1_ID;
  std::vector<double> m_S2_ID;
  std::vector<double> m_S1_MS;
  std::vector<double> m_S2_MS;
  std::vector<double> m_S1_CB;
  std::vector<double> m_S2_CB;
  std::vector<double> m_S1Corr_ID;
  std::vector<double> m_S2Corr_ID;
  std::vector<double> m_S1Corr_MS;
  std::vector<double> m_S2Corr_MS;
  std::vector<double> m_S1Corr_CB;
  std::vector<double> m_S2Corr_CB;
  std::vector<double> m_S1ACorr_ID;
  std::vector<double> m_S2ACorr_ID;
  std::vector<double> m_S1ACorr_MS;
  std::vector<double> m_S2ACorr_MS;
  std::vector<double> m_S1ACorr_CB;
  std::vector<double> m_S2ACorr_CB;
  std::vector<double> m_p1_ID, m_p2_ID, m_p2_ID_TAN, m_p0_MS, m_p1_MS, m_p2_MS;
  std::vector<double> m_E_p1_ID, m_E_p2_ID, m_E_p2_ID_TAN, m_E_p0_MS, m_E_p1_MS, m_E_p2_MS;
  // syst. errors on resolution parameters corrections:
  //one if it's simmetrized, then Stat and Sys err are separate in cfg file.
  std::vector<double> m_S_p1_ID, m_S_p2_ID, m_S_p2_ID_TAN, m_S_p0_MS, m_S_p1_MS, m_S_p2_MS;
  //Two if Up != Dw, Stat and Sys err added in quadrature in cfg file.
  std::vector<double> m_SUp_p1_ID, m_SUp_p2_ID, m_SUp_p2_ID_TAN, m_SUp_p0_MS, m_SUp_p1_MS, m_SUp_p2_MS;
  std::vector<double> m_SDw_p1_ID, m_SDw_p2_ID, m_SDw_p2_ID_TAN, m_SDw_p0_MS, m_SDw_p1_MS, m_SDw_p2_MS;
  std::vector<double> m_MC_p1_ID, m_MC_p2_ID, m_MC_p2_ID_TAN, m_MC_p0_MS, m_MC_p1_MS, m_MC_p2_MS;
  std::vector<double> m_CorrMatC0;
  std::vector<double> m_CorrMatC1;
  std::vector<double> m_CorrMatC2;
  std::vector<double> m_CorrMatC3;
  std::vector<double> m_CorrMatC4;
  std::vector<double> m_CorrMatC5;
  std::vector<double> m_CorrMatTanC0;
  std::vector<double> m_CorrMatTanC1;
  std::vector<double> m_CorrMatTanC2;
  std::vector<double> m_CorrMatTanC3;
  std::vector<double> m_CorrMatTanC4;
  std::vector<double> m_CorrMatTanC5;
  std::ifstream m_InValues; //! don't persistify
  double m_weightMS, m_weightID;
  double m_g0, m_g1, m_g2, m_g3, m_g4, m_charge;

  bool m_verb;
  bool m_loadNames;
  /* number of regions */
  //int m_nb_regions;
  /* eta boundaries of the regions */
  std::vector<float> m_eta_min;
  std::vector<float> m_eta_max;
  /* phi boundaries of the regions */
  std::vector<float> m_phi_min;
  std::vector<float> m_phi_max;
  std::vector<std::string> m_names;
  //In same cases I need to collect the simple regions in macroRegions according to specific criteria
  bool m_doMacroRegions;
  //This maps the standard regions indexes into the indexes of the macroRegions.

  std::map<int, int> m_macroRegionIdxMap;
  // The macro regions themselves are stored in a vector with their name
  std::vector<std::string> m_macroRegionName;
  std::vector<double> m_macroRegionInnerEta;//I need this var in few occasions


  int m_scaleRegion;
}; // class MuonCalibrationAndSmearingTool

} // namespace CP

#endif // MCAST_MUONSMEARINGTOOL_H
