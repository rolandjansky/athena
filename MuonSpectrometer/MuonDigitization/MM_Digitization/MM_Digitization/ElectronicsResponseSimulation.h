/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_ELECTRONICSRESPONSESIMULATION_H
#define MM_DIGITIZATION_ELECTRONICSRESPONSESIMULATION_H
/** @class ElectronicsResponseSimulation

// ------------
// Authors:
//   Iakovidis George        <george.iakovidis@cern.ch>
//   Karakostas Konstantinos <konstantinos.karakostas@cern.ch>
//   Leontsinis Stefanos     <stefanos.leontsinis@cern.ch>
//   Nektarios Chr. Benekos  <nbenekos@cern.ch>
//   Jessica Metcalfe        <jessica.metcalfe@gmail.com>
//
// Major Contributions From: Verena Martinez
//                           Tomoyuki Saito
//
// Major Restructuring for r21+ From: Lawrence Lee <lawrence.lee.jr@cern.ch>
//
//////////////////////////////////////////////////////////////////////////////

*/


#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Service.h"
#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/StatusCode.h"


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
// #include "MM_Digitization/StripsResponseSimulation.h"

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

class ElectronicsResponseSimulation {

private:
  /** power of responce function */
  float m_peakTime;
  float m_alpha;
  float m_timeWindowLowerOffset;
  float m_timeWindowUpperOffset;
  float m_electronicsThreshold;
  float m_stripdeadtime;
  float m_ARTdeadtime;
  float m_stripResponse_qThreshold;
  float m_stripResponse_driftGapWidth;
  float m_stripResponse_driftVelocity;

  int  m_decoupleShaperFunctionParamaters;

  TF1 *h_intFn;
  // StripsResponseSimulation* m_stripObject ;
  ElectronicsResponseSimulation & operator=(const ElectronicsResponseSimulation &right);
  ElectronicsResponseSimulation(const ElectronicsResponseSimulation&);
public :

  ElectronicsResponseSimulation();
  virtual ~ElectronicsResponseSimulation();

  void initialize();

  void clearValues ();
  void vmmPeakResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip);
  void VMMThresholdResponseFunction(const vector <int> & numberofStrip, const vector<vector <float>> & qStrip, const vector<vector <float>> & tStrip);
  MmDigitToolOutput GetPeakResponseFrom(const MmElectronicsToolInput & digiInput);
  MmDigitToolOutput GetThresholdResponseFrom(const MmElectronicsToolInput & digiInput);

  MmElectronicsToolTriggerOutput GetTheFastestSignalInVMM(const MmDigitToolOutput & ElectronicThresholdOutput, const int chMax, const int stationEta);
  int GetIdTheFastestSignalInVMM(float time, int VMM_id, std::vector<int> trigger_VMM_id, const std::vector<float> ElectronicsThreshold_stripTime, float timeWindowLower, float timeWindowUpper);
  void GetVMMId(const std::vector< int > & ElectronicsThreshold_stripPos, const int chMax, const int stationEta, std::vector< int > & trigger_VMM_id, std::vector< int > & trigger_MMFE_VMM_id);
  MmDigitToolOutput ApplyDeadTimeStrip(const MmDigitToolOutput & ElectronicsTriggerOutput);
  MmElectronicsToolTriggerOutput ApplyDeadTimeART(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput);
  MmElectronicsToolTriggerOutput ApplyARTTiming(const MmElectronicsToolTriggerOutput & ElectronicsTriggerOutput, float jitter, float offset);
  bool DeadChannel(int id, float time, std::vector<int> & v_id, const std::vector<float> & v_time, float deadtime);

  vector <float> tStripElectronicsAbThr;
  vector <float> qStripElectronics;
  vector <int> nStripElectronics;

  inline void set_peakTime(float val) { m_peakTime = val;  };
  inline void set_timeWindowLowerOffset(float val) { m_timeWindowLowerOffset = val;};
  inline void set_timeWindowUpperOffset(float val) { m_timeWindowUpperOffset = val;};
  inline void set_electronicsThreshold(float val) { m_electronicsThreshold = val;};
  inline void set_stripdeadtime(float val) { m_stripdeadtime = val;};
  inline void set_ARTdeadtime(float val) { m_ARTdeadtime = val;};

  inline void set_StripResponse_qThreshold    (float val) { m_stripResponse_qThreshold    = val; };
  inline void set_StripResponse_driftVelocity (float val) { m_stripResponse_driftVelocity = val; };
  inline void set_StripResponse_driftGapWidth (float val) { m_stripResponse_driftGapWidth = val; };


  float get_peakTime() const { return m_peakTime;};
  float get_alpha() const { return m_alpha;};
  float get_timeWindowLowerOffset() const { return m_timeWindowLowerOffset ;};
  float get_timeWindowUpperOffset() const { return m_timeWindowUpperOffset ;};
  float get_electronicsThreshold() const { return m_electronicsThreshold;};
  float get_stripdeadtime() const { return m_stripdeadtime;};
  float get_ARTdeadtime() const { return m_ARTdeadtime;};


  vector <float>  get_tStripElectronicsAbThr () const { return tStripElectronicsAbThr;};
  vector <float>  get_qStripElectronics () const { return qStripElectronics;};
  vector <int>    get_nStripElectronics () const { return nStripElectronics;};


};

#endif
