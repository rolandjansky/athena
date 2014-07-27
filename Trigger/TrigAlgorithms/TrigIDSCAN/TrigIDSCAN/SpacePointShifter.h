/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: SpacePointShifter.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// Description: reads in the ATLAS SpacePoints
// 
// date: 02/12/2002
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_SPACEPOINTSHIFTER_H )
#define IDSCAN_SPACEPOINTSHIFTER_H

#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "shiftRoI.h"


class TrigSiSpacePoint;
class ISpacePointProvider;


namespace TrigIdScan {

class RoI_Shape;

typedef   std::vector< TrigSiSpacePoint* >   spacePointVec;

class SpacePointShifter
{
 public:
 SpacePointShifter( MsgStream&           log,
                    int                  outputLevel,
                    ToolHandle<ISpacePointProvider> spacePointProvider,
                    RoI_Shape*           roiShape,
                    double etaRoI,       double phiRoI, 
		    double x=0, double y=0, bool translateOnly=false);

  ~SpacePointShifter()   {  
    // delete the local copy of the TrigSiSpacePoints vector.
    // This should only be "non-empty" if they have been
    // copied because we are running in TB of Cosmic mode and 
    // need to a local copy rather than those in StoreGate.

    std::vector<TrigSiSpacePoint*>::const_iterator  spItr( m_spVec.begin() );
    std::vector<TrigSiSpacePoint*>::const_iterator  spEnd( m_spVec.end() );

    while (  spItr!=spEnd  )  delete *(spItr++);

    // clear map of shifted and unshifted points
    m_spMap.clear();
  }

  long HitCount()        const;

  bool isInPhiBoundary() const;

  std::vector<TrigSiSpacePoint*> spVec() const;

  double get_xshift() const { return m_xshift; }
  double get_yshift() const { return m_yshift; }

  void translate(std::vector<TrigSiSpacePoint*>& sp, double x, double y);
  
  TrigSiSpacePoint* mapPoint(TrigSiSpacePoint* sp) const;

 private:  // member functions

  double shift(std::vector<TrigSiSpacePoint*>& sp);

  double TrackShift(std::vector<point>& hin) const;

 private:  // data members

  mutable MsgStream m_athenaLog;
  
  bool              m_boundary;
  
  spacePointVec     m_commonSPs;

  double            m_xshift;
  double            m_yshift;

  std::vector<TrigSiSpacePoint*> m_spVec;

  // map of the shifted to unshifted points
  std::map<TrigSiSpacePoint*, TrigSiSpacePoint*> m_spMap;


};
 



inline long SpacePointShifter::HitCount() const 
{  
  return m_commonSPs.size(); 
}

inline bool SpacePointShifter::isInPhiBoundary() const 
{
  return m_boundary; 
}

inline std::vector< TrigSiSpacePoint* > SpacePointShifter::spVec() const
{
  return m_commonSPs;
}

} //namespace


#endif






