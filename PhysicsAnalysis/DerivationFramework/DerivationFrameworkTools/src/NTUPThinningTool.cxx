/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NTUPThinningTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: James Catmore (james.catmore@cern.ch)
//

#include "DerivationFrameworkTools/NTUPThinningTool.h"

#include <vector>
#include <string>

#include "ExpressionEvaluation/ExpressionParser.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"

namespace DerivationFramework {

  NTUPThinningTool::NTUPThinningTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_expression("true"),
    m_parser(0),
    m_suffix("_thinned")
  {
    declareInterface<DerivationFramework::IThinningTool>(this);
    declareProperty("expression", m_expression);
    declareProperty("suffix", m_suffix);
    declareProperty("branchesForThinning", m_branches);
  }

  StatusCode NTUPThinningTool::initialize()
  {
    m_parser = new ExpressionParsing::ExpressionParser(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
    m_parser->loadExpression(m_expression);
    return StatusCode::SUCCESS;
  }

  StatusCode NTUPThinningTool::finalize()
  {
    if (m_parser) {
      delete m_parser;
      m_parser = 0;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode NTUPThinningTool::doThinning() const
  {

    // get the positions of the elements which pass the requirement
    std::vector<int> entries =  m_parser->evaluateAsVector();
    unsigned int nEntries = entries.size();
    // loop over the container list, get data from SG
    for (std::vector<std::string>::const_iterator strItr = m_branches.begin(); strItr!=m_branches.end(); ++strItr) {
	std::vector<float> *thinVecFloat = new std::vector<float>();
	std::vector<double> *thinVecDouble = new std::vector<double>();
	std::vector<int> *thinVecInt = new std::vector<int>();
	std::vector<bool> *thinVecBool = new std::vector<bool>();
	std::string thinName = (*strItr)+m_suffix;
	std::vector<int> *vecInt = NULL;
	std::vector<double> *vecDouble = NULL;
	std::vector<float> *vecFloat = NULL;
	std::vector<bool> *vecBool = NULL;
	if (evtStore()->contains<std::vector<int> >(*strItr)) 
		CHECK(evtStore()->retrieve((const std::vector<int>*&)vecInt,*strItr));
        if (evtStore()->contains<std::vector<float> >(*strItr)) 
                CHECK(evtStore()->retrieve((const std::vector<float>*&)vecFloat,*strItr));
        if (evtStore()->contains<std::vector<double> >(*strItr)) 
                CHECK(evtStore()->retrieve((const std::vector<double>*&)vecDouble,*strItr));
        if (evtStore()->contains<std::vector<bool> >(*strItr)) 
                CHECK(evtStore()->retrieve((const std::vector<bool>*&)vecBool,*strItr));
	if (vecInt!=NULL) {
		if (vecInt->size()!=nEntries) {
			ATH_MSG_ERROR("Number of entries in object with StoreGate key " << thinName << "is not congruent with thin requirement!");
			return StatusCode::FAILURE;
		}
		thinner(vecInt,entries,thinVecInt);
		if (evtStore()->contains<std::vector<int> >(thinName)) {
      			ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << thinName << " which already exists. Please use a different key");
      			return StatusCode::FAILURE;
    		}
    		CHECK(evtStore()->record(thinVecInt,thinName));
	}
        if (vecDouble!=NULL) {
		if (vecDouble->size()!=nEntries) {
                        ATH_MSG_ERROR("Number of entries in object with StoreGate key " << thinName << "is not congruent with thin requirement!");
                        return StatusCode::FAILURE;
                }
                thinner(vecDouble,entries,thinVecDouble);
                if (evtStore()->contains<std::vector<double> >(thinName)) {
                        ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << thinName << " which already exists. Please use a different key");
                        return StatusCode::FAILURE;
                }       
                CHECK(evtStore()->record(thinVecDouble,thinName)); 
        }
        if (vecFloat!=NULL) {
		if (vecFloat->size()!=nEntries) {
                        ATH_MSG_ERROR("Number of entries in object with StoreGate key " << thinName << "is not congruent with thin requirement!");
                        return StatusCode::FAILURE;
                }
                thinner(vecFloat,entries,thinVecFloat);
                if (evtStore()->contains<std::vector<float> >(thinName)) {
                        ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << thinName << " which already exists. Please use a different key");
                        return StatusCode::FAILURE;
                }       
                CHECK(evtStore()->record(thinVecFloat,thinName)); 
        }
        if (vecBool!=NULL) {
		if (vecBool->size()!=nEntries) {
                        ATH_MSG_ERROR("Number of entries in object with StoreGate key " << thinName << "is not congruent with thin requirement!");
                        return StatusCode::FAILURE;
                }
                thinner(vecBool,entries,thinVecBool);
                if (evtStore()->contains<std::vector<bool> >(thinName)) {
                        ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << thinName << " which already exists. Please use a different key");
                        return StatusCode::FAILURE;
                }
                CHECK(evtStore()->record(thinVecBool,thinName));
        }

    } 
    return StatusCode::SUCCESS;

  }  

  template<typename T> 
  void NTUPThinningTool::thinner(std::vector<T> *theVector, std::vector<int> theMask, std::vector<T>* &thinned) const
  {
    unsigned int nEntries = theMask.size();
    for (unsigned int i=0; i<nEntries; ++i) {
	if (theMask.at(i)==1) thinned->push_back(theVector->at(i));
    }
    return;
  }      

}
