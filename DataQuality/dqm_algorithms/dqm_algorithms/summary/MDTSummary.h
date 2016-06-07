/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MDTSummary.h file declares the dqm_algorithms::summary::NeutralSumamry class.
 * \author Andrea Dotti, added by R. Calkins
*/

#ifndef DQM_ALGORITHMS_MDT_SUMMARY_H
#define DQM_ALGORITHMS_MDT_SUMMARY_H

#include <map>
#include <dqm_core/Parameter.h>
#include <dqm_core/SummaryMaker.h>
#include <dqm_core/Result.h>
#include <boost/thread/mutex.hpp>

namespace dqm_algorithms
{
  namespace summary
  {
    /*! \class MDTSummary interface
     * Declares the MDTSummary SummaryMaker. 
     * Information about number of Red/Green/Yellow is attached to tags of the result
     */
    class MDTSummary : public dqm_core::SummaryMaker
    {
    public:
      MDTSummary() ;
      MDTSummary* clone();
      virtual dqm_core::Result * execute( const std::string& , const dqm_core::Result&, const dqm_core::ParametersMap& map);
    private:
      float m_thresholdRed;
      float m_thresholdYellow;
    };
  }
}

#endif // DQM_ALGORITHMS_GENERIC_SUMMARY_H
                                                                                                           
