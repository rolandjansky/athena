/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************

   @class CombinerToolTag
   General tool to combine likelihoods
   (uses Andi Wildauer's CombinerTool)

   Created - 23 April 2007

   @author: 
   * Giacinto Piacquadio (giacinto.piacquadio AT physik.uni-freiburg.de)
   * Christian Weiser (christian.weiser AT physik.uni-freiburg.de)

   --- ( University of FREIBURG ) ---

   (c) 2007 - ATLAS Detector Software

********************************************************/

#include "JetTagTools/CombinerToolTag.h"
#include "JetTagInfo/BaseTagInfo.h"
#include "JetTagTools/ICombinerTool.h"
//#include "JetEvent/Jet.h"


namespace Analysis {


  CombinerToolTag::CombinerToolTag(const std::string& t, const std::string& n, const IInterface* p)
    : AthAlgTool(t,n,p),
      m_combinedTagString("combinedTag"),
      m_combinerTool("Analysis::CombinerTool")
  {
    declareInterface<ITagTool>(this);
    // global configuration:
    declareProperty("listTaggers",m_listTaggers);
    //    declareProperty("useCharm",m_useCharm);
    declareProperty("combinedTagString",m_combinedTagString);
    declareProperty("CombinerTool",m_combinerTool);
  }

  CombinerToolTag::~CombinerToolTag() {

  }


  StatusCode CombinerToolTag::initialize() {

    StatusCode sc = m_combinerTool.retrieve();
    if( StatusCode::SUCCESS != sc ) {
      ATH_MSG_ERROR("#BTAG# creation of Analysis::CombinerTool failed.");
      return sc;
    }

    return StatusCode::SUCCESS;
  }


  void CombinerToolTag::finalizeHistos() {
  }



  StatusCode CombinerToolTag::finalize() {
    return StatusCode::SUCCESS;
  }

  void CombinerToolTag::tagJet(xAOD::Jet& jetToTag) {
    
    std::vector<double> combination=m_combinerTool->simpleCombine(jetToTag,
								  m_listTaggers);

    BaseTagInfo* newTagInfo=new BaseTagInfo(m_combinedTagString);
    newTagInfo->setTagLikelihood(combination);
    jetToTag.addInfo(newTagInfo);
    newTagInfo->makeValid();    
    
  }
  
}//end namespace

  
