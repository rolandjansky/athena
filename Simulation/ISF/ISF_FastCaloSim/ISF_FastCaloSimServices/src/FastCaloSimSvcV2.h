/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// FastCaloSimSvcV2.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FASTCALOSIMSVCV2_H
#define ISF_FASTCALOSIMSVCV2_H 1

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"

#include "CaloInterface/ICaloCellMakerTool.h"
#include "ISF_FastCaloSimEvent/IntArray.h"

#include "TH2D.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

namespace CLHEP
{
  class HepRandomEngine;
}

//forward declarations
class CaloCellContainer;
class CaloGeometryFromFile;


namespace ISF {
  /** @class FastCaloSimSvcV2

      @author Elmar.Ritsch -at- cern.ch, Geraldine.Conti -at- cern.ch, Flavia.Dias -at- cern.ch
  */
  class FastCaloSimSvcV2 : public BaseSimulationSvc {
  public:
    /** Constructor with parameters */
    FastCaloSimSvcV2(const std::string& name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~FastCaloSimSvcV2();

    /** Athena algorithm's interface methods */
    virtual StatusCode  initialize() override final;
    virtual StatusCode  finalize() override final;

    /** Simulation Call */
    virtual StatusCode simulate(const ISFParticle& isp) override final;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent() override final;

    /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent() override final;

  private:

    int hitCalc(double energy, int calolayer, int pdgid);
    double findHitEta(const double alpha,const double r, const double r_layer,const double z_particle,const double eta_particle);

    //** Calculate the wiggle for the hit-to-cell assignment on layers with accordion shape **//
    double doWiggle(int layer);

    /** FCSParam file **/
    TFile* m_paramsFile;
    TFile* m_energyDensity2D;

    std::string m_paramsFilename;

    //TFCSPCAEnergyParametrization    m_epara;

    std::vector<TFCSPCAEnergyParametrization*>  m_eparas;

    ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup ;
    ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease ;

    CaloCellContainer *       m_theContainer;

    TH2F* m_histEnergyDensity2D;

    ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
    CLHEP::HepRandomEngine*         m_randomEngine;
    std::string                     m_randomEngineName;

    CaloGeometryFromFile* m_caloGeo;

    std::string  m_caloCellsOutputName;

    std::vector<double> m_rlayer;

    /** enable/disable random fluctuations in calorimeter layers */
    bool m_doRandomFluctuations;

    //** Array for the hit-to-cell assignment accordion structure fix (wiggle)  **//
    //** To be moved to the conditions database at some point **//
    static const double m_wiggleLayer1[50];
    static const double m_wiggleLayer2[50];
    static const double m_wiggleLayer3[50];

  };

}

#endif //> !ISF_FASTCALOSIMSVCV2_H
