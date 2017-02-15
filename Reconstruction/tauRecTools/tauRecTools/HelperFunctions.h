/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAURECTOOLS_HELPERFUNCTIONS_H
#define TAURECTOOLS_HELPERFUNCTIONS_H

#include "MVAUtils/BDT.h"

#include <vector>
#include <map>

// ROOT include(s)
#include "TLorentzVector.h"
#include "TString.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

#define TRT_CHECK_BOOL( dec, action )		\
  do {						\
    if (!dec) {					\
      ATH_MSG_ERROR("TauRecTools failed");	\
      return action;				\
    }						\
  } while(0)

namespace tauRecTools
{

  void createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s);
  
  xAOD::TauTrack::TrackFlagType isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag);
  bool sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2);

  //keep track of whether input var is float or int
  struct DummyAccessor {
    SG::AuxElement::ConstAccessor<int>* iacc = 0;
    SG::AuxElement::ConstAccessor<float>* facc = 0;
    float operator()(const xAOD::TauJet& tau) {
      if(facc) return (*facc)(tau);
      else return (int) (*iacc)(tau);
    }
    ~DummyAccessor(){
      if(iacc) delete iacc;
      else delete facc;
    }
    DummyAccessor(const char* name, bool isfloat=true){
      if(isfloat) facc = new SG::AuxElement::ConstAccessor<float>(name);
      else iacc = new SG::AuxElement::ConstAccessor<int>(name);
    }
  private:
    //discourage use of this as an object
    //must implement this properly if you want 
    //DummyAccessor as an object
    DummyAccessor(const DummyAccessor&){
    }
  };

  struct TRTBDT {
    float GetGradBoostMVA();//GradBost
    float GetClassification();//AdaBoost
    float GetResponse();//regression
    MVAUtils::BDT* bdt=0;
    std::map< float*, DummyAccessor* > m_data;
    TRTBDT( const char* weightFile);
    bool init(const char* weightFile);
    ~TRTBDT();
    bool updateVariables(const xAOD::TauJet& tau);
  };

  MVAUtils::BDT* configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile);

  std::vector<TString> parseString(const TString& str, const TString& delim=",");
  std::vector<TString> parseStringMVAUtilsBDT(const TString& str, const TString& delim=",");
}

#endif // not TAUANALYSISTOOLS_HELPERFUNCTIONS_H
