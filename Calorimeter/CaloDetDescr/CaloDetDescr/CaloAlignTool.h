/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloAlignTool.h
 *
 * @brief Declaration of CaloAlignTool class
 *
 * @author Vakho Tsulaia <tsulaia@mail.cern.ch>
 *
 * $Id: CaloAlignTool.h,v 1.3 2008-12-11 16:46:46 tsulaia Exp $
 */

#ifndef CALODETDESCR_CALOALIGNTOOL_H
#define CALODETDESCR_CALOALIGNTOOL_H

#include "GeoModelInterfaces/IGeoAlignTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IIOVDbSvc.h"

#include "CxxUtils/checker_macros.h"

class CaloDetDescrManager;

/**
 * @class CaloAlignTool
 *
 * @brief CaloAlignTool updates existing CaloDD elements and
 * descriptors with alignment information.
 * Calo DD cashe is updated by CaloAlignTool::align() method
 * which is registered as a callback by CaloAlignTool::initialize()
 */

class ATLAS_NOT_THREAD_SAFE CaloAlignTool : public AthAlgTool, virtual public IGeoAlignTool
{
 public:

  /**
   * @brief Constructor
   * @param type [IN] Alg Tool type
   * @param name [IN] Alg Tool name
   * @param parent [IN] Alg Tool parent interface
   */
  CaloAlignTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 
  
  /**
   * @brief Destructor
   */
  virtual ~CaloAlignTool();
  
  /**
   * @brief  Standard initialize method of Alg Tools
   */
  StatusCode initialize();

  /**
   * @brief  Standard finalize method of Alg Tools
   */
  StatusCode finalize();
  
  /**
   * @brief Callback function trigerred when alignment constants are updated
   * in the Store Gate. Is called after LAr and Tile alignment callbacks
   * The same callback is registered on Sagging object, so both sagging and alignment 
   * are applied at the same time
   */
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS);

 protected:
   CaloAlignTool(); //> not implemented
   CaloAlignTool (const CaloAlignTool&); //> not implemented
   CaloAlignTool& operator= (const CaloAlignTool&); //> not implemented

 private:
  /**
   * @brief handle on the IOVDbSvc for releasing conditions objects from memory
   */
  ServiceHandle<IIOVDbSvc>  m_IOVDbSvc; 
};

#endif
