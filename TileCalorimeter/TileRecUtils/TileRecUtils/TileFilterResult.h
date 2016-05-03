/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILEFILTERRESULT_H
#define TILERECUTILS_TILEFILTERRESULT_H
//*********************************************************************************
// Filename : TileFilterResult.h
// Authors  : F. Merritt, A. Aurisano
// Created  : March 2004
//*********************************************************************************


#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <vector>

//using namespace CLHEP;
/** Auxiliary class for TileRawChannelMakerManyAmps. 
 */
class TileFilterResult
{
 public:

  // Constructor
  TileFilterResult(std::vector<float> &dig, double sigma);

  // Destructor
  ~TileFilterResult();

  double       getSigDig() const;
  CLHEP::HepVector&   getDigRef();
  std::vector<int>& getVcrossRef();
  int&         getFitIndexRef();
  int&         getNparRef();
  CLHEP::HepVector&   getParamRef();
  CLHEP::HepVector&   getErrRef();
  CLHEP::HepVector&   getResidRef();
  double&      getChi2Ref();
  int          addCross(int kcrIndex);
  int          dropCross(int kcrIndex);
  void         printFitParam();
  double       getInTime(double &, double &, double &, double &, double &);
  void         snapShot(int imode);

 private:

  bool m_debug;

  CLHEP::HepVector m_digits;
  double m_sigDigit;

  int m_nParam;
  int m_nPileup;
  int m_iFitIndex;
  std::vector<int> m_vCross;

  CLHEP::HepVector m_fitParam;
  CLHEP::HepVector m_fitErr;

  CLHEP::HepVector m_residuals;
  double m_chi2;
};
// **********************************************************************************
#endif // TILERECUTILS_TILEFILTERRESULT_H
