/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file MDTTDCOfflineSpectrum.h author Elena Solfaroli and Valerio Consorti.
 * \ Adapted for dqm_algorithms by Justin Griffiths and adapted for MDTOffline Sept 09
*/

#ifndef DQM_ALGORITHMS_MDTTDCOFFLINESPECTRUM_H
#define DQM_ALGORITHMS_MDTTDCOFFLINESPECTRUM_H

#include <dqm_core/Algorithm.h>

namespace dqm_algorithms
{

  struct MDTTDCOfflineSpectrum : public dqm_core::Algorithm
  {
    MDTTDCOfflineSpectrum(const std::string &name);

    MDTTDCOfflineSpectrum * clone( );
    dqm_core::Result * execute( const std::string & , const TObject & , const dqm_core::AlgorithmConfig & );
    using dqm_core::Algorithm::printDescription;
    void  printDescription(std::ostream& out);
    
    private:
    std::string m_name;
  };
}

#endif // DQM_ALGORITHMS_MDTTDCOFFLINESPECTRUM_H
