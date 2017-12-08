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
#include "ISF_FastCaloSimParametrization/CaloGeometryFromCaloDDM.h"
#include "ISF_FastCaloSimParametrization/IFastCaloSimCaloExtrapolation.h"
#include "TrkExInterfaces/ITimedExtrapolator.h" 

#include "CaloInterface/ICaloCellMakerTool.h"
#include "ISF_FastCaloSimEvent/IntArray.h"

#include "TH2D.h"

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
class CaloGeometryFromFile;


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
      
      int hitCalc(double energy, int calolayer,int pcabin, int pdgid);
      double findHitEta(const double delta_eta_mm,const double R,const double eta_particle);
      inline double transformationEtaToMM(const double R,const double deltaEta, const double sech)
      {
         return R*sech*deltaEta;
      }
      inline double transformationEtaToMMFirstDerivative(const double R,const double deltaEta, const double sech,const double tanh)
      {
         return R*sech*(1.-(deltaEta)*tanh);
      }
      inline double transformationEtaToMMSecondDerivative(const double R,const double deltaEta, const double sech,const double tanh)
      {
         return R*sech*((deltaEta)*(1-2.*sech*sech) - 2.*tanh);
      }      
      void findEtaMMRange(const double R,const double eta_particle,double & etaMin,double & etaMax);
      double findStartingPoint(const double R,const double eta_particle,bool forMaximum);
      
      void LoopOverHits(const double totalEnergy, double minR = 0, double maxR = 0);
      void TestCell();
      
      //** Calculate the wiggle for the hit-to-cell assignment on layers with accordion shape **//
      double doWiggle(int layer);
      
      AtlasDetectorID* detID;
      LArEM_ID* larID;
      TileID*   tileID;
      LArHEC_ID* hecID;
      LArFCAL_ID* fcalID;
      
      int cellcheck;
      
      /** FCSParam file **/
      TFile* m_paramsFile;
      TFile* m_photonFile;
      TFile* m_elFile;
      TFile* m_pionFile;
      TFile* m_paramsFile_photons;
      
      std::string m_paramsFilename;
      
      //TFCSPCAEnergyParametrization    m_epara;
      
      std::vector<TFCSPCAEnergyParametrization*>  m_eparas;
      
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsSetup ;
      ToolHandleArray<ICaloCellMakerTool> m_caloCellMakerToolsRelease ;

      ToolHandle<IFastCaloSimCaloExtrapolation> m_FastCaloSimCaloExtrapolation;
      ToolHandle<Trk::ITimedExtrapolator> m_extrapolator;
      
      CaloCellContainer *       m_theContainer;
      Amg::Vector3D particle_position;
      TH2F* m_histEnergyDensity2D;
      
      ServiceHandle<IAtRndmGenSvc>    m_rndGenSvc;
      CLHEP::HepRandomEngine*         m_randomEngine;
      std::string                     m_randomEngineName;
      
      CaloGeometryFromCaloDDM* m_caloGeo;
      
      std::string  m_caloCellsOutputName;

      std::vector<double> m_rlayer;

      /** enable/disable random fluctuations in calorimeter layers */
      bool m_doRandomFluctuations;
      bool m_useFastCaloSimCaloExtrapolation;
      
      // Settings for new shape parametrisation
      bool m_useOneDShapeParametrisation;
      int m_nHits;
      
      double eta;
      double phi;
      double eta_isfp;
      double phi_isfp;
      double R;
      
      IntArray *m_layers;
      double r_layer;
      double z_particle;
      int layer;
      double layerE;

      double deltaEtaMMmin;
      double deltaEtaMMmax;

      double esum;
      int nCells;
      int nCells_Tile;
      
      TFCSSimulationState simulstate;
      // iterator over layers
      int ilayer;

      //** Array for the hit-to-cell assignment accordion structure fix (wiggle)  **//
      //** To be moved to the conditions database at some point **// 
      double m_wiggleLayer1[50];
      double m_wiggleLayer2[50];
      double m_wiggleLayer3[50];

  };

}

#endif //> !ISF_FASTCALOSIMSVCV2_H
