// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/LBMetadataTool.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2009
 * @brief Add luminosity metadata info to a D3PD.
 */


#include "LBMetadataTool.h"
#include "D3PDMakerInterfaces/ID3PD.h"
#include "AthenaKernel/errorcheck.h"
#include "TString.h"

namespace D3PD {


/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
LBMetadataTool::LBMetadataTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
  : AthAlgTool (type, name, parent)
{
  declareProperty ("Metakey", m_metakey = "Lumi",
                   "Key for output metadata.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode LBMetadataTool::initialize()
{
  return AthAlgTool::initialize();
}


/**
 * @brief Standard Gaudi @c queryInterface method.
 */
StatusCode
LBMetadataTool::queryInterface( const InterfaceID& riid, void** ppvIf )
{
  if ( riid == IMetadataTool::interfaceID() )  {
    *ppvIf = static_cast<IMetadataTool*> (this);
    addRef();
    return StatusCode::SUCCESS;
  }

  return AlgTool::queryInterface( riid, ppvIf );
}


/**
 * @brief Write metadata to a D3PD.
 * @param d3pd The D3PD to which to write.
 */
StatusCode LBMetadataTool::writeMetadata (ID3PD* d3pd)
{
  TString xml;
  ATH_MSG_WARNING( "Luminosity metadata not added correctly to the D3PD!" );
  CHECK( d3pd->addMetadata (m_metakey, &xml) );
  return StatusCode::SUCCESS;
}


} // namespace D3PD
