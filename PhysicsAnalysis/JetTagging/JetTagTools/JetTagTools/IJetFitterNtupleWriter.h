/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*- header for JetFitterNtupleWriter
#ifndef BTAGTOOL_IJETFITTERNTUPLEWRITER_C
#define BTAGTOOL_IJETFITTERNTUPLEWRITER_C

/******************************************************
    @class IJetFitterNtupleWriter.h
     Package : BTagTools 
     Created : Aug 2012
     DESCRIPTION:
     base class for JetFitterNtupleWriter

    @author Dan Guest

********************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"

class ITHistSvc;

class TTree;

//class Jet;

namespace Analysis {

  static const InterfaceID 
  IID_IJetFitterNtupleWriter("Analysis::IJetFitterNtupleWriter", 1, 0);


  class IJetFitterTagInfo;


  class IJetFitterNtupleWriter : virtual public IAlgTool  {
    
  public:
    
    virtual ~IJetFitterNtupleWriter(){};
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { 
      return IID_IJetFitterNtupleWriter; }


    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize() = 0;
    
    virtual StatusCode bookNtuple(const std::string & jetAuthor) = 0;
    
    
    virtual StatusCode fillNtuple(const std::string & hypothesis,
				  const std::string & jetAuthor,
				  const IJetFitterTagInfo & jetFitterTagInfo,
				  const xAOD::Jet & myParticleJet) = 0;
    
    
    
  private:
   
  };
  
}//end Analysis namespace

#endif
