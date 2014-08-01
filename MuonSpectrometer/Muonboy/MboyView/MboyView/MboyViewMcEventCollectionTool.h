/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyViewMcEventCollectionTool_H
#define MboyViewMcEventCollectionTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "HepPDT/ParticleDataTable.hh"

/////////////////////////////////////////////////////////////////////////////
#include "MboyView/MboyViewMisc.h"

  /**
   @class MboyViewMcEventCollectionTool

   This class 
    is a tool to view MC evts Container

  @author samusog@cern.ch
  
  */

class MboyViewMcEventCollectionTool:public AthAlgTool{
public:
    MboyViewMcEventCollectionTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyViewMcEventCollectionTool 	 ();

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();
   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /**View */
   StatusCode ViewMcEventCollection(
               std::string McEventCollectionLocation,
               std::ofstream* pOut,
               float& PtMinID
              );

private:
///////////////////////////////////

   const HepPDT::ParticleDataTable*	m_particleDataTable ; //!< PDG Table

   int m_KounterErrorMessage    ; //!< Message Control 
   int m_KounterErrorMessageMax ; //!< Message Control 

};

#endif

