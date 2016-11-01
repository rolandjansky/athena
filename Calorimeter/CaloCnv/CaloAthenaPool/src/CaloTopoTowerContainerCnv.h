/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CaloTopoTowerContainerCnv_H
#define CaloTopoTowerContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "CaloEvent/CaloTopoTowerContainer.h"
#include "CaloTPCnv/CaloTopoTowerContainer_p1.h"
#include "CaloTPCnv/CaloTopoTowerContainerCnv_p1.h"

class    CaloTopoTowerBuilderToolBase ; 
class    CaloTopoTowerBuilderTool ; 

typedef CaloTopoTowerContainer_p1 CaloTopoTowerContainerPERS;
typedef T_AthenaPoolCustomCnv<CaloTopoTowerContainer,CaloTopoTowerContainerPERS> CaloTopoTowerContainerCnvBase;

/**

 @class CaloTopoTowerContainerCnv
 @brief POOL Converter for CaloTopoTowerContainer

 **/

class CaloTopoTowerContainerCnv : public CaloTopoTowerContainerCnvBase
{
   friend class CnvFactory<CaloTopoTowerContainerCnv >;
public:
    CaloTopoTowerContainerCnv(ISvcLocator* svcloc);
    virtual ~CaloTopoTowerContainerCnv();

    /// initialization
    //virtual StatusCode initialize();

    /// Extend base-class conversion method to modify when reading in
    //virtual StatusCode     PoolToDataObject(DataObject*& pObj,const std::string &token);

    virtual CaloTopoTowerContainer* createTransient();
    virtual CaloTopoTowerContainerPERS* createPersistent(CaloTopoTowerContainer*);


    CaloTopoTowerBuilderToolBase* getTool(const std::string& type,
					const std::string& nm); 

    CaloTopoTowerBuilderToolBase* m_TopoTowerBldr;
    pool::Guid  p0_guid;
    pool::Guid  p1_guid;
    CaloTopoTowerContainerCnv_p1 m_converter;

};

#endif


