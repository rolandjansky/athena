/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERLIKELIHOOD_C
#define BTAGTOOL_JETFITTERLIKELIHOOD_C

/******************************************************
    @class JetFitterLikelihood
     Package : JetTagTools 
     Created : March 2007

     DESCRIPTION:

     This class computes the likelihood value for being b-jet, c-jet or uds-jet for 
     a single event. The RooFit toolkit for data modeling is used here.

    @author Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
    @author2 Christian Weiser (christian.weiser AT physik.uni-freiburg.de) (University of Freiburg)
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include <RooSimultaneous.h>
#include <vector>
#include <map>
#include <string>
#include "GaudiKernel/ToolHandle.h"

#include "JetTagTools/IJetFitterClassifierTool.h"

class ITHistSvc;
class TH1;

class RooArgSet;
class RooCategory;
class RooMappedCategory;
class RooAbsCategory;


namespace Analysis {

class CalibrationBroker;

static const InterfaceID IID_JetFitterLikelihood("Analysis::JetFitterLikelihood", 1, 0);

  class IJetFitterTagInfo;

  class JetFitterLikelihood : public AthAlgTool, public IJetFitterClassifierTool {
    
  public:
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_JetFitterLikelihood; };

    JetFitterLikelihood(const std::string& name,
			const std::string& n, const IInterface* p);
    ~JetFitterLikelihood();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    

    void fillLikelihoodValues(IJetFitterTagInfo & myTagInfo,
			      const std::string & jetauthor,
			      const std::string & hypothesis);

    // IJetFitterClassifier interface
    void fillLikelihoodValues(IJetFitterTagInfo & myTagInfo,
			      const std::string & jetauthor,
                              double jetpT,
                              double jeteta,
                              double IP3dlike=-5000);


    void fillLikelihoodValues(xAOD::BTagging* BTag,
			      const std::string & jetauthor,
			      const std::string & hypothesis);

    // IJetFitterClassifier interface
    void fillLikelihoodValues(xAOD::BTagging* BTag,
			      const std::string & jetauthor,
                              double jetpT,
                              double jeteta,
                              double IP3dlike=-5000);


    TH1* retrieveHistoFromFile(const std::string& refFileName, 
			      const std::string& histoName);
    

    void initializeCalibrationFile(const std::string & hypothesis);
    
    void loadCalibration(const std::string & jetauthor,const std::string & hypothesis);
    
    bool isCalibrationUpdated(const std::string& jetauthor,
                              const std::string& histoName);

  private:

    std::vector<std::string> m_hypothesis; 

    std::map<std::string,RooSimultaneous*> m_likelihoods;
    //    std::string m_textFileBaseName;

    std::map<std::string, std::vector<RooDataHist*>* > m_likelihoodHistograms;
    std::map<std::string, RooArgSet* > m_likelihoodObjects;

    //ITHistSvc* m_histoSvc;
    
    RooArgSet* m_myRooVariables;
    //    std::map<RooAbsCategory*,RooCategory*> m_fictiousToRealCategory;

    ToolHandle<CalibrationBroker> m_calibrationTool;

    std::string m_calibrationDirectory;
    std::string m_calibrationSubDirectory;

//    std::vector<std::string> m_hypothesis;

    /*
    RooCategory *cat_nVTX;
    RooCategory* cat_nTracksAtVtx;
    RooCategory* cat_nSingleTracks;
    RooMappedCategory* mycat_nVTX;
    RooMappedCategory* mycat_nTracksAtVtx6Bin;
    RooMappedCategory* mycat_nSingleTracks;
    RooRealVar *energyFraction;
    RooRealVar *significance3d;
    RooRealVar *mass;
    RooThresholdCategory* energyFractionCat;
    RooMappedCategory* allCategoriesCombined_reduced;
    RooSuperCategory* allCategoriesCombined_reduced_super;
    */
      
    
  };
  
}//end Analysis namespace

#endif
