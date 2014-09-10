/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETPERIGEEFITTER_H__
#define __TRIGINDETPERIGEEFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"

class TrigTimer;
namespace MagField {	
	class IMagFieldSvc;
}

class TrigInDetPerigeeFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetPerigeeFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetPerigeeFitter();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  void fit(TrigInDetTrackCollection*);
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis&) {return nullptr;}
private:

  void m_clear();

  double m_DChi2;
  double m_consb;
  bool m_straightLineMode; 
  bool m_doMultScatt;
  bool m_doBremmCorr;
  std::string m_bfieldToolName;

  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;

#define PERIGEEFIT_NTIMERS 8
  TrigTimer* m_timer[PERIGEEFIT_NTIMERS];
  bool m_timers;

};







#endif
