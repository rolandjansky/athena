/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOISOCORRECTION_H
#define CALOISOCORRECTION_H

#include <TString.h>
/*
-----------------------------------------------------------------------
CaloIsoCorrection: computes corrected EtconeXX variables for electrons and photons

authors:   Mike Hance <michael.hance@cern.ch>
           Dominick Olivito <olivito@cern.ch>
modified by (9/10/2013): Quentin Buat <quentin.buat@cern.ch> 
This namespace contains functions for computing corrections for 
    the EtconeXX isolation variables, to account for energy leakage and pileup effects.  
    User functions are defined in this header file,
    while internal methods are defined in CaloIsoCorrection.cxx.
    
The default corrections returned are for rel 17.  Rel 16 corrections can be retrieved by passing
    CaloIsoCorrection::REL16 as the version argument.
    
for more information:
https://twiki.cern.ch/twiki/bin/view/AtlasProtected/CaloIsolationCorrections

-----------------------------------------------------------------------
usage examples:

#include "CaloIsoCorrection.h"

// to get the nPV pileup corrected variables for electrons (similar for photons):
float EtconeXX_npv_corrected =   CaloIsoCorrection::GetNPVCorrectedIsolation(nPV,
                                                                            etaS2,
                                                                            radius,
                                                                            is_mc,
                                                                            Etcone_value,
                                                                            CaloIsoCorrection::ELECTRON,
                                                                            CaloIsoCorrection::REL17);

// to get the error on the nPV pileup corrected variables for electrons (similar for photons):
float EtconeXX_npv_corrected_error =   CaloIsoCorrection::GetNPVCorrectedIsolationError(nPV,
                                                                                        etaS2,
                                                                                        radius,
                                                                                        is_mc,
                                                                                        Etcone_value,
                                                                                        CaloIsoCorrection::ELECTRON,
                                                                                        CaloIsoCorrection::REL17);

// to get the pt corrected isolation variables (similar for electrons):
float EtconeXX_pt_corrected = CaloIsoCorrection::GetPtCorrectedIsolation(energy,
                                                                         etaS2,
                                                                         etaPointing,
                                                                         etaCluster,
                                                                         radius,
                                                                         is_mc,
                                                                         Etcone_value,
                                                                         isConversion,
                                                                         CaloIsoCorrection::PHOTON,
                                                                         CaloIsoCorrection::REL17);
                                                                               
// to get the error on the data-derived corrections (similar for electrons):
float EtconeXX_pt_corrected_error = CaloIsoCorrection::GetPtCorrectedIsolationError(energy,
                                                                                    etaS2,
                                                                                    etaPointing,
                                                                                    etaCluster,
                                                                                    radius,
                                                                                    is_mc,
                                                                                    CaloIsoCorrection::PHOTON,
                                                                                    CaloIsoCorrection::REL17);
                                                                              
-----------------------------------------------------------------------

The input variables are the following.  Variable names in the D3PDs are given in (parantheses)
    and are similar for electrons and photons unless otherwise noted.
    
    - nPV: number of primary vertices (corrections were derived requiring >= 2 tracks/vertex in 2011 data)
    
    - Etcone40: (ph_Etcone40) *** in MeV!
    - Etcone40_ED_corrected: ED corrected version of Etcone40 (ph_Etcone40_ED_corrected) *** in Mev!
    
    - energy: cluster energy (ph_cl_E) *** in MeV!
    - etaS2: eta in the 2nd calo sampling (ph_etas2)
    - etaPointing: pointing eta -> from 1st and 2nd calo samplings (ph_etap)
    - etaCluster: cluster eta (ph_cl_eta)
    - radius: radius of the input EtconeXX variable.  Can be given as, e.g., .20 or 20
    - is_mc: set to true if running on MC
    - Etcone_value: value of uncorrected EtconeXX variable (ph_EtconeXX) *** in MeV!
    - isConversion: photons only: conversion flag (ph_isConv)
    - parttype: ELECTRON or PHOTON, enum defined below
    - version: REL17_2 or REL17 or REL16, enum defined below
    
    *** energies must be in MeV!
    
-----------------------------------------------------------------------
*/

namespace CaloIsoCorrection{

  typedef enum { ELECTRON=0, PHOTON=1 } ParticleType;
  typedef enum { REL16=0, REL17=1, REL17_2=2 } Version;

  // ------------------------------------------------------------
  // --------------------- user methods -------------------------
  // ------------------------------------------------------------


  // ------------------------------------------------------------
  // --------------Set Rel 17.2 pt corrections file -------------
  bool SetPtLeakageCorrectionsFile(TString st);


  // ------------------------------------------------------------
  // ---------------- nPV pileup corrections --------------------
  
  float GetNPVCorrectedIsolation(unsigned int nPV,
                                 float etaS2,
                                 float radius,
                                 bool is_mc,
                                 float Etcone_value,
                                 ParticleType parttype = ELECTRON,
                                 Version ver = REL17_2);
  
  // ------------------------------------------------------------
  // --------- energy density (ED) pileup corrections -----------
  // - equivalent to "EtconeXX_ED_corrected" variables
  
  float GetEDCorrectedIsolation(float Etcone40,
                                float Etcone40_ED_corrected,
                                float radius,
                                float Etcone_value,
                                ParticleType parttype = ELECTRON,
                                Version ver = REL17_2);
  
  // --------------------------------------------------------
  // --------------- pT leakage corrections -----------------
  // - equivalent to "EtconeXX_pt_corrected" variables
  
  float GetPtCorrectedIsolation(float energy,
                                float etaS2,
                                float etaPointing,
                                float etaCluster,
                                float radius,
                                bool is_mc,
                                float Etcone_value,
                                bool isConversion = false,
                                ParticleType parttype = ELECTRON,
                                Version ver = REL17_2);
  
  // --------------------------------------------------------
  // ---------- pT leakage corrections, topoIso -------------
  
  float GetPtCorrectedTopoIsolation(float energy,
                                    float etaS2,
                                    float etaPointing,
                                    float etaCluster,
                                    float radius,
                                    bool is_mc,
                                    float Etcone_value,
                                    bool isConversion = false,
                                    ParticleType parttype = ELECTRON,
                                    Version ver = REL17_2);

  // -------------------------------------------------------------
  // ---------- pT leakage + ED corrections, topoIso -------------
  
  float GetPtEDCorrectedTopoIsolation(float ED_median,
                                      float energy,
                                      float etaS2,
                                      float etaPointing,
                                      float etaCluster,
                                      float radius,
                                      bool is_mc,
                                      float Etcone_value,
                                      bool isConversion = false,
                                      ParticleType parttype = ELECTRON,
                                      Version ver = REL17_2);

  // ----------------------------------------------------------------------------------------
  // ---------- construct topoIso, with pT leakage + ED corrections, rel17 only -------------
  // deprecated for rel 17.2+ 
   
  //JBdV
  float GetTopoIsolation(float energy,
			 float eta,
			 float radius,
			 bool is_mc,
			 float Iso,
			 float Rho,
			 float Eraw,
			 float etaraw,
			 float EtTG,
			 bool isConversion = false,
			 ParticleType parttype = ELECTRON);

  
  // --------------------------------------------------------
  // --------- pT leakage + nPV pileup corrections ----------
  
  float GetPtNPVCorrectedIsolation(unsigned int nPV,
                                   float energy,
                                   float etaS2,
                                   float etaPointing,
                                   float etaCluster,
                                   float radius,
                                   bool is_mc,
                                   float Etcone_value,
                                   bool isConversion = false,
                                   ParticleType parttype = ELECTRON,
                                   Version ver = REL17_2);
  
  // -----------------------------------------------------------------
  // ----- pT leakage + energy density (ED) pileup corrections -------
  // - equivalent to "EtconeXX_corrected" variables
  
  float GetPtEDCorrectedIsolation(float Etcone40,
                                  float Etcone40_ED_corrected,
                                  float energy,
                                  float etaS2,
                                  float etaPointing,
                                  float etaCluster,
                                  float radius,
                                  bool is_mc,
                                  float Etcone_value,
                                  bool isConversion = false,
                                  ParticleType parttype = ELECTRON,
                                  Version ver = REL17_2);
  
  // ---------------------------------------------------------
  // ----------- errors on nPV pileup corrections ------------
  
  // to get the error on the data-derived corrections
  float GetNPVCorrectedIsolationError(unsigned int nPV,
                                      float etaS2,
                                      float radius,
                                      bool is_mc,
                                      ParticleType parttype = ELECTRON,
                                      Version ver = REL17_2);
  
  // ---------------------------------------------------------
  // ----------- errors on pT leakage corrections ------------
  
  // to get the error on the data-derived corrections
  float GetPtCorrectedIsolationError(float energy,
                                     float etaS2,
                                     float etaPointing,
                                     float etaCluster,
                                     float radius,
                                     bool is_mc,
                                     ParticleType parttype = ELECTRON,
                                     Version ver = REL17_2);
  
  // ----------------------------------------------------------------
  // ----------- get median energy density from iso vars ------------
  
  float GetEDMedian(float Etcone40, float Etcone40_ED_corrected);
  
}
#endif
