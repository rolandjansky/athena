/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelRIOs_h
#define PixelRIOs_h

#include "CxxUtils/checker_macros.h"

#include <TROOT.h>
#include <TChain.h>
#include <vector>

class string;
class TStopwatch;

namespace PixelCalib{

class PixelChargeInterpolationCalibration;
class PixelChargeInterpolationValidation;


class PixelRIOs {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           PixelEventNumber;
   Int_t           PixelClusNum;
   Int_t           LVL1TriggerType;
   std::vector<float>   *PixClusLocX;
   std::vector<float>   *PixClusLocY;
   std::vector<float>   *PixClusLocXcentroid;
   std::vector<float>   *PixClusLocYcentroid;
   std::vector<float>   *PixClusLocX_LorentzCorrection;
   std::vector<float>   *PixClusGloX;
   std::vector<float>   *PixClusGloY;
   std::vector<float>   *PixClusGloZ;
   std::vector<float>   *PixClusEta;
   std::vector<float>   *PixClusPhi;
   std::vector<float>   *PixECBarrel;
   std::vector<float>   *PixClusLayer;
   std::vector<float>   *PixEtaModule;
   std::vector<float>   *PixPhiModule;
   std::vector<float>   *PixClusGroupsize;
   std::vector<float>   *PixClusRow;
   std::vector<float>   *PixClusCol;
   std::vector<int>     *PixDeltaRow;
   std::vector<int>     *PixDeltaCol;
   std::vector<float>   *PixOmegaPhi;
   std::vector<float>   *PixOmegaEta;
   std::vector<int>     *PixClusToT;
   std::vector<float>   *PixClusCharge;
   std::vector<int>     *PixClusLvl1;
   std::vector<int>     *PixClusGanged;
   std::vector<int>     *PixClusFake;
   std::vector<int>     *PixClusAmbiguous;
   std::vector<std::vector<int> > *PixClusEtaIndex;
   std::vector<std::vector<int> > *PixClusPhiIndex;
   std::vector<std::vector<float> > *PixClusChargeList;
   std::vector<std::vector<int> > *PixClusToTList;
   std::vector<std::vector<int> > *PixClusLVLAList;
   Int_t           PixelHitsNum;
   std::vector<float>   *PixHitXstartpos;
   std::vector<float>   *PixHitYstartpos;
   std::vector<float>   *PixHitZstartpos;
   std::vector<float>   *PixHitXendpos;
   std::vector<float>   *PixHitYendpos;
   std::vector<float>   *PixHitZendpos;
   std::vector<int>     *PixHitBarrelEndcap;
   std::vector<int>     *PixHitLayerDisk;
   std::vector<int>     *PixHitEtaModule;
   std::vector<int>     *PixHitPhiModule;
   std::vector<int>     *PixHitEtaIndex;
   std::vector<int>     *PixHitPhiIndex;
   std::vector<float>   *PixHitAngle;
   std::vector<float>   *PixHitAngle2;
   std::vector<float>   *PixHitAngle_Lorcorr;
   std::vector<float>   *PixHitEnergyLoss;
   std::vector<float>   *PixHitTime;
   std::vector<int>     *PixHitPDGParticle;
   std::vector<float>   *PixHitParticlePt;
   std::vector<int>     *PixHitFlag;
   std::vector<float>   *MC_Xpos;
   std::vector<float>   *MC_Ypos;
   std::vector<float>   *MC_IncidentAngle;
   std::vector<float>   *MC_IncidentAngle2;
   std::vector<float>   *MC_IncidentAngle_Lorcorr;
   std::vector<float>   *MC_EnergyLoss;
   std::vector<float>   *MC_Time;
   std::vector<int>     *MC_PDGParticle;
   std::vector<float>   *MC_ParticlePt;
   std::vector<int>     *MC_Flag;
   std::vector<float>   *PixClusLocXana;
   std::vector<float>   *PixClusLocYana;
   std::vector<float>   *PixClusErrX;
   std::vector<float>   *PixClusErrY;
   std::vector<float>   *NotAssociated_Xpos;
   std::vector<float>   *NotAssociated_Ypos;
   std::vector<int>     *NotAssociated_BarrelEndcap;
   std::vector<int>     *NotAssociated_LayerDisk;
   std::vector<int>     *NotAssociated_EtaModule;
   std::vector<int>     *NotAssociated_PhiModule;
   std::vector<float>   *NotAssociated_IncidentAngle;
   std::vector<float>   *NotAssociated_IncidentAngle2;
   std::vector<float>   *NotAssociated_IncidentAngle_Lorcorr;
   std::vector<float>   *NotAssociated_EnergyLoss;
   std::vector<float>   *NotAssociated_Time;
   std::vector<int>     *NotAssociated_PDGParticle;
   std::vector<float>   *NotAssociated_ParticlePt;
   std::vector<int>     *NotAssociated_Flag;

   // List of branches
   TBranch        *b_event_number;   //!
   TBranch        *b_number_cluster_per_event;   //!
   TBranch        *b_type_of_trigger;   //!
   TBranch        *b_PixClusLocX;   //!
   TBranch        *b_PixClusLocY;   //!
   TBranch        *b_PixClusLocXcentroid;   //!
   TBranch        *b_PixClusLocYcentroid;   //!
   TBranch        *b_PixClusLocX_LorentzCorrection;   //!
   TBranch        *b_PixClusGloX;   //!
   TBranch        *b_PixClusGloY;   //!
   TBranch        *b_PixClusGloZ;   //!
   TBranch        *b_PixClusEta;   //!
   TBranch        *b_PixClusPhi;   //!
   TBranch        *b_PixECBarrel;   //!
   TBranch        *b_PixClusLayer;   //!
   TBranch        *b_PixEtaModule;   //!
   TBranch        *b_PixPhiModule;   //!
   TBranch        *b_PixClusGroupsize;   //!
   TBranch        *b_PixClusRow;   //!
   TBranch        *b_PixClusCol;   //!
   TBranch        *b_PixDeltaRow;   //!
   TBranch        *b_PixDeltaCol;   //!
   TBranch        *b_PixOmegaPhi;   //!
   TBranch        *b_PixOmegaEta;   //!
   TBranch        *b_PixClusToT;   //!
   TBranch        *b_PixClusCharge;   //!
   TBranch        *b_PixClusLvl1;   //!
   TBranch        *b_PixClusGanged;   //!
   TBranch        *b_PixClusFake;   //!
   TBranch        *b_PixClusAmbiguous;   //!
   TBranch        *b_PixClusEtaIndex;   //!
   TBranch        *b_PixClusPhiIndex;   //!
   TBranch        *b_PixClusChargeList;   //!
   TBranch        *b_PixClusToTList;   //!
   TBranch        *b_PixClusLVLAList;   //!
   TBranch        *b_number_hits_per_event;   //!
   TBranch        *b_PixHitXstartpos;   //!
   TBranch        *b_PixHitYstartpos;   //!
   TBranch        *b_PixHitZstartpos;   //!
   TBranch        *b_PixHitXendpos;   //!
   TBranch        *b_PixHitYendpos;   //!
   TBranch        *b_PixHitZendpos;   //!
   TBranch        *b_PixHitBarrelEndcap;   //!
   TBranch        *b_PixHitLayerDisk;   //!
   TBranch        *b_PixHitEtaModule;   //!
   TBranch        *b_PixHitPhiModule;   //!
   TBranch        *b_PixHitEtaIndex;   //!
   TBranch        *b_PixHitPhiIndex;   //!
   TBranch        *b_PixHitAngle;   //!
   TBranch        *b_PixHitAngle2;   //!
   TBranch        *b_PixHitAngle_Lorcorr;   //!
   TBranch        *b_PixHitEnergyLoss;   //!
   TBranch        *b_PixHitTime;   //!
   TBranch        *b_PixHitPDGParticle;   //!
   TBranch        *b_PixHitParticlePt;   //!
   TBranch        *b_PixHitFlag;   //!
   TBranch        *b_MC_Xpos;   //!
   TBranch        *b_MC_Ypos;   //!
   TBranch        *b_MC_IncidentAngle;   //!
   TBranch        *b_MC_IncidentAngle2;   //!
   TBranch        *b_MC_IncidentAngle_Lorcorr;   //!
   TBranch        *b_MC_EnergyLoss;   //!
   TBranch        *b_MC_Time;   //!
   TBranch        *b_MC_PDGParticle;   //!
   TBranch        *b_MC_ParticlePt;   //!
   TBranch        *b_MC_Flag;   //!
   TBranch        *b_PixClusLocXana;   //!
   TBranch        *b_PixClusLocYana;   //!
   TBranch        *b_PixClusErrX;   //!
   TBranch        *b_PixClusErrY;   //!
   TBranch        *b_NotAssociated_Xpos;   //!
   TBranch        *b_NotAssociated_Ypos;   //!
   TBranch        *b_NotAssociated_BarrelEndcap;   //!
   TBranch        *b_NotAssociated_LayerDisk;   //!
   TBranch        *b_NotAssociated_EtaModule;   //!
   TBranch        *b_NotAssociated_PhiModule;   //!
   TBranch        *b_NotAssociated_IncidentAngle;   //!
   TBranch        *b_NotAssociated_IncidentAngle2;   //!
   TBranch        *b_NotAssociated_IncidentAngle_Lorcorr;   //!
   TBranch        *b_NotAssociated_EnergyLoss;   //!
   TBranch        *b_NotAssociated_Time;   //!
   TBranch        *b_NotAssociated_PDGParticle;   //!
   TBranch        *b_NotAssociated_ParticlePt;   //!
   TBranch        *b_NotAssociated_Flag;   //!

   PixelRIOs(const std::string& starting_tag,
             const std::string& input,
             const std::string& final_tag,
             const std::string& collection = "Validation/PixelRIOs");
   virtual ~PixelRIOs();
   virtual void     Iterate(long nentries);
   void ReadHistoFile(const std::string& input);
   void Analyze(const std::string& reference, const std::string& reference_tag);

private:

   PixelRIOs(const PixelRIOs &);
   PixelRIOs &operator=(const PixelRIOs&);

   // My variables
   TStopwatch *m_timer;
   PixelChargeInterpolationCalibration *m_Calibration;
   PixelChargeInterpolationValidation *m_Validation;

   std::string m_StartingTag;
   std::string m_FinalTag;
};

}

#endif
