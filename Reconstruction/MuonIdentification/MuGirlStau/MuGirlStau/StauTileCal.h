/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRLNSSTAUTILECAL_H
#define MUGIRLNSSTAUTILECAL_H

#include "GaudiKernel/MsgStream.h"
#include "MuGirlStau/StauToolDefinitions.h"
#include "MuGirlStau/StauTool.h"
#include "ITrackInCaloTools/ITrackInCaloTools.h"
#include "CaloEvent/CaloCell.h"
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "MuGirlStau/StauCalibration.h"

namespace MuGirlNS
{

  class StauTileCal
    {
    public:
      StauTileCal(StauTool* pStau, MsgStream& log, const xAOD::TrackParticle* trkParticle);
      StauTileCal(StauTool* pStau, MsgStream& log, const Trk::Track* trk);

       /** default destructor */
      virtual ~StauTileCal();
      
      void initStepData(TileCalStepData* pTileCalStepData, double beta, double tTrack = 0);
      void clearStepData(TileCalStepData* pTileCalStepData);
      void printStepData(TileCalStepData* pTileCalStepData);
      void processTileCalWithBeta(double currentBeta, TileCalStepData* tileCalStepData);
      void processTileCalWithTTrack(double tTrack, TileCalStepData* tileCalStepData, double beta = 1.0);
      void clear();
      void averageBeta();
      void setAvgBeta();
      void setBeta(double beta) { m_beta = beta; }
      double beta(){ return m_beta; }
      double avgBeta() { return m_avgBeta; }
      double rmsBeta() { return m_rmsBeta; }
      bool hasCells() { return m_hasCells; }
      StauTileCalCells* caloCells() { return m_pCaloCells; }
      inline StauHits getHits() { return m_hits; }
      void initCaloCells(std::vector<const CaloCell*> cellsOnTrack);
      unsigned int tileMinIsAt();	 
    private:
      StauTool*                  m_pStau;
      MsgStream&                 m_log;
      const xAOD::TrackParticle* m_pTrkParticle;
      double m_beta;//the beta of the tile cal alone - obained from the minimization process
      double m_avgBeta;
      double m_rmsBeta;

      ToolHandle<ITrackInCaloTools>   m_pToCalo;
      ToolHandle< Trk::ITrackParticleCreatorTool >        m_particleCreatorTool;
      StauTileCalCells*                   m_pCaloCells;
      bool m_hasCells;
      StauHits m_hits;
      std::map<int, StauCalibrationParameters >* m_pCalibration;
   };

} // end of namespace

#endif
