/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 
//////////////////////////////////////////////////////////////////
// DNNCaloSimSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_DNNCALOSIMSVC_H
#define ISF_DNNCALOSIMSVC_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// Framework includes
#include "GaudiKernel/IChronoStatSvc.h"

// FastCaloSim includes
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
#include "TrkExInterfaces/ITimedExtrapolator.h" 

#include "CaloInterface/ICaloCellMakerTool.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/TileID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "lwtnn/LightweightGraph.hh"
#include "CaloEvent/CaloCellContainer.h"


namespace CLHEP
{
 class HepRandomEngine;
}

//forward declarations
class CaloCellContainer;
class CaloGeometryFromCaloDDM;
class TFCSParametrizationBase;


namespace ISF {
  /** @class DNNCaloSimSvc

    @author Aishik.Ghosh -at- cern.ch, David Rousseau -at- cern.ch,
    */
  
  class DNNCaloSimSvc : public BaseSimulationSvc
  {
    public:
      /** Constructor with parameters */
      DNNCaloSimSvc(const std::string& name, ISvcLocator* pSvcLocator);
       
      /** Destructor */
      virtual ~DNNCaloSimSvc();
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();
      
      /** helper for initialize */
      StatusCode initializeNetwork();

      /** Simulation Call */
      StatusCode simulate(const ISFParticle& isp);
      // type of input requested by lwtnn
      typedef std::map<std::string, std::map<std::string, double> >  NetworkInputs ;
      typedef std::map<std::string, double> NetworkOutputs;
      StatusCode fillNetworkInputs(const ISF::ISFParticle& isfp, NetworkInputs  & inputs, double & trueEnergy);
      
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();
      
      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();
      
      std::string m_paramsFilename;
      std::string m_paramsInputArchitecture;

      lwt::LightweightGraph * m_graph ;
      
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup ;
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease ;

      ToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation;
      ToolHandle<Trk::ITimedExtrapolator> m_extrapolator;
      
      CaloCellContainer *       m_theContainer;

      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;
      std::string                     m_randomEngineName;
      

      const CaloDetDescrManager* m_caloDetDescrManager;
      CaloGeometryFromCaloDDM* m_caloGeo;
      const LArEM_ID* m_emID;
      std::vector<CaloCell*> m_windowCells;

      // specific to architecture
      // preprocessing of input
      int m_GANLatentSize = 0;
      double m_logTrueEnergyMean = 0.;
      double m_logTrueEnergyScale = 0.;
      double m_riImpactEtaMean = 0.;
      double m_riImpactEtaScale = 0.;
      double m_riImpactPhiMean = 0.;
      double m_riImpactPhiScale = 0.;

      // building of the 266 cells cluster
      const int m_numberOfCellsForDNN = 266;
      const double m_middleCellWidthEta = 0.025;
      const double m_middleCellWidthPhi = CLHEP::pi / pow(2,7);
      const double m_etaRawMiddleCut = m_middleCellWidthEta * 3.5;
      const double m_etaRawBackCut = m_middleCellWidthEta * 4.;
      const double m_phiRawMiddleCut = m_middleCellWidthPhi * 3.5;
      const double m_phiRawStripCut = m_middleCellWidthPhi * 6.0;

      
      std::string  m_caloCellsOutputName;
  };

}

#endif //> !ISF_DNNCALOSIMSVC_H
