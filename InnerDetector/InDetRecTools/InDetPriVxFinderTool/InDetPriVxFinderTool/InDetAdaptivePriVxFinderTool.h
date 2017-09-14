/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @class InDet::InDetAdaptivePriVxFinderTool
 *
 * @author Giacinto Piacquadio (Freiburg University)
 *
 *
 * This class provides an implementation for a primary 
 * vertex finding tool, which uses the Adaptive Vertex 
 * Fitter to reject outliers not belonging to the primary 
 * vertex interaction.
 *
 * The steps done are simply;
 * - Tracks are selected according to the specified cuts
 * - The Adaptive Vertex Finder is used to fit them
 *
 * Contrary to the InDetPriVxFinderTool, the outlier 
 * rejection is done by the fitter and not by the finder.
 *
 * One only vertex can be fit, so it is not suited (as a 
 * finder) when many minimum bias vertices can be expected.
 * In this case please use the <i>InDetPriVxFinderTool</i>.
 *
 *
 * (this is a modified verson of InDetPriVxFinderTool.h of A. Wildauer & F. Akesson)
 *  changes :
 *      06/12/2006   Kirill.Prokofiev@cern.ch 
 *      EDM cleanup and switching to the FitQuality use 
 *
 ***************************************************************************/

//implemented using as template the InDetPriVxFinderTool class of A. Wildauer and F. Akesson

#ifndef INDETPRIVXFINDERTOOL_INDETADAPTIVEPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETADAPTIVEPRIVXFINDERTOOL_H

#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkTrack/TrackCollection.h" // type def ...
#include "TrkParticleBase/TrackParticleBaseCollection.h" // type def ...
#include "TrkParameters/TrackParameters.h"
/**
 * Forward declarations 
 */
 
class VxContainer;
class IBeamCondSvc;
#include "xAODTracking/VertexFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"

namespace Trk
{
 class IVertexFitter;
 class VxCandidate;
 class Track;
 class TrackParticleBase;
 class IVxCandidateXAODVertex;
}

namespace InDet
{
  class IInDetTrackSelectionTool;
  
 class InDetAdaptivePriVxFinderTool : public AthAlgTool, virtual public IVertexFinder
 {

public:

   /**
    * Constructor
    */
   
   InDetAdaptivePriVxFinderTool(const std::string& t, const std::string& n, const IInterface*  p);
   
   /**
    * Destructor
    */
   
   virtual ~InDetAdaptivePriVxFinderTool();
    
   StatusCode initialize();

   /** 
    * Finding method.
    * Has as input a track collection and as output 
    * a VxContainer.
    */

   VxContainer* findVertex(const TrackCollection* trackTES);//by GP: deleted const before VxContainer*
   VxContainer* findVertex(const Trk::TrackParticleBaseCollection* trackTES);
   std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(const xAOD::TrackParticleContainer* trackParticles);
   
   StatusCode finalize();
   
 private:
   
   /** the common finding code (regardless of Track or TrackParticle(Base) is here */
   VxContainer* m_findVertex(std::vector<const Trk::TrackParameters*>& origParameters);

   ToolHandle< Trk::IVertexFitter > m_iVertexFitter;
   ToolHandle< InDet::IInDetTrackSelectionTool > m_trkFilter;
   ToolHandle< Trk::IVxCandidateXAODVertex > m_VertexEdmFactory;
   
   ServiceHandle<IBeamCondSvc> m_iBeamCondSvc; //!< pointer to the beam condition service
   

   void m_SGError(std::string errService);

   /**
    * Internal method to print the parameters setting
    */

   virtual void m_printParameterSettings();
 
 };//end of class definitions
}//end of namespace definitions
#endif
                                                                                                             
