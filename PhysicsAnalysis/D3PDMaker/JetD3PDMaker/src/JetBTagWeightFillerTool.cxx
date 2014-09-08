/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetBTagWeightFillerTool.cxx
 * @author Jet Goodson <jgoodson@cern.ch>
 * @date Jan, 2010
 * @brief Block filler tool for Jet BTag related quantities
 */


#include "JetBTagWeightFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"

#include <iostream>

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetBTagWeightFillerTool::JetBTagWeightFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  m_doAllTags = true;
  book().ignore();
  m_flavorweight = 0;

  declareProperty ("BTagKey", m_tagKey   = "BaselineTagger");
  declareProperty("DoAllTags", m_doAllTags = false);
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetBTagWeightFillerTool::book()
{
  if(!m_doAllTags){
      if(m_tagKey == "BaselineTagger" || m_tagKey == "")
	CHECK(addVariable ("flavor_weight",  m_flavorweight));
      else
	CHECK(addVariable ("flavor_weight_"+m_tagKey, m_flavorweight));
    }//end of !doAllTags

  if(m_doAllTags){
    //    CHECK(addVariable ("flavor_weight_default", m_flavorweight));
    CHECK(addVariable ("flavor_weight_TrackCounting2D", m_TrackCounting2D));
    CHECK(addVariable ("flavor_weight_JetProb", m_JetProb));
    CHECK(addVariable ("flavor_weight_IP1D", m_IP1D));
    CHECK(addVariable ("flavor_weight_IP2D", m_IP2D));
    CHECK(addVariable ("flavor_weight_IP3D", m_IP3D));
    CHECK(addVariable ("flavor_weight_SV0", m_SV0));
    CHECK(addVariable ("flavor_weight_SV1", m_SV1));
    CHECK(addVariable ("flavor_weight_SV2", m_SV2));
    CHECK(addVariable ("flavor_weight_JetFitterTag", m_JetFitterTag));
    CHECK(addVariable ("flavor_weight_JetFitterCOMB", m_JetFitterCOMB));
    CHECK(addVariable ("flavor_weight_JetFitterTagNN", m_JetFitterTagNN));
    CHECK(addVariable ("flavor_weight_JetFitterCOMBNN", m_JetFitterCOMBNN));
    CHECK(addVariable ("flavor_weight_SoftMuonTag", m_SoftMuonTag));
    CHECK(addVariable ("flavor_weight_SoftElectronTag", m_SoftElectronTag));
    CHECK(addVariable ("flavor_weight_IP3DSV1", m_IP3DSV1)); 
  }//end of doAllTags

  
  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode JetBTagWeightFillerTool::fill(const Jet& p)
{
  if(!m_doAllTags)
    {
      if (m_tagKey == "")
	{
	  *m_flavorweight = p.getFlavourTagWeight();
	}
      else
	{*m_flavorweight = p.getFlavourTagWeight(m_tagKey);}
    }
  if(m_doAllTags){
    //*m_weight = p.getFlavourTagWeight();
    *m_TrackCounting2D = p.getFlavourTagWeight("TrackCounting2D");
    *m_JetProb = p.getFlavourTagWeight("JetProb");
    *m_IP1D = p.getFlavourTagWeight("IP1D");
    *m_IP2D = p.getFlavourTagWeight("IP2D");
    *m_IP3D = p.getFlavourTagWeight("IP3D");
    *m_SV0 = p.getFlavourTagWeight("SV0");
    *m_SV1 = p.getFlavourTagWeight("SV1");
    *m_SV2 = p.getFlavourTagWeight("SV2");
    *m_JetFitterTag = p.getFlavourTagWeight("JetFitterTag");
    *m_JetFitterCOMB = p.getFlavourTagWeight("JetFitterCOMB");
    *m_JetFitterTagNN = p.getFlavourTagWeight("JetFitterTagNN");
    *m_JetFitterCOMBNN = p.getFlavourTagWeight("JetFitterCOMBNN");
    *m_SoftMuonTag = p.getFlavourTagWeight("SoftMuonTag");
    *m_SoftElectronTag = p.getFlavourTagWeight("SoftElectronTag");
    *m_IP3DSV1 = p.getFlavourTagWeight();
  }//end of all tags getting

  return StatusCode::SUCCESS;
}


} // namespace D3PD
