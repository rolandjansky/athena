/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CombinedExtrapolatorTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXALGS_COMBINEDEXTRAPOLATORTEST_H
#define TRKEXALGS_COMBINEDEXTRAPOLATORTEST_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>


namespace Trk 
{

  class IExtrapolator;
  class Surface;
  class TrackingVolume;
  class TrackingGeometry;

  /** @class CombinedExtrapolatorTest

     The ExtrapolatorTest Algorithm runs a number of n test extrapolations
     from randomly distributed Track Parameters to geometry outer boundary and back to perigee

      @author Sarka Todorova <sarka.todorova@cern.ch>
  */  

  class CombinedExtrapolatorTest : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       CombinedExtrapolatorTest(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~CombinedExtrapolatorTest();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      /** The Extrapolator to be retrieved */
      ToolHandle<IExtrapolator>         m_extrapolator;

      /** Random Number setup */
      Rndm::Numbers*                    m_gaussDist;
      Rndm::Numbers*                    m_flatDist;

      double                   m_sigmaD0;                    //!< Sigma of distribution for D0
      double                   m_minZ0;                      //!< min Z0
      double                   m_maxZ0;                      //!< max Z0
      double                   m_minEta;                     //!< Minimal eta value
      double                   m_maxEta;                     //!< Maximal eta value
      double                   m_minP;                       //!< Minimal p value 
      double                   m_maxP;                       //!< Maximal p value

      const Trk::TrackingVolume*       m_outerBoundary;
      mutable const Trk::TrackingGeometry*       m_trackingGeometry;

      int                      m_particleType;               //!< the particle typre for the extrap.

      
    }; 
} // end of namespace

#endif
