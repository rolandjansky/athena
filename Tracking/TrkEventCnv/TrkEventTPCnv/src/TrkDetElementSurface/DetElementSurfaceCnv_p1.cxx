/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Identifier/Identifier.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"
#include "GaudiKernel/MsgStream.h"


Trk::DetElementSurface* DetElementSurfaceCnv_p1::createTransient( const Trk::DetElementSurface_p1 *persObj, MsgStream & /*log*/ )
{
   //transObj->m_id = persObj->m_id;
    //std::cout<<"DetElementSurfaceCnv_p1::createTransient persObj->m_id:"<<persObj->m_id<<std::endl;
   
    m_id = persObj->m_id; // store Id, and then use in createSurface
    return 0;
}

const Trk::Surface* DetElementSurfaceCnv_p1::createSurface( MsgStream & /*log*/ )
{
   //transObj->m_id = persObj->m_id;
    //std::cout<<"DetElementSurfaceCnv_p1::createSurface for m_id"<<m_id<<std::endl;
   // use tool to get detector element's surface 
    return m_eventCnvTool->getSurface(m_id);
}

void DetElementSurfaceCnv_p1::persToTrans( const Trk::DetElementSurface_p1 * /**persObj*/, Trk::DetElementSurface * /**transObj*/, MsgStream &/**log*/ )
{
    // NOT USED - but need to have something here to fufill interface.
    
    // std::cout<<"DetElementSurfaceCnv_p1::persToTrans"<<std::endl;
    // transObj = new Trk::DetElementSurface( createTransient( persObj, log ) );
}

void DetElementSurfaceCnv_p1::transToPers( const Trk::DetElementSurface *transObj, Trk::DetElementSurface_p1 *persObj, MsgStream & /**log*/ )
{
   //std::cout<<"DetElementSurfaceCnv_p1::transToPers: id = "<<transObj->associatedDetectorElementIdentifier()<<std::endl; 
 //  persObj->m_id = transObj->associatedDetectorElementIdentifier();
  //persObj->m_id = transObj->associatedDetectorElementIdentifier().get_compact();
  persObj->m_id = transObj->associatedDetectorElementIdentifier().get_identifier32().get_compact();
}
