/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                          InDetJetFitterUtils.cxx  -  Description
                             -------------------

    begin : December 2007
    authors: Giacinto Piacquadio (University of Freiburg)
    email : nicola.giacinto.piacquadio@cern.ch
    changes: new!
 
  2007 (c) Atlas Detector Software

  This class implements the some utilities for creating neutral tracks 
  from conversion, KS, Lambdas. Treatment of covariance matrices, jacobians, 
  ... is all correctly included.
     
 ***************************************************************************/

#ifndef TrkJetVxFitter_InDetJetFitterUtils_H
#define TrkJetVxFitter_InDetJetFitterUtils_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Vector/LorentzVector.h"
//#include "TrkParticleBase/LinkToTrackParticleBase.h"
//#include "TrkParticleBase/TrackParticleBaseCollection.h"
//#include "TrkParticleBase/TrackParticleBase.h"
//#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkParameters/TrackParameters.h"
#include "xAODTracking/Vertex.h" 

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"

namespace Trk {
  class VxCandidate;
  class LinearizedTrack;
  class RecVertex;
  class Vertex;
  class LinkToTrackParticleBase;
  class ITrackLink;
  class TrackParticleBase;
  class VxVertexOnJetAxis;
}

namespace InDet {

  static const InterfaceID IID_InDetJetFitterUtils("InDetJetFitterUtils", 1, 0);

  class InDetJetFitterUtils : public AthAlgTool
  {

  public:

    static const InterfaceID& interfaceID() {
      return IID_InDetJetFitterUtils;
    }
 

    StatusCode initialize();
    StatusCode finalize();

    InDetJetFitterUtils(const std::string& t, const std::string& n, const IInterface*  p);

    ~InDetJetFitterUtils();
    
    //std::pair<double,double> compatibility(const Trk::ParametersBase<5,Trk::Charged> & measPerigee,
    //                                     const Trk::RecVertex & vertex) const;
    //std::pair<double,double> compatibility(const Trk::ParametersBase<5,Trk::Neutral> & measPerigee,
    //                                     const Trk::RecVertex & vertex) const;

     std::pair<double,double> compatibility(const Trk::TrackParameters & measPerigee,
                                           const Trk::RecVertex & vertex) const;
    std::pair<double,double> compatibility(const Trk::ParametersBase<5,Trk::Neutral> & measPerigee,
                                           const Trk::RecVertex & vertex) const;
    
    double get3DLifetimeSignOfTrack(const Trk::TrackParameters & track,
                                    const Amg::Vector3D & jetMomentum,
                                    const Trk::RecVertex & primaryVertex) const;

    int getTwoTrackVtxCharge(const Trk::VxCandidate & myVxCandidate) const;
    int getTwoTrackVtxCharge(const xAOD::Vertex & myVxCandidate) const;
    
    double getTwoTrackVtxMass(const Trk::VxCandidate & myVxCandidate,
                              double highestMomMass,
                              double lowestMomMass) const;

    double getTwoTrackVtxMass(const xAOD::Vertex & myVxCandidate,
                              double highestMomMass,
                              double lowestMomMass) const;


    std::pair<double,double> getDistanceAndErrorBetweenTwoVertices(const xAOD::Vertex &,
                                                                   const Trk::RecVertex &) const;
    
    std::pair<double,double> getD0andZ0IP(const Trk::TrackParameters & trackPerigee,
                                          const Trk::Vertex & vertexToExtrapolateTo) const;

    std::pair<double,double> getD0andZ0IPSig(const Trk::TrackParameters & trackPerigee,
					     const Trk::RecVertex & vertex) const;
    
    const Trk::LinkToTrackParticleBase* findNeutralTrackParticleBase(const std::vector<const Trk::LinkToTrackParticleBase*> &,
                                                                     const Trk::VxCandidate &) const;

    const Trk::LinkToTrackParticleBase* findNeutralTrackParticleBase(const std::vector<const Trk::LinkToTrackParticleBase*> &,
                                                                     const xAOD::Vertex &) const;

    bool checkIfTrackIsInVector(const Trk::ITrackLink * trackToCheck,
                                const std::vector<const Trk::ITrackLink*> & vectorOfTracks) const;
    
    bool checkIfTrackIsInVector(const Trk::ITrackLink * trackToCheck,
                                const std::vector<const Trk::LinkToTrackParticleBase*> & vectorOfTracks) const;

    bool checkIfTrackIsInNeutralTrackVector(const Trk::ITrackLink * trackToCheck,
                                            const std::vector<const Trk::LinkToTrackParticleBase*> & vectorOfNeutrals) const;

    bool checkIfTrackIsInV0CandidatesVector(const Trk::ITrackLink * trackToCheck,
                                            const std::vector<const xAOD::Vertex*> & vectorOfVxCandidates) const;

    bool checkIfVxCandidateIsInVector(const xAOD::Vertex * vertexToCheck,
                                      const std::vector<const xAOD::Vertex*> & vectorOfCandidates) const;

    CLHEP::HepLorentzVector fourMomentumAtVertex(const Trk::VxVertexOnJetAxis &) const;
    
    

  private:

    std::pair<AmgMatrix(3,3),AmgSymMatrix(3)>  getPosMomentumAndMomentumCovMatrix(const Trk::LinearizedTrack* linTrack,
										const AmgSymMatrix(3) & vrt_cov,
										const AmgSymMatrix(3) & vrt_weight) const;
                                                                                                           


    ToolHandle<Trk::IVertexLinearizedTrackFactory> m_LinearizedTrackFactory {this,"LinearizedTrackFactory","Trk::FullLinearizedTrackFactory/FullLinearizedTrackFactory",""};
    ToolHandle<Trk::IExtrapolator>  m_extrapolator {this,"Extrapolator","Trk::Extrapolator/InDetExtrapolator",""};   

    bool m_extrapolatorIsAvailable;
    bool m_linearizedTrackFactoryIsAvailable;
    
    //ToolHandle<Trk::JetFitterInitializationHelper> m_initializationHelper;

  };//end class declaration

}//end namespace Rec


#endif

