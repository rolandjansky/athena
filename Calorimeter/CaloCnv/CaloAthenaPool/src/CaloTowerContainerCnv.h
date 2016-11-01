/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CaloTowerContainerCnv_H
#define CaloTowerContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "CaloEvent/CaloTowerContainer.h"
#include "CaloTPCnv/CaloTowerContainer_p1.h"
#include "CaloTPCnv/CaloTowerContainerCnv_p1.h"

class    CaloTowerBuilderToolBase ; 
class    CaloTowerBuilderTool ; 

typedef CaloTowerContainer_p1 CaloTowerContainerPERS;
typedef T_AthenaPoolCustomCnv<CaloTowerContainer,CaloTowerContainerPERS> CaloTowerContainerCnvBase;

/**

 @class CaloTowerContainerCnv
 @brief POOL Converter for CaloTowerContainer

 **/

class CaloTowerContainerCnv : public CaloTowerContainerCnvBase
{
   friend class CnvFactory<CaloTowerContainerCnv >;
public:
    CaloTowerContainerCnv(ISvcLocator* svcloc);
    virtual ~CaloTowerContainerCnv();

    /// initialization
    //virtual StatusCode initialize();

    /// Extend base-class conversion method to modify when reading in
    //virtual StatusCode     PoolToDataObject(DataObject*& pObj,const std::string &token);

    virtual CaloTowerContainer* createTransient();
    virtual CaloTowerContainerPERS* createPersistent(CaloTowerContainer*);


    CaloTowerBuilderToolBase* getTool(const std::string& type,
					const std::string& nm); 

    CaloTowerBuilderTool* m_emHecTowerBldr;
    CaloTowerBuilderToolBase* m_fcalTowerBldr; 
    CaloTowerBuilderToolBase* m_tileTowerBldr; 
    pool::Guid  p0_guid;
    pool::Guid  p1_guid;
    CaloTowerContainerCnv_p1 m_converter;

};

#endif


