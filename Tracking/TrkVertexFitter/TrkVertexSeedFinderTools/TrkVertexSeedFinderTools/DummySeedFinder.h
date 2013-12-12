/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_DUMMYSEEDFINDER_H
#define TRKVERTEXSEEDFINDERTOOLS_DUMMYSEEDFINDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

namespace Trk
{
  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a dummy seed finder

  class DummySeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    DummySeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~DummySeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Vertex findSeed(const std::vector<const Trk::Track*> & vectorTrk,const RecVertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Vertex findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const RecVertex * constraint=0);
  };
}
#endif
