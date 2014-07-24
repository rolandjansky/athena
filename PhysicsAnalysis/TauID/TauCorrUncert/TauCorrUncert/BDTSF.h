/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BDTSF_H
#define BDTSF_H

#include "CommonSF.h"

namespace TauCorrUncert {

  typedef enum{
    TOTAL,
    QCD,
    WJETS,
    TES,
    REMAIN
  } SYS;
  
  class BDTSF : public CommonSF
  {

  public:

    BDTSF(const std::string& sharepath,
	  bool debug=false);

    ~BDTSF(){}

    double getBDTbinnedSF(int prongness,
			  const double& BDT);
    double getBDTbinnedSF(int prongness,
			  const double& eta,
			  const double& BDT);
    
    double getBDTbinnedSFSysUnc(int prongness,
				const double& BDT,
				const std::string& direction,
				SYS sys = TOTAL);
    double getBDTbinnedSFSysUnc(int prongness,
				const double& eta,
				const double& BDT,
				const std::string& direction,
				SYS sys = TOTAL);

    double getBDTbinnedSFStatUnc(int prongness,
				 const double& BDT,
				 const std::string& direction);
    double getBDTbinnedSFStatUnc(int prongness,
				 const double& eta,
				 const double& BDT,
				 const std::string& direction);

  private:
    std::string ConvertSystematicToString(SYS sys);
    std::string ConvertDirectionToString(const std::string& direction);
    std::string ConvertEtaToString(const float& fEta);
    float GetBDTbinnedSFValue(const std::string& sWorkingPoint, const float& fBDT);
    ClassDef(BDTSF, 1)
  };
} // namespace TauCorrUncert

#endif // BDTSF_H
