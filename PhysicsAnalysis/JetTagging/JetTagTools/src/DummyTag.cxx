/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagTools/DummyTag.h"
 
// ROOT include(s):
#include "TRandom.h"

#include "GaudiKernel/IToolSvc.h"
#include "xAODBTagging/BTagging.h"

#include <cmath>
#include <sstream>
#include <algorithm>

namespace Analysis {

  typedef std::vector<double> FloatVec;
  typedef std::vector<double>::iterator FloatVecIter;
  
  DummyTag::DummyTag(const std::string& name, const std::string& n, const IInterface* p)
    : AthAlgTool(name,n,p), m_runModus("analysis") {
    
    declareInterface<ITagTool>(this);
    declareProperty("Runmodus", m_runModus);
    // information to persistify:
    declareProperty("writeInfo", m_writeInfo = true);

  }

  
  StatusCode DummyTag::initialize() {
    
    return StatusCode::SUCCESS;
  }


  StatusCode DummyTag::finalize() {
    
    return StatusCode::SUCCESS;
  }


  StatusCode DummyTag::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging * bTag, const std::string jetName) {
    
    /** author to know which jet algorithm: */

    //std::cout << "georges says dummy tagging jet " << std::endl;
    //std::flush(std::cout);

    xAOD::JetAlgorithmType::ID jetAlgID =  jetToTag.getAlgorithmType();
    std::string name = xAOD::JetAlgorithmType::algName(jetAlgID);
    xAOD::JetInput::Type jetAlgType = jetToTag.getInputType();
    std::string type =  xAOD::JetInput::typeName(jetAlgType);
    std::string size = std::to_string(int(jetToTag.getSizeParameter()*10));

    std::string author = name;
    author.append(size);
    author.append(type);

    //std::string author = jetToTag.jetAuthor();
    //ATH_MSG_VERBOSE("#BTAG# Using jet type " << author);
    ATH_MSG_WARNING("#BTAG# Using jet type " << author);
    //xAOD::BTagging * bTag = const_cast<xAOD::BTagging*>(jetToTag.getBTagging() ); 

    double sv0_significance3D = gRandom->Gaus(0.,1);
    bTag->setSV0_significance3D(sv0_significance3D);


    return StatusCode::SUCCESS;
  }
 
}//end namespace



