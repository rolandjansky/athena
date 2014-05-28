/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file AddReference_BinContentComp.h file declares the dqm_algorithms::AddReference_BinContentComp  class.
 * \author andrea.dotti@cern.ch
*/

#ifndef DQM_ALGORITHMS_ADDREFERENCE_BINCONTENTCOMP_H
#define DQM_ALGORITHMS_ADDREFERENCE_BINCONTENTCOMP_H

#include <dqm_core/Algorithm.h>
#include <dqm_algorithms/AddReference.h>

namespace dqm_algorithms
{
	struct AddReference_BinContentComp : public AddReference
        {
	  AddReference_BinContentComp(): AddReference("BinContentComp") {};

	};
}

#endif // DQM_ALGORITHMS_ADDREFERENCE_BINCONTENTCOMP_H
