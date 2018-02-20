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
        const Trk::TrackParameters* getTrackInCellSampling(const TRACK* track, CaloSampling::CaloSample sampling);
        std::vector< std::vector<double> > getXYZEtaPhiPerLayer(const TRACK* track);
        std::vector< std::vector<double> > getXYZEtaPhiPerSampling(const TRACK* track);
        double getPathInsideCell(const TRACK *track, const CaloCell *cell);
        int retrieveIndex(int sampling, float eta);


    private:

        ToolHandle <Trk::IParticleCaloExtensionTool>       m_caloExtensionTool; //!< Tool to make the step-wise extrapolation

        // JOBOPTIONS PROPERTIES
        std::string         m_cellContainerName;
        double getPath(const CaloCell* cell, const Trk::TrackParameters *entrance, const Trk::TrackParameters *exit);

        // DEFAULT CONSTRUCTOR
        TrackTools(const TrackTools&);
        TrackTools &operator= (const TrackTools&);
};

#endif //TrackTools_H
