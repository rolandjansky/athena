/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file JetD3PDMaker/src/JetShapeFillerTool.h
 * @author Renaud Bruneliere < Renaud.Bruneliere AT cern.ch > 
 * @date April, 2010
 * @brief Block filler tool for Jet shapes
 * @brief Jet shapes are defined section 5.2 of ATL-COM-PHYS-2010-125 
 */

#include "JetShapeFillerTool.h"

#include "JetEvent/Jet.h"
//#include "JetUtils/JetCaloHelper.h"

#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"

#include <iostream>
#include <cmath>
#include <limits>
#include <climits>

namespace D3PD {
  
  
/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
JetShapeFillerTool::JetShapeFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{

  // Properties

  declareProperty ("deltaR",  m_deltaR = 0.02);
  declareProperty ("Rmax",    m_Rmax   = -1.);
  declareProperty ("NormToA", m_do_normToA = false);

  // Extra properties for Signal States

  declareProperty ("SignalStateNumber", m_sigstate = 0);
  declareProperty ("SignalStatePrefix", m_prefix   = "");

  m_shape = 0;
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetShapeFillerTool::book()
{
  if (m_prefix == "")
    CHECK( addVariable ("shapeBins" , m_shape));
  else
    CHECK( addVariable (m_prefix + "_shapeBins" , m_shape));
  
  if (m_Rmax > 0. && m_deltaR > 0. && m_deltaR < m_Rmax) {
    m_shape->resize(int(m_Rmax/m_deltaR));
    m_shapeA.resize(int(m_Rmax/m_deltaR));
    for (unsigned int idr = 0; idr < m_shape->size(); idr++)
      m_shapeA.at(idr) = M_PI*(2*idr+1)*m_deltaR*m_deltaR;
  }
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

StatusCode JetShapeFillerTool::fill(const Jet& p)
{
  if (m_Rmax == -1.) {
    // Get Rmax from jet author name if undefined
    std::string jet_author = p.jetAuthor();
    for (unsigned int ic = 0; ic < jet_author.size(); ic++) {
      if (!isdigit(jet_author.at(ic))) continue;
      m_Rmax = atof(&jet_author.at(ic))*0.1;
      break;
    } 
    //m_log << MSG::INFO << "jetAuthor=" << p.jetAuthor() 
    //<< " is used to define Rmax=" << m_Rmax << endmsg;
    
    if (m_Rmax > 0. && m_deltaR > 0. && m_deltaR < m_Rmax) {
      m_shape->resize(int(m_Rmax/m_deltaR));
      m_shapeA.resize(int(m_Rmax/m_deltaR));
      for (unsigned int idr = 0; idr < m_shape->size(); idr++)
	m_shapeA.at(idr) = M_PI*(2*idr+1)*m_deltaR*m_deltaR;
    }
   }
	
  Jet::const_iterator iterJet(p.begin());
  Jet::const_iterator lJet(p.end()); 

  if (m_shape->size() != m_shapeA.size()) m_shape->resize(m_shapeA.size());
  for (unsigned int idr = 0; idr < m_shape->size(); idr++)
    m_shape->at(idr) = 0.;

  for ( ; iterJet != lJet; ++iterJet) {
    if (! *iterJet) continue;
    double dr = (*iterJet)->hlv().deltaR(p.hlv());
    if (dr>m_deltaR*static_cast<float>(UINT_MAX)) continue;
    unsigned int idr = unsigned(dr/m_deltaR);
    if (idr < m_shape->size()) m_shape->at(idr) += (*iterJet)->pt();
  }
  
  if (m_do_normToA)
    for (unsigned int idr = 0; idr < m_shape->size(); idr++)
      m_shape->at(idr) /= m_shapeA.at(idr);

  return StatusCode::SUCCESS;
}


} // namespace D3PD
