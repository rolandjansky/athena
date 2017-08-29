/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Tool to decorate the EventInfo object with truth categories informations 
// Authors: T. Guillemin, J. Lacey, D. Gillberg
	
#ifndef DerivationFrameworkHiggs_TruthCategoriesDecorator_H
#define DerivationFrameworkHiggs_TruthCategoriesDecorator_H

#include <string>
#include <vector>
#include <TEnv.h>
#include <TString.h>
#include <TSystem.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "xAODTruth/TruthParticleContainer.h"

class IHiggsTruthCategoryTool;
class IxAODtoHepMCTool;

class TLorentzVector;

namespace DerivationFramework {
  
  class TruthCategoriesDecorator : public AthAlgTool, public IAugmentationTool {
        
  public:
    TruthCategoriesDecorator(const std::string& t, const std::string& n, const IInterface* p);
    ~TruthCategoriesDecorator();
    StatusCode initialize();
    StatusCode finalize();
    virtual StatusCode addBranches() const;

  private:
    ToolHandle<IxAODtoHepMCTool> m_xAODtoHepMCTool; 
    ToolHandle<IHiggsTruthCategoryTool> m_higgsTruthCatTool;    

    // Path to TEnv file containing MC-channel-numbre <-> HiggsProdMode map
    TEnv *m_config;
    std::string m_configPath;

    // Detail level. Steers amount of decoration.
    //  0: basic information. Categoization ints, Higgs prod mode, Njets, Higgs pT
    //  1: the above + Higgs boson 4-vec + associated V 4-vec
    //  2: the above + truth jets built excluding the Higgs boson decay
    //  3: the above + 4-vector sum of all decay products from Higgs boson and V-boson
    int m_detailLevel;
    
    // methods to locate the TEnv input file
    bool fileExists(TString fileName) { return gSystem->AccessPathName(fileName) == false; }

    // Converts a string to a vector of integers
    std::vector<int> vectorize(TString string, TString sep=" ") const;
    std::string findConfigFile();

    // Method to access the production mode for a given MC channel number
    HTXS::HiggsProdMode getHiggsProductionMode(uint32_t mc_channel_number,HTXS::tH_type &th_type) const;
 
    // Methods for decoration of four vectors
    void decorateFourVec  ( const xAOD::EventInfo *eventInfo, TString prefix, const TLorentzVector p4 ) const;
    void decorateFourVecs ( const xAOD::EventInfo *eventInfo, TString prefix, const std::vector<TLorentzVector> p4s ) const;


  }; /// class
  
} /// namespace

#endif
