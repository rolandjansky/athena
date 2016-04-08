/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimpleCookedObjMaker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SIMPLECOOKEDOBJMAKER_H
#define SIMPLECOOKEDOBJMAKER_H 1

// framework includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

class AtlasDetectorID;
class Identifier;


  /** @class SimpleCookedObjMaker

      This is for the Doxygen-Documentation.  
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.
      
      @author  David Rousseau <rousseau@lal.in2p3.fr>
  */  

  class SimpleCookedObjMaker : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       SimpleCookedObjMaker(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~SimpleCookedObjMaker();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
      
      /** member variables for algorithm properties: */
      std::vector<std::string>  m_rawObjNames;
      std::string  m_cookedObjOutputName;
      std::string  m_cookedMessage;
      
    }; 


#endif 
