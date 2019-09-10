/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFlagsMT.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHOR: Gabriel Gallardo
DATE: April 25, 2019

Based on: EFMissingETFlags.cxx
AUTHORS:  Diego Casadei
CREATED:  May 15, 2008

PURPOSE:  Performs checks with global and auxiliary information
and updates the global event status flag

 ********************************************************************/

#include "EFMissingETFlagsMT.h"
#include "TrigEFMissingET/EFMissingETHelper.h"

#include <string>
#include <cmath>
using namespace std;

const std::vector<float> EFMissingETFlagsMT::s_dummyMax   (24, +1e9);
const std::vector<float> EFMissingETFlagsMT::s_dummyMin   (24, -1e9);
const std::vector<float> EFMissingETFlagsMT::s_dummyRatio (24,  0.8);


EFMissingETFlagsMT::EFMissingETFlagsMT(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  base_class(type, name, parent)
{


}

StatusCode EFMissingETFlagsMT::update(xAOD::TrigMissingET* /*met*/,
                            TrigEFMissingEtHelper *metHelper, const EventContext& /*ctx*/) const
{
  ATH_MSG_DEBUG( "EFMissingETFlagsMT::execute() called" );

  if (metHelper==0) {
     ATH_MSG_FATAL( "Null pointers as input!  Aborting" );
    return StatusCode::FAILURE;
  }


  // global flag (bit meaning defined in IMissingETTool)
  int flag=metHelper->GetStatus();

  unsigned char nHelperComp = metHelper->GetElements();

  float caloSumE = 0;
  float caloSumEt = 0;
  float caloSumBadEt = 0;
  float EM_SumE=0;

  float EMB_SumE=0;
  float EME_SumE=0;
  float HEC_SumE=0;
  float TileBar_SumE=0;
  float TileGap_SumE=0;
  float TileExt_SumE=0;
  float FCal_SumE=0;

  // compute subdetector energies
  if (nHelperComp == static_cast<uint>(TrigEFMissingEtComponent::ComponentSize)) { 
    for (uint helper_i=0; helper_i < TrigEFMissingEtComponent::TCLCW ; ++helper_i) { 
      TrigEFMissingEtComponent* metComp = metHelper->GetComponent(helper_i);
      string Name=metComp->m_name;

      float c0 =         metComp->m_calib0;
      float c1 =         metComp->m_calib1;
      short sumOfSigns = metComp->m_sumOfSigns;
      float sumE =       sumOfSigns * c0 + c1 *  metComp->m_sumE;
      float sumEt =      sumOfSigns * c0 + c1 *  metComp->m_sumEt;
      float sumBadEt =   sumOfSigns * c0 + c1 *  metComp->m_sumBadEt;

      caloSumE += sumE;
      caloSumEt += sumEt;
      caloSumBadEt += sumBadEt;

      // compute auxiliary quantities
      switch(static_cast<TrigEFMissingEtComponent::Component>(helper_i))
      {
        case TrigEFMissingEtComponent::PreSamplB:
        case TrigEFMissingEtComponent::EMB1: case TrigEFMissingEtComponent::EMB2: case TrigEFMissingEtComponent::EMB3:
          EM_SumE += sumE;
          EMB_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::PreSamplE:
        case TrigEFMissingEtComponent::EME1: case TrigEFMissingEtComponent::EME2: case TrigEFMissingEtComponent::EME3:
          EM_SumE += sumE;
          EME_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::HEC0:
        case TrigEFMissingEtComponent::HEC1: case TrigEFMissingEtComponent::HEC2: case TrigEFMissingEtComponent::HEC3:
          HEC_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::TileBar0: case TrigEFMissingEtComponent::TileBar1: case TrigEFMissingEtComponent::TileBar2:
          TileBar_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::TileGap1: case TrigEFMissingEtComponent::TileGap2: case TrigEFMissingEtComponent::TileGap3:
          TileGap_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::TileExt0: case TrigEFMissingEtComponent::TileExt1: case TrigEFMissingEtComponent::TileExt2:
          TileExt_SumE += sumE;
          break;
        case TrigEFMissingEtComponent::FCalEM:
          EM_SumE += sumE;
          /* FALLTHROUGH */
        case TrigEFMissingEtComponent::FCalHad1:
        case TrigEFMissingEtComponent::FCalHad2:
          FCal_SumE += sumE;
          break;
        default:
          break;
      }
    }  //end nHelperComponent == ComponentSize
  } else {
    ATH_MSG_WARNING( "Found " << nHelperComp << " (!=" << 
                  static_cast<uint>(TrigEFMissingEtComponent::ComponentSize) << ") auxiliary components.  Skipping checks!" );
  }

  /// main loop over components ///
  for (unsigned char helper_i=0; helper_i<nHelperComp; ++helper_i) {
    TrigEFMissingEtComponent* metComp = metHelper->GetComponent(helper_i);
    float c0 =         metComp->m_calib0;
    float c1 =         metComp->m_calib1;
    short sumOfSigns = metComp->m_sumOfSigns;
    float MEx   = sumOfSigns * c0 + c1 *  metComp->m_ex;
    float MEy   = sumOfSigns * c0 + c1 *  metComp->m_ey;
    float sumEt = sumOfSigns * c0 + c1 *  metComp->m_sumEt;
    float sumE  = sumOfSigns * c0 + c1 *  metComp->m_sumE;
    float MET = sqrtf(MEx*MEx+MEy*MEy);
    float sumBadEt =   sumOfSigns * c0 + c1 *  metComp->m_sumBadEt;

    switch(static_cast<TrigEFMissingEtComponent::Component>(helper_i))
    {
      // All cell components. Count there should be 24. 
      case TrigEFMissingEtComponent::PreSamplB:
      case TrigEFMissingEtComponent::EMB1: case TrigEFMissingEtComponent::EMB2: case TrigEFMissingEtComponent::EMB3:
      case TrigEFMissingEtComponent::PreSamplE:
      case TrigEFMissingEtComponent::EME1: case TrigEFMissingEtComponent::EME2: case TrigEFMissingEtComponent::EME3:
      case TrigEFMissingEtComponent::HEC0:
      case TrigEFMissingEtComponent::HEC1: case TrigEFMissingEtComponent::HEC2: case TrigEFMissingEtComponent::HEC3:
      case TrigEFMissingEtComponent::TileBar0: case TrigEFMissingEtComponent::TileBar1: case TrigEFMissingEtComponent::TileBar2:
      case TrigEFMissingEtComponent::TileGap1: case TrigEFMissingEtComponent::TileGap2: case TrigEFMissingEtComponent::TileGap3:
      case TrigEFMissingEtComponent::TileExt0: case TrigEFMissingEtComponent::TileExt1: case TrigEFMissingEtComponent::TileExt2:
      case TrigEFMissingEtComponent::FCalEM:
      case TrigEFMissingEtComponent::FCalHad1: case TrigEFMissingEtComponent::FCalHad2:
        // flag component if |MET/SumET| is too large
        if (sumEt>0 && fabsf(MET/sumEt)>m_CompMaxMEtSumEtRatio[helper_i]) metComp->m_status |= m_maskCompBigMEtSEtRatio;

        // flag component if its energy is too large (positive or negative)
        if ((sumE<m_MinCompE[helper_i] || sumE>m_MaxCompE[helper_i])) metComp->m_status |= m_maskBadCompEnergy;

        // flag component if cell energy is outside the allowed boundaries
        if ((metComp->m_minE<m_MinCellE[helper_i] || metComp->m_maxE>m_MaxCellE[helper_i])) metComp->m_status |= m_maskBadCellEnergy;

        // flag component if cell time is outside the allowed boundaries
        if ((metComp->m_minTime<m_MinCellTime[helper_i] || metComp->m_maxTime>m_MaxCellTime[helper_i])) metComp->m_status |= m_maskBadCellTime;

        // flag component if cell quality is poor
        if (metComp->m_maxQlty>m_WorstCellQlty[helper_i]) metComp->m_status |= m_maskBadCellQuality;

        //flag component if noiy energy ratio is too large
        if (sumEt>1e-3 && sumBadEt>1e-3 && (sumBadEt/sumEt)>m_CompMaxNoisyEnergyRatio[helper_i]) metComp->m_status |= m_maskNoisyEnergyRatio;

      default:
        continue; // non cell components not needed for the rest of this for(helperComp : metHelper  loop)
    }
    

    // flag component if SumE/SumE_subdet is outside the allowed range
    switch(static_cast<TrigEFMissingEtComponent::Component>(helper_i))
    {
      case TrigEFMissingEtComponent::PreSamplB:
      case TrigEFMissingEtComponent::EMB1: case TrigEFMissingEtComponent::EMB2: case TrigEFMissingEtComponent::EMB3:
        if (EMB_SumE != 0) {
          float ratio = sumE / EMB_SumE;
          if (ratio<m_MinSumEratioInEMB || ratio>m_MaxSumEratioInEMB) metComp->m_status |= m_maskBadEnergyRatio;
        } break;

      case TrigEFMissingEtComponent::PreSamplE:
      case TrigEFMissingEtComponent::EME1: case TrigEFMissingEtComponent::EME2: case TrigEFMissingEtComponent::EME3:
        if (EME_SumE != 0) {
          float ratio = sumE / EME_SumE;
          if (ratio<m_MinSumEratioInEME || ratio>m_MaxSumEratioInEME) metComp->m_status |= m_maskBadEnergyRatio;
        } break;

      case TrigEFMissingEtComponent::HEC0:
      case TrigEFMissingEtComponent::HEC1: case TrigEFMissingEtComponent::HEC2: case TrigEFMissingEtComponent::HEC3:
        if (HEC_SumE != 0) {
          float ratio = sumE / HEC_SumE;
          if (ratio<m_MinSumEratioInHEC || ratio>m_MaxSumEratioInHEC) metComp->m_status |= m_maskBadEnergyRatio;
        } break;

      case TrigEFMissingEtComponent::TileBar0: case TrigEFMissingEtComponent::TileBar1: case TrigEFMissingEtComponent::TileBar2:
        if (TileBar_SumE != 0) {
          float ratio = sumE / TileBar_SumE;
          if (ratio<m_MinSumEratioInTileBar || ratio>m_MaxSumEratioInTileBar) metComp->m_status |= m_maskBadEnergyRatio;
        } break;

      case TrigEFMissingEtComponent::TileGap1: case TrigEFMissingEtComponent::TileGap2: case TrigEFMissingEtComponent::TileGap3:
        if (TileGap_SumE != 0) {
          float ratio = sumE / TileGap_SumE;
          if (ratio<m_MinSumEratioInTileGap || ratio>m_MaxSumEratioInTileGap) metComp->m_status |= m_maskBadEnergyRatio;
        } break;

      case TrigEFMissingEtComponent::TileExt0: case TrigEFMissingEtComponent::TileExt1: case TrigEFMissingEtComponent::TileExt2:
        if (TileExt_SumE != 0) {
          float ratio = sumE / TileExt_SumE;
          if (ratio<m_MinSumEratioInTileExt || ratio>m_MaxSumEratioInTileExt) metComp->m_status |= m_maskBadEnergyRatio;
        } break;
      case TrigEFMissingEtComponent::FCalEM:
      case TrigEFMissingEtComponent::FCalHad1:
      case TrigEFMissingEtComponent::FCalHad2:
        if (FCal_SumE != 0) {
          float ratio = sumE / FCal_SumE;
          if (ratio<m_MinSumEratioInFCal || ratio>m_MaxSumEratioInFCal) metComp->m_status |= m_maskBadEnergyRatio;
        } break;
      default:
        break;
    }

    /** define the lowest 16 bits of global flag as the OR of all
     ** component flags, excluding bits "Processing" and "Processed"
     **/
    unsigned short int exclude = m_maskProcessing | m_maskProcessed;
    flag |= (metComp->m_status & ~exclude);
  } // end for (helperComp : metHelper)

  /// map component error bit into event error bit
  if (flag & m_maskCompErrors) flag |= m_maskGlobErrors;


  /*
  /// set error bit if any subdetector is missing
  if (flag & (m_maskEMB_A_Missing|m_maskEMB_C_Missing
             |m_maskEME_A_Missing|m_maskEME_C_Missing
	     |m_maskHEC_A_Missing|m_maskHEC_C_Missing
	     |m_maskFCAL_A_Missing|m_maskFCAL_C_Missing
	     |m_maskTileB_A_Missing|m_maskTileB_C_Missing
	     |m_maskTileE_A_Missing|m_maskTileE_C_Missing))
    flag |= m_maskGlobErrors;
  */

  /// flag events with anomalous EM fraction
  if (EM_SumE !=0 && caloSumE != 0) {
    float ratio = EM_SumE / caloSumE;
    if (ratio<m_MinEMfraction || ratio>m_MaxEMfraction)
      flag |= m_maskBadEMfraction;
  }


  /// flag events with anomalous TileGap fraction
  if (TileGap_SumE !=0 && caloSumE != 0) {
    float ratio = TileGap_SumE / caloSumE;
    if (ratio>m_MaxTileGapEratio)
      flag |= m_maskObjInCrack;
  }

  /// flag event if |MET/SumET| is too large
  float mex=metHelper->GetEx();
  float mey=metHelper->GetEy();
  float SET=metHelper->GetSumEt();
  float MET=sqrt(mex*mex+mey*mey);

  if (fabsf(SET)>1e-3 && fabsf(MET/SET)>m_GlobMaxMEtSumEtRatio) {
    flag |= m_maskGlobBigMEtSEtRatio;
  }

  if ( caloSumEt>1e-3 && caloSumBadEt>1e-3 && (caloSumBadEt/caloSumEt)>m_GlobMaxNoisyEnergyRatio) {
     flag |= m_maskGlobNoisyEnergyRatio;
  }

  metHelper->SetStatus(flag); // update helper object


  /// TODO: monitor component bits

  return StatusCode::SUCCESS;
}
