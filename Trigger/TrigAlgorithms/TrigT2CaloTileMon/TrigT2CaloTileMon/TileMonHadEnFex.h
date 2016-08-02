/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     TileMonHadEnFex.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon
 
 AUTHOR:   D.O.Damazio
 
 PURPOSE:  Based on Cluster position defined in the second
           EM Calorimeter layer, calculates total Had cluster
           energy.
 *******************************************************************/

#ifndef TRIGT2CALOTILEMON_TILEMONHADENFEX_H 
#define TRIGT2CALOTILEMON_TILEMONHADENFEX_H

#include "TrigT2CaloCommon/IAlgToolCalo.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TH1.h"
#include "TH2.h"


/** Feature extraction Tool for LVL2 Calo. Hadronic EndCaps
	and Tile Calorimeter. All Samples */
class TileMonHadEnFex: public IAlgToolCalo, virtual public IIncidentListener
{
   public:
      TileMonHadEnFex(const std::string & type, const std::string & name,
                  const IInterface* parent);
      virtual ~TileMonHadEnFex();
      using IAlgToolCalo::execute;
      /** @brief execute feature extraction for the EM Calorimeter
      *   second layer
      *   @param[out] rtrigEmCluster is the output cluster.
      *   @param[in] eta/phi-min/max = RoI definition.
      */
      StatusCode execute(TrigEMCluster &rtrigEmCluster,double etamin,
            double etamax, double phimin, double phimax);
      StatusCode initialize();
      StatusCode finalize();
      TH1*** hists(){ return m_hists; }
      /** handle to init a new event */
      void handle (const Incident& );
      double logEnergy(double energy);
//       void BinLogY(TH2* h);

   private:
      ServiceHandle<ITHistSvc> m_thistSvc;
      float m_THR_1;
      float m_THR_2;
      float m_end_detec_eta;
      float m_end_detec_phi;
      float m_eta_division;
      float m_phi_division;
      float m_energyCell_Thr;

      //Monitoring
      TH1*** m_hists;
      //TH1*** m_hist_TB0; //TileBar0
      //TH1*** m_hist_TB1; //TileBar1
      //TH1*** m_hist_TB2; //TileBar2
      TH2F** m_2dhists;
      TH2F** m_2dhists_TB0; //TileBar0
      TH2F** m_2dhists_TB1; //TileBar1
      TH2F** m_2dhists_TB2; //TileBar2
      //TH2* m_mainHist;
      std::string m_histPath;
      bool m_configured;

};


#endif
