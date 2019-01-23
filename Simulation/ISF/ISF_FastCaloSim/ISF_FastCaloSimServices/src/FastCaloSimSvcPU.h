/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSVCPU_H
#define ISF_FASTCALOSIMSVCPU_H 1

// STL includes
#include <string>
#include <set>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "CaloInterface/ICaloCellMakerTool.h"
#include "AthenaBaseComps/AthService.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// Tracking includes
#include "TrkExInterfaces/ITimedExtrapolator.h"
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/LArEM_ID.h"

#include "TRandom3.h"

// forward declarations
class ITrackingGeometrySvc;
class CaloCellContainer;

namespace HepMC
{
    class GenEvent;
}

namespace Trk
{
    class TrackingVolume;
    class TrackingGeometry;
}

namespace ISF
{

  class ISFParticle;
  class ITruthSvc;
  class IPunchThroughTool;

  /** @class FastCaloSimSvcPU
  @author Michael.Duehrssen -at- cern.ch
  */
  
  class FastCaloSimSvcPU : public BaseSimulationSvc
  { 
    public: 
      
      //** Constructor with parameters */
      FastCaloSimSvcPU( const std::string& name, ISvcLocator* pSvcLocator );
      
      /** Destructor */
      virtual ~FastCaloSimSvcPU(); 
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Simulation Call */
      StatusCode simulate(const ISFParticle& isp, McEventCollection* mcEventCollection);
                                                                 
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();

      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();
      
      AtlasDetectorID* detID;
      LArEM_ID* larID;

    private:     
      /** Default constructor */
      FastCaloSimSvcPU();

      /** process the given particle */
      StatusCode processOneParticle( const ISF::ISFParticle &isfp);

      /** extrapolation through Calo */
      std::vector<Trk::HitInfo>* caloHits(const ISF::ISFParticle &isfp) const;

      /** The Extrapolator setup */
      ToolHandle<Trk::ITimedExtrapolator>      m_extrapolator;          

      /** whether CellContainer to be created will own (default) its cells or not */
      int m_ownPolicy;

      // particle processing mode
      bool m_batchProcessMcTruth;       //!< process particles from McTruth at end of event

      bool m_simulateUndefinedBCs;      //!< do/don't simulate undefined barcode particles

      std::string  m_caloCellsOutputName;

      // authorise input to be the same as output (to be done with care)
      bool m_caloCellHack ;
      //check if punch through simulation is used
      bool m_doPunchThrough;
      
      Trk::PdgToParticleHypothesis        m_pdgToParticleHypothesis;

      // list of tools to be used
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_setup ;
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_simulate ;
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerTools_release ;
      ToolHandle< IPunchThroughTool >     m_punchThroughTool;
      
      CaloCellContainer* 		  m_theContainer;
      
      //->PU Development:
      std::vector<CaloCellContainer*> m_puCellContainer;
      //std::vector<float> m_puEnergyWeights;
      
      std::vector<float> m_puEnergyWeights_lar_bapre;
      std::vector<float> m_puEnergyWeights_lar_hec;
      std::vector<float> m_puEnergyWeights_lar_em;
      std::vector<float> m_puEnergyWeights_tile;
      	      //<-

      TRandom3 m_rndm;
  }; 
  
}

#endif //> !ISF_FASTCALOSIMSVCPU_H
