/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration  
*/

#ifndef JETFITTER_V0FINDER_TOOL_H
#define JETFITTER_V0FINDER_TOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "VxJetVertex/TwoTrackVerticesInJet.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"    
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"

namespace InDet {

    static const InterfaceID IID_JetFitterV0FinderTool("JetFitterV0FinderTool", 1, 0);

    class JetFitterV0FinderTool : public AthAlgTool {
    public:

        static const InterfaceID& interfaceID() {
	  return IID_JetFitterV0FinderTool;
        }

        StatusCode initialize();
        StatusCode finalize();

	JetFitterV0FinderTool(const std::string &t, const std::string &n, const IInterface *p);
	~JetFitterV0FinderTool();

	const Trk::TwoTrackVerticesInJet* doV0Finding( const xAOD::Vertex&,
						       const TLorentzVector&,
						       std::vector< const Trk::ITrackLink* >&,
						       const std::vector< const xAOD::Vertex* >&,
						       std::vector< const Trk::ITrackLink* >&,
						       std::vector< const Trk::ITrackLink* >&,
						       Amg::Vector3D& ) const;

    private:
 	std::vector< const xAOD::Vertex* > findV0candidates( const xAOD::Vertex&,
							     const TLorentzVector&,
							     std::vector< const Trk::ITrackLink* >&,
							     const std::vector< const xAOD::Vertex* >& ) const;
	
	bool checkCriteriaFirstFit( const xAOD::Vertex&,
				    const TLorentzVector&,
				    const xAOD::Vertex& ) const;

	bool checkCriteriaSecondFit( const xAOD::Vertex&,
				     const Trk::ITrackLink* ) const;

	Amg::Vector3D computeSeedDirection( const xAOD::Vertex&,
					    const TLorentzVector&,
					    const std::vector< Trk::PositionAndWeight >& ) const;

    private:
	ToolHandle< InDet::InDetJetFitterUtils > m_jetFitterUtils {this,"InDetJetFitterUtils","InDet::InDetJetFitterUtils/InDetJetFitterUtils",""}; 
	ToolHandle< Trk::IMode3dFinder > m_mode3dfinder {this,"Mode3dFinder","Trk::Mode3dTo1dFinder/Mode3dTo1dFinder",""};

    private:
	SG::AuxElement::Accessor< float > m_compatibilityAccessor;
	SG::AuxElement::Accessor< std::vector< const Trk::ITrackLink* > > m_tracksAccessor;

    private:
	// @brief
	Gaudi::Property< bool > m_revertFromPositiveToNegativeTags {this,"revertFromPositiveToNegativeTags",false,""};

	Gaudi::Property< double > m_cutTwoTrkVtxVertexProbForBFirstSelectionFirstCriterium {this,"cutTwoTrkVtxVtxProbForBFirstSelectCriteriumA",0.05,""};
	Gaudi::Property< double > m_cutTwoTrkVtxVertexProbForBFirstSelectionSecondCriterium {this,"cutTwoTrkVtxVtxProbForBFirstSelectCriteriumB",0.034,""};

	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection {this,"cutCompatibilityPrimaryVertexSingleTrackForBFirstSelection",1e-1,""};
	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexBothTracksForBFirstSelection {this,"cutCompatibilityPrimaryVertexBothTracksForBFirstSelection",1e-2,""};
	Gaudi::Property< double > m_cutIPD0BothTracksForBFirstSelection {this,"cutIPD0BothTracksForBFirstSelection",3.5,""};
	Gaudi::Property< double > m_cutIPZ0BothTracksForBFirstSelection {this,"cutIPZ0BothTracksForBFirstSelection",5.,""};
	Gaudi::Property< double > m_cutPtBothTracksForBFirstSelection {this,"cutPtBothTracksForBFirstSelection",500.,""};

	Gaudi::Property< double > m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionFirstCriterium {this,"cutTwoTrkVtxLifeSignForBFirstSelectCriteriumA",1.,""};
	Gaudi::Property< double > m_cutTwoTrkVtxLifetimeSignificanceForBFirstSelectionSecondCriterium {this,"cutTwoTrkVtxLifeSignForBFirstSelectCriteriumB",1.5,""};
	Gaudi::Property< double > m_cutCompatibilityToPrimarySingleTrackForMatInteractions {this,"cutCompToPrimarySingleTrackForMatInterac",1e-4,""};
	Gaudi::Property< double > m_cutCompatibilityToPrimaryBothTracksForMatInteractions {this,"cutCompToPrimaryBothTracksForMatInterac",1e-6,""};

	Gaudi::Property< double > m_firstBeam_min {this,"firstBeam_min",23,""};
	Gaudi::Property< double > m_firstBeam_max {this,"firstBeam_max",25,""};
	Gaudi::Property< double > m_secondBeam_min {this,"secondBeam_min",23,""};
	Gaudi::Property< double > m_secondBeam_max {this,"secondBeam_max",25,""};
	Gaudi::Property< double > m_firstLayer_min {this,"firstLayer_min",34.0-2.5,""};
	Gaudi::Property< double > m_firstLayer_max {this,"firstLayer_max",34.0+2.5,""};
	Gaudi::Property< double > m_secondLayer_min {this,"secondLayer_min",51.5-3,""};
	Gaudi::Property< double > m_secondLayer_max {this,"secondLayer_max",51.5+3,""};

	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexSinglePositiveLifetimeTrackForBSecondSelection {this,"cutCompPVSinglePosLifeTrackForBSecondSelect",5e-2,""};
	Gaudi::Property< double > m_cutCompatibilityPrimaryVertexSingleNegativeLifetimeTrackForBSecondSelection {this,"cutCompPVSingleNegLifeTrackForBSecondSelect",1e-2,""};
	Gaudi::Property< double > m_cutIPD0SigBoxSingleTrackForBSecondSelection {this,"cutIPD0SigBoxSingleTrackForBSecondSelection",2.,""};
	Gaudi::Property< double > m_cutIPZ0SigBoxSingleTrackForBSecondSelection {this,"cutIPZ0SigBoxSingleTrackForBSecondSelection",5.,""};

	Gaudi::Property< double > m_cutIPD0SingleTrackForBSecondSelection {this,"cutIPD0SingleTrackForBSecondSelection",1.5,""};
	Gaudi::Property< double > m_cutIPZ0SingleTrackForBSecondSelection {this,"cutIPZ0SingleTrackForBSecondSelection",3.,""};
	Gaudi::Property< double > m_cutPtSingleTrackForBSecondSelection {this,"cutPtSingleTrackForBSecondSelection",750,""};
    };

}

#endif
