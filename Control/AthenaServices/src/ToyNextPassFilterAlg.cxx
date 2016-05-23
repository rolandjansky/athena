/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ToyNextPassFilterAlg.cxx,v 1.4 2008-09-23 22:00:47 binet Exp $
#include "GaudiKernel/AlgFactory.h"
#include "ToyNextPassFilterAlg.h"

DECLARE_ALGORITHM_FACTORY( ToyNextPassFilterAlg )

ToyNextPassFilterAlg::ToyNextPassFilterAlg( const std::string& name,
					    ISvcLocator* pLoc ):
AthAlgorithm ( name, pLoc ), m_passesDone(0) 
{
  declareProperty( "NPasses", m_nPasses=2 );
}

bool
ToyNextPassFilterAlg::doNextPass() { return ++m_passesDone < m_nPasses; }

StatusCode 
ToyNextPassFilterAlg::execute() { return StatusCode::SUCCESS; }
