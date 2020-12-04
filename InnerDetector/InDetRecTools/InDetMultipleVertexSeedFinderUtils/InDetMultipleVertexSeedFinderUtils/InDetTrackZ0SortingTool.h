/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetMultipleVertexSeedFinderUtils_InDetTrackZ0SortingTool_H
#define InDetMultipleVertexSeedFinderUtils_InDetTrackZ0SortingTool_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/TrackParticle.h"

/**
 * A tool to sort tracks according to their Z0
 * impact parameter with respoect to the coordinate
 * origin. The tracks are extrapolated to a given point 
 * before the the sorting is done. If no point is provided, 
 * the sorting is performed on the basis of perigee from 
 * Track or TrackParticle.
 *
 * Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * Mai 2007
 */

namespace Trk
{
 class Track;
 class Vertex;
 class TrackParticleBase;
 class IExtrapolator;
}


namespace InDet
{
 static const InterfaceID IID_InDetTrackZ0SortingTool("InDetTrackZ0SortingTool", 1, 1);
 
 class InDetTrackZ0SortingTool : public AthAlgTool
 {
 
  public:
   
   virtual StatusCode initialize() override;
    
   InDetTrackZ0SortingTool(const std::string& t, const std::string& n, const IInterface*  p);
    
   static const InterfaceID& interfaceID() {return IID_InDetTrackZ0SortingTool;}
 
   ~InDetTrackZ0SortingTool();
   
/**
 * Sorting method itself . Returns an ordered (ascending in Z0) vector of 
 * indexes of  tracks in initial container. Index range is [0.. tracks.size())
 */ 
   std::vector<int> sortedIndex(const std::vector<const Trk::Track*>& tracks, const Trk::Vertex * reference = 0 )const;
   
   
   std::vector<int> sortedIndex(const std::vector<const Trk::TrackParticleBase*>& tracks ,const Trk::Vertex * reference = 0 )const;
   
   std::vector<int> sortedIndex(const std::vector<const xAOD::TrackParticle*>& perigeeList, const xAOD::Vertex * beamVtx = 0) const; 
   
  private:
   
   ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool 

 };//end of class definitions
 
}//end of namespace definitions
#endif //TrkMultipleVertexSeedFinders_InDetTrackZ0SortingTool_H
