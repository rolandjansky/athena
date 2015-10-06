/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToyNextPassFilterTool.cxx,v 1.4 2008-09-23 22:00:47 binet Exp $
#include "GaudiKernel/ToolFactory.h"
#include "ToyNextPassFilterTool.h"

DECLARE_TOOL_FACTORY(ToyNextPassFilterTool)

ToyNextPassFilterTool::ToyNextPassFilterTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : AthAlgTool ( type, name , parent ), m_passesDone(0)
{
  //  declareInterface<INextPassFilter>(this);
  declareProperty( "NPasses", m_nPasses=2 );
}

bool
ToyNextPassFilterTool::doNextPass() { 
  return ++m_passesDone < m_nPasses;
}
