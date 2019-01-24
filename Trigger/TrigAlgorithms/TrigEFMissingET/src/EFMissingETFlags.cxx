/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EFMissingETFlags.cxx
PACKAGE:  Trigger/TrigAlgorithms/TrigEFMissingET

AUTHORS:  Diego Casadei
CREATED:  May 15, 2008

PURPOSE:  Performs checks with global and auxiliary information
and updates the global event status flag

 ********************************************************************/

#include "TrigEFMissingET/EFMissingETFlags.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <cmath>
using namespace std;


EFMissingETFlags::EFMissingETFlags(const std::string& type,
    const std::string& name,
    const IInterface* parent):
  EFMissingETBaseTool(type, name, parent)
{
  /// NB: Trivial limits are set here.
  ///     Look at TrigEFMissingETConfig.py for actual initialization
  declareProperty("MaxEMfraction", m_MaxEMfraction = 1.0, "max reasonable energy ratio in EM samplings");
  declareProperty("MinEMfraction", m_MinEMfraction = 0.0, "min reasonable energy ratio in EM samplings");

  declareProperty("MaxTileGapEratio", m_MaxTileGapEratio = 1.0, "max energy in TileGap scint. compared to global SumE");

  declareProperty("MaxSumEratioInEMB", m_MaxSumEratioInEMB = 1.0, "max reasonable component energy ratio in EMB");
  declareProperty("MaxSumEratioInEME", m_MaxSumEratioInEME = 1.0, "max reasonable component energy ratio in EME");
  declareProperty("MaxSumEratioInHEC", m_MaxSumEratioInHEC = 1.0, "max reasonable component energy ratio in HEC");
  declareProperty("MaxSumEratioInTileBar", m_MaxSumEratioInTileBar = 1.0, "max reasonable component energy ratio in Tile Barrel");
  declareProperty("MaxSumEratioInTileGap", m_MaxSumEratioInTileGap = 1.0, "max reasonable component energy ratio in TileGap scintillators");
  declareProperty("MaxSumEratioInTileExt", m_MaxSumEratioInTileExt = 1.0, "max reasonable component energy ratio in Tile Extended-Barrel");
  declareProperty("MaxSumEratioInFCal", m_MaxSumEratioInFCal = 1.0, "max reasonable component energy ratio in FCal");

  declareProperty("MinSumEratioInEMB", m_MinSumEratioInEMB = 0.0, "min reasonable component energy ratio in EMB");
  declareProperty("MinSumEratioInEME", m_MinSumEratioInEME = 0.0, "min reasonable component energy ratio in EME");
  declareProperty("MinSumEratioInHEC", m_MinSumEratioInHEC = 0.0, "min reasonable component energy ratio in HEC");
  declareProperty("MinSumEratioInTileBar", m_MinSumEratioInTileBar = 0.0, "min reasonable component energy ratio in Tile Barrel");
  declareProperty("MinSumEratioInTileGap", m_MinSumEratioInTileGap = 0.0, "min reasonable component energy ratio in TileGap scintillators");
  declareProperty("MinSumEratioInTileExt", m_MinSumEratioInTileExt = 0.0, "min reasonable component energy ratio in Tile Extended-Barrel");
  declareProperty("MinSumEratioInFCal", m_MinSumEratioInFCal = 0.0, "min reasonable component energy ratio in FCal");

  declareProperty("GlobMaxNoisyEnergyRatio", m_GlobMaxNoisyEnergyRatio = 0.9, "max reasonable BadSET/SET");

  declareProperty("GlobMaxMEtSumEtRatio", m_GlobMaxMEtSumEtRatio = 0.7, "max reasonable |MET/SumET|");

  declareProperty("MaxCompE", m_MaxCompE, "(vector) max reasonable component energy");
  declareProperty("MinCompE", m_MinCompE, "(vector) min reasonable component energy");

  declareProperty("MaxCellE", m_MaxCellE, "(vector) max reasonable cell energy for each component");
  declareProperty("MinCellE", m_MinCellE, "(vector) min reasonable cell energy for each component");

  declareProperty("MaxCellTime", m_MaxCellTime, "(vector) max reasonable cell time for each component");
  declareProperty("MinCellTime", m_MinCellTime, "(vector) min reasonable cell time for each component");

  declareProperty("CompMaxNoisyEnergyRatio", m_CompMaxNoisyEnergyRatio, "(vector) max reasonable BadSET/SET for each component");

  declareProperty("WorstCellQuality", m_WorstCellQlty, "(vector) max reasonable CaloCell::quality() for each component");

  declareProperty("CompMaxMEtSumEtRatio", m_CompMaxMEtSumEtRatio, "(vector) max reasonable |MET/SumET| for each component");

  unsigned char N=24; // number of calo samplings
  m_MaxCompE.reserve(N);
  m_MinCompE.reserve(N);
  m_MaxCellE.reserve(N);
  m_MinCellE.reserve(N);
  m_MaxCellTime.reserve(N);
  m_CompMaxNoisyEnergyRatio.reserve(N);
  m_MinCellTime.reserve(N);
  m_WorstCellQlty.reserve(N);
  m_CompMaxMEtSumEtRatio.reserve(N);
  for (int i=0; i<N; ++i) {
    m_MaxCompE.push_back(+1e9);
    m_MinCompE.push_back(-1e9);
    m_MaxCellE.push_back(+1e9);
    m_MinCellE.push_back(-1e9);
    m_MaxCellTime.push_back(+1e9);
    m_MinCellTime.push_back(-1e9);
    m_CompMaxNoisyEnergyRatio.push_back(0.9);
    m_WorstCellQlty.push_back(1e9);
    m_CompMaxMEtSumEtRatio.push_back(0.7);
  }

  m_hCompFlags=nullptr;

  m_fextype = FexType::OTHER;

}



EFMissingETFlags::~EFMissingETFlags()
{
}



StatusCode EFMissingETFlags::initialize()
{

  ATH_MSG_DEBUG( "called EFMissingETFlags::initialize()" );

  /// timers
  if( service( "TrigTimerSvc", m_timersvc).isFailure() ) {
    ATH_MSG_WARNING( name() << ": Unable to locate TrigTimer Service" );
  }
  if (m_timersvc) {
    // global time
    std::string basename=name()+".TotalTime";
    m_glob_timer = m_timersvc->addItem(basename);
  } // if timing service

  return StatusCode::SUCCESS;
}



StatusCode EFMissingETFlags::start()
{
// get histogramming service
  ITHistSvc *rootHistSvc;
  if (service("THistSvc", rootHistSvc).isFailure()) {
    ATH_MSG_WARNING( "ERROR: unable to locate THistSvc. No histogram for component status bits!" );
    return StatusCode::SUCCESS;
  }

 const unsigned int comp = 25;
 const unsigned int bits = 16;
 std::string Title[comp] = { // see EFMissingET
   "PreSamplB", "EMB1", "EMB2", "EMB3",   // LAr barrel
   "PreSamplE", "EME1", "EME2", "EME3",   // LAr EM endcap
   "HEC0",      "HEC1", "HEC2", "HEC3",   // Hadronic end cap cal.
   "TileBar0", "TileBar1", "TileBar2",    // Tile barrel
   "TileGap1", "TileGap2", "TileGap3",    // Tile gap (ITC & scint)
   "TileExt0", "TileExt1", "TileExt2",    // Tile extended barrel
   "FCalEM",   "FCalHad2", "FCalHad3",    // Forward cal endcap
   "Muons"                                // Muons
 };
 std::string bitName[bits] = { // see EFMissingETBaseTool
   "Processing",         // bit  0
   "ErrBSconv",          // bit  1
   "ErrMuon",            // bit  2
   "ErrFEB",             // bit  3
   "Skipped",            // bit  4
   "CompBigMEtSEtRatio", // bit  5
   "BadCompEnergy",      // bit  6
   "BadEnergyRatio",     // bit  7
   "spare",              // bit  8
   "BadCellQuality",     // bit  9
   "BadCellEnergy",      // bit 10
   "BadCellTime",        // bit 11
   "NoMuonTrack",        // bit 12
   "spare",              // bit 13
   "Processed",          // bit 14
   "CompError"           // bit 15
 };

// create histo
  m_hCompFlags = new TH2F("compStatus","Status bits VS component",comp,-0.5,comp-0.5,bits,-0.5,bits-0.5);
  for (unsigned int b=0; b<comp; ++b) m_hCompFlags->GetXaxis()->SetBinLabel(b+1, Title[b].c_str());
  for (unsigned int b=0; b<bits; ++b) m_hCompFlags->GetYaxis()->SetBinLabel(b+1, bitName[b].c_str());

  std::string histoName = "/EXPERT/";
  histoName += m_FexName;
  histoName += "/compStatus";

// register histo
  if ( rootHistSvc->regHist(histoName,m_hCompFlags).isFailure() ) {
    ATH_MSG_WARNING( "ERROR: can not book histo" << histoName << "  No histogram for component status bits!" );
  }


  return StatusCode::SUCCESS;
}



StatusCode EFMissingETFlags::finalize()
{
  return StatusCode::SUCCESS;
}



StatusCode EFMissingETFlags::execute()
{
  return StatusCode::SUCCESS;
}

StatusCode EFMissingETFlags::execute(xAOD::TrigMissingET *met ,
    TrigEFMissingEtHelper *metHelper ,
    const xAOD::CaloClusterContainer * /* caloCluster */ , const xAOD::JetContainer  * /* jets */,
                                        const xAOD::TrackParticleContainer * /*trackContainer*/,
                                        const xAOD::VertexContainer * /*vertexContainer*/,
                                        const xAOD::MuonContainer * /*muonContainer*/ )
{
  ATH_MSG_DEBUG( "EFMissingETFlags::execute() called" );

  if (met==0 || metHelper==0) {
     ATH_MSG_FATAL( "Null pointers as input!  Aborting" );
    return StatusCode::FAILURE;
  }

  if(m_timersvc)
    m_glob_timer->start(); // total time

/// global flag (bit meaning defined in EFMissingETBaseTool ctor)
  int flag=metHelper->GetStatus();

  unsigned char elem = metHelper->GetElements();

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


  if (elem == 42) { // compute subdetector energies

     for (unsigned char i=0; i<elem-18; ++i) { // EMB
      TrigEFMissingEtComponent* metComp = metHelper->GetComponent(i);
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
      if (i<4) { // EMB
      	if (Name.substr(0,3)=="EMB" || Name=="PreSamplB" ) {
      	  EM_SumE += sumE;
      	  EMB_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find EMB!  Skipping check" );
      	  break;
      	}
      } else if (i<8) { // EME
      	if (Name.substr(0,3)=="EME" || Name=="PreSamplE" ) {
      	  EM_SumE += sumE;
      	  EME_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find EME!  Skipping check" );
      	  break;
      	}
      } else if (i<12) { // HEC
      	if (Name.substr(0,3)=="HEC") {
      	  HEC_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find HEC!  Skipping check" );
      	  break;
      	}
      } else if (i<15) { // TileBar
      	if (Name.substr(0,7)=="TileBar") {
      	  TileBar_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find TileBar!  Skipping check" );
      	  break;
      	}
      } else if (i<18) { // TileGap
      	if (Name.substr(0,7)=="TileGap") {
      	  TileGap_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find TileGap!  Skipping check" );
      	break;
      	}
      } else if (i<21) { // TileExt
      	if (Name.substr(0,7)=="TileExt") {
      	  TileExt_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find TileExt!  Skipping check" );
      	  break;
      	}
      } else { // FCal
        // note: i must be < 28 because i < elem-18 = 42-14 = 28
      	if (Name.substr(0,4)=="FCal") {
      	  if (Name == "FCalEM") EM_SumE += sumE;
      	  FCal_SumE += sumE;
      	} else {
          ATH_MSG_WARNING( "Cannot find FCal!  Skipping check" );
      	  break;
      	}
      }  // end loop over i

     }  //end elem == 42

  } else {
    ATH_MSG_WARNING( "Found " << elem << " (!=42) auxiliary components.  Skipping checks!" );
  }

  /// main loop over components ///

  for (unsigned char i=0; i<elem; ++i) {
    TrigEFMissingEtComponent* metComp = metHelper->GetComponent(i);
    string Name=metComp->m_name;
    float c0 =         metComp->m_calib0;
    float c1 =         metComp->m_calib1;
    short sumOfSigns = metComp->m_sumOfSigns;
    float MEx   = sumOfSigns * c0 + c1 *  metComp->m_ex;
    float MEy   = sumOfSigns * c0 + c1 *  metComp->m_ey;
    float sumEt = sumOfSigns * c0 + c1 *  metComp->m_sumEt;
    float sumE  = sumOfSigns * c0 + c1 *  metComp->m_sumE;
    float MET = sqrtf(MEx*MEx+MEy*MEy);
    float sumBadEt =   sumOfSigns * c0 + c1 *  metComp->m_sumBadEt;

    // flag component if |MET/SumET| is too large
    if (elem==42 && i<24 && sumEt>0 && fabsf(MET/sumEt)>m_CompMaxMEtSumEtRatio[i]) {
      metComp->m_status |= m_maskCompBigMEtSEtRatio;
    }

    // flag component if its energy is too large (positive or negative)
    if (elem==42 && i<24 && (sumE<m_MinCompE[i] || sumE>m_MaxCompE[i])) {
      metComp->m_status |= m_maskBadCompEnergy;
    }

    // flag component if cell energy is outside the allowed boundaries
    if (elem==42 && i<24 && (metComp->m_minE<m_MinCellE[i] || metComp->m_maxE>m_MaxCellE[i])){
      metComp->m_status |= m_maskBadCellEnergy;
    }

    // flag component if cell time is outside the allowed boundaries
    if (elem==42 && i<24 &&
       (metComp->m_minTime<m_MinCellTime[i] || metComp->m_maxTime>m_MaxCellTime[i])){
      metComp->m_status |= m_maskBadCellTime;
    }

    // flag component if cell quality is poor
    if (elem==42 && i<24 && metComp->m_maxQlty>m_WorstCellQlty[i]){
      metComp->m_status |= m_maskBadCellQuality;
    }

    //flag component if noiy energy ratio is too large
    if (elem==42 && i<24 && sumEt>1e-3 && sumBadEt>1e-3 && (sumBadEt/sumEt)>m_CompMaxNoisyEnergyRatio[i]){
       metComp->m_status |= m_maskNoisyEnergyRatio;
    }

    // flag component if SumE/SumE_subdet is outside the allowed range
    if (i<4) {
      if (EMB_SumE != 0) {
      	float ratio = sumE / EMB_SumE;
      	if (ratio<m_MinSumEratioInEMB || ratio>m_MaxSumEratioInEMB) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<8) {
      if (EME_SumE != 0) {
      	float ratio = sumE / EME_SumE;
      	if (ratio<m_MinSumEratioInEME || ratio>m_MaxSumEratioInEME) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<12) {
      if (HEC_SumE != 0) {
      	float ratio = sumE / HEC_SumE;
      	if (ratio<m_MinSumEratioInHEC || ratio>m_MaxSumEratioInHEC) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<15) {
      if (TileBar_SumE != 0) {
      	float ratio = sumE / TileBar_SumE;
      	if (ratio<m_MinSumEratioInTileBar || ratio>m_MaxSumEratioInTileBar) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<18) {
            if (TileGap_SumE != 0) {
      	float ratio = sumE / TileGap_SumE;
      	if (ratio<m_MinSumEratioInTileGap || ratio>m_MaxSumEratioInTileGap) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<21) {
            if (TileExt_SumE != 0) {
      	float ratio = sumE / TileExt_SumE;
      	if (ratio<m_MinSumEratioInTileExt || ratio>m_MaxSumEratioInTileExt) {
      	  metComp->m_status |= m_maskBadEnergyRatio;
      	}
      }
    } else if (i<24) {
       if (FCal_SumE != 0) {
          float ratio = sumE / FCal_SumE;
          if (ratio<m_MinSumEratioInFCal || ratio>m_MaxSumEratioInFCal) {
             metComp->m_status |= m_maskBadEnergyRatio;
          }
       }

    }


    /** define the lowest 16 bits of global flag as the OR of all
     ** component flags, excluding bits "Processing" and "Processed"
     **/
    unsigned short int exclude = m_maskProcessing | m_maskProcessed;
    flag |= (metComp->m_status & ~exclude);

  }


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


  /// monitor component bits
  if (m_hCompFlags) {
    for (unsigned char c=0; c<elem; ++c) {
      TrigEFMissingEtComponent* metComp = metHelper->GetComponent(c);
      for (int b=0; b<16; ++b) {
      	unsigned short mask = (1<<b);
      	if (metComp->m_status & mask) m_hCompFlags->Fill(c,b);
      }
    }
  }


  if(m_timersvc)
    m_glob_timer->stop(); // total time

  return StatusCode::SUCCESS;
}
