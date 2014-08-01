/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewVertexContainerTool_H
#define MboyViewVertexContainerTool_H

#include "AthenaBaseComps/AthAlgTool.h"


/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewVertexContainerTool

   This class 
    is a tool to view Vertex Container

  @author samusog@cern.ch
  
  */

class MboyViewVertexContainerTool:public AthAlgTool{
public:
    MboyViewVertexContainerTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewVertexContainerTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewVertexContainer(
               std::string VertexContainerLocation,
               std::ofstream* pOut              );

private:
///////////////////////////////////

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

