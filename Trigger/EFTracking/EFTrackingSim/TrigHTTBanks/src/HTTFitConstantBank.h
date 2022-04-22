// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTFITCONSTANTBANK_H
#define HTTFITCONSTANTBANK_H

#include <Eigen/StdVector>
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::MatrixXf)
#include <Eigen/Core>
#include <Eigen/LU>

#include <bitset>
#include <iomanip>
#include <fstream>

#include "AthenaBaseComps/AthMessaging.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTMaps/HTTPlaneMap.h"

class HTTTrack;


class HTTFitConstantBank : public AthMessaging
{
 public:
  
  // missing plane -1 means we guess hits, we don't have separate banks for a missing plane
  HTTFitConstantBank(HTTPlaneMap const * pmap, int ncoords, std::string const & fname, bool isFirstStage, int missingPlane = -1);
  void prepareInvFitConstants();
  
  ///////////////////////////////////////////////////////////////////////
  // Simple Getters
  
  int getBankID() const { return m_bankID; }
  int getNConstr() const { return m_nconstr; }
  int getNCoords() const { return m_ncoords; }
  int getNPars() const { return m_npars; }
  int getNSectors() const { return m_nsectors; }
  int getMissingPlane() const { return m_missingPlane; }
  
  
  bool getIsGood(sector_t sector) const { return m_sector_good[sector]; }
  float getFitConst(int isec, int ipar) const { return m_fit_const(isec, ipar); }
  float getFitPar(int isec, int ipar, int icoord) const { return m_fit_pars(isec, ipar, icoord); }
  float getKaverage(int isec, int iconstr ) const { return m_kaverage(isec, iconstr); }
  float getKernel(int isec, int iconstr, int icoord) const { return m_kernel(isec, iconstr, icoord); }
  
  ///////////////////////////////////////////////////////////////////////
  // Main Interface
  
  bool linfit(sector_t sector, HTTTrack & track, bool isSecondStage) const;
  void linfit_chisq(sector_t sector, HTTTrack &trk) const;
  void linfit_pars_eval(sector_t sector, HTTTrack &trk) const;
  int missing_point_guess(sector_t sector, HTTTrack &track, bool isFirstStage, bool doExtrapolation) const;
  
  void invlinfit(sector_t sector, HTTTrack &track, double const *constr) const;
  void setIdealCoordFit(bool v) { m_isIdealCoordFit = v;}
  
 private:
  
  HTTPlaneMap const * m_pmap = nullptr;
  
  ///////////////////////////////////////////////////////////////////////
  // Configuration
  
  int m_bankID; // ID of the related pattern bank
  int m_nsectors; // number of sectors in this bank
  int m_npars; // numbers of parameters
  int m_ncoords; // number of coordinates used in the linear fit
  int m_nconstr; // number of constraints: m_ncoords-m_npars
  int m_npixcy; // number of 2d coordinates
  int m_missingPlane; // plane that is missing
  bool m_isFirstStage; // is this a first stage fit?
  bool m_isIdealCoordFit; // fitting for ideal coordinates?
  
  ///////////////////////////////////////////////////////////////////////
  // Data
  
  std::vector<int> m_missid; // ids of the missing coordinates, [ncoords]
  
  // these are what we read in
  // they are also used in the inverse function since that is not for the hardware but for bank generation
  std::vector<bool> m_sector_good; //[m_nsectors] false if all constants are 0
  vector3D<float> m_fit_pars;      //[m_nsectors][m_npars][m_ncoords] fit parameters
  vector2D<float> m_fit_const;     //[m_nsectors][m_npars] parameter offset in this sector
  vector3D<float> m_kernel;        //[m_nsectors][m_nconstr][m_ncoords] covariance matrix
  vector2D<float> m_kaverage;      //[m_nsectors][m_nconstr]
  
  vector2D<float> m_maj_a;     // offset for missing coordinate
  vector3D<float> m_maj_kk;    // for majority fits
  vector3D<float> m_maj_invkk; // for majority inverse
  
  std::vector<Eigen::MatrixXf, Eigen::aligned_allocator<Eigen::MatrixXf>> m_invfit_consts;
  
  vector2D<int> m_WCs; //[m_nsectors][m_ncoords] which coordinates are WCs
  
  
  ///////////////////////////////////////////////////////////////////////
  // Helper Functions
  
  void readHeader(std::ifstream & geocfile);
  void readSectorInfo(std::ifstream & geocfile);
  void calculateMajority();
  
};


#endif // HTTFITCONSTANTBANK_H
