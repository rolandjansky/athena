/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoHardware_L1TopoHardware
#define L1TopoHardware_L1TopoHardware

#include <stdint.h>
#include <string>
#include <map>



namespace TCS {

   struct HardwareParam {
      std::string name{""};
      uint32_t    value{0};
      std::string description{""};
      std::string rule{""};
   };

   struct L1TopoHWParameters {
      static const std::map<std::string, HardwareParam>& get();
   };

   // New constants for the Phase-1 Topo system
   const uint32_t eEmInputWidth  = 144; // 24 eFEX modules * 6 TOBs per module
   const uint32_t eTauInputWidth = 144; // 24 eFEX modules * 6 TOBs per module

   const uint32_t jJetInputWidth = 168; // 24 jFEX FPGAs * 7 TOBs per FPGA
   const uint32_t jLargeRJetInputWidth = 24; // 24 jFEX FPGAs * 1 TOB per FPGA
   const uint32_t jTauInputWidth = 144; // 24 jFEX FPGAs * 6 TOBs per FPGA
   const uint32_t jEmInputWidth = 48; // 8 forward jFEX FPGAs * 6 TOBs per FPGA
   const uint32_t jMetInputWidth = 1;

   const uint32_t muonInputWidth = 32;
   // Two fibers from each gFEX pFPGA (A,B,C) to Topo => 6 fibers
   const uint32_t gJetInputWidth = 12; // 6 fibers * 2 TOBs / fiber
   const uint32_t gLargeRJetInputWidth = 6; // 6 fibers * 1 TOB / fiber
   const uint32_t gMetInputWidth = 3;

   const uint32_t eEmOutputWidthSort = 6;
   const uint32_t eTauOutputWidthSort = 6;
   const uint32_t jJetOutputWidthSort = 6;
   const uint32_t jLargeRJetOutputWidthSort = 6;
   const uint32_t jTauOutputWidthSort = 6;
   const uint32_t jEmOutputWidthSort = 6;
   const uint32_t muonOutputWidthSort = 6;
   const uint32_t metOutputWidth = 1;

   const uint32_t eEmOutputWidthSelect = 10;
   const uint32_t eTauOutputWidthSelect = 10;
   const uint32_t jJetOutputWidthSelect = 10;
   const uint32_t jLJetOutputWidthSelect = 10;
   const uint32_t jTauOutputWidthSelect = 10;
   const uint32_t jEmOutputWidthSelect = 10;
   const uint32_t muonOutputWidthSelect = 10;


   // Legacy Topo constants
   const uint32_t InputWidthEM                = 120;
   const uint32_t InputWidthTAU               = 120;
   const uint32_t InputWidthJET               = 64;
   const uint32_t InputWidthMU                = 32;

   const uint32_t InputWidth1stStageSortEM                = 20;
   const uint32_t InputWidth1stStageSortTAU               = 20;
   const uint32_t InputWidth1stStageSortJET               = 16;
   const uint32_t InputWidth1stStageSortMU                = 16;

   const uint32_t InputWidth1stStageSelectEM                = 30;
   const uint32_t InputWidth1stStageSelectTAU               = 30;
   const uint32_t InputWidth1stStageSelectJET               = 16;
   const uint32_t InputWidth1stStageSelectMU                = 16;
		
   const uint32_t OutputWidthSortEM           = 6;
   const uint32_t OutputWidthSelectEM         = 10;
   const uint32_t OutputWidthSortTAU          = 6;
   const uint32_t OutputWidthSelectTAU        = 10;
   const uint32_t OutputWidthSortJET          = 6;
   const uint32_t OutputWidthSelectJET        = 8;
   const uint32_t OutputWidthSortMU           = 6;
   const uint32_t NumberOfDelayedMuons        = 1;
   const uint32_t OutputWidthSelectMU         = 10;
	
   const uint32_t NumberOfAlgorithms          = 30;
   const uint32_t NumberOfSortAlgorithms      = 35;
   const uint32_t NumberOfResultBits          = 31;  // ratio of results to overflow is adjustable (needs some more thoughts, which overflows are useful)
   const uint32_t NumberOfOverflowBits        = 32;  // fixed! do not touch!
   const uint32_t NumberOfOutputBits          = 32;  // there are only 32 lines from a L1Topo-Module to CTP => 16 lines per processor -> TODO: Change to 16! (but now 32 needed for testing)
	
	
   const uint32_t KalmanMETCorrection_numberOfEtaBins    = 8;  // needs to be multiple of 2
   const uint32_t KalmanMETCorrection_numberOfEtBins     = 10; // needs to be multiple of 2
   const uint32_t KalmanMETCorrection_correctionBitWidth = 8;  // assuming format unsigned [0.0000000]

   const uint32_t InputWidthMET                = 1;
   const uint32_t OutputWidthMET               = 1;	
   const uint32_t METEnergyBitWidth           = 16;
   const uint32_t METSqrEnergyBitWidth        = 2 * METEnergyBitWidth;
   const uint32_t Arctan2InputEnergyBitWidth  = 15;	


   const uint32_t CosSinBitWidth              = 9;
   const uint32_t CoshBitWidth                = 11 + 7;
   const uint32_t SechTanhBitWidth            = 8;
	
   const uint32_t DefaultJetSize              = 2;
	
   const uint32_t ClusterEtBitWidth           = 8;
   const uint32_t ClusterIsolBitWidth         = 5;
   const uint32_t ClusterEtaBitWidth          = 6;
   const uint32_t ClusterPhiBitWidth          = 6;
	
   const uint32_t JetEt1BitWidth              = 9;
   const uint32_t JetEt2BitWidth              = 10;
   const uint32_t JetEtaBitWidth              = 5;
   const uint32_t JetPhiBitWidth              = 5;
	
   const uint32_t MuonPtBitWidth              = 2;
   const uint32_t MuonEtaBitWidth             = 3;
   const uint32_t MuonPhiBitWidth             = 3;
	
   const uint32_t GenericEtBitWidth           = 10;
   const uint32_t GenericEtaBitWidth          = 7;
   const uint32_t GenericAbsoluteEtaBitWidth  = GenericEtaBitWidth - 1;
   const uint32_t GenericPhiBitWidth          = 6;
	
   const uint32_t MassSqrBitWidth             = GenericEtBitWidth +  METEnergyBitWidth + 1 + CosSinBitWidth + 1;
   const uint32_t RatioBitWidth               = 8;
   const uint32_t InvariantMassSqrBitWidth    = 2 * GenericEtBitWidth + CoshBitWidth + 1 - 7;
   const uint32_t TransverseMassSqrBitWidth    = 2 * GenericEtBitWidth + CosSinBitWidth + 1 - 7;
   const uint32_t ContratransverseMassSqrBitWidth    = TransverseMassSqrBitWidth ; 
}

#endif
