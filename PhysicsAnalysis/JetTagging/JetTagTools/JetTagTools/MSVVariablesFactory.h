// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGTOOL_MSVVARIABLESFACTORY_C
#define BTAGTOOL_MSVVARIABLESFACTORY_C

/******************************************************
    @class  MSVVariableFactory
    
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/IMSVVariablesFactory.h"
#include <utility> 

class StoreGateSvc;

namespace Analysis {


  //static const InterfaceID IID_JetFitterVariablesFactory("Analysis::JetFitterVariablesFactory", 1, 0);


  class MSVVariablesFactory : public AthAlgTool , virtual public IMSVVariablesFactory  {
    
  public:
    
    
    MSVVariablesFactory(const std::string& name,
			      const std::string& n, const IInterface* p);
    virtual ~MSVVariablesFactory();
    
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    
    virtual StatusCode fillMSVVariables(const xAOD::Jet &, xAOD::BTagging* BTag, const Trk::VxSecVKalVertexInfo* myInfoVKal, xAOD::VertexContainer* btagVertex, const xAOD::Vertex& PV, std::string basename) const ;

    void setOrigin(const xAOD::Vertex* priVtx); 
    //  static const InterfaceID& interfaceID() { return IID_JetFitterVariablesFactory; };

   
  private:
    double get3DSignificance(const xAOD::Vertex* priVertex,
                             std::vector<const xAOD::Vertex*>& secVertex,
                             const Amg::Vector3D jetDirection) const;
    //const xAOD::Vertex* m_priVtx;
   
    
  };
  //inline void MSVVariablesFactory::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }
  
}//end Analysis namespace

#endif
