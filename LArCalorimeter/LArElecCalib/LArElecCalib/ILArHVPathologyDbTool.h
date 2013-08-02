/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILARHVPATHOLOGYDBTOOL_H
#define ILARHVPATHOLOGYDBTOOL_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ILArHVPathologyDbTool("ILArHVPathologyDbTool", 1 ,0);

class AthenaAttributeList;
class LArHVPathologiesDb;

/**
 @class ILArHVPathologyDBTool 
 @brief Interface for Tool which transforms LArHVPathologiesDb into AttributeLists and vice versa
*/

class ILArHVPathologyDbTool: virtual public IAlgTool
{
 public: 
  virtual ~ILArHVPathologyDbTool(){} 

  static const InterfaceID& interfaceID() { return IID_ILArHVPathologyDbTool; }

  /** @brief Transform LArCellHVPathologyDBContainer into Attribute List
   */
  virtual AthenaAttributeList* hvPathology2AttrList(const LArHVPathologiesDb&) = 0;

  /** @brief Transform Attribute List into LArDBCellHVPathologyDBContainer
      NB: clients must delete returned pointer!
   */
  virtual LArHVPathologiesDb* attrList2HvPathology(const AthenaAttributeList&) = 0;
};

#endif
