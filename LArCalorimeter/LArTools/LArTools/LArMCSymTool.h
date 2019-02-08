/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARTOOLS_LARMCSYMTOOL_H
#define LARTOOLS_LARMCSYMTOOL_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "Identifier/HWIdentifier.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include "AthenaKernel/IOVSvcDefs.h"

class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArCablingLegacyService;

/** @brief Tool for converting LAr readout identifier to a subset of the identifiers.

 
Used in MC when symmetry can be assumed. For most of the detector phi symmetry is assumed.

*/

class LArMCSymTool: public AthAlgTool, virtual public ILArMCSymTool 
{
 public:
  
  // constructor
  LArMCSymTool(const std::string& type, 
	 const std::string& name, 
	 const IInterface* parent); 
  
  // destructor 
  virtual ~LArMCSymTool();
  
  // retrieve methods 
  virtual HWIdentifier symOnline(const HWIdentifier & id) const override;
  virtual HWIdentifier symOnline(const Identifier & id) const override;
  
  // initialize and finalize methods
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /** IOV Call Back
   */
  virtual StatusCode iovCallBack (IOVSVC_CALLBACK_ARGS);

  StatusCode initData() ;


  static const InterfaceID& interfaceID() { 
    return ILArMCSymTool::interfaceID();}
  

 private:
  // Disallow assignment.
  LArMCSymTool& operator= (const LArMCSymTool&);

  ToolHandle<LArCablingLegacyService> m_cablingService;
    
  unsigned m_ncellem,m_ncellhec,m_ncellfcal;
  std::vector<HWIdentifier> m_listOnline;

  const LArEM_ID*        m_lar_em_id; 
  const LArHEC_ID*       m_lar_hec_id; 
  const LArFCAL_ID*      m_lar_fcal_id;
};




#endif
