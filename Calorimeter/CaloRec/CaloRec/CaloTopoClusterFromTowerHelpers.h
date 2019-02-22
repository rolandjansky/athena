// -*- c++ -*- 
/* Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration */
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERHELPERS
#define CALOREC_CALOTOPOCLUSTERFROMTOWERHELPERS

#include "CaloGeoHelpers/CaloSampling.h"

#include "xAODCaloEvent/CaloCluster.h"

#include <cstdarg>
#include <string>
#include <map>

#define _SNAMELU( NAME ) { CaloSampling::NAME , #NAME }
#define _SIDLU( ID ) { #ID , CaloSampling::ID } 

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
    static const std::map<CaloSampling::CaloSample,std::string> samplingNames;
    static const std::map<std::string,CaloSampling::CaloSample> samplingIds;
    static const std::string&       getSamplingName(CaloSampling::CaloSample sid);
    static CaloSampling::CaloSample getSamplingId(const std::string& sname);
  }
} // CaloRec

std::map<CaloSampling::CaloSample,std::string> CaloRec::Lookup::samplingNames = {
  // EM
  _SNAMELU( PreSamplerB ), _SNAMELU( EMB1 ),  _SNAMELU( EMB2 ),  _SNAMELU( EMB3 ), 
  _SNAMELU( PreSamplerE ), _SNAMELU( EME1 ),  _SNAMELU( EME2 ),  _SNAMELU( EME3 ),
  // HAD 
  _SNAMELU( HEC0 ),     _SNAMELU( HEC1 ),     _SNAMELU( HEC2 ),     _SNAMELU( HEC3 ), 
  _SNAMELU( TileBar0 ), _SNAMELU( TileBar1 ), _SNAMELU( TileBar2 ),
  // Tile
  _SNAMELU( TileBar0 ), _SNAMELU( TileBar1 ), _SNAMELU( TileBar2 ),
  _SNAMELU( TileGap0 ), _SNAMELU( TileGap1 ), _SNAMELU( TileGap2 ),
  _SNAMELU( TileExt0 ), _SNAMELU( TileExt1 ), _SNAMELU( TileExt2 ),
  // FCal
  _SNAMELU( FCAL0 ),    _SNAMELU( FCAL1 ),    _SNAMELU( FCAL2 ),
  // Mini-FCal
  _SNAMELU( MINIFCAL0 ),    _SNAMELU( MINIFCAL1 ),    _SNAMELU( MINIFCAL2 ), _SNAMELU( MINIFCAL3 ),
  // unknown
  _SNAMELU( Unknown )
};

std::map<CaloSampling::CaloSample,std::string> CaloRec::Lookup::samplingIds = {
  // EM
  _SIDLU( PreSamplerB ), _SIDLU( EMB1 ),  _SIDLU( EMB2 ),  _SIDLU( EMB3 ), 
  _SIDLU( PreSamplerE ), _SIDLU( EME1 ),  _SIDLU( EME2 ),  _SIDLU( EME3 ),
  // HAD 
  _SIDLU( HEC0 ),     _SIDLU( HEC1 ),     _SIDLU( HEC2 ),     _SIDLU( HEC3 ), 
  _SIDLU( TileBar0 ), _SIDLU( TileBar1 ), _SIDLU( TileBar2 ),
  // Tile
  _SIDLU( TileBar0 ), _SIDLU( TileBar1 ), _SIDLU( TileBar2 ),
  _SIDLU( TileGap0 ), _SIDLU( TileGap1 ), _SIDLU( TileGap2 ),
  _SIDLU( TileExt0 ), _SIDLU( TileExt1 ), _SIDLU( TileExt2 ),
  // FCal
  _SIDLU( FCAL0 ),    _SIDLU( FCAL1 ),    _SIDLU( FCAL2 ),
  // Mini-FCal
  _SIDLU( MINIFCAL0 ),    _SIDLU( MINIFCAL1 ),    _SIDLU( MINIFCAL2 ), _SIDLU( MINIFCAL3 ),
  // unknown
  _SIDLU( Unknown )
};

inline const std::string&       CaloRec::Lookup::getSamplingName(CaloSampling::CaloSample sid) { return samplingNames.find(sid)->second; }
inline CaloSampling::CaloSample CaloRec::Lookup::getSamplingId(const std::string& sname) { auto fid(samplingIds.find(sname)); return fid != samplingIds.end() ? fid.second : samplingIds.find("Unknown")->second; }
 
#endif
