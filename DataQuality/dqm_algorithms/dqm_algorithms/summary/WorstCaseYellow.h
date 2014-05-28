/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file WorstCaseYellow.h alternative to worst case summary. Ensures that correctable errors do not determine summary status.
 * \author Andrea Dotti
*/
   


#ifndef DQM_ALGORITHMS_WORSTCASEYELLOW_H
#define DQM_ALGORITHMS_WORSTCASEYELLOW_H

#include <dqm_core/SummaryMaker.h>

namespace dqm_algorithms
{
  namespace summary
    {
      /*! Restricted summary. The worst case output is yellow. This is useful when assessing correctable distributions.
      */
      struct WorstCaseYellow : public dqm_core::SummaryMaker
      {
	WorstCaseYellow();
	~WorstCaseYellow();
	WorstCaseYellow* clone();
	dqm_core::Result* execute( const std::string & , const dqm_core::Result & , const dqm_core::ParametersMap & );
      };
    }
}

#endif
