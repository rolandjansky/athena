// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloDetMgrDetDescrCnv/CaloSuperCellMgrDetDescrCnv.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Converter for CaloSuperCellMgrDetDescrCnv.
 *        Creates the supercell geometry from the offline geometry.
 */


#ifndef CALODETMGRDETDESCRCNV_CALOSUPERCELLMGRDETDESCRCNV_H
#define CALODETMGRDETDESCRCNV_CALOSUPERCELLMGRDETDESCRCNV_H


#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/ICaloSuperCellAlignTool.h"
#include "CaloDetDescr/ICaloSuperCellIDTool.h"


class CaloSuperCellDetDescrManager;
class CaloDetDescrManager;
class CaloDetDescrElement;


/**
 * @brief Converter for CaloSuperCellMgrDetDescrCnv.
 *
 * This class is a converter for CaloSuperCellDetDescrManager which is
 * stored in the detector store.
 *
 * It creates the supercell geometry based on the offline geometry,
 * which should already be available in the detector store.
 *
 * First, we use the identifier information to create the descriptors
 * and elements, then call the update* methods below to initialize
 * the geometry information from the offline geometry.  The update
 * methods may be called again if the offline geometry changes
 * (due to alignment changes).
 */
class CaloSuperCellMgrDetDescrCnv
  : public DetDescrConverter
{
public:
  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();

  /// Standard Gaudi finalize method.
  virtual StatusCode finalize();


  /**
   * @brief Return the service type.  Required by the base class.
   */
  virtual long int   repSvcType() const;


  /**
   * @brief Called by the converter infrastructure to create an object.
   * @param pAddr Address of the object to create.
   * @param pObj[out] Set to a reference to the created helper.
   */
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);
  
  /// Storage type and class ID (used by CnvFactory)
  static long  storageType();


  /**
   * @brief Return the CLID of the class we create.
   * Required by the converter infrastructure.
   */
  static const CLID& classID();

  /**
   * @brief Constructor.
   * @param clid The CLID if the class we're constructing.
   * @param svcloc Gaudi service locator.
   */
  CaloSuperCellMgrDetDescrCnv(ISvcLocator* svcloc);

private:
  /// offline <> SC mapping tool.
  ToolHandle<ICaloSuperCellIDTool> m_scidtool;

  /// Supercell alignment tool.
  ToolHandle<ICaloSuperCellAlignTool> m_scAlignTool;


  /**
   * @brief Create the manager object.
   * @param helperKey StoreGate key of the manager.
   * @param idhelper[out] Set to a pointer to the new manager.
   * @param pObj[out] Set to a reference to the created manager.
   */
  StatusCode createManager (const std::string& mgrKey,
                              CaloSuperCellDetDescrManager* & mgr,
                              DataObject*& pObj);


  /**
   * @brief Create the descriptors for the supercell geometry.
   * @param mgr The supercell geometry manager.
   */
  StatusCode createDescriptors (CaloSuperCellDetDescrManager* mgr);


  /**
   * @brief Create the elements for the supercell geometry.
   * @param mgr The supercell geometry manager.
   */
  StatusCode createElements (CaloSuperCellDetDescrManager* mgr);
};


#endif // CALODETMGRDETDESCRCNV_CALOSUPERCELLMGRDETDESCRCNV_H
