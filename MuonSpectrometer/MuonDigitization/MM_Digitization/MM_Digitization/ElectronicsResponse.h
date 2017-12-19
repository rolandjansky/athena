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
#include "MM_Digitization/MmElectronicsToolTriggerOutput.h"
#include "MM_Digitization/MmDigitToolOutput.h"
#include "MM_Digitization/StripsResponse.h"
//VMM Mapping
#include "MM_Digitization/MMStripVmmMappingTool.h"

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
  float timeWindowLowerOffset;
  float timeWindowUpperOffset;
  float electronicsThreshold;
  float stripdeadtime;
  float ARTdeadtime;
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
  void bnlPeakResponceFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip);
  void bnlThresholdResponceFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip);
  MmDigitToolOutput GetPeakResponceFrom(const MmElectronicsToolInput & digiInput);
  MmDigitToolOutput GetThresholdResponceFrom(const MmElectronicsToolInput & digiInput);

  MmElectronicsToolTriggerOutput GetTheFastestSignalInVMM(const MmDigitToolOutput & ElectronicThresholdOutput, const int chMax, const int stationEta);
  int GetIdTheFastestSignalInVMM(float time, int VMM_id, std::vector<int> trigger_VMM_id, const std::vector<float> ElectronicsThreshold_stripTime, float timeWindowLower, float timeWindowUpper);
  void GetVMMId(const std::vector< int > & ElectronicsThreshold_stripPos, const int chMax, const int stationEta, std::vector< int > & trigger_VMM_id, std::vector< int > & trigger_MMFE_VMM_id);
  MmDigitToolOutput ApplyDeadTimeStrip(const MmDigitToolOutput & ElectronicsTriggerOutput);
  MmElectronicsToolTriggerOutput ApplyDeadTimeART(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput);
  MmElectronicsToolTriggerOutput ApplyARTTiming(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput, float jitter, float offset);
  bool DeadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime);

  float tMinFromIntegration;
  float tminFromIntegrationAboveThreshold;
  float tMinFromFirstPeak;
  vector <float> tStripElectronicsAbThr;
  vector <float> qStripElectronics;
  vector <int> nStripElectronics;

  inline void set_alpha(float val) {alpha = val; intFn->SetParameter( 0, alpha); };
  inline void set_RC (float val) {RC = val;intFn->SetParameter( 1, RC); };
  inline void set_timeWindowLowerOffset(float val) { timeWindowLowerOffset = val;};
  inline void set_timeWindowUpperOffset(float val) { timeWindowUpperOffset = val;};
  inline void set_electronicsThreshold(float val) {electronicsThreshold = val;};
  inline void set_stripdeadtime(float val) {stripdeadtime = val;};
  inline void set_ARTdeadtime(float val) {ARTdeadtime = val;};
 
  float get_alpha() const { return alpha;};
  float get_RC () const { return RC;};
  float get_timeWindowLowerOffset() const { return timeWindowLowerOffset ;};
  float get_timeWindowUpperOffset() const { return timeWindowUpperOffset ;};
  float get_electronicsThreshold() const { return electronicsThreshold;};
  float get_stripdeadtime() const { return stripdeadtime;};
  float get_ARTdeadtime() const { return ARTdeadtime;};
   
  float get_tMinFromIntegration () const { return tMinFromIntegration;};
  float get_tminFromIntegrationAboveThreshold () const { return tminFromIntegrationAboveThreshold;};
  float get_tMinFromFirstPeak () const { return tMinFromFirstPeak;};
 
  vector <float>  get_tStripElectronicsAbThr () const { return tStripElectronicsAbThr;};
  vector <float>  get_qStripElectronics () const { return qStripElectronics;};
  vector <int>  get_nStripElectronics () const { return nStripElectronics;};


};

#endif
