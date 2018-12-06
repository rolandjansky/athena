/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBREMCOLLECTIONBUILDER_H
#define EMBREMCOLLECTIONBUILDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "egammaInterfaces/IEMBremCollectionBuilder.h"
#include "TrkTrack/TrackCollection.h"

class IegammaTrkRefitterTool;
class IegammaCheckEnergyDepositTool;
class IEMExtrapolationTools;

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "TrkTrack/TrackCollection.h"


namespace Trk
{
  class ITrackParticleCreatorTool;
  class ITrackSlimmingTool;
  class ITrackSummaryTool;
}

class CaloCluster;


class EMBremCollectionBuilder : public AthAlgTool,virtual public IEMBremCollectionBuilder{
    public:
        EMBremCollectionBuilder(const std::string& type, const std::string& name, const IInterface* parent);
        ~EMBremCollectionBuilder();

        virtual StatusCode initialize();
        virtual StatusCode finalize();
        virtual StatusCode contExecute();
	virtual StatusCode hltExecute(const xAOD::CaloClusterContainer *clusters,
				      const xAOD::TrackParticleContainer *inputTracks,
				      TrackCollection *refitTracks,
				      xAOD::TrackParticleContainer *refitTrackParticles);
	
    private:
	//------------------------------------------------------------------------
	//      methods
	//------------------------------------------------------------------------
	//
        /** @brief Refit of track */
	StatusCode refitTrack(const xAOD::TrackParticle* tmpTrkPart, 
			      TrackCollection *refitTracks, xAOD::TrackParticleContainer *refitTrackParticles);
        /** @brief broad track selection */
	bool Select(const xAOD::CaloCluster*        cluster,
              bool                      trkTRT,
              const xAOD::TrackParticle* track) const;
	//------------------------------------------------------------------------
	//      configurable data members
	//------------------------------------------------------------------------       
	/** @brief The track refitter */
	ToolHandle<IegammaTrkRefitterTool>  m_trkRefitTool;
	/** @brief Tool to create track particle */
	ToolHandle< Trk::ITrackParticleCreatorTool > m_particleCreatorTool;
	/** @brief Tool to slim tracks  */
	ToolHandle<Trk::ITrackSlimmingTool>  m_slimTool;
	/** @brief Tool for Track summary  */
	ToolHandle<Trk::ITrackSummaryTool>   m_summaryTool;
	/** @brief Tool for extrapolation */
	ToolHandle<IEMExtrapolationTools> m_extrapolationTool;
	/** @brier Option to do truth*/
	bool                              m_doTruth;
	/** @brief Names of input output collections */
	std::string                       m_clusterContainerName;
	std::string                       m_trackParticleContainerName;
	std::string                       m_OutputTrkPartContainerName;	
	std::string                       m_OutputTrackContainerName;	
	/** @Cut on minimum silicon hits*/
	int                               m_MinNoSiHits;
	/** @brief broad cut on deltaEta*/
	double                m_broadDeltaEta;
	/** @brief broad cut on deltaPhi*/
	double                m_broadDeltaPhi;
	/** @narrow windows*/
	double                m_narrowDeltaEta;
	double                m_narrowDeltaPhi;
	double                m_narrowDeltaPhiBrem;
	double                m_narrowRescale;
	double                m_narrowRescaleBrem;
	//bool                  m_useBremFinder;
	//collections
	TrackCollection*                    m_finalTracks;
	xAOD::TrackParticleContainer*       m_finalTrkPartContainer;
	//counters
	unsigned int m_AllClusters;
	unsigned int m_AllTracks;
	unsigned int m_AllTRTTracks;
	unsigned int m_AllSiTracks;
	unsigned int m_SelectedTracks;
	unsigned int m_SelectedTRTTracks;
	unsigned int m_SelectedSiTracks;
	unsigned int m_FailedFitTracks;
	unsigned int m_FailedSiliconRequirFit;
	unsigned int m_RefittedTracks;
//------------------------------------------------------------------------

};
#endif // ANA1_H
