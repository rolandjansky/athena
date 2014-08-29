/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
// filename: TrigiVertexxAODCnv.h
//
// author: Ondrej Penc 
//         ondrej.penc@cern.ch
//
//
// -------------------------------
// ATLAS Collaboration
*/
////////////////////////////////////////////////////////////////////////////

#ifndef TRIGVERTEXXAODCNV__H
#define TRIGVERTEXXAODCNV__H

#include <string>
#include "TrigInterfaces/FexAlgo.h"

namespace InDet 
{
   class TrigVertexxAODCnv : public HLT::FexAlgo
   {

   public:
      /// Regular algorithm constructor
      TrigVertexxAODCnv( const std::string& name, ISvcLocator* pSvcLocator );
      /// Function initialising the algorithm
      HLT::ErrorCode hltInitialize();
      /// Function executing the algorithm
      HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement*);
      /// Function finalizing the algorithm
      HLT::ErrorCode hltFinalize();	

   private:
      /// The key of the input Container
      std::string m_VxContName;
      /// The key for the output xAOD::Container
      std::string m_xVxContName;

   };

}

#endif // TRIGVERTEXXAODCNV__H
