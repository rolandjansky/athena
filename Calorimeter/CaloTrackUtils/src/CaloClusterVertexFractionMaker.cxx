/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// in contrary to CaloClusterVertexFractionMakerAthAlg this is a CaloClusterCollectionProcessor and not an Athena Algorithm
#include "CaloTrackUtils/CaloClusterVertexFractionMaker.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParameters/TrackParameters.h"

#include "VxVertex/VxContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

//#include "CaloEvent/CaloClusterMoment.h"
//#include "CaloEvent/CaloCluster.h"
//#include "CaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

#include "Particle/TrackParticle.h"

#include <cmath>

// Amg
// #include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"


CaloClusterVertexFractionMaker::CaloClusterVertexFractionMaker(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type, name, parent),
  m_CALO_INNER_R(1450.), // millimeter
  m_CALO_INNER_Z(3641.), // millimeter
  m_dRMatchMax(0.1),
  m_dR2MatchMax(0.), // will be overwritten in initialize
  m_maxClusterEta(2.5),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_vxContainerName("VxPrimaryCandidate"),
  m_cylinderSurface_atCaloEntrance(0),
  m_discSurface_atCaloEntrance_positiveZ(0),
  m_discSurface_atCaloEntrance_negativeZ(0),
  m_numTracksPerVertex(0),
  m_trkParticlePt_atOrigin(0),
  m_trkParticleEta_atCaloEntrance(0),
  m_trkParticlePhi_atCaloEntrance(0)

{
  declareProperty ( "Extrapolator", m_extrapolator );
  declareProperty ( "VxContainerName", m_vxContainerName );
  declareProperty ( "dRMatchMax", m_dRMatchMax );
  declareProperty ( "maxClusterEta", m_maxClusterEta );
}

CaloClusterVertexFractionMaker::~CaloClusterVertexFractionMaker()
{
  // all delete statements are in finalize (otherwise genconf crashes .. no idea why)
}

StatusCode CaloClusterVertexFractionMaker::initialize()
{
  ATH_MSG_INFO( "Initializing " << name()  );
  ATH_MSG_INFO( "Only considering clusters with |eta| < " << m_maxClusterEta << "\tdRMatchMax Cluster-Track = " << m_dRMatchMax  );

  m_dR2MatchMax = m_dRMatchMax*m_dRMatchMax;

  /* Get the extrapolator */
  ATH_CHECK( m_extrapolator.retrieve() );
  ATH_MSG_INFO( "Retrieved tool " << m_extrapolator  );

  // a surface at the entrance to the calorimeter
//   HepGeom::TranslateZ3D* translateAlongPositiveZ = new HepGeom::TranslateZ3D(m_CALO_INNER_Z);
//   HepGeom::TranslateZ3D* translateAlongNegativeZ = new HepGeom::TranslateZ3D(-m_CALO_INNER_Z);
  Amg::Transform3D* translateAlongPositiveZ = new Amg::Transform3D(Amg::Vector3D(0.,0.,m_CALO_INNER_Z));
  Amg::Transform3D* translateAlongNegativeZ = new Amg::Transform3D(Amg::Vector3D(0.,0.,m_CALO_INNER_Z));

  m_cylinderSurface_atCaloEntrance = new Trk::CylinderSurface(m_CALO_INNER_R, 8000.);
  m_discSurface_atCaloEntrance_positiveZ = new Trk::DiscSurface(translateAlongPositiveZ, 0., 10000.);
  m_discSurface_atCaloEntrance_negativeZ = new Trk::DiscSurface(translateAlongNegativeZ, 0., 10000.);

  m_numTracksPerVertex = new std::vector<unsigned int>;
  m_trkParticlePt_atOrigin = new std::vector<float>;
  m_trkParticleEta_atCaloEntrance = new std::vector<float>;
  m_trkParticlePhi_atCaloEntrance = new std::vector<float>;

  return StatusCode::SUCCESS;
}

StatusCode
CaloClusterVertexFractionMaker::execute(const EventContext& /*ctx*/,
                                        xAOD::CaloClusterContainer* caloClusterContainer) const
{
  const VxContainer* primcontainer(0);
  if ( evtStore()->contains<VxContainer> ( m_vxContainerName ) )
  {
    if ( evtStore()->retrieve ( primcontainer, m_vxContainerName ).isFailure() )
    {
      ATH_MSG_WARNING( "Could not retrieve collection " << m_vxContainerName << " in StoreGate, but contains<> says it is there."  );
      return false;
    }
  } else {
    ATH_MSG_WARNING( "No collection " << m_vxContainerName << " in StoreGate."  );
    return false;
  }

  // loop over vertices, extrapolate tracks to calo, remember num tracks per vertex (for cluster vertex fraction calculation later)
  m_numTracksPerVertex->resize(primcontainer->size()-1, 0);
  for (unsigned int v = 0 ; v < primcontainer->size()-1; v++)
  {
    const std::vector<Trk::VxTrackAtVertex*>* vxTrackAtVertex = primcontainer->at(v)->vxTrackAtVertex();
    for (std::vector<Trk::VxTrackAtVertex*>::const_iterator vxTrkItr = vxTrackAtVertex->begin(); vxTrkItr != vxTrackAtVertex->end(); vxTrkItr++)
    {
      /** usual complicated procedure to get from the vertex to the track(particle) */
      Trk::ITrackLink*              trklink             = (*vxTrkItr)->trackOrParticleLink();
      Trk::LinkToTrackParticleBase* linkToTrackParticle = dynamic_cast<Trk::LinkToTrackParticleBase*>(trklink);
      if (linkToTrackParticle != NULL && linkToTrackParticle->isValid()) {
        const Rec::TrackParticle* theTrackParticle = dynamic_cast<const Rec::TrackParticle*>(linkToTrackParticle->cachedElement());
        if (theTrackParticle != NULL)
        {
          const Trk::TrackParameters* trackParameters_atCaloEntrance(0);
          m_numTracksPerVertex->at(v)++;
          m_trkParticlePt_atOrigin->push_back (theTrackParticle->pt()/1.e3);

          const Trk::TrackParameters* lastTrackParametersInID(0);
          const std::vector< const Trk::TrackParameters * > trackParametersVector = theTrackParticle->trackParameters();
          if (trackParametersVector.size() > 1)  lastTrackParametersInID = trackParametersVector.at(trackParametersVector.size()-2);
          else lastTrackParametersInID = trackParametersVector.at(0); // the perigee

          // very simplified way for now to decide to extrapolate a track to barrel or endcap
          if (theTrackParticle->eta() > 1.35) // track most likely in endcap A, extrapolate track to a disc at ID exist in positive z direction
          {
            trackParameters_atCaloEntrance = dynamic_cast<const Trk::AtaDisc*>(m_extrapolator->extrapolate(*lastTrackParametersInID, *m_discSurface_atCaloEntrance_positiveZ, Trk::alongMomentum, true, Trk::pion));
//             std::cout << "CaloClusterVertexFractionMaker " << theTrackParticle->eta() << " extrapolate to endcap A " << trackParameters_atCaloEntrance << std::endl;
          } else if (theTrackParticle->eta() < -1.35) // track most likely in endcap C, extrapolate track to a disc at ID exist in negative z direction
          {
            trackParameters_atCaloEntrance = dynamic_cast<const Trk::AtaDisc*>(m_extrapolator->extrapolate(*lastTrackParametersInID, *m_discSurface_atCaloEntrance_negativeZ, Trk::alongMomentum, true, Trk::pion));
//             std::cout << "CaloClusterVertexFractionMaker " << theTrackParticle->eta() << " extrapolate to endcap C " << trackParameters_atCaloEntrance << std::endl;
          } else // track is in barrel, extrapolate to cylinder at ID exit
          {
            trackParameters_atCaloEntrance = dynamic_cast<const Trk::AtaCylinder*>(m_extrapolator->extrapolate(*lastTrackParametersInID, *m_cylinderSurface_atCaloEntrance, Trk::alongMomentum, true, Trk::pion));
//             std::cout << "CaloClusterVertexFractionMaker " << theTrackParticle->eta() << " extrapolate to barrel " << trackParameters_atCaloEntrance << std::endl;
          }
          if (trackParameters_atCaloEntrance!=0) {
            m_trkParticleEta_atCaloEntrance->push_back(trackParameters_atCaloEntrance->position().eta());
            m_trkParticlePhi_atCaloEntrance->push_back(trackParameters_atCaloEntrance->position().phi());
            ATH_MSG_DEBUG( "At calo entrance R(1150mm) " << *trackParameters_atCaloEntrance  );
            ATH_MSG_DEBUG( "TrkParticle eta/phi/pt[GeV] at calo " << trackParameters_atCaloEntrance->position().eta() << "\t"
                           << trackParameters_atCaloEntrance->position().phi() << "\t"
                           << trackParameters_atCaloEntrance->position().perp()/1.e3  );
            delete trackParameters_atCaloEntrance;
          } else {
            m_trkParticleEta_atCaloEntrance->push_back(999.);
            m_trkParticlePhi_atCaloEntrance->push_back(999.);
          }
        }
      }
    }
  }

  double fabs_Dphi(9999.);
  double fabs_Deta(9999.);
  double dR2(9999.);
  std::vector<float>* sumPtOfMatchedTracksPerVertex = new std::vector<float>(m_numTracksPerVertex->size(), 0.);

  for (xAOD::CaloClusterContainer::iterator clItr = caloClusterContainer->begin(); clItr != caloClusterContainer->end(); clItr++)
  {
     xAOD::CaloCluster* theCluster = (*clItr);
//     double cl_theta = 2.*atan(exp(-theCluster->eta()));
//     double cl_et = theCluster->e()/1.e3*sin(cl_theta);
    if (std::fabs(theCluster->eta()) < 2.5)
    {
      //  Calculation of Cluster Vertex Fraction
      std::vector<unsigned int>::iterator numTrksPerVertexItr  = m_numTracksPerVertex->begin();
      std::vector<unsigned int>::iterator numTrksPerVertexItrE = m_numTracksPerVertex->end();
      unsigned int vertexCounter(0);
      unsigned int totalTrackCounter(0);
      for ( ; numTrksPerVertexItr != numTrksPerVertexItrE ; numTrksPerVertexItr++, vertexCounter++)
      {
        sumPtOfMatchedTracksPerVertex->at(vertexCounter) = 0.;
        for (unsigned int track = 0; track < (*numTrksPerVertexItr); track++, totalTrackCounter++)
        {
          if (m_trkParticleEta_atCaloEntrance->at(totalTrackCounter) < 900.) // no need to check phi as well (not extrapolated eta is set to 999.)
          {
            fabs_Dphi = calculateDPhi(m_trkParticlePhi_atCaloEntrance->at(totalTrackCounter), theCluster->phi());
            fabs_Deta = fabs(m_trkParticleEta_atCaloEntrance->at(totalTrackCounter) - theCluster->eta());
            dR2 = fabs_Deta * fabs_Deta + fabs_Dphi * fabs_Dphi;
            if (dR2 < m_dR2MatchMax) {
              sumPtOfMatchedTracksPerVertex->at(vertexCounter) += m_trkParticlePt_atOrigin->at(totalTrackCounter);
            }
          }
        }
      }
      double totalSumPtOfMatchedTracksWhichWereAlsoUsedInAVertex(0.);
      for (unsigned int mTpV = 0; mTpV < sumPtOfMatchedTracksPerVertex->size(); mTpV++) {
        totalSumPtOfMatchedTracksWhichWereAlsoUsedInAVertex += sumPtOfMatchedTracksPerVertex->at(mTpV);
        //std::cout << "CaloClusterVertexFractionMakerAOD: " << mTpV << "\t" << sumPtOfMatchedTracksPerVertex->at(mTpV) << std::endl;
      }

      double cvf(-1.);
      double ncvf(-1.);
      if (totalSumPtOfMatchedTracksWhichWereAlsoUsedInAVertex > 0.)
      {
        double sumPtInPrimary = sumPtOfMatchedTracksPerVertex->at(0);
        cvf = sumPtOfMatchedTracksPerVertex->at(0)/totalSumPtOfMatchedTracksWhichWereAlsoUsedInAVertex;
        std::sort(sumPtOfMatchedTracksPerVertex->begin(), sumPtOfMatchedTracksPerVertex->end());
        double highestSumPt = sumPtOfMatchedTracksPerVertex->at(sumPtOfMatchedTracksPerVertex->size()-1);
        if (highestSumPt > 0) ncvf = sumPtInPrimary/highestSumPt;
      }
      //std::cout << "CaloClusterVertexFractionMakerAOD: cvf " << cvf << "\tncvf " << ncvf << std::endl;
      theCluster->insertMoment(xAOD::CaloCluster::VERTEX_FRACTION, cvf); // false at the end writes the moment into the moment store of the cluster directly (the moment is then written to AOD as well)
      theCluster->insertMoment(xAOD::CaloCluster::NVERTEX_FRACTION, ncvf); // false at the end writes the moment into the moment store of the cluster directly (the moment is then written to AOD as well)
    } else {
      theCluster->insertMoment(xAOD::CaloCluster::VERTEX_FRACTION, -1.); // false at the end writes the moment into the moment store of the cluster directly (the moment is then written to AOD as well)
      theCluster->insertMoment(xAOD::CaloCluster::NVERTEX_FRACTION, -1.); // false at the end writes the moment into the moment store of the cluster directly (the moment is then written to AOD as well)
    }
  }

  delete sumPtOfMatchedTracksPerVertex;

  m_numTracksPerVertex->clear();
  m_trkParticlePt_atOrigin->clear();
  m_trkParticleEta_atCaloEntrance->clear();
  m_trkParticlePhi_atCaloEntrance->clear();

  return StatusCode::SUCCESS;
}

StatusCode CaloClusterVertexFractionMaker::finalize()
{
  delete m_cylinderSurface_atCaloEntrance;
  delete m_discSurface_atCaloEntrance_positiveZ;
  delete m_discSurface_atCaloEntrance_negativeZ;
  delete m_numTracksPerVertex;
  delete m_trkParticlePt_atOrigin;
  delete m_trkParticleEta_atCaloEntrance;
  delete m_trkParticlePhi_atCaloEntrance;
  return StatusCode::SUCCESS;
}

double CaloClusterVertexFractionMaker::calculateDPhi(double phi1, double phi2) const
{
  double dPhi = fabs(phi1 - phi2);
  if (dPhi > M_PI) dPhi = 2.*M_PI - dPhi;
//   if (dPhi > 3.1415926535897931) dPhi = 6.2831853071795862 - dPhi;
  return dPhi;
}
