/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAALGS_SMALLCHRONO_H
#define EGAMMAALGS_SMALLCHRONO_H

#include "GaudiKernel/IChronoStatSvc.h"

namespace{
  class smallChrono{
  public:
    smallChrono(IChronoStatSvc &timeProfile, const std::string& name, bool doChrono):
      m_name(name),
      m_time(timeProfile),
      m_doChrono(doChrono){
       if( m_doChrono ){
        m_time.chronoStart(m_name);
      }
    }
    ~smallChrono(){
      if( m_doChrono ){
        m_time.chronoStop(m_name);
      }
    }
  private:
    const std::string m_name;
    IChronoStatSvc &m_time;
    bool m_doChrono;
  };
}
#endif
