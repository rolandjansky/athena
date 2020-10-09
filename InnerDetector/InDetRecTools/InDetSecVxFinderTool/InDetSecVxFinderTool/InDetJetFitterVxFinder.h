/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : March 2007
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  This class implements the finder functionality for using 
  the JetFitter algorithm. It uses as input a ParticleJet and 
  then gives the VxJetCandidate out after having performed the 
  fit.
     
 ***************************************************************************/

#ifndef TrkJetVxFitter_InDetJetFitterVxFinder_H
#define TrkJetVxFitter_InDetJetFitterVxFinder_H

#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParticleBase/TrackParticleBase.h"

namespace Trk {
  class VxSecVertexInfo;
  class VxJetCandidate;
  class RecVertex;
  class JetFitterInitializationHelper;
  class JetFitterHelper;
  class JetFitterRoutines;
  class ITrackSelectorTool;
}

namespace xAOD
{
  class IParticle;  
}

namespace InDet {


  class InDetJetFitterVxFinder : public AthAlgTool, public ISecVertexInJetFinder
  {

  public:
 

    StatusCode initialize();
    StatusCode finalize();

    InDetJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p);

    ~InDetJetFitterVxFinder();

    Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & /*primaryVertex*/,
					const TLorentzVector & /*jetMomentum*/,
					const std::vector<const xAOD::IParticle*> & /*inputTracks*/ ) const
    {
      /* not implemented */
      return 0;
    }
    
    const Trk::VxSecVertexInfo* findSecVertex(const Trk::RecVertex & primaryVertex,
                                              const TLorentzVector & jetMomentum,
					      const std::vector<const Trk::TrackParticleBase*> & inputTracks) const;

    const Trk::VxSecVertexInfo* findSecVertex(const Trk::RecVertex & primaryVertex,
                                              const TLorentzVector & jetMomentum,
                                              const std::vector<const Trk::TrackParticleBase*> & firstInputTracks,
                                              const std::vector<const Trk::TrackParticleBase*> & secondInputTracks,
                                              const Amg::Vector3D & vtxSeedDirection) const;

  private:
    
    void doTheFit(Trk::VxJetCandidate* myJetCandidate,
                  bool performClustering=true) const;

    ToolHandle< Trk::JetFitterInitializationHelper > m_initializationHelper {this,"JetFitterInitializationHelper","Trk::JetFitterInitializationHelper",""};
    ToolHandle< Trk::JetFitterHelper > m_helper {this,"JetFitterHelper","Trk::JetFitterHelper",""};
    ToolHandle< Trk::JetFitterRoutines > m_routines {this,"JetFitterRoutines","Trk::JetFitterRoutines",""};
    ToolHandle< Trk::ITrackSelectorTool > m_trkFilter {this,"TrackSelector","InDet::InDetDetailedTrackSelectorTool",""}; 


    Gaudi::Property< int > m_maxNumDeleteIterations {this,"MaxNumDeleteIterations",30,""};
    Gaudi::Property< float > m_vertexProbCut {this,"VertexProbCut",0.01,""};
    Gaudi::Property< int >  m_maxClusteringIterations {this,"MaxClusteringIterations",30,""};
    Gaudi::Property< float > m_vertexClusteringProbabilityCut {this,"VertexClusteringProbabilityCut",0.01,""};
    Gaudi::Property< bool > m_useFastClustering {this,"UseFastClustering",false,""};

    Gaudi::Property< int > m_maxTracksToFitAtOnce {this,"MaxTracksToFitAtOnce",15,""};

  };//end class declaration

}//end namespace Rec


#endif

