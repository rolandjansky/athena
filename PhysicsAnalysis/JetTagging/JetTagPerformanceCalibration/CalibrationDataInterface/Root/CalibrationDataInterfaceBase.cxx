/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CalibrationDataInterface/CalibrationDataInterfaceBase.h"

#include "TMath.h"

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataInterfaceBase                                                             //
//                                                                                          //
// This is the base class for user-level b-tagging calibration information.                 //
// It is a left-over of the aim for commonality between Athena access (through the          //
// deprecated CalibrationDataInterfaceTool class) and stand-alone (framework independent)   //
// access as provided by the CalibrationDataInterfaceROOT class.                            //
//                                                                                          //
// The only functionality provided at present is that of maintaining the list of            //
// calibration object names to be used in physics analyses.                                 //
//                                                                                          //
// CalibrationDataInterfaceBase.cxx, (c) ATLAS Detector software                            //
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __CINT__
ClassImp(Analysis::CalibrationDataInterfaceBase)
#endif

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceBase::CalibrationDataInterfaceBase()
{}

//================ Destructor ==================================================

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceBase::~CalibrationDataInterfaceBase()
{}

//================ calibration names =============================================

//________________________________________________________________________________
const std::string& Analysis::CalibrationDataInterfaceBase::EffCalibrationName(const std::string& flavour,
									      unsigned int mapIndex) const
{
  // Return the MC efficiency name for the given flavour.
  // Note that no check is performed on the validity of the flavour.
  
  // future: use map<>::find
  return m_calibrationEffNames[flavour][mapIndex];
}

//________________________________________________________________________________
void Analysis::CalibrationDataInterfaceBase::setEffCalibrationNames(const std::map<std::string,
										   std::vector<std::string> >& names)
{
  // Set the MC efficiency names.

  m_calibrationEffNames = names;
}
      
//________________________________________________________________________________
const std::string& Analysis::CalibrationDataInterfaceBase::SFCalibrationName(const std::string& flavour) const
{
  // Return the efficiency scale factor calibration name for the given flavour.
  // Note that no check is performed on the validity of the flavour.
  
  // future: use map<>::find
  return m_calibrationSFNames[flavour];
}

//________________________________________________________________________________
void Analysis::CalibrationDataInterfaceBase::setSFCalibrationNames(const std::map<std::string, std::string>& names)
{
  // Set the efficiency scale factor calibration names.

  m_calibrationSFNames = names;
}
      
// //________________________________________________________________________________
// std::string
// Analysis::CalibrationDataInterfaceBase::getBasename (const std::string& OP,
// 						     const std::string& flavour,
// 						     const std::string& extra,
// 						     bool SF, unsigned int mapIndex) const
// {
//   // Construct the full pathname corresponding to the container indicated by the combination
//   // of tagging operating point, jet flavour, and a possible extra extension. The calibration
//   // container name (stored internally) is also attached.

//   std::string basename(OP);
//   basename += "/";
//   basename += (flavour == "N/A") ? "Light" : flavour;
//   basename += "/";
//   // future: use map<>::find
//   basename += SF ? m_calibrationSFNames[flavour] : m_calibrationEffNames[flavour][mapIndex];
//   basename += extra;

//   return basename;
// }

//________________________________________________________________________________
std::string
Analysis::CalibrationDataInterfaceBase::getContainername (const std::string& flavour,
							  bool SF, unsigned int mapIndex) const
{
  // Construct the full pathname corresponding to the container indicated by the combination
  // of tagging operating point, jet flavour, and a possible extra extension. The calibration
  // container name (stored internally) is also attached.

  if (!SF && mapIndex >= m_calibrationEffNames[flavour].size()) {
    std::cerr << "getContainername: given mapIndex=" << mapIndex << " incompatible with array size "
	      << m_calibrationEffNames[flavour].size() << "; resetting to 0" << std::endl;
    mapIndex = 0;
  }
  std::string name = SF ? m_calibrationSFNames[flavour] : m_calibrationEffNames[flavour][mapIndex];
  name += SF ? "_SF" : "_Eff";

  return name;
}

//________________________________________________________________________________
std::string Analysis::CalibrationDataInterfaceBase::getBasename (const std::string& name) const
{
  // Retrieve the name within the directory starting from the full name

  return name.substr(name.find_last_of('/')+1, std::string::npos);
}

//________________________________________________________________________________
double
Analysis::CalibrationDataInterfaceBase::combinedUncertainty (double stat,
							     const std::pair<double, double>& syst) const 
{
  // Return the total (combined statistical and systematic) uncertainty started from
  // its individual components. The result is symmetrised by using only the larger of
  // the (a priori asymmetric) up- and downward systematic uncertainties.

  // The systematic uncertainty is (a priori) asymmetric, but this interface doesn't
  // at present allow for asymmetric uncertainties.
  // Address this by taking the larger (absolute) value of the two.
  double largest = syst.first;
  if (TMath::Abs(syst.second) > TMath::Abs(largest)) largest = syst.second;

  return TMath::Sqrt(stat*stat + largest*largest);
}
