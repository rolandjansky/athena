/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

/**
 * @file  Blob2ToolConstants.h
 * @author Walter Lampl <walter.lampl@cern.ch>
 * @date August, 2010
 * @brief An AlgTool to convert btw ToolConstants and coral::AttributeList 
 */


#ifndef BLOB2TOOLCONSTANTS_H
#define BLOB2TOOLCONSTANTS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "CoralBase/AttributeList.h"
#include "CaloConditions/ToolConstants.h"

class Blob2ToolConstants : public AthAlgTool  {
  
public:
  Blob2ToolConstants (const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent) ;
  

  virtual StatusCode initialize() ;

  static const InterfaceID& interfaceID();

   /**
   * @brief Method convert ToolConstants -> AttributeList (writing) 
   * @param tc Pointer to ToolConstants object
   * @param mLog MessageStream for logging
   * @return Pointer to AttributeList (owned by client)
   *
   * Uses a ROOT streamer to serialize the underlying Array class
   */
  coral::AttributeList* ToolConstantsToAttrList(const CaloRec::ToolConstants* tc) const;

  /**
   * @brief Method convert AttributeList->ToolConstants (reading) 
   * @param attrList Reference to coral::AttributeList (source)
   * @param [out] tc Reference to ToolConstants object (destination)
   * @param mLog MessageStream for logging
   * @return True if successful, False otherwise
   *
   * Uses a ROOT streamer to serialize the underlying Array class
   */
  StatusCode AttrListToToolConstants(const coral::AttributeList& attrList, CaloRec::ToolConstants& tc) const;


  /**
   * @brief Method to create a channel number (hash)
   * @param name of the channel (usually the Tool name)
   * @return channel number
   *
   * Uses SG::crc64 method as hash-function (only lower 32bits uses)
   */
  static unsigned nameToChannelNumber(const std::string& name) ;


}; //class definition




#endif
