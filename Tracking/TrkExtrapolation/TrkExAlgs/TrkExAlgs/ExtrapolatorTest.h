/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtrapolatorTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_TRKEXTRAPOLATORTEST_H
#define TRKEXALGS_TRKEXTRAPOLATORTEST_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
#include <vector>
#include "TrkParameters/TrackParameters.h"

namespace Trk 
{

  class IExtrapolator;
  class IPropagator;
  class Surface;
  class MagneticFieldProperties;
  /** @class ExtrapolatorTest

     The ExtrapolatorTest Algorithm runs a number of n test extrapolations
     from randomly distributed Track Parameters to reference surfcas within
    
     - a) the Inner Detector if DetFlags.ID_On()
     - b) the Calorimeter if DetFlags.Calo_On()
     - c) the Muon System if DetFlags.Muon_On()

     It is the TestAlgorithm for 'the' Extrapolator instance provided 
     to client algorithms.

      @author  Andreas Salzburger <Andreas.Salzburger@cern.ch>
  */  

  class ExtrapolatorTest : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       ExtrapolatorTest(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~ExtrapolatorTest();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:

      void runTest( const Trk::Perigee& perigee );

      Trk::Perigee generatePerigee();
      
      /** The Extrapolator to be retrieved */
      ToolHandle<IExtrapolator>         m_extrapolator;
      ToolHandle<IPropagator>           m_propagator;
      MagneticFieldProperties*          m_magFieldProperties; //!< magnetic field properties

      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;

      double                   m_sigmaD0;                    //!< Sigma of distribution for D0
      double                   m_sigmaZ0;                    //!< Sigma of distribution for Z0
      double                   m_minPhi;                     //!< Minimal phi value
      double                   m_maxPhi;                     //!< Maximal phi value
      double                   m_minEta;                     //!< Minimal eta value
      double                   m_maxEta;                     //!< Maximal eta value
      double                   m_minP;                       //!< Minimal p value 
      double                   m_maxP;                       //!< Maximal p value

      int                      m_direction;                  //!< extrapolation direction

      int                      m_particleType;               //!< the particle typre for the extrap.

      /** member variables for algorithm properties: */
      unsigned int                                     m_referenceSurfaces;

      std::vector<double>                              m_referenceSurfaceRadius;
      std::vector<double>                              m_referenceSurfaceHalflength;
      std::vector< std::vector<const Surface*> >       m_referenceSurfaceTriples;

      std::vector<double>                              m_referenceSurfaceNegativeBoundary;
      std::vector<double>                              m_referenceSurfacePositiveBoundary;

      int m_eventsPerExecute;
      std::vector<Trk::Perigee> m_perigees;
      bool m_useExtrapolator;
    }; 
} // end of namespace

#endif
