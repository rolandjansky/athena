/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///============================================================
///
/// T2VertexBeamSpot.h, (c) ATLAS Detector software
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Online beam spot measurement and monitoring using
/// L2 recontructed primary vertices.
///
/// Authors : David W. Miller, Rainer Bartoldus,
///           Su Dong
///
/// Date : 11 May, 2008
///
///============================================================

/**********************************************************************************
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2VertexBeamSpot
 *
 * @brief  PESA algorithm that measures and monitors beam spot position using vertices
 *
 * @author David W. Miller    <David.W.Miller@cern.ch>     - SLAC, Stanford University
 *
 * File and Version Information:
 * $Id: T2VertexBeamSpot.h 702277 2015-10-22 10:33:51Z smh $
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H
#define TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H

/// trigger EDM
#include "TrigInterfaces/AllTEAlgo.h"

namespace HLT {
  class TriggerElement;
}

namespace PESA {

  /**
   *  @class T2VertexBeamSpot
   *  This class uses primary vertex reconstruction to measure
   *  and monitor the LHC beam as seen by the ATLAS detector. 
   *  Fast, online vertexing is implemented using Dmitry 
   *  Emeliyanov's (D.Emeliyanov@rl.ac.uk) TrigPrimaryVertexFitter.
   *   
   *  As an AllTEAlgo, this must appear first in the sequence.
   *
   *  @author Ignacio Aracena     <ignacio.aracena@cern.ch>
   *  @author Rainer Bartoldus    <bartoldu@slac.stanford.edu>
   *  @author Su Dong             <sudong@slac.stanford.edu>
   *  @author David W. Miller     <David.W.Miller@cern.ch>
   *   
   */

  class T2VertexBeamSpot : public HLT::AllTEAlgo
    {
    public:

      T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator ); //!< std Gaudi Algorithm constructor
      virtual ~T2VertexBeamSpot();

      HLT::ErrorCode hltInitialize();
      HLT::ErrorCode hltFinalize();

      /**
       * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
       *
       * @param input outer vector describeds the different input TE types,
       *              inner vector provides all TE instances of the given type
       * @param output the output TE type
       */
      HLT::ErrorCode hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& input,
                                 unsigned int output );

    private:

      /// Implementation class
      class T2VertexBeamSpotImpl* impl;

      //Switch to use TrigInDetTrack (i.e. L2Star algorithms)
      bool m_doTrigInDetTrack;

      // Need this to get access to protected msg() and msgLvl()
      friend class T2VertexBeamSpotImpl;
    };

} // end namespace

#endif
