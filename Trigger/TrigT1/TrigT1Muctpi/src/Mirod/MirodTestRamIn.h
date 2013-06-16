// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodTestRamIn.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODTESTRAMIN_H
#define TRIGT1MUCTPI_MIRODTESTRAMIN_H

// STL include(s):
#include <list>

/**
 *******************************************************************
 *
 *    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
 *
 *    @short Input for the Mirod Test Ram
 *
 *           MirodTestRamIn is a helper class holding the
 *           needed to load the Mirod test ram
 *
 *   @author Thorsten Wengler
 *  @version $Revision: 362102 $
 *
 *******************************************************************
 */
class MirodTestRamIn {

 public:
  /**
   * The constructor of MirodTestRamIn
   */
  MirodTestRamIn();
  /**
   * get or set the DataLow list
   */
  void dataLow( std::list< unsigned int > dataLow ) { m_mirodTestDataLow = dataLow; }
  const std::list< unsigned int >& dataLow() { return m_mirodTestDataLow; }
  /**
   * get or set the DataHigh list
   */
  void dataHigh( std::list< unsigned int > dataHigh ) { m_mirodTestDataHigh = dataHigh; }
  const std::list< unsigned int >& dataHigh() { return m_mirodTestDataHigh; }
  /**
   * get or set the DataControl list
   */
  void dataControl (std::list< unsigned int > dataControl ) { m_mirodTestControl = dataControl; }
  const std::list< unsigned int >& dataControl() { return m_mirodTestControl; }

  /**
   * The destructor of MirodTestRamIn
   */
  ~MirodTestRamIn();

 private:

  std::list< unsigned int > m_mirodTestDataLow;
  std::list< unsigned int > m_mirodTestDataHigh;
  std::list< unsigned int > m_mirodTestControl;

}; // class MirodTestRamIn

#endif // TRIGT1MUCTPI_MIRODTESTRAMIN_H
