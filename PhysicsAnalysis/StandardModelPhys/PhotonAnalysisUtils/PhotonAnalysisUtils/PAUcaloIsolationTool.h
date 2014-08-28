/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// PAUcaloIsolationTool.h
///////////////////////////////////////////////////////////////////


#ifndef PHOTONANALYSISUTILS_CALOISOLATIONTOOL_H
#define PHOTONANALYSISUTILS_CALOISOLATIONTOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"

#include "PhotonAnalysisUtils/IPAUcaloIsolationTool.h"
#include "egammaInterfaces/IegammaIsoPtCorrection.h"

#include <string>

#include "egammaEvent/Photon.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egamma.h"

typedef struct{
  double median;
  double sigma;
  unsigned int nJets;
} EnergyDensityResults;


class PAUcaloIsolationTool : public AthAlgTool, virtual public IPAUcaloIsolationTool , virtual public IIncidentListener {
 public:
           
      /** AlgTool like constructor */
      PAUcaloIsolationTool(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~PAUcaloIsolationTool();
       
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** handle for incident service */
      virtual void handle(const Incident& inc) ;

      float EtConeCorrectionPt       (const egamma*, float) const;
      float EtConeCorrectionPt       (const Analysis::Photon*, float) const;
      float EtConeCorrectionPt       (const Analysis::Electron*, float) const;

      float EtConeCorrectionJetAreas (float               , float, int jetsToIgnore=0);
      float EtConeCorrectionJetAreas (const egamma*       , float, int jetsToIgnore=0);
      float EtConeCorrectionJetAreas (const TruthParticle*, float, int jetsToIgnore=0);

      double EnergyDensityMedian     (float               , int jetsToIgnore=0);
      double EnergyDensityMedian     (const egamma*       , int jetsToIgnore=0);
      double EnergyDensityMedian     (const TruthParticle*, int jetsToIgnore=0);

      double EnergyDensitySigma      (float               , int jetsToIgnore=0);
      double EnergyDensitySigma      (const egamma*       , int jetsToIgnore=0);
      double EnergyDensitySigma      (const TruthParticle*, int jetsToIgnore=0);

      int    EnergyDensityNjets      (float               , int jetsToIgnore=0);
      int    EnergyDensityNjets      (const egamma*       , int jetsToIgnore=0);
      int    EnergyDensityNjets      (const TruthParticle*, int jetsToIgnore=0);

 private:

      std::map<int, std::map<int, float> > m_photon_pt_corrections;
      std::map<int, std::map<int, float> > m_electron_pt_corrections;

      std::vector<float> m_eta_bins;
      std::vector<float> m_eta_bins_areaoffset;
      std::vector<float> m_ptDensity;
      std::vector<float> m_sigma;
      std::vector<int>   m_Njets;

      bool m_useOldLeakageCorrections;
      float EtConeCorrectionPt_new    (const egamma*, float)             const;
      float EtConeCorrectionPt_old    (const Analysis::Photon*, float)   const;
      float EtConeCorrectionPt_old    (const Analysis::Electron*, float) const;

      bool m_useOldAreaCorrections;
      StatusCode CalculateEventBackground(int jetsToIgnore=0);
      EnergyDensityResults CalculateEventBackground(int jetsToIgnore, float eta, float eta_range);

      const egamma* m_last_egamma_ptr;
      const TruthParticle* m_last_truthpart_ptr;
      
      int GetEtaBin(double eta, bool area_eta=false) const;

      float m_eta_range;

      std::string m_jetAreaMomentName;

      bool m_do_area_corrections;
      bool m_area_corrections_done;
      int  m_previous_jetsToIgnore;

      std::string m_jet_collection_name;

      ToolHandle<IegammaIsoPtCorrection> m_emCaloIsoPtCorrectionTool;
};


#endif // PHOTONANALYSISUTILS_CALOISOLATIONTOOL_H
