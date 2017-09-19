/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_TGCCABLINGDBTOOL_H
#define MUONCONDTOOL_TGCCABLINGDBTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonCondInterface/ITGCCablingDbTool.h"
#include "GaudiKernel/IChronoStatSvc.h"

class IIOVSvc;
class IChronoStatSvc;

class TGCCablingDbTool: public AthAlgTool, public ITGCCablingDbTool
{
 public:    
  /** Constructor */
  TGCCablingDbTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent); 
  
  /** Required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx);

  /** Initilize */
  virtual StatusCode initialize();
  /** Finalize */
  virtual StatusCode finalize();
  /** Method to provide database */
  virtual std::vector<std::string>* giveASD2PP_DIFF_12();
  /** Get the folder name */
  virtual std::string getFolderName() const;
  /** Load parameters using IOV keys */
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  /** Load parameters using the folder name */
  virtual StatusCode loadASD2PP_DIFF_12(IOVSVC_CALLBACK_ARGS);

 private: 
  /** Load parameters from text database */
  virtual StatusCode readASD2PP_DIFF_12FromText();

  /** IIOVSvc member */
  IIOVSvc* m_IOVSvc;
  /** IChronoStatSvc member */
  IChronoStatSvc* m_chronoSvc;

  /** Data location */
  std::string m_DataLocation;
  /** Folder name */
  std::string m_Folder;

  /** Database as strings */
  std::vector<std::string>* m_ASD2PP_DIFF_12;

  /** Flag for readASD2PP_DIFF_12FromText() */
  bool m_readASD2PP_DIFF_12FromText;
  /** File name of the text database */ 
  std::string m_filename;
};

#endif // MUONCONDTOOL_TGCCABLINGDBTOOL_H
