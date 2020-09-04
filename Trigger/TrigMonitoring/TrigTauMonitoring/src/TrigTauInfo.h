/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// Trigger Information struct
typedef struct _trigtauinfo
{
  std::string trigName; //Trigger Name
  std::string trigWP; //Trigger ID working point
  std::string trigL1Item; //L1 item for HLT
  std::string trigL1Type; //VHI
  std::string trigType; //Chain Type
  bool isL1; // Level1 Trigger
  bool isRNN; // is RNN chain
  bool isBDT; // is BDT chain
  bool isPerf; // Performance chain
  float HLTthr; // HLT Et threshold
  float L1thr; // L1 Et threshold
  bool trigIsEmulation;
} TrigInfo;
