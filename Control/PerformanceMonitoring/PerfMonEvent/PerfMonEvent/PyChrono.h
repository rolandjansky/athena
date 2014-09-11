///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// PyChrono.h 
// Header file for class PerfMon::PyChrono
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PYCHRONO_H 
#define PERFMONEVENT_PYCHRONO_H 

// STL includes
#include <string>

// GaudiKernel includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

// PerfMonEvent includes

// Forward declaration

namespace PerfMon {

class PyChrono
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /// Constructor with parameters: 
  PyChrono();

  /// Destructor: 
  ~PyChrono(); 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /**    start   chrono, tagged by its name 
   *     @param t chrno tag (name)
   *     @return status code 
   */
  void  chronoStart( const std::string& t );
  
  /**    stop    chrono, tagged by its name 
   *     @param t chrono tag (name)
   *     @return status code 
   */         
  void  chronoStop( const std::string& t );
  
  /**    return chrono delta time of last start/stop pair
   *     @param t chrono tag (name)
   *     @param f chtono type 
   *     @return delta time 
   */
  double chronoDelta( const std::string& t, const std::string& f );

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 
  // not implemented
  PyChrono( const PyChrono& );
  PyChrono& operator=( const PyChrono& );

  /// Pointer to the IChronoStatSvc
  ServiceHandle<IChronoStatSvc> m_chronoSvc;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

} // end namespace PerfMon

#endif //> PERFMONEVENT_PYCHRONO_H
