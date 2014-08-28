/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeuralPhoton_h
#define NeuralPhoton_h

#include <map>
#include <string>

#include "TSystem.h"

namespace TMVA{
  class Reader;
  class IMethod;
}

const float NP_eta[] = {0.0, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37};

class NeuralPhoton {

  public:

    NeuralPhoton(std::string tmvaFile="tmva", int netNumber=0, int cutNumber=0);

    ~NeuralPhoton();

    std::string GetNetworkIdentifier(float eta, float pt, int convFlag);
    float GetNetworkResponse(float eta, float pt, 
			     int convFlag, 
			     float rhadX, float rhad1X, 
			     float retaX, float weta2X, float rphiX, 
			     float w1X, float wtotX, float deltaeX, 
			     float eratioX, float fsideX, 
			     float f1X, float e277X);

    void SetPhotonVars(float eta, 
		       float rhadX, float rhad1X, 
		       float retaX, float weta2X, float rphiX, 
		       float w1X, float wtotX, float deltaeX, 
		       float eratioX, float fsideX, 
		       float f1X, float e277X);

    float GetNetworkDiscriminantCut(float eta, float pt, int convFlag);

    bool GetPassNetwork(float eta, float pt, 
			int convFlag, 
			float rhadX, float rhad1X, 
			float retaX, float weta2X, float rphiX, 
			float w1X, float wtotX, float deltaeX, 
			float eratioX, float fsideX, 
			float f1X, float e277X,
			float* netResponse=0);

    void LoadCutsFromFile(int cs = 0);

  private:

    std::string tmvaFile;
    std::string netName;

    std::vector<float> NP_pt;

    // Dummy 'spectator' vars.
    float etaSp, ptSp;
    int convFlagSp, tightSp;

    // All the REAL variables.
    Float_t fside, w1, wtot, deltae, eratio, rphi, reta, weta2, f1, rhad, e277;

    TMVA::Reader* reader;

    unsigned int cutSel, netSel, m_Nconv;

    float GetNetworkResponse(float eta, float pt, int convFlag);
    bool GetPassNetwork(float eta, float pt, int convFlag, float* netResponse=0);

    void SelectNetwork();
    void LoadPtsFromFile();
    std::map<std::string, bool> LoadVariablesFromFile();
    float disc_constr[8][6][3];

    std::vector<TMVA::IMethod*> m_bookedMethods;

};

#endif
