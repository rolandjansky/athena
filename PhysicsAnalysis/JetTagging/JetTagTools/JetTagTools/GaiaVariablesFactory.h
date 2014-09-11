/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*- header for JetFitterVariablesFactory
#ifndef BTAGTOOL_GAIA_VARIABLESFACTORY_C
#define BTAGTOOL_GAIA_VARIABLESFACTORY_C

/******************************************************
    @class  GaiaVariablesFactory
    Package : JetTagTools 
    Created : Sept 2013
    DESCRIPTION:
    This class is a "variable factory". The variables aren't really made here
    though, they are created by JetFitterCOMBNN or JetFitterCharm. This
    class just organizes them for Gaia
    @author Dan Guest
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/IJetFitterVariablesFactory.h"
// #include <vector>
#include <string> 
// #include <map>

//class Jet;

namespace Analysis {


  static const InterfaceID IID_GaiaVariablesFactory
  ("Analysis::GaiaVariablesFactory", 1, 0);


  class JetFitterGenericTagInfo;
  class IJetFitterTagInfo; 
  
  class GaiaVariablesFactory : 
    public AthAlgTool, 
    public IJetFitterVariablesFactory 
  {
    
  public:
    
    
    GaiaVariablesFactory(const std::string& name,
				const std::string& n, const IInterface* p);
    virtual ~GaiaVariablesFactory();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    //Manu: link with JetTagInfo package. Also commented out in .cxx 
    //IJetFitterTagInfo* getITagInfoObject(const xAOD::Jet &) const;
 
    static const InterfaceID& interfaceID() { 
      return IID_GaiaVariablesFactory; 
    };

   
  private:
    void add_jfc_info(const JetFitterGenericTagInfo* from, 
		      JetFitterGenericTagInfo* to) const; 
    void add_cnn_info(const IJetFitterTagInfo* from, 
		      JetFitterGenericTagInfo* to) const; 
    std::string m_jetFitterInstance;
    std::string m_jetFitterCharmInstance; 
    std::string m_gaiaInstance; 

  };  
}//end Analysis namespace

#endif
