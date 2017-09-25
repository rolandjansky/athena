/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HFOR_HFOR_Truth_H
#define HFOR_HFOR_Truth_H

#include <vector>
#include <map>
#include <string>

// Framework include(s):
#include "AsgTools/AsgTool.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthEventAuxContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

enum HFORType{
  isBB = 0, isCC = 1, isC = 2, isLight = 3, kill = 4, noType = 5
};

class HFOR_Truth
{
 public:
  HFOR_Truth ();

  //This method will navigate through the MC container, finds final state quarks
  //and finds if overlaps are present.
  HFORType findOverlap( const xAOD::TruthEventContainer &truthEvent,const xAOD::JetContainer* jets =nullptr );

  // This method will determine the type of action (keep or kill the event) using
  // b and c truth jets.
  HFORType jetBasedRemoval( const xAOD::JetContainer* jets );
                              
  void        finalize() ;

  //Sets the sample type based on the sample DSID
  void        setSampleType(unsigned int runNumber) ;

  //Helper to get back the current sample type
  HFORType    getSampleType() ;

  //Helper to get back the name of the current sample (light, bb, cc, c or unknown)
  std::string getSampleName() ;

  //Calling this tells the method we want to do jet based removal
  void        setJetBasedHFOR(){m_jetBased_HFOR = true; m_angularBased_HFOR = false; } ;

  //Calling this tells the method we want to do angular based removal
  void        setAngularBasedHFOR(){m_jetBased_HFOR = false; m_angularBased_HFOR = true; } ;

  //Set the size of the cone to be used in the heavy quark classification
  void        setMatchConeSize(double deltaR) ;

  //Returns the current size of the cone to be used in the heavy quark classification
  double      getMatchConeSize() ;

  //Reads a file written according to BOOST property tree with the DSID and type
  int readRunConfig(std::string runConfigFile) ;

  //Bookkeeper accessors
  unsigned long int getNquarks(std::string process, int flavor) ;
  unsigned long int getNclass(HFORType) ;

  std::map<std::string, std::map<int, std::vector<float> > > getdR() ;

 private :
  bool m_debug;
  double m_jetBasedHFOR_pT_min;
  double m_jetBasedHFOR_eta_max;

  bool m_angularBased_HFOR;
  bool m_jetBased_HFOR;

  double       m_matchCone ;
  std::string  m_sampleName ;
  HFORType     m_sampleType ;
  std::map < HFORType, std::string> m_sampleNameMap ;

  std::string m_runConfigFile ;

  std::map < HFORType, std::vector<unsigned int> > m_runConfigMap ;

  std::map< std::string, std::map <int, std::vector<const xAOD::TruthParticle* > > >  m_qq ;


  //Helper to check the parent type of the b or c quark
  std::map<std::string, bool> checkAncestor(const xAOD::TruthParticle* ancestor, const xAOD::TruthParticle* bcQuark) ;

  // Here we will take the final state quarks found in this event and classify
  // them as from Matrix Element, Gluon Spliting PDF or MPI.
  bool findHFQuarks(const std::map <int, std::vector<const xAOD::TruthParticle* > > fsQuarksMap) ;

  //Based on the angle between the 2 b or c quarks and it's classification as from
  //ME or GS the event will be flagged to be killed or kept
  HFORType angularBasedRemoval( void ) ;

  //Helper to return if 2 particles are same flavor
  bool is_sameFlavor(const xAOD::TruthParticle* x, const xAOD::TruthParticle* y) ;

  //Helper to return if a given particle is final state particle
  bool is_FinalState(const xAOD::TruthParticle* bcQuark) ;

  //Helper to return if a particle is a b quark
  bool is_b(const xAOD::TruthParticle* particle) ;

  //Helper to return if a particle is a c quark
  bool is_c(const xAOD::TruthParticle* particle) ;

  //Helper to return if a particle is a hadron
  bool is_bHadron(const xAOD::TruthParticle* x) ;

  //Helper to return if a particle is a proton
  bool is_proton(const xAOD::TruthParticle* x) ;

  //Book-keepers for what the tool is doing
  //Create a map to hold the number of quarks identifyed
  std::map<std::string, std::map<int, unsigned long int > > m_Nquarks ;
  std::map<HFORType, unsigned long int> m_Nclass ;
  std::map<std::string, std::map<int, std::vector<float> > > m_dR ;

};


#endif

