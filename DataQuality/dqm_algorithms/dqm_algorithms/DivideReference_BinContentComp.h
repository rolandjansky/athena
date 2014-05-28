/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file DivideReference_BinContentComp.h file declares the dqm_algorithms::DivideReference_BinContentComp  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_DIVIDEREFERENCE_BINCONTENTCOMP_H
#define DQM_ALGORITHMS_DIVIDEREFERENCE_BINCONTENTCOMP_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/DivideReference.h>

namespace dqm_algorithms
{
	struct DivideReference_BinContentComp : public DivideReference
        {
	  DivideReference_BinContentComp(): DivideReference("BinContentComp") {};

	};
}

#endif // DQM_ALGORITHMS_DIVIDEREFERENCE_BINCONTENTCOMP_H
