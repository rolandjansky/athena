/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolatorComparisonTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_TRKEXTRAPOLATORCOMPARISONTEST_H
#define TRKEXALGS_TRKEXTRAPOLATORCOMPARISONTEST_H

// Gaudi includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/MsgStream.h"

// ATHENA
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/RndmGenerators.h"
#include "TrkExInterfaces/IExtrapolator.h"
// ACTS
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "TrkExAlgs/PropResultRootWriterSvc.h"
#include "Acts/EventData/TrackParameters.hpp"
// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

namespace Acts {
  class TrackingGeometry;
  
  namespace detail {
    class Step;
  }
}

struct perigeeParameters {
 double m_d0    ;
 double m_z0    ;
 double m_phi   ;
 double m_eta   ;
 double m_pt    ;
 double m_charge;
 perigeeParameters(double d0, double z0, double phi, double eta, double pt, double charge):
 m_d0(d0), m_z0(z0), m_phi(phi), m_eta(eta), m_pt(pt), m_charge(charge) {}
};

namespace Trk 
{
  /** @class ExtrapolatorComparisonTest

     The ExtrapolatorComparisonTest Algorithm runs a number of n test extrapolations
     from randomly distributed Track Parameters to reference surfcas within
    
     - a) the Inner Detector if DetFlags.ID_On()
     - b) the Calorimeter if DetFlags.Calo_On()
     - c) the Muon System if DetFlags.Muon_On()

     It uses and compares the output obtaining using 
     the ATLAS Extrapolator and the ACTS Extrapolation

  */  

  class ExtrapolatorComparisonTest : public AthReentrantAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       ExtrapolatorComparisonTest(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~ExtrapolatorComparisonTest();

       /** standard Athena-Algorithm method */
       StatusCode          initialize() override;
       /** standard Athena-Algorithm method */
       StatusCode          execute(const EventContext& ctx) const override;
       /** standard Athena-Algorithm method */
       StatusCode          finalize() override;
       
    private:

      /** @struct ActsTrackWrapper
         Wrapper code for Acts track parameters, to provide a position() method
         without the need of explicitly passing the Acts::GeometryContext
      */
      struct ActsTrackWrapper
      {
        /** Constructor */
        ActsTrackWrapper(const Acts::BoundTrackParameters* trackParams, Acts::GeometryContext& ctx)
        : m_params(trackParams)
        , m_geometryContext(ctx) {};

        /** Position getter */
        Acts::Vector3D position() const {return m_params->position(m_geometryContext);}
        /** Parameter getter */
        Acts::BoundVector parameters() const {return m_params->parameters();}
        /** Covariance getter */
        const std::optional<Acts::BoundSymMatrix>& covariance() const {return m_params->covariance();}

      private:
        /** The Acts track parameters */
        const Acts::BoundTrackParameters* m_params;
        /** The Acts geometry context */
        Acts::GeometryContext m_geometryContext;
      };
      
      void generatePerigee(std::vector<perigeeParameters>& parameters);

      /** The ACTS ExtrapolationTool to be retrieved */
      ToolHandle<IActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "ActsExtrapolationTool"};
      
      /** The ATLAS Extrapolator to be retrieved */
      ToolHandle<Trk::IExtrapolator> m_atlasExtrapolator {this, "Extrapolator", "Trk::Extrapolator/AtlasExtrapolator"};


      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;
      
      double                            m_sigmaD0;                    //!< Sigma of distribution for D0
      double                            m_sigmaZ0;                    //!< Sigma of distribution for Z0
      double                            m_minPhi;                     //!< Minimal phi value
      double                            m_maxPhi;                     //!< Maximal phi value
      double                            m_minEta;                     //!< Minimal eta value
      double                            m_maxEta;                     //!< Maximal eta value
      double                            m_minPt;                      //!< Minimal p value 
      double                            m_maxPt;                      //!< Maximal p value
                                        
      int                               m_particleType;               //!< the particle typre for the extrap.

      /** member variables for algorithm properties: */
      unsigned int                                     m_referenceSurfaces;

      std::vector<double>                              m_referenceSurfaceRadius;
      std::vector<double>                              m_referenceSurfaceHalflength;
      
      std::vector<double>                              m_referenceSurfaceNegativeBoundary;
      std::vector<double>                              m_referenceSurfacePositiveBoundary;
      
      std::vector< std::vector<const Trk::Surface*> >  m_atlasReferenceSurfaceTriples;
      std::vector< std::vector< std::shared_ptr<const Acts::Surface> > > m_actsReferenceSurfaceTriples;
      
      int m_eventsPerExecute;
      
      ServiceHandle<PropResultRootWriterSvc> m_atlasPropResultWriterSvc;
      ServiceHandle<PropResultRootWriterSvc> m_actsPropResultWriterSvc;
      
    }; 
} // end of namespace

#endif
