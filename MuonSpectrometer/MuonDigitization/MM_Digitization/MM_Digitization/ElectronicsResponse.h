/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronicsResponse_h
#define ElectronicsResponse_h
/** @class ElectronicsResponse

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch> 
//   Jessica Metcalfe        <jessica.metcalfe@gmail.com>
//////////////////////////////////////////////////////////////////////////////
 
Comments to be added here....


*/

/// ROOT
#include <TROOT.h>
#include <TH1.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TMath.h>

/// STD'S
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <sstream>
#include <sys/stat.h>

/// Projects
#include "MM_Digitization/MmElectronicsToolInput.h"
#include "MM_Digitization/MmDigitToolOutput.h"
#include "MM_Digitization/StripsResponse.h"

using std::vector;
using std::cout;
using std::endl;

/// ROOT Classed
class TF1;
class TH1;
class TH1F;
class TH2F;

class ElectronicsResponse {
  
private:
  /** power of responce function */
  float alpha; 
  /**  */
  float RC ;
  /** hreshold "Voltage" for histoBNL */
  float electronicsThreshold;
  float m_StripResponse_qThreshold;
  float m_StripResponse_driftGap;
  float m_StripResponse_driftVelocity;

  TF1 *intFn;
  StripsResponse* stripObject ;  
  ElectronicsResponse & operator=(const ElectronicsResponse &right);
  ElectronicsResponse(const ElectronicsResponse&);
public :

  ElectronicsResponse();
  virtual ~ElectronicsResponse();
  void clearValues ();
  void bnlResponceFunction(const vector <int> & numberofStrip, const vector <float> & qStrip, const vector <float> & tStrip);
  MmDigitToolOutput GetResponceFrom(const MmElectronicsToolInput & digiInput);

  float tMinFromIntegration;
  float tminFromIntegrationAboveThreshold;
  float tMinFromFirstPeak;
  vector <float> tStripElectronicsAbThr;
  vector <float> qStripElectronics;
  vector <int> nStripElectronics;

  inline void set_alpha(float val) {alpha = val;};
  inline void set_RC (float val) {RC = val;};
  inline void set_electronicsThreshold(float val) {electronicsThreshold = val;};
 
  float get_alpha() const { return alpha;};
  float get_RC () const { return RC;};
  float get_electronicsThreshold() const { return electronicsThreshold;};
   
  float get_tMinFromIntegration () const { return tMinFromIntegration;};
  float get_tminFromIntegrationAboveThreshold () const { return tminFromIntegrationAboveThreshold;};
  float get_tMinFromFirstPeak () const { return tMinFromFirstPeak;};
 
  vector <float>  get_tStripElectronicsAbThr () const { return tStripElectronicsAbThr;};
  vector <float>  get_qStripElectronics () const { return qStripElectronics;};
  vector <int>  get_nStripElectronics () const { return nStripElectronics;};


};

#endif
