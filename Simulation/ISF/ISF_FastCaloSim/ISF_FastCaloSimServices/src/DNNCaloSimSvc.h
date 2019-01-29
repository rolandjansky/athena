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
      
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();
      
      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();
      
      std::string m_paramsFilename;
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
      const double m_logTrueEnergyMean_const = 9.70406053;
      const double m_logTrueEnergyScale_const = 1.76099569;
      const double m_riImpactEtaMean_const = 3.47603256e-05;
      const double m_riImpactEtaScale_const = 0.00722316;
      const double m_riImpactPhiMean_const = -5.42153684e-05;
      const double m_riImpactPhiScale_const = 0.00708241;

      const double m_MiddleCellWidthEta_const = 0.025;
      const double m_MiddleCellWidthPhi_const = CLHEP::pi / pow(2,7);
      const double m_EtaRawMiddleCut_const = m_MiddleCellWidthEta_const * 3.5;
      const double m_EtaRawBackCut_const = m_MiddleCellWidthEta_const * 4.;
      const double m_PhiRawMiddleCut_const = m_MiddleCellWidthPhi_const * 3.5;
      const double m_PhiRawStripCut_const = m_MiddleCellWidthPhi_const * 6.0;
      
      std::string  m_caloCellsOutputName;
  };

}

#endif //> !ISF_DNNCALOSIMSVC_H
