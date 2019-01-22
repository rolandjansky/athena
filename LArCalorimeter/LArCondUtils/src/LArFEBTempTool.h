/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARFEBTEMPTOOL_H
#define LARCONDUTILS_LARFEBTEMPTOOL_H

/**
 @class LArFEBTempTool
 @brief concrete tool to provide FEB TEMP DCS information.

 Environment:
      Software developed for the ATLAS Detector at the CERN LHC

 @author 
      Kirill Skovpen <kskovpen@cern.ch>
*/

// Includes for Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "LArElecCalib/ILArFEBTempTool.h"
#include "StoreGate/DataHandle.h" 
#include "Identifier/Identifier.h" 
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// forward declaration
//using namespace coral;
class AttributeList;
class HWIdentifier;
class LArOnlineID;
class CondAttrListCollection; 

class LArFEBTempTool: public AthAlgTool, virtual public ILArFEBTempTool
{
 
 public: 
   LArFEBTempTool(const std::string& type, const std::string& name,
		  const IInterface* parent);
   
   virtual ~LArFEBTempTool();
   
   virtual StatusCode initialize();
   virtual StatusCode finalize(){return StatusCode::SUCCESS;}
   
   // Given a FEB ID, return std::vector of temp1 and temp2 values
   FEBTemp getFebTemp( const HWIdentifier& id )  ;

 private:
   
   std::string m_foldername;
   
   const CondAttrListCollection* m_atrlistcol;
   bool m_isinit;
};

#endif
