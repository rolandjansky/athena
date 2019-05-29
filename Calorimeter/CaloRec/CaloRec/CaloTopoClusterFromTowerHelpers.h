// -*- c++ -*- 
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERHELPERS
#define CALOREC_CALOTOPOCLUSTERFROMTOWERHELPERS

#include "CaloGeoHelpers/CaloSampling.h"

#include "xAODCaloEvent/CaloCluster.h"

#include <cstdarg>
#include <string>
#include <map>

#define _SNAMELU( NAME ) { CaloSampling::NAME , std::string( #NAME ) }
#define _SIDLU( ID ) { std::string( #ID ) , CaloSampling::ID } 

#define _MNAMELU( NAME ) { xAOD::CaloCluster::NAME, std::string( #NAME ) }
#define _MIDLU( ID ) { std::string( #ID ) , xAOD::CaloCluster::ID }

class CaloCell;

// namespace xAOD {
//   class CaloCluster;
// }

/// @brief Namespace for helper functions
///
/// These functions are largely a copy of @c xAOD::CaloEvent::CaloKineHelper but avoid the proximity stepper which 
/// seems to create a problem. Additional functions are added.
///
namespace CaloRec {
  namespace Helpers {
    /// @name Types
    /// @{
    typedef std::array<double,CaloSampling::Unknown> valarray_t;  ///< @brief Array accommodating data for samplings
    typedef std::vector<bool>                        boolarray_t; ///< @brief Vector of flags indicating sampling contribution 
    /// @}
    
    /// @name Messaging
    /// @{
    /// @brief Message formatter
    std::string fmtMsg(const char* fmt,...) ;//{
    //   char _buffer[1024];
    //   va_list args;
    //   va_start(args,fmt);
    //   vsprintf(_buffer,fmt,args);
    //   va_end(args);
    //   return std::string(_buffer);
    // }
    /// @}
    
    /// @brief Cache for data accumulator
    struct CaloClusterSignalAccumulator
    {
      /// @brief Pre-fill array with given value
      /// @param array array of numbers
      /// @param value pre-fill value (optional, default is 0.)
      static void fill(valarray_t& array,double value=0.) { array.fill(value); }

      /// @brief Sampling quantities
      struct Sampling
      {
	/// @{ 
	boolarray_t presenceInSample;     ///< @brief Flag for presens of sampling in cluster
	valarray_t  energyInSample;       ///< @brief Energy
	valarray_t  etaInSample;          ///< @brief Eta
	valarray_t  phiInSample;          ///< @brief Phi
	valarray_t  maxEnergyInSample;    ///< @brief Maxmimum energy
	valarray_t  etaMaxEnergyInSample; ///< @brief Eta of maximum energy
	valarray_t  phiMaxEnergyInSample; ///< @brief Phi of maximum energy
	valarray_t  posNormInSample;      ///< @brief Positive (absolute) normalization energy
	/// @}
	Sampling() 
	  : presenceInSample((size_t)CaloSampling::Unknown,false)
	{
	  fill(energyInSample);
	  fill(etaInSample);
	  fill(phiInSample);
	  fill(maxEnergyInSample);
	  fill(etaMaxEnergyInSample);
	  fill(phiMaxEnergyInSample);
	  fill(posNormInSample);
	}
      };
      Sampling sampling;
	
      /// @brief Cluster quantities
      struct Cluster
      { 
	/// @name Actual cell signals
	/// @{
	double cellWeight;    ///< @brief Signal weight
	double cellAbsWeight; ///< @brief Absolute Signal weight
	double cellE;         ///< @brief Signal
	double cellAbsE;      ///< @brief Absolute signal
	/// @}
	/// @name Accumulated cell signals
	/// @{
	double accumE;        ///< @brief Signal
	double accumAbsE;     ///< @brief Absolute signal
	double accumTimeNorm; ///< @brief Time normalization
	double accumEta;      ///< @brief Accumulated energy-weighted eta
	double accumPhi;      ///< @brief Accumulated energy-weighted phi 
	double accumTime;     ///< @brief Time
	int    nBarrel;       ///< @brief Number of cells in barrel 
	int    nEndcap;       ///< @brief Number of cells in endcap
	/// @}
	/// @name Parameters 
	double phiSeed;       ///< @brief Phi seed
	bool   firstCell;     ///< @brief Flags first cell
	/// @}
	/// @brief Constructor
	Cluster() 
	  : cellWeight(0.)
	  , cellAbsWeight(0.)
	  , cellE(0.)
	  , cellAbsE(0.)
	  , accumE(0.)
	  , accumAbsE(0.)
	  , accumTimeNorm(0.)
	  , accumEta(0.)
	  , accumPhi(0.)
	  , accumTime(0.)
	  , nBarrel(0)
	  , nEndcap(0)
	  , phiSeed(0.)
	  , firstCell(true)
	{ }
      };
      Cluster cluster;

      /// @brief Constructor
      CaloClusterSignalAccumulator() 
	: sampling(), cluster()
      { } 
    };

    /// @name Cluster building and refinement
    /// @{
    /// @brief Accumulator function
    ///
    /// This function fills the @c CaloClusterSignalAccumulator object. It is invoked for each cell in a cluster. 
    /// 
    /// @param rcell    reference to non-modifiable @c CaloCell object
    /// @param accum    reference to modifiable @c CaloClusterSignalAccumulator object
    /// @param weight   weight of cell contribution
    /// @param onlyKine update only global cluster kinematics when @c true, else update all cluster variables
    bool cellAccumulator(const CaloCell& rcell,CaloClusterSignalAccumulator& accum,double weight,bool onlyKine=false);
    /// @brief Kinematic updates
    ///
    /// This function updates the kinematics of a cluster e.g. after calibration
    ///
    /// @param pClus    pointer to modifiable cluster object
    /// @param onlyKine update only global cluster kinematics when @c true, else update all cluster variables
    bool calculateKine(xAOD::CaloCluster* pClus,bool onlyKine=false);
    /// @}
  } // Helpers

  ///@brief Stores
  namespace Lookup {
    ///@name Sampling names and identifier maps
    ///@{
    static const std::map<CaloSampling::CaloSample,std::string> samplingNames  = {
      // EM
      _SNAMELU( PreSamplerB ), _SNAMELU( EMB1 ),  _SNAMELU( EMB2 ),  _SNAMELU( EMB3 ), 
      _SNAMELU( PreSamplerE ), _SNAMELU( EME1 ),  _SNAMELU( EME2 ),  _SNAMELU( EME3 ),
      // HAD 
      _SNAMELU( HEC0 ),     _SNAMELU( HEC1 ),     _SNAMELU( HEC2 ),     _SNAMELU( HEC3 ), 
      _SNAMELU( TileBar0 ), _SNAMELU( TileBar1 ), _SNAMELU( TileBar2 ),
      // Tile
      _SNAMELU( TileBar0 ), _SNAMELU( TileBar1 ), _SNAMELU( TileBar2 ),
      _SNAMELU( TileGap1 ), _SNAMELU( TileGap2 ), _SNAMELU( TileGap3 ),
      _SNAMELU( TileExt0 ), _SNAMELU( TileExt1 ), _SNAMELU( TileExt2 ),
      // FCal
      _SNAMELU( FCAL0 ),    _SNAMELU( FCAL1 ),    _SNAMELU( FCAL2 ),
      // Mini-FCal
      _SNAMELU( MINIFCAL0 ),    _SNAMELU( MINIFCAL1 ),    _SNAMELU( MINIFCAL2 ), _SNAMELU( MINIFCAL3 ),
      // unknown
      _SNAMELU( Unknown ) 
    };  ///< Name lookup by sampling identifier (data)
    static const std::map<std::string,CaloSampling::CaloSample> samplingIds = {
      // EM
      _SIDLU( PreSamplerB ), _SIDLU( EMB1 ),  _SIDLU( EMB2 ),  _SIDLU( EMB3 ), 
      _SIDLU( PreSamplerE ), _SIDLU( EME1 ),  _SIDLU( EME2 ),  _SIDLU( EME3 ),
      // HAD 
      _SIDLU( HEC0 ),     _SIDLU( HEC1 ),     _SIDLU( HEC2 ),     _SIDLU( HEC3 ), 
      _SIDLU( TileBar0 ), _SIDLU( TileBar1 ), _SIDLU( TileBar2 ),
      // Tile
      _SIDLU( TileBar0 ), _SIDLU( TileBar1 ), _SIDLU( TileBar2 ),
      _SIDLU( TileGap1 ), _SIDLU( TileGap2 ), _SIDLU( TileGap3 ),
      _SIDLU( TileExt0 ), _SIDLU( TileExt1 ), _SIDLU( TileExt2 ),
      // FCal
      _SIDLU( FCAL0 ),    _SIDLU( FCAL1 ),    _SIDLU( FCAL2 ),
      // Mini-FCal
      _SIDLU( MINIFCAL0 ),    _SIDLU( MINIFCAL1 ),    _SIDLU( MINIFCAL2 ), _SIDLU( MINIFCAL3 ),
      // unknown
      _SIDLU( Unknown )
    };  ///< Identifier lookup by sampling name (data)
    static const std::string&       getSamplingName(CaloSampling::CaloSample sid); ///< Lookup sampling name by identifier (function) 
    static CaloSampling::CaloSample getSamplingId(const std::string& sname);       ///< Lookup sampling identifier by name (function)
    ///@}
    ///@name Cluster moment names and identifier maps
    ///@{
    static const std::map<xAOD::CaloCluster::MomentType,std::string> clusterMomentNames = {
      _MNAMELU( FIRST_PHI ),_MNAMELU( FIRST_ETA ),_MNAMELU( SECOND_R ),_MNAMELU( SECOND_LAMBDA ),_MNAMELU( DELTA_PHI ),_MNAMELU( DELTA_THETA ),_MNAMELU( DELTA_ALPHA ),      
      _MNAMELU( CENTER_X ),_MNAMELU( CENTER_Y ),_MNAMELU( CENTER_Z ),_MNAMELU( CENTER_MAG ),_MNAMELU( CENTER_LAMBDA ),_MNAMELU( LATERAL ),_MNAMELU( LONGITUDINAL ),     
      _MNAMELU( ENG_FRAC_EM ),_MNAMELU( ENG_FRAC_MAX ),_MNAMELU( ENG_FRAC_CORE ),_MNAMELU( FIRST_ENG_DENS ),_MNAMELU( SECOND_ENG_DENS ),_MNAMELU( ENG_POS ),  
      _MNAMELU( ISOLATION ),_MNAMELU( ENG_BAD_CELLS ),_MNAMELU( N_BAD_CELLS ),_MNAMELU( N_BAD_CELLS_CORR ),_MNAMELU( BAD_CELLS_CORR_E ),_MNAMELU( BADLARQ_FRAC ),    
      _MNAMELU( SIGNIFICANCE ),_MNAMELU( CELL_SIGNIFICANCE ),_MNAMELU( CELL_SIG_SAMPLING ),_MNAMELU( AVG_LAR_Q ),_MNAMELU( AVG_TILE_Q ),_MNAMELU( ENG_BAD_HV_CELLS ),
      _MNAMELU( N_BAD_HV_CELLS ),_MNAMELU( PTD ),_MNAMELU( MASS ),_MNAMELU( EM_PROBABILITY ),_MNAMELU( HAD_WEIGHT ),_MNAMELU( OOC_WEIGHT ),_MNAMELU( DM_WEIGHT ),                  
      _MNAMELU( TILE_CONFIDENCE_LEVEL ),_MNAMELU( VERTEX_FRACTION ),_MNAMELU( NVERTEX_FRACTION ),_MNAMELU( ETACALOFRAME ),_MNAMELU( PHICALOFRAME ),_MNAMELU( ETA1CALOFRAME ),    
      _MNAMELU( PHI1CALOFRAME ),_MNAMELU( ETA2CALOFRAME ),_MNAMELU( PHI2CALOFRAME ),_MNAMELU( ENG_CALIB_TOT ),_MNAMELU( ENG_CALIB_OUT_L ),_MNAMELU( ENG_CALIB_OUT_M ),  
      _MNAMELU( ENG_CALIB_OUT_T ),_MNAMELU( ENG_CALIB_DEAD_L ),_MNAMELU( ENG_CALIB_DEAD_M ),_MNAMELU( ENG_CALIB_DEAD_T ),_MNAMELU( ENG_CALIB_EMB0 ),_MNAMELU( ENG_CALIB_EME0 ),
      _MNAMELU( ENG_CALIB_TILEG3 ),_MNAMELU( ENG_CALIB_DEAD_TOT ),_MNAMELU( ENG_CALIB_DEAD_EMB0 ),_MNAMELU( ENG_CALIB_DEAD_TILE0 ),_MNAMELU( ENG_CALIB_DEAD_TILEG3 ),
      _MNAMELU( ENG_CALIB_DEAD_EME0 ),_MNAMELU( ENG_CALIB_DEAD_HEC0 ),_MNAMELU( ENG_CALIB_DEAD_FCAL ),_MNAMELU( ENG_CALIB_DEAD_LEAKAGE ),_MNAMELU( ENG_CALIB_DEAD_UNCLASS ),
      _MNAMELU( ENG_CALIB_FRAC_EM ),_MNAMELU( ENG_CALIB_FRAC_HAD ),_MNAMELU( ENG_CALIB_FRAC_REST ),_MNAMELU( ENERGY_DigiHSTruth ),_MNAMELU( ETA_DigiHSTruth ),_MNAMELU( PHI_DigiHSTruth ),
      _MNAMELU( TIME_DigiHSTruth ),_MNAMELU( ENERGY_CALIB_DigiHSTruth ),_MNAMELU( ETA_CALIB_DigiHSTruth ),_MNAMELU( PHI_CALIB_DigiHSTruth ),_MNAMELU( TIME_CALIB_DigiHSTruth ),
      _MNAMELU( FIRST_PHI_DigiHSTruth ),_MNAMELU( FIRST_ETA_DigiHSTruth ),_MNAMELU( SECOND_R_DigiHSTruth ),_MNAMELU( SECOND_LAMBDA_DigiHSTruth ),_MNAMELU( DELTA_PHI_DigiHSTruth ),
      _MNAMELU( DELTA_THETA_DigiHSTruth ),_MNAMELU( DELTA_ALPHA_DigiHSTruth ),_MNAMELU( CENTER_X_DigiHSTruth ),_MNAMELU( CENTER_Y_DigiHSTruth ),_MNAMELU( CENTER_Z_DigiHSTruth ),
      _MNAMELU( CENTER_MAG_DigiHSTruth ),_MNAMELU( CENTER_LAMBDA_DigiHSTruth ),_MNAMELU( LATERAL_DigiHSTruth ),_MNAMELU( LONGITUDINAL_DigiHSTruth ),_MNAMELU( ENG_FRAC_EM_DigiHSTruth ),
      _MNAMELU( ENG_FRAC_MAX_DigiHSTruth ),_MNAMELU( ENG_FRAC_CORE_DigiHSTruth ),_MNAMELU( FIRST_ENG_DENS_DigiHSTruth ),_MNAMELU( SECOND_ENG_DENS_DigiHSTruth ),
      _MNAMELU( ISOLATION_DigiHSTruth ),_MNAMELU( ENG_BAD_CELLS_DigiHSTruth ),_MNAMELU( N_BAD_CELLS_DigiHSTruth ),_MNAMELU( N_BAD_CELLS_CORR_DigiHSTruth ),
      _MNAMELU( BAD_CELLS_CORR_E_DigiHSTruth ),_MNAMELU( BADLARQ_FRAC_DigiHSTruth ),_MNAMELU( ENG_POS_DigiHSTruth ),_MNAMELU( SIGNIFICANCE_DigiHSTruth ),
      _MNAMELU( CELL_SIGNIFICANCE_DigiHSTruth ),_MNAMELU( CELL_SIG_SAMPLING_DigiHSTruth ),_MNAMELU( AVG_LAR_Q_DigiHSTruth ),_MNAMELU( AVG_TILE_Q_DigiHSTruth ),
      _MNAMELU( ENG_BAD_HV_CELLS_DigiHSTruth ),_MNAMELU( N_BAD_HV_CELLS_DigiHSTruth ),_MNAMELU( EM_PROBABILITY_DigiHSTruth ),_MNAMELU( HAD_WEIGHT_DigiHSTruth ),
      _MNAMELU( OOC_WEIGHT_DigiHSTruth ),_MNAMELU( DM_WEIGHT_DigiHSTruth )
    }; ///< Moment names by moment types
    static const std::map<std::string,xAOD::CaloCluster::MomentType> clusterMomentTypes = {
      _MIDLU( FIRST_PHI ),_MIDLU( FIRST_ETA ),_MIDLU( SECOND_R ),_MIDLU( SECOND_LAMBDA ),_MIDLU( DELTA_PHI ),_MIDLU( DELTA_THETA ),_MIDLU( DELTA_ALPHA ),      
      _MIDLU( CENTER_X ),_MIDLU( CENTER_Y ),_MIDLU( CENTER_Z ),_MIDLU( CENTER_MAG ),_MIDLU( CENTER_LAMBDA ),_MIDLU( LATERAL ),_MIDLU( LONGITUDINAL ),     
      _MIDLU( ENG_FRAC_EM ),_MIDLU( ENG_FRAC_MAX ),_MIDLU( ENG_FRAC_CORE ),_MIDLU( FIRST_ENG_DENS ),_MIDLU( SECOND_ENG_DENS ),_MIDLU( ENG_POS ),  
      _MIDLU( ISOLATION ),_MIDLU( ENG_BAD_CELLS ),_MIDLU( N_BAD_CELLS ),_MIDLU( N_BAD_CELLS_CORR ),_MIDLU( BAD_CELLS_CORR_E ),_MIDLU( BADLARQ_FRAC ),    
      _MIDLU( SIGNIFICANCE ),_MIDLU( CELL_SIGNIFICANCE ),_MIDLU( CELL_SIG_SAMPLING ),_MIDLU( AVG_LAR_Q ),_MIDLU( AVG_TILE_Q ),_MIDLU( ENG_BAD_HV_CELLS ),
      _MIDLU( N_BAD_HV_CELLS ),_MIDLU( PTD ),_MIDLU( MASS ),_MIDLU( EM_PROBABILITY ),_MIDLU( HAD_WEIGHT ),_MIDLU( OOC_WEIGHT ),_MIDLU( DM_WEIGHT ),                  
      _MIDLU( TILE_CONFIDENCE_LEVEL ),_MIDLU( VERTEX_FRACTION ),_MIDLU( NVERTEX_FRACTION ),_MIDLU( ETACALOFRAME ),_MIDLU( PHICALOFRAME ),_MIDLU( ETA1CALOFRAME ),    
      _MIDLU( PHI1CALOFRAME ),_MIDLU( ETA2CALOFRAME ),_MIDLU( PHI2CALOFRAME ),_MIDLU( ENG_CALIB_TOT ),_MIDLU( ENG_CALIB_OUT_L ),_MIDLU( ENG_CALIB_OUT_M ),  
      _MIDLU( ENG_CALIB_OUT_T ),_MIDLU( ENG_CALIB_DEAD_L ),_MIDLU( ENG_CALIB_DEAD_M ),_MIDLU( ENG_CALIB_DEAD_T ),_MIDLU( ENG_CALIB_EMB0 ),_MIDLU( ENG_CALIB_EME0 ),
      _MIDLU( ENG_CALIB_TILEG3 ),_MIDLU( ENG_CALIB_DEAD_TOT ),_MIDLU( ENG_CALIB_DEAD_EMB0 ),_MIDLU( ENG_CALIB_DEAD_TILE0 ),_MIDLU( ENG_CALIB_DEAD_TILEG3 ),
      _MIDLU( ENG_CALIB_DEAD_EME0 ),_MIDLU( ENG_CALIB_DEAD_HEC0 ),_MIDLU( ENG_CALIB_DEAD_FCAL ),_MIDLU( ENG_CALIB_DEAD_LEAKAGE ),_MIDLU( ENG_CALIB_DEAD_UNCLASS ),
      _MIDLU( ENG_CALIB_FRAC_EM ),_MIDLU( ENG_CALIB_FRAC_HAD ),_MIDLU( ENG_CALIB_FRAC_REST ),_MIDLU( ENERGY_DigiHSTruth ),_MIDLU( ETA_DigiHSTruth ),_MIDLU( PHI_DigiHSTruth ),
      _MIDLU( TIME_DigiHSTruth ),_MIDLU( ENERGY_CALIB_DigiHSTruth ),_MIDLU( ETA_CALIB_DigiHSTruth ),_MIDLU( PHI_CALIB_DigiHSTruth ),_MIDLU( TIME_CALIB_DigiHSTruth ),
      _MIDLU( FIRST_PHI_DigiHSTruth ),_MIDLU( FIRST_ETA_DigiHSTruth ),_MIDLU( SECOND_R_DigiHSTruth ),_MIDLU( SECOND_LAMBDA_DigiHSTruth ),_MIDLU( DELTA_PHI_DigiHSTruth ),
      _MIDLU( DELTA_THETA_DigiHSTruth ),_MIDLU( DELTA_ALPHA_DigiHSTruth ),_MIDLU( CENTER_X_DigiHSTruth ),_MIDLU( CENTER_Y_DigiHSTruth ),_MIDLU( CENTER_Z_DigiHSTruth ),
      _MIDLU( CENTER_MAG_DigiHSTruth ),_MIDLU( CENTER_LAMBDA_DigiHSTruth ),_MIDLU( LATERAL_DigiHSTruth ),_MIDLU( LONGITUDINAL_DigiHSTruth ),_MIDLU( ENG_FRAC_EM_DigiHSTruth ),
      _MIDLU( ENG_FRAC_MAX_DigiHSTruth ),_MIDLU( ENG_FRAC_CORE_DigiHSTruth ),_MIDLU( FIRST_ENG_DENS_DigiHSTruth ),_MIDLU( SECOND_ENG_DENS_DigiHSTruth ),
      _MIDLU( ISOLATION_DigiHSTruth ),_MIDLU( ENG_BAD_CELLS_DigiHSTruth ),_MIDLU( N_BAD_CELLS_DigiHSTruth ),_MIDLU( N_BAD_CELLS_CORR_DigiHSTruth ),
      _MIDLU( BAD_CELLS_CORR_E_DigiHSTruth ),_MIDLU( BADLARQ_FRAC_DigiHSTruth ),_MIDLU( ENG_POS_DigiHSTruth ),_MIDLU( SIGNIFICANCE_DigiHSTruth ),
      _MIDLU( CELL_SIGNIFICANCE_DigiHSTruth ),_MIDLU( CELL_SIG_SAMPLING_DigiHSTruth ),_MIDLU( AVG_LAR_Q_DigiHSTruth ),_MIDLU( AVG_TILE_Q_DigiHSTruth ),
      _MIDLU( ENG_BAD_HV_CELLS_DigiHSTruth ),_MIDLU( N_BAD_HV_CELLS_DigiHSTruth ),_MIDLU( EM_PROBABILITY_DigiHSTruth ),_MIDLU( HAD_WEIGHT_DigiHSTruth ),
      _MIDLU( OOC_WEIGHT_DigiHSTruth ),_MIDLU( DM_WEIGHT_DigiHSTruth )
    }; ///< Moment names buy moment identifiers
    static const std::string& getMomentName(xAOD::CaloCluster::MomentType momentType);                                ///< Get moment name associated with a moment type 
    static bool               getMomentType(const std::string& momentName,xAOD::CaloCluster::MomentType& momentType); ///< Get moment type associated with a moment name
    static bool               haveMomentType(const std::string& momentName);                                          ///< Returns @c true in case moment is known  
    ///@}
  } // Lookup
} // CaloRec

inline const std::string&       CaloRec::Lookup::getSamplingName(CaloSampling::CaloSample sid) { return samplingNames.find(sid)->second; }
inline CaloSampling::CaloSample CaloRec::Lookup::getSamplingId(const std::string& sname) { 
  auto fid(samplingIds.find(sname)); return fid != samplingIds.end() ? fid->second : samplingIds.find("Unknown")->second; 
}

inline bool               CaloRec::Lookup::haveMomentType(const std::string& momentName) { return clusterMomentTypes.find(momentName) != clusterMomentTypes.end(); }
inline const std::string& CaloRec::Lookup::getMomentName(xAOD::CaloCluster::MomentType momentType) { return clusterMomentNames.at(momentType); }
inline bool               CaloRec::Lookup::getMomentType(const std::string& momentName,xAOD::CaloCluster::MomentType& momentType) {
  bool isOk(haveMomentType(momentName)); if ( isOk ) { momentType = clusterMomentTypes.at(momentName); } return isOk;
}

 
#endif
