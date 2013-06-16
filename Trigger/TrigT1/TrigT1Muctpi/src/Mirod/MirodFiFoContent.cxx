/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MirodFiFoContent.cxx 362102 2011-04-28 13:17:28Z krasznaa $

// Local include(s):
#include "MirodFiFoContent.h"
#include "MirodExtractor.h"
#include "MirodLvl2Processor.h"
#include "MirodDaqProcessor.h"

/*******************************************************************
 * $Date: 2011-04-28 15:17:28 +0200 (Thu, 28 Apr 2011) $ 
 *
 * Implementation of class MirodFiFoContent
 * @author   Author: Thorsten Wengler
 * @version $Revision: 362102 $
 ******************************************************************/

// constructor of class MirodFiFoContent
MirodFiFoContent::MirodFiFoContent() {

}

//destructor of class MirodFiFoContent
MirodFiFoContent::~MirodFiFoContent() {

}

// print content of Monitoring Fifo in "human" , "hex" or "bin"
void MirodFiFoContent::printMonFiFo( std::string format ) {

  LVL1MUCTPI::MirodExtractor hardwareExtrOut( m_monFiFo );

  if( format == "human" ) hardwareExtrOut.printEvent( "human" );
  else if( format == "hex" ) hardwareExtrOut.printEvent( "hex" );
  else if( format == "bin" ) hardwareExtrOut.printEvent("bin");

  return;

}

// print content of Lvl2 Fifo in "human" , "hex" or "bin"
void MirodFiFoContent::printLvl2FiFo( std::string format ) {

  LVL1MUCTPI::MirodLvl2Processor hardwareLvl2Output( m_lvl2FiFo );

  if( format == "human" ) hardwareLvl2Output.printEvent( "human" );
  else if( format == "hex" ) hardwareLvl2Output.printEvent( "hex" );
  else if( format == "bin" ) hardwareLvl2Output.printEvent( "bin" );

  return;

}

// print content of Daq Fifo in "human" , "hex" or "bin"
void MirodFiFoContent::printDaqFiFo(std::string format) {

  LVL1MUCTPI::MirodDaqProcessor hardwareDaqOutput( m_daqFiFo );

  if( format == "human" ) hardwareDaqOutput.printEvent( "human" );
  else if( format == "hex" ) hardwareDaqOutput.printEvent( "hex" );
  else if( format == "bin" ) hardwareDaqOutput.printEvent( "bin" );

  return;

}
