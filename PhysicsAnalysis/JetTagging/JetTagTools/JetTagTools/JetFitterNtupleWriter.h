/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_JETFITTERNTUPLEWRITER_C
#define BTAGTOOL_JETFITTERNTUPLEWRITER_C

/******************************************************
    @class JetFitterNtupleWriter.h
     Package : BTagTools 
     Created : March 2007
     DESCRIPTION:
     This class writes an ntuple out which is needed to calibrate the BTagging algorithm 
     based on JetFitter.

    @author Giacinto Piacquadio
    @author2 Christian Weiser (University of Freiburg)
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/IJetFitterNtupleWriter.h"

class ITHistSvc;

class TTree;

//class Jet;

namespace Analysis {

static const InterfaceID IID_JetFitterNtupleWriter("Analysis::JetFitterNtupleWriter", 1, 0);


  class IJetFitterTagInfo;


  class JetFitterNtupleWriter : public IJetFitterNtupleWriter, 
				public AthAlgTool  {
    
  public:
    
    
    JetFitterNtupleWriter(const std::string& name,const std::string&,const IInterface*);
    ~JetFitterNtupleWriter();
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_JetFitterNtupleWriter; }


    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    StatusCode bookNtuple(const std::string & jetAuthor);
    
    
    StatusCode fillNtuple(const std::string & hypothesis,
			  const std::string & jetAuthor,
			  const IJetFitterTagInfo & jetFitterTagInfo,
			  const xAOD::Jet & myParticleJet);
    
    
    
  private:
    
    struct JetFitterVariables {
      public:
      int nVTX;
      int nTracksAtVtx;
      int nSingleTracks;
      double energyFraction;
      double mass;
      double significance3d;
      int flavour;
      double deltaphi;
      double deltaeta;
      double jetpt;
      double jeteta;
    };

    std::vector<std::pair<std::string,TTree*> > m_tree;
    
    std::vector<std::pair<std::string,JetFitterVariables*> > m_variables;

    ITHistSvc* m_histoSvc;
   
  };
  
}//end Analysis namespace

#endif
