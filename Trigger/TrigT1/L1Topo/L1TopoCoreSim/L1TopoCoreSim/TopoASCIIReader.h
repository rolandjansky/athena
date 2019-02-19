/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//  TopoASCIIReader.h
//  L1TopoCoreSimulation

#ifndef L1TOPOCORESIM_TOPOASCIIREADER_H
#define L1TOPOCORESIM_TOPOASCIIREADER_H

#include "L1TopoEvent/TopoInputEvent.h"
#include "L1TopoEvent/ClusterTOB.h"
#include "L1TopoEvent/JetTOB.h"
#include "L1TopoCommon/Exception.h"

#include <vector>

#include <iostream>
#include <fstream>   // for std::ifstream
#include <istream>
#include <ostream>

#include <algorithm> // for std::string::iterator
#include <iterator>

#include <string>
#include <sstream>

#include <vector>


namespace TCS {
  
  class TopoASCIIReader {

  public:
    
     // default constructor
     TopoASCIIReader(int verbose=0);

     // destructor
     ~TopoASCIIReader();

     // load file
     void loadInput(const std::string & input);

     // input validation
     void validateInput();

     void setVerbosity(int verbosity) { m_verbosity=verbosity; }

     void setInputEvent(TCS::TopoInputEvent *evt) { m_event = evt; }

     // get TOBS for a single event
     bool getNextEvent();
    
     // print single event
     void printEvent();

     // print file summary
     void printFileSummary();
    
     // reset all variables
     void reset();
    
  private:
    
     std::ifstream m_fs;
     std::string m_inputFile { "" };
     TCS::TopoInputEvent *m_event { nullptr };
     int m_verbosity { 0 };
     
     //file summary info
     int m_nEvents { 0 };

  };
  
  
}

#endif
