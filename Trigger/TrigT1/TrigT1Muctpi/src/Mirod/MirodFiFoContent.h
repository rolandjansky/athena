// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodFiFoContent.h 362102 2011-04-28 13:17:28Z krasznaa $
#ifndef TRIGT1MUCTPI_MIRODFIFOCONTENT_H
#define TRIGT1MUCTPI_MIRODFIFOCONTENT_H

// STL include(s):
#include <list>
#include <string>

/**
*******************************************************************
*
*    $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $
*
*    @short Helper class to hold the output of the Mirod FiFo readout
*
*           This is a helper class to group the output from reading
*           out the Mirod FiFos.
*
*   @author Thorsten Wengler
*  @version $Revision: 362102 $
*
*******************************************************************
*/
class MirodFiFoContent {

 public:
  /**
   * The constructor of MirodFiFoContent
   */
  MirodFiFoContent();
  /**
   * set and get the DaqFiFo content
   */
  void daqFiFo( std::list< unsigned int > daqFiFoContent ) { m_daqFiFo = daqFiFoContent; }
  const std::list< unsigned int >& daqFiFo() { return m_daqFiFo; }
  /**
   * set and get the Lvl2FiFo content
   */
  void lvl2FiFo( std::list< unsigned int > lvl2FiFoContent ) { m_lvl2FiFo = lvl2FiFoContent; }
  const std::list< unsigned int >& lvl2FiFo() { return m_lvl2FiFo; }
  /**
   * set and get the MonFiFo content
   */
  void monFiFo( std::list< unsigned int > monFiFoContent ) { m_monFiFo = monFiFoContent; }
  const std::list< unsigned int >& monFiFo() { return m_monFiFo; }

  /**
   * print content of Monitoring Fifo in "human" , "hex" or "bin"
   * format depending on the format string
   */
  void printMonFiFo( std::string format );
  /**
   * print content of Lvl2 Fifo in "human" , "hex" or "bin"
   * format depending on the format string
   */
  void printLvl2FiFo( std::string fromat );
  /**
   * print content of Daq Fifo in "human" , "hex" or "bin"
   * format depending on the format string
   */
  void printDaqFiFo( std::string format );

  /**
   * The destructor of MirodFiFoContent 
   */
  ~MirodFiFoContent();  

 private:

  std::list< unsigned int > m_daqFiFo;
  std::list< unsigned int > m_lvl2FiFo;
  std::list< unsigned int > m_monFiFo;

}; // class MirodFiFoContent

#endif // TRIGT1MUCTPI_MIRODFIFOCONTENT_H
