/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
///============================================================
/// T2VertexBeamSpot.h, (c) ATLAS Detector software
/// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
///
/// Online beam spot measurement and monitoring using
/// L2 recontructed primary vertices.
///
/// Authors : David W. Miller, Rainer Bartoldus,
///           Su Dong
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
 **********************************************************************************/
#ifndef TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H
#define TRIGT2BEAMSPOT_T2VERTEXBEAMSPOT_H

#include "AthenaMonitoringKernel/Monitored.h"
/// trigger EDM
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
//Interface for the beam spot tool
#include "T2VertexBeamSpotTool.h"

namespace PESA {
  /**
   *  @class T2VertexBeamSpot
   *  This class uses primary vertex reconstruction to measure
   *  and monitor the LHC beam as seen by the ATLAS detector. 
   *  Fast, online vertexing is implemented using Dmitry 
   *  Emeliyanov's (D.Emeliyanov@rl.ac.uk) TrigPrimaryVertexFitter.
   *
   *  @author Ignacio Aracena     <ignacio.aracena@cern.ch>
   *  @author Rainer Bartoldus    <bartoldu@slac.stanford.edu>
   *  @author Su Dong             <sudong@slac.stanford.edu>
   *  @author David W. Miller     <David.W.Miller@cern.ch>
   *   
   */

  class T2VertexBeamSpot : public AthReentrantAlgorithm 
    {
    public:

      T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator ); //!< std Gaudi Algorithm constructor

      /** Loop over events, selecting tracks and reconstructing vertices out of these tracks   */
      virtual StatusCode execute(const EventContext& ctx) const final;

      /** Initialize the beamspot algorithm for Run3 Athena MT configuration, initialize all the handles and retrieve the tools associated with the algorithm */
      virtual StatusCode initialize() final;

      

    private:

      //Tools
      ToolHandle<T2VertexBeamSpotTool> m_beamSpotTool {this, "BeamSpotTool", "PESA::T2VertexBeamSpotTool/T2VertexBeamSpotTool" };
      ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};

      SG::ReadHandleKey<TrackCollection> m_trackCollectionKey;   /*track collection name which should be used for the algorithms*/

      //The same as in Run2 (m_vertexCollName)
      SG::WriteHandleKey<TrigVertexCollection> m_outputVertexCollectionKey;
      //TODO: to be added SG::WriteHandleKeyArray<TrigVertexCollection> m_outputSplitVertexCollectionKey;   /*Input list of track collection names which should be used for the algorithms*/


    };

} // end namespace




#endif
