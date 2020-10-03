/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETFITTER_TWOTRACKVTX_FINDER_TOOL_H
#define JETFITTER_TWOTRACKVTX_FINDER_TOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"

namespace InDet {

    static const InterfaceID IID_JetFitterTwoTrackVtxFinderTool("JetFitterTwoTrackVtxFinderTool", 1, 0);

    class JetFitterTwoTrackVtxFinderTool : public AthAlgTool {
    public:

        static const InterfaceID& interfaceID() {
	  return IID_JetFitterTwoTrackVtxFinderTool;
        }

	JetFitterTwoTrackVtxFinderTool(const std::string &t, const std::string &n, const IInterface *p);
	~JetFitterTwoTrackVtxFinderTool();

        StatusCode initialize();
        StatusCode finalize();

	const Trk::TwoTrackVerticesInJet* doVertexFinding( const xAOD::Vertex&,
							   const TLorentzVector&,
							   std::vector< const Trk::ITrackLink* >& ) const;


    private:
	xAOD::Vertex* computeVtxcandidate( const xAOD::Vertex&,
					   const TLorentzVector&,
					   const Trk::ITrackLink* trackA,
					   const Trk::ITrackLink* trackB ) const;

    private:
	ToolHandle< Trk::IVertexSeedFinder > m_CrossDistancesSeedFinder {this,"CrossDistancesSeedFinder","Trk::CrossDistancesSeedFinder/CrossDistancesSeedFinder",""};
	ToolHandle< Trk::IVertexFitter > m_SequentialVertexFitter {this,"SequentialVertexFitter","Trk::SequentialVertexFitter/SequentialVertexFitter",""};

	SG::AuxElement::Decorator< std::vector< const Trk::ITrackLink* > > m_tracksDecorator;

	Gaudi::Property< double > m_maxR {this,"ID_maxR",1150.,""};
	Gaudi::Property< double > m_maxZ {this,"ID_maxZ",2727.,""};
	Gaudi::Property< double > m_twoVertexProbabilityCut {this,"twoVertexProbabilityCut",3.4e-2,""};
	Gaudi::Property< bool > m_revertFromPositiveToNegativeTags {this,"revertFromPositiveToNegativeTags",false,""};
    };

}

#endif
