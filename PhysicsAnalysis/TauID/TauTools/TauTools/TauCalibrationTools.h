/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUCALIBRATIONTOOLS_H
#define TAUCALIBRATIONTOOLS_H

///////////////////////////////////////////////////////
//                                                   //
// Name:   TauTools/TauCalibrationTools.h            //
// Author: Michel Trottier-McDonald                  //
//                                                   //
// Created: June 2009                                //
//                                                   //
// DESCRIPTION:                                      //
//                                                   //
// Provides methods to derive the calibration        //
// factors for taus while running tau reconstruction.//
//                                                   //
///////////////////////////////////////////////////////



//*******************************
// Include statements
//*******************************

#include "TauTools/RecoHandler.h"
#include "TauTools/TruthHandler.h"
#include "TauTools/MatchingTools.h"

#include "TF1.h"
#include "TTree.h"
#include "TLeaf.h"


#include <string>
#include <vector>



//*******************************
// Class Definition
//*******************************

class TauCalibrationTools {

 public:

  TauCalibrationTools();

  virtual ~TauCalibrationTools();


  // Getters
  //------------------------------------

  std::string get_TruthParticleContainerName() {return m_TruthParticleContainerName;}

  std::string get_TauJetContainerName()        {return m_TauJetContainerName;}

  float get_DeltaRMatchCut()                   {return m_DeltaRMatchCut;}

  TTree* get_TTreePointer()                    {return tree;}




  // Setters
  //------------------------------------

  void set_TruthParticleContainerName(const std::string& newName)             {m_TruthParticleContainerName = newName;}

  void set_TauJetContainerName(const std::string& newName)                    {m_TauJetContainerName = newName;}

  void set_TruthParticleContainer(const TruthParticleContainer* newContainer) {truthTauContainer = newContainer;}

  void set_TauJetContainer(const Analysis::TauJetContainer* newContainer)     {recoTauContainer  = newContainer;}

  void set_DeltaRMatchCut(float newCut)                                       {m_DeltaRMatchCut = newCut;}

  void set_TTreePointer(TTree* newtree)                                       {tree = newtree;}




  // Tools provided
  //-------------------------------------

  void create_TreeStructure();

  void fill_Tree();


 private:

  // Tree stuff
  //------------------------------------

  TTree* tree;

  // Data structure of the branch

  struct tauData_t
  {
    float ETratio;
    float ETpull;
    float ET;
    float truthVisET;
    float eta;
    float truthViseta;
    int ntracks;
    int nPi0;
  }; 

  tauData_t tauData;
  


  // Names
  //------------------------------------

  std::string m_TruthParticleContainerName;
  std::string m_TauJetContainerName;


  // Container pointers
  //------------------------------------

  const TruthParticleContainer*    truthTauContainer;

  const Analysis::TauJetContainer* recoTauContainer;


  // Extern Tools
  //------------------------------------

  MatchingTools matchtool;

  TruthHandler truthHandler;

  RecoHandler recoHandler;


  // Parameters
  //------------------------------------

  float m_DeltaRMatchCut;

};




#endif
