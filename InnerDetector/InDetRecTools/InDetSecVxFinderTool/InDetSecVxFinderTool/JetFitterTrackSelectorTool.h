/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration     
*/

/*
 * This tooltakes the input track collection of a jet candidate
 * It runs a filter on track quality
 * And separates tracks into primary vertex and secondary vertex tracks
 * By computing their compatibility with the primary vertex
 */

#ifndef JETFITTER_TRACKSELECTOR_TOOL_H
#define JETFITTER_TRACKSELECTOR_TOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "VxJetVertex/SelectedTracksInJet.h"
#include "xAODTracking/Vertex.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "xAODTracking/TrackParticleContainer.h"

#include "TrkLinks/LinkToXAODTrackParticle.h" 
#include "TrkTrackLink/ITrackLink.h"

#include "VxVertex/RecVertex.h"

namespace InDet {

    static const InterfaceID IID_JetFitterTrackSelectorTool("JetFitterTrackSelectorTool", 1, 0);

    class JetFitterTrackSelectorTool : public AthAlgTool {
    public:

        static const InterfaceID& interfaceID() {
            return IID_JetFitterTrackSelectorTool;
        }

        StatusCode initialize();
        StatusCode finalize();

        JetFitterTrackSelectorTool(const std::string &t, const std::string &n, const IInterface *p);
        ~JetFitterTrackSelectorTool();

        /*
         * method for performing track selection
         */
        const Trk::SelectedTracksInJet* doTrackSelection(const xAOD::Vertex &primaryVertex,
							 const TLorentzVector &jetMomentum,
							 const std::vector<const xAOD::IParticle *> &inputTracks) const;

    private:
	int computeTrackCompatibility( const xAOD::Vertex &primaryVertex,
				       const TLorentzVector &jetMomentum,
				       const xAOD::TrackParticle &trk_iter ) const;
	
    private:
	ToolHandle< Trk::IExtrapolator > m_extrapolator {this,"Extrapolator","Trk::Extrapolator/InDetExtrapolator",""};
	ToolHandle< InDet::InDetJetFitterUtils > m_jetFitterUtils {this,"InDetJetFitterUtils","InDet::InDetJetFitterUtils/InDetJetFitterUtils",""};   
	ToolHandle< Trk::ITrackSelectorTool > m_trkFilter {this,"TrackSelector","InDet::InDetDetailedTrackSelectorTool",""};  

    private:
	Gaudi::Property< bool > m_revertFromPositiveToNegativeTags {this,"revertFromPositiveToNegativeTags",false,""};
	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexForPositiveLifetimeTracks {this,"cutCompPrimaryVertexForPosLifetimeTracks",1e-1,""};
	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexForNegativeLifetimeTracks {this,"cutCompPrimaryVertexForNegLifetimeTracks",5e-2,""};
    };

}

#endif
