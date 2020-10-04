/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/***************************************************************************
                          InDetImprovedJetFitterVxFinder.cxx  -  Description
                             -------------------

    begin : January 2008!
    authors: Giacinto Piacquadio (University of Freiburg),
             Christian Weiser (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch,
            christian.weiser@cern.ch
    changes: second version in January 2008!
 
  2008 (c) Atlas Detector Software

  This class implements the finder functionality for using 
  the JetFitter algorithm. It uses as input a ParticleJet and 
  then gives the VxJetCandidate out after having performed the 
  fit.

  SECOND VERSION: many improvements, detailed reconstruction of conversions,
  KS and Lambdas. KS are even used in the multi vertex fit in JetFitter in a 
  region of space of enough purity to allow it.
     
 ***************************************************************************/

#ifndef TrkJetVxFitter_InDetImprovedJetFitterVxFinder_H
#define TrkJetVxFitter_InDetImprovedJetFitterVxFinder_H

#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkTrackLink/ITrackLink.h"

namespace Trk {
  class TrackParticleBase;
  class VxSecVertexInfo;
  class VxJetCandidate;
  class RecVertex;
}

// Hackathon: add any new package here
namespace InDet {
  class JetFitterTrackSelectorTool;
  class JetFitterTwoTrackVtxFinderTool;
  class JetFitterV0FinderTool;
  class JetFitterMultiStageFit;
}

namespace xAOD
{
  class IParticle;  
}

namespace InDet {

  class InDetImprovedJetFitterVxFinder : public AthAlgTool, public ISecVertexInJetFinder
  {
    
  public:
    

    StatusCode initialize();
    StatusCode finalize();

    InDetImprovedJetFitterVxFinder(const std::string& t, const std::string& n, const IInterface*  p);

    ~InDetImprovedJetFitterVxFinder();


    virtual Trk::VxSecVertexInfo* findSecVertex(const xAOD::Vertex & primaryVertex,
						const TLorentzVector & jetMomentum,
						const std::vector<const xAOD::IParticle*> & inputTracks) const;

  private:

    void sortTrackCollectionByPt( std::vector< const Trk::ITrackLink* >& ) const;

    Trk::VxJetCandidate* findSecVertex(const Trk::RecVertex & primaryVertex,
                                       const TLorentzVector & jetMomentum,
                                       const std::vector<const Trk::ITrackLink*> & firstInputTracks,
                                       const std::vector<const Trk::ITrackLink*> & secondInputTracks,
                                       const Amg::Vector3D & vtxSeedDirection) const;
        

    void doTheFit(Trk::VxJetCandidate* myJetCandidate,
                  bool performClustering=true) const;

  private:
    ToolHandle< JetFitterTrackSelectorTool > m_theTrackSelector {this,"JetFitterTrackSelectorTool","JetFitterTrackSelectorTool/JetFitterTrackSelectorTool",""}; //Hackathon addition    
    ToolHandle< JetFitterTwoTrackVtxFinderTool > m_theTwoTrackVtxFinderTool {this,"JetFitterTwoTrackVtxFinderTool","JetFitterV0FinderTool/JetFitterTwoTrackVtxFinderTool",""}; //Hackathon addition
    ToolHandle< JetFitterV0FinderTool > m_theV0FinderTool {this,"JetFitterV0FinderTool","JetFitterV0FinderTool/JetFitterV0FinderTool",""}; //Hackathon addition
    ToolHandle< JetFitterMultiStageFit > m_multiStageFitter {this,"JetFitterMultiStageFit","JetFitterMultiStageFit/JetFitterMultiStageFit",""}; //Hackathon addition
    
  };//end class declaration

}//end namespace Rec


#endif

