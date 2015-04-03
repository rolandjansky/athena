/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECALGS_TILEFILTERRESULT_H
#define TILERECALGS_TILEFILTERRESULT_H
//*********************************************************************************
// Filename : TileFilterResult.h
// Authors  : F. Merritt, A. Aurisano
// Created  : March 2004
//*********************************************************************************


#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>
#include <vector>

using namespace CLHEP;
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
  HepVector&   getDigRef();
  HepVector*   getDigPt();
  std::vector<int>& getVcrossRef();
  int&         getFitIndexRef();
  int&         getNparRef();
  HepVector&   getParamRef();
  HepVector&   getErrRef();
  HepVector&   getResidRef();
  double&      getChisqRef();
  int          addCross(int kcrIndex);
  int          dropCross(int kcrIndex);
  void         PrintFitParam();
  double       getInTime(double &, double &, double &, double &, double &);
  void         SnapShot(int imode);

 private:

  bool debug;

  HepVector digits;
  double sigDigit;

  int Nparam;
  int Npileup;
  int iFitIndex;
  std::vector<int> Vcross;

  HepVector fitParam;
  HepVector fitErr;

  HepVector residuals;
  double chisq;
};
// **********************************************************************************
#endif // TILERECALGS_TILEFILTERRESULT_H
