/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMVERTEXBUILDER_H
#define EMVERTEXBUILDER_H

#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMVertexBuilder.h"
#include "egammaBaseTool.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

class  StoreGateSvc;

namespace InDet
{
  class IVertexFinder;
  class SingleTrackConversionTool;
}

class IEMExtrapolationTools;

/**
   @class EMVertexBuilder
   This tool builds ID vertices from a given TrackParticleContainer
   @author Kerstin Tackmann (based on work by many others)
*/

class EMVertexBuilder : virtual public IEMVertexBuilder, public egammaBaseTool {

	public:
		EMVertexBuilder (const std::string& type,const std::string& name, const IInterface* parent);
		virtual ~EMVertexBuilder();

		virtual StatusCode initialize();
		virtual StatusCode finalize();

		virtual StatusCode contExecute();

	private:
	
	  /** Maximum radius accepted for conversion vertices **/
	  float m_maxRadius;

    /** @brief create single track vertices from tracks that are not part of other vertices **/
    void addSingleTrackVertices(const xAOD::TrackParticleContainer*,
                                xAOD::VertexContainer*) const;


		StoreGateSvc          *m_storeGate;
		
		/** @brief TrackParticle container input name*/
		std::string            m_inputTrackParticleContainerName;

		/** @brief conversion container output name*/
		std::string            m_outputConversionContainerName;

    /** @brief Tool to find vertices (creates double-track conversions) */
		ToolHandle<InDet::IVertexFinder>    m_vertexFinderTool;

    /** @brief Tool to create single-track conversions */
    ToolHandle<InDet::SingleTrackConversionTool> m_singleTrkConvTool;

    /** @brief EMExtrapolationTool */
    ToolHandle<IEMExtrapolationTools>  m_EMExtrapolationTool;
    		
};

#endif // EMVERTEXBUILDER_H 
