/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef LARFEBCONFIGREADERTOOL
#define LARFEBCONFIGREADERTOOL

#include "AthenaBaseComps/AthAlgTool.h"
#include "LArElecCalib/ILArFEBConfigReader.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "CoralBase/AttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "LArRawConditions/LArConditionsContainer.h"

#include <map>

class LArOnlineID;

class LArFEBConfigReader: public AthAlgTool, 
			  virtual public ILArFEBConfigReader {

 public:
  LArFEBConfigReader(const std::string& type, const std::string& name, const IInterface* parent); 
  virtual ~LArFEBConfigReader();

  StatusCode initialize();
  StatusCode finalize();
  
  //Accessor methods from ILArFEBConfigReader
  virtual short lowerGainThreshold(const HWIdentifier& id) const; 
  virtual short upperGainThreshold(const HWIdentifier& id) const;
  
  //IOV Callback function
  virtual StatusCode loadData(IOVSVC_CALLBACK_ARGS);

  static const InterfaceID& interfaceID() { return ILArFEBConfigReader::interfaceID();}

 private:

  const LArOnlineID* m_onlineID;
  std::vector<std::string> m_listOfFolders;

  std::map<HWIdentifier,const coral::AttributeList*> m_attrPerFeb;
  mutable std::map<HWIdentifier,const coral::AttributeList*>::const_iterator m_lastIt;

  std::vector<DataHandle<CondAttrListCollection> > m_handles;

  short getThreshold(const char* MedLow, const HWIdentifier& chid) const;
};


inline short LArFEBConfigReader::lowerGainThreshold(const HWIdentifier& chid) const {
  return getThreshold("lower",chid);
} 
  

inline short LArFEBConfigReader::upperGainThreshold(const HWIdentifier& chid) const {
  return getThreshold("upper",chid);
} 

#endif
