/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MboyCoreEvt/MbIOObject.h"

MbIOObject::MbIOObject(const MbIOObject& aMbIOObject )
{
 m_Type_PRD       = aMbIOObject.IsTypePRD   () ;
 m_Type_ROT       = aMbIOObject.IsTypeROT   () ;
 m_Type_HOLE      = aMbIOObject.IsTypeHOLE  () ;
 m_Type_MUSC      = aMbIOObject.IsTypeMUSC  () ;
 
 m_Tech_MDT       = aMbIOObject.IsTechMDT   () ;
 m_Tech_RPC       = aMbIOObject.IsTechRPC   () ;
 m_Tech_TGC       = aMbIOObject.IsTechTGC   () ;
 m_Tech_CSC       = aMbIOObject.IsTechCSC   () ;
 m_Tech_CSCclu    = aMbIOObject.IsTechCSCclu() ;

 m_Status_Outlier = aMbIOObject.IsOutlier   () ;

}

MbIOObject::MbIOObject()
{
 ResetType() ;
 ResetTech() ;
 m_Status_Outlier = false ;
}
MbIOObject::~MbIOObject(){}
