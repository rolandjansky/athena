/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * File: TrackTools.h
 * Author: Marco van Woerden <mvanwoer@cern.ch>
 * Description: Track tools.
 *
 * Created in February 2013, based on TrackInCaloTools.
 */

#ifndef TrackTools_H
#define TrackTools_H
#include "ITrackTools.h"


//========================================================================================================================== 
class TrackTools: public AthAlgTool, virtual public ITrackTools{ 
//==========================================================================================================================
    public:
        TrackTools (const std::string& type,const std::string& name,const IInterface* parent);
        ~TrackTools();

        StatusCode initialize();
        StatusCode finalize();

        // METHOD FOR CALOCELL FILTERING
        void getCellsWithinConeAroundTrack(const xAOD::TrackParticle* track, 
                                           const CaloCellContainer* input,
                                           ConstDataVector<CaloCellContainer>* output,  
                                           double cone,
                                           bool includelar);

        std::vector< double > getXYZEtaPhiInCellSampling(const TRACK* track, const CaloCell *cell);
        std::vector< double > getXYZEtaPhiInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling);
        std::unique_ptr<const Trk::TrackParameters> getTrackInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling);
        std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const TRACK* track);
        std::vector< std::vector<double> > getXYZEtaPhiPerSampling(const TRACK* track);
        double getPathInsideCell(const TRACK *track, const CaloCell *cell);
        int retrieveIndex(int sampling, float eta);

        bool trackIsolation( xAOD::TrackIsolation& result, const xAOD::TrackParticle &tp, const std::vector<xAOD::Iso::IsolationType>& cones )
        {
          if(!m_trackIsolationTool.empty()) {
            xAOD::TrackCorrection corrlist;
            corrlist.trackbitset.set(static_cast<unsigned int>(xAOD::Iso::IsolationTrackCorrection::coreTrackPtr));
            return m_trackIsolationTool->trackIsolation(result, tp, cones, corrlist);
          }
          return false; 
        };

/*  THIS IS GOING TO BE USED
        bool caloTopoClusterIsolation( xAOD::CaloIsolation& result, const xAOD::TrackParticle& tp, 
                                               const std::vector<xAOD::Iso::IsolationType>& cones, xAOD::CaloIso::SubtractionStrategy strategy)
        {
          if(!m_caloIsolationTool.empty()) ; //GOING TO CALL PROPER FUNCTION
          return false;
        };
        bool caloCellIsolation( xAOD::CaloIsolation& result, const xAOD::TrackParticle& tp,
                                              const std::vector<xAOD::Iso::IsolationType>& cones, xAOD::CaloIso::SubtractionStrategy strategy)
       	{ 
       	  if(!m_caloIsolationTool.empty()) ; //GOING TO CALL PROPER FUNCTION
       	  return false;
        };
        bool neutralEflowIsolation( xAOD::CaloIsolation& result, const xAOD::TrackParticle& tp,
       	                                       const std::vector<xAOD::Iso::IsolationType>& cones, xAOD::CaloIso::SubtractionStrategy strategy)
       	{ 
       	  if(!m_caloIsolationTool.empty()) ; //GOING TO CALL PROPER FUNCTION
       	  return false;
        };
*/
    private:

        ToolHandle <Trk::IParticleCaloExtensionTool>       m_caloExtensionTool; //!< Tool to make the step-wise extrapolation
        ToolHandle <Rec::IParticleCaloCellAssociationTool> m_caloCellAssociationTool; //!< Tool to make the step-wise extrapolation
        //ToolHandle <Trk::ITrackSelectorTool>               m_trackSelector; //!< Tool to select tracks

        ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
        ToolHandle<xAOD::INeutralEFlowIsolationTool> m_neutralEFlowIsolationTool;

        // JOBOPTIONS PROPERTIES
        std::string         m_cellContainerName;
        double getPath(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);

        // DEFAULT CONSTRUCTOR
        TrackTools(const TrackTools&);
        TrackTools &operator= (const TrackTools&);
};

#endif //TrackTools_H
