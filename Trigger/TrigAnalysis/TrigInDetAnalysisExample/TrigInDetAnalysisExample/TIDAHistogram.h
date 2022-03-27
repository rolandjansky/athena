/// emacs: this is -* c++ -*-
/**
 **   @file         TIDAHistogram.h  
 **
 **   @author       sutt  
 **   @date         Sun  2 Jan 2022 06:57:06 GMT  
 ** 
 **   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef   TIDA_HISTOGRAM_H
#define   TIDA_HISTOGRAM_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

#include "AthenaMonitoringKernel/Monitored.h"

#include <string>

namespace TIDA {

template<typename T> 
class Histogram { 

public:

 Histogram() : m_monTool(0), m_name("") { } 

  Histogram( ToolHandle<GenericMonitoringTool>* m, const std::string& name ) : m_monTool(m), m_name(name)  { 
    //    std::cout << "book: " << m_name << "  " << m_monTool->name() << std::endl;
  } 

  void Fill( T d ) const { 
    if ( m_monTool ) { 
        //  std::cout << "Histogram::Fill() monTool " << m_monTool << "\tname: " << m_name << "\td: " << d << "\t" << monTool()->name() << std::endl;
        auto s = Monitored::Scalar<T>( m_name, d ); 
        Monitored::Group( *m_monTool, s );
    }
    else std::cerr << "Histogram " << m_name << "\tmonTool not defined" << std::endl;
  }

  void Fill( T d, T w ) const { 
    if ( m_monTool ) { 
        // std::cout << "Histogram::Fill() monTool " << m_monTool << "\tname: " << m_name << "\td: " << d << "\tw:" << w << std::endl;
	auto s  = Monitored::Scalar<T>( m_name, d );
	auto sw = Monitored::Scalar<T>( m_name+"_weight", w ); 
	Monitored::Group( *m_monTool, s, sw );
    }
    else std::cerr << "Histogram " << m_name << "\tmonTool not defined" << std::endl;
  }

  std::string name() const { return m_name; } 

  ToolHandle<GenericMonitoringTool>* monTool() const { return m_monTool; };

private:

  ToolHandle<GenericMonitoringTool>* m_monTool;

  std::string m_name;

};



}


#endif  /* TIDA_HISTOGRAM_H */










