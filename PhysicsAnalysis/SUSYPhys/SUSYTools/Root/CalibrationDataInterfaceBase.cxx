/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceBase.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SUSYTools/CalibrationDataInterfaceBase.h"

#include "TF1.h"
#include "TMatrixT.h"
#include "TMatrixDSym.h"
#include "TMath.h"

using std::cout;
using std::cerr;
using std::endl;

//================ Constructor =================================================

SUSY::CalibrationDataInterfaceBase::CalibrationDataInterfaceBase()
{}

//================ Destructor ==================================================

SUSY::CalibrationDataInterfaceBase::~CalibrationDataInterfaceBase()
{}

//================ calibration names =============================================

const std::string& SUSY::CalibrationDataInterfaceBase::EffCalibrationName(const std::string& flavour) const
{
  
  // future: use map<>::find
  return m_calibrationEffNames[flavour];
}

void SUSY::CalibrationDataInterfaceBase::setEffCalibrationNames(const std::map<std::string, std::string>& names)
{
  m_calibrationEffNames = names;
}
      
const std::string& SUSY::CalibrationDataInterfaceBase::SFCalibrationName(const std::string& flavour) const
{
  // future: use map<>::find
  return m_calibrationSFNames[flavour];
}

void SUSY::CalibrationDataInterfaceBase::setSFCalibrationNames(const std::map<std::string, std::string>& names)
{
  m_calibrationSFNames = names;
}
      

//====================== retrieval of results ====================================

SUSY::CalibResult
SUSY::CalibrationDataInterfaceBase::getResults (const Double_t* x, const Double_t* xsyst,
						    TF1* fct, TF1* fctsys,
						    const TMatrixDSym* cov) const
{
  // add protection to guard xmin,xmax limits for all pameters??
  double value = fct->EvalPar(x);
  // cout << "value: " << value << endl;
  double variance = (cov) ? parametricVariance(fct, cov, x) : 0;
  //  cout << "variance: " << variance << endl;
  if (fctsys) {
    double syst = fctsys->EvalPar(xsyst);
    variance += syst*syst;
  }

  //  cout << "btag Calib result: " << value << " variance^2: " << variance << endl;

  return std::make_pair<double,double>(value,TMath::Sqrt(variance));
}

//====================== computation of stat. uncertainty ========================

double
SUSY::CalibrationDataInterfaceBase::parametricVariance(TF1* fct,
							   const TMatrixDSym* cov,
							   const Double_t* x) const {

  // use a large value for "eps": this multiplies the uncertainties that
  // are expected to be associated with the parameters.
  // const Double_t eps = 1.0;
  // test: set to 0.5
  const Double_t eps = 0.5;

  int npar = fct->GetNpar();
  if (npar == 0) return 0;

  TMatrixT<double> gradients(npar,1);
  //  std::cout << "parametricVariance: gradients:";
  for (int ipar = 0; ipar < npar; ++ipar) {
    gradients(ipar,0) = fct->GradientPar(ipar, x, eps);
    // std::cout << " " << gradients(ipar,0);
  }
  //  std::cout << std::endl;

  // carry out the matrix multiplication
  TMatrixT<double> gradientsTransposed(TMatrixT<double>::kTransposed, gradients);
  //  std::cout << "parametricVariance: transposed gradients:";
  //  for (int ipar = 0; ipar < npar; ++ipar)
  //    std::cout << " " << gradients(0,ipar);
  //  std::cout << std::endl;
  TMatrixT<double> tmp1(*cov, TMatrixT<double>::kMult, gradients);
  //  std::cout << "parametricVariance: cov * gradients:";
  //  for (int ipar = 0; ipar < npar; ++ipar)
  //    std::cout << " " << tmp1(ipar,0);
  TMatrixT<double> tmp2(gradientsTransposed, TMatrixT<double>::kMult, tmp1);

  return tmp2(0,0);
}

//============================================================================================

std::string SUSY::CalibrationDataInterfaceBase::getBasename (const std::string& OP,
								 const std::string& flavour,
								 const std::string& extra,
								 bool SF) const
{
  std::string basename(OP);
  basename += "/";
  basename += flavour;
  basename += "/";
  // future: use map<>::find
  basename += SF ? m_calibrationSFNames[flavour] : m_calibrationEffNames[flavour];
  basename += extra;

  return basename;
}

const std::vector<int>&
SUSY::CalibrationDataInterfaceBase::functionArguments(const std::string& object) const
{
  return m_functionArguments[object];
}

bool
SUSY::CalibrationDataInterfaceBase::existFunctionArguments(const std::string& object) const
{
  return (m_functionArguments.find(object) != m_functionArguments.end());
}

bool
SUSY::CalibrationDataInterfaceBase::computeFunctionArguments (const std::string& object,
								  const char* ctitle) const
{
  std::string title(ctitle);
  std::string::size_type pos = title.find(";");
  if (pos == std::string::npos || pos == title.size()) {
    // Commented out the following, since the "curve" may have no kinematic dependence
    // (i.e., be a constant).
    // cerr << "in CalibrationDataInterfaceBase: unable to retrieve function arguments from title "
    // 	 << title << endl;
    // return false;
    return true;
  }

  std::vector<int> argumentTypes;
  do {
    title = title.substr(pos+1);
    pos = title.find(";");
    std::string var = title.substr(0, pos);
    int vartype = -1;
    if (var == "eta") {
      vartype = kEta;
      //      msg() << MSG::DEBUG << " eta";
    } else if (var == "abseta") {
      vartype = kAbsEta;
      //      msg() << MSG::DEBUG << " abs(eta)";
    } else if (var == "pt") {
      vartype = kPt;
      //      msg() << MSG::DEBUG << " pt";
    } else {
      cerr << "in CalibrationDataInterfaceBase: cannot construct function argument from name "
	   << var << endl;
      return false;
    }
    argumentTypes.push_back(vartype);
  } while (pos != std::string::npos && pos != title.size());
  m_functionArguments[object] = argumentTypes;

  return true;
}
