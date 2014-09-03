/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloConditions/ToolConstants.h"


class MsgStream;
class StoreGateSvc;

class Blob2ToolConstants : public AthAlgTool  {
  
public:
  Blob2ToolConstants (const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent) ;
  

  virtual StatusCode initialize() ;

  static const InterfaceID& interfaceID();



   /**
   * @brief IOV callback function
   * @param Macro defined in AthenaKernel/IOVSvcDefs.h
   *
   * Converts the COOL inline BLOB(s) into ToolConstants objects and
   * registeres them to the detector store. 
   * One COOL channel == one ToolConstants object
   * 
   */
  StatusCode fillObjects(IOVSVC_CALLBACK_ARGS);

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
  unsigned nameToChannelNumber(const std::string& name) const;

  /**
   * @brief Method to register a folder to be read from COOL inline storage
   * @param fn COOL inline folder name
   * @param key COOL channel name, will be also used as SG key of resulting ToolConstants object
   * @return a StatusCode
   *
   * Registeres a IOV callback method for this folder (if not done already) 
   */
  StatusCode addFolder(const std::string& fn, const std::string& key);

  /**
   * @brief Method to register a folder to be read from COOL inline storage
   * @param fn COOL inline folder name
   * @param keys vector of COOL channel name, will be also used as SG keys for the resulting ToolConstants object
   * @return a StatusCode
   *
   * Registeres a IOV callback method for this folder (if not done already) 
   */
  StatusCode addFolder(const std::string& fn, const std::vector<std::string>& keys);

  /**
   * @brief Check if a certain folder and channel has been already registered for callback
   * @param fn COOL inline folder name
   * @param key COOL channel name
   * @return True if the folder is already konwn, false otherwise
   */
  bool hasChannel(const std::string& inlineFolder, const std::string& chName) const;

private:
  
  typedef std::map<std::string,CaloRec::ToolConstants*> KEYOBJMAP; //the Key is the old SG key of the non-inline obj
  struct FOLDEROBJ {
    const DataHandle<CondAttrListCollection> inlineData;
    KEYOBJMAP key_tc;
  };

  std::map< std::string, FOLDEROBJ > m_data;
  
}; //class definition




#endif
