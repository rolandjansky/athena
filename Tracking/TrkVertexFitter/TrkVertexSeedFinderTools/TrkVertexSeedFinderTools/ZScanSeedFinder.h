/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H
#define TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"

namespace Trk
{

  class Vertex;
  class Track;
  class IMode1dFinder;
  

  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a seed finder for the primary vertex finding 
  // which is based on the use of a clustering algorithm over the z position at 
  // point of closest approach to the beam (2d).
  // 


  class ZScanSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    ZScanSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~ZScanSeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Vertex findSeed(const std::vector<const Trk::Track*> & vectorTrk,const RecVertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Vertex findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const RecVertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Vertex> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const RecVertex * constraint=0);

  private:
    ToolHandle< IMode1dFinder > m_mode1dfinder;

   /**
    * Estimate Z and D0 given a certain track parameters and beam spot center position
    * ONLY TEMPORARY 15-08-2009: common tool needed to collect this method
    */
    
  std::pair<double,double> estimateZandD0(const Trk::Perigee& myPerigee, const Trk::Vertex& myTransvVertex);

    bool m_disableAllWeights;
    float m_constraintcutoff;
    float m_constrainttemp;
    bool m_usept;
    double m_exppt;
    double m_zBfieldApprox;
  };
}
#endif
