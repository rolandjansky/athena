/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "VxVertex/Vertex.h"
#undef private
#undef protected

#include "TrkEventTPCnv/VxVertex/VertexCnv_p1.h"

void  VertexCnv_p1::persToTrans(const Trk::Vertex_p1 * persObj, Trk::Vertex * transObj, MsgStream &/*log*/){
    // std::cout<<"reading Vertex_p1 "<<std::endl;
    std::vector<float> pers_position = persObj->m_position; 
    Amg::Vector3D trans_position(pers_position[0], pers_position[1], pers_position[2]);
    transObj->m_position = trans_position; 
    // std::cout<<" vertex "<<trans_position<<std::endl;
}
  
void  VertexCnv_p1::transToPers(const Trk::Vertex * transObj, Trk::Vertex_p1 * persObj, MsgStream &/*log*/){
    // std::cout<<"writing Vertex_p1 "<<std::endl;
    std::vector<float> pers_position(3);
    Amg::Vector3D trans_position = transObj->m_position;
    pers_position[0] = trans_position(0);
    pers_position[1] = trans_position(1);
    pers_position[2] = trans_position(2);
    persObj->m_position = pers_position; 
    // std::cout<<" vertex "<<trans_position<<std::endl;
}
