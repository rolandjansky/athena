/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROMPT_PROMPTUTILS_H
#define PROMPT_PROMPTUTILS_H

/**********************************************************************************
 * @Package: LeptonTaggers
 * @Class  : PromptUtils
 * @Author : Rustem Ospanov
 * @Author : Rhys Roberts
 *
 * @Brief  :
 * 
 *  Helper functions
 *  
 **********************************************************************************/

// Athena
#include "AthContainers/AuxElement.h"
#include "GaudiKernel/MsgStream.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

// ROOT
#include "TStopwatch.h"
#include "TH1.h"

// C/C++
#include <string>

namespace Prompt
{
  //=============================================================================
  double GetVertexFitProb(const xAOD::Vertex *vtx);

  std::string VtxAsStr(const xAOD::Vertex        *vtx, bool print_tracks=false);
  std::string TrkAsStr(const xAOD::TrackParticle *trk);
  
  std::string TruthAsStr(const xAOD::IParticle &particle);

  double GetDistance(const xAOD::Vertex  *vtx1, const xAOD::Vertex  *vtx2);
  double GetDistance(const Amg::Vector3D &vtx1, const Amg::Vector3D &vtx2);

  double GetNormDist(const Amg::Vector3D &PrimVtx, const Amg::Vector3D &SecVtx, const std::vector<float> &ErrorMatrix, MsgStream &msg);

  void FillTH1(TH1 *h, double val, double weight = 1.0);

  std::string PrintPromptVertexAsStr(const xAOD::Vertex *vtx, MsgStream &msg);

  //=============================================================================
  struct SortByIDTrackPt
  {
    bool operator()(const xAOD::TrackParticle *lhs, const xAOD::TrackParticle *rhs) { return lhs->pt() > rhs->pt(); }
  };

  //=============================================================================
  template<class T1, class T2> bool GetVar(T1 &obj, T2 &value, const std::string &var_name)
  {
    //
    // get the int aux-variable
    //
    typename SG::AuxElement::Accessor<T2> acc(var_name);
    
    if(!acc.isAvailable(*obj)) {
      return false;
    }
    
    value = acc(*obj);
    return true;
  }

  //=============================================================================
  template<class T1, class T2> bool GetAuxVar(const T1 &obj, T2 &value, const std::string &var_name)
  {
    //
    // get the int aux-variable
    //
    typename SG::AuxElement::Accessor<T2> acc(var_name);
    
    if(!acc.isAvailable(obj)) {
      return false;
    }
    
    value = acc(obj);
    return true;
  }

  //=============================================================================
  std::string PrintResetStopWatch(TStopwatch &watch);

  class TimerScopeHelper
  {
  public:
    
    explicit TimerScopeHelper(TStopwatch &timer) 
      :fTimer(timer) { fTimer.Start(false); }
    ~TimerScopeHelper() { fTimer.Stop(); }
    
  private:
      
    TStopwatch &fTimer;
  };
}

#endif //PROMPT_PROMPTUTILS_H
