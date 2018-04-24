/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FASTCALOSIMSVCV2_H
#define ISF_FASTCALOSIMSVCV2_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

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

namespace CLHEP
{
 class HepRandomEngine;
}

//forward declarations
class CaloCellContainer;
class CaloGeometryFromCaloDDM;
class TFCSParametrizationBase;


namespace ISF {
  /** @class FastCaloSimSvcV2

    @author Elmar.Ritsch -at- cern.ch, Geraldine.Conti -at- cern.ch, Flavia.Dias -at- cern.ch
    */
  
  class FastCaloSimSvcV2 : public BaseSimulationSvc
  {
    public:
      /** Constructor with parameters */
      FastCaloSimSvcV2(const std::string& name, ISvcLocator* pSvcLocator);
      
      /** Destructor */
      virtual ~FastCaloSimSvcV2();
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();
      
      /** Simulation Call */
      StatusCode simulate(const ISFParticle& isp);
      
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();
      
      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();
      
      std::string m_paramsFilename;
      std::string m_paramsObject;
      
      TFCSParametrizationBase*  m_param;
      
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup ;
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease ;

      ToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation;
      ToolHandle<Trk::ITimedExtrapolator> m_extrapolator;
      
      CaloCellContainer *       m_theContainer;

      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;
      std::string                     m_randomEngineName;
      
      CaloGeometryFromCaloDDM* m_caloGeo;
      
      std::string  m_caloCellsOutputName;
  };

}

#endif //> !ISF_FASTCALOSIMSVCV2_H
