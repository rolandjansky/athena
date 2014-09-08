/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetAuthorFillerTool.cxx
 * @author Pier-Olivier DeViveiros < viveiros AT physics.utoronto.ca >
 * @author Haifeng Li <Haifeng.Li@cern.ch>
 * @date October, 2009
 * @brief Block filler tool for Jet Author related quantities
 */


#include "JetAuthorFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetAuthorFillerTool::JetAuthorFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent),
      m_doAuthor(false),
      m_doCalibTags(false)
{
  declareProperty("WriteAuthor",    m_doAuthor    = false);
  declareProperty("WriteCalibTags", m_doCalibTags = false);

  m_author = 0;
  m_calibTags = 0;
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetAuthorFillerTool::book()
{
  if(m_doAuthor)
    CHECK( addVariable ("author",    m_author)  );
  if(m_doCalibTags)
    CHECK( addVariable ("calibtags", m_calibTags)  );
  
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
StatusCode JetAuthorFillerTool::fill(const Jet& p)
{
  if(m_doAuthor)
    *m_author = p.jetAuthor();
  
  if(m_doCalibTags)
    {
      *m_calibTags = "";
      int n = p.numCalibTag();
      
      // loop over all calibTags and append to string
      for(int i=0; i<n; i++)
	{
	  // Separate entries with underscores
	  if(i!=0)
	    *m_calibTags += "_";
	  // Append current tag
	  *m_calibTags += p.getCalibTag(i+1);
	}
    }
  
  return StatusCode::SUCCESS;
}


} // namespace D3PD
