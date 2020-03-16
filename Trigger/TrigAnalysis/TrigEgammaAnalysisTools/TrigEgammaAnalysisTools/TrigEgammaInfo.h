/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Trigger Information struct
typedef struct _triginfo
{
    std::string trigName; //Trigger Name
    std::string trigType; //Electron or Photon
    std::string trigL1Item; //L1 item for HLT
    std::string trigL1Type; //VHI
    std::string trigPidType; //Loose, Medium, Tight, etc...
    std::string trigPidDecorator; //Aux decoration
    bool trigL1; // Level1 Trigger
    bool trigPerf; // Performance chain
    bool trigEtcut; // Et cut only chain
    float trigThrHLT; // HLT Et threshold
    float trigThrL1; // L1 Et threshold
    bool trigIsEmulation;
} TrigInfo;

// Enums
enum class TrigEgammaMonLevel {SHIFTER, EXPERT, DETAIL};

// Monte carlo enum
namespace MonteCarlo{
  enum class PDGID {Electron, Photon, ZMother, WMother, EnhancedBias, Unknown};
}

