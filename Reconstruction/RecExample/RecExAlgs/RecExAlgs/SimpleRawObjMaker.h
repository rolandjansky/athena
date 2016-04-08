/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleRawObjMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SIMPLERAWOBJMAKER_H
#define SIMPLERAWOBJMAKER_H 1

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class AtlasDetectorID;
class Identifier;


  /** @class SimpleRawObjMaker

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  David Rousseau <rousseau@lal.in2p3.fr>
  */  

  class SimpleRawObjMaker : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       SimpleRawObjMaker(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~SimpleRawObjMaker();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      
      /** member variables for algorithm properties: */
      std::string  m_rawObjOutputName;
      std::string  m_rawMessage;
      
    }; 


#endif 
