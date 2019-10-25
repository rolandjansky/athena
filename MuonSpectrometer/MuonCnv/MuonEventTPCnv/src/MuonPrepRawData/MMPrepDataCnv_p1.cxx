/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MMPrepDataCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataCnv_p1.h"
#include "CxxUtils/make_unique.h"

/// function used to create an MMPrepDataObject when converting Pers to trans
Muon::MMPrepData
MMPrepDataCnv_p1::
createMMPrepData ( const Muon::MMPrepData_p1 *persObj,
		   Identifier clusId,
                   const MuonGM::MMReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
  //log << MSG::DEBUG << "MMPrepDataCnv_p3::persToTrans" << endmsg;
  // Fill localposition
  Amg::Vector2D localPos;
  localPos[Trk::locX] = persObj->m_locX; 
  localPos[Trk::locY] = 0.0; 
    
  // copy the list of identifiers of the cluster
  std::vector<Identifier> rdoList;
  unsigned int clusIdCompact = clusId.get_identifier32().get_compact();
  std::vector<signed char> rdoListPers = persObj->m_rdoList;
  for ( auto& diff : rdoListPers ) {
    Identifier rdoId (clusIdCompact + diff);
    rdoList.push_back(rdoId);
  }

  auto cmat = CxxUtils::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::MMPrepData data (clusId, 
                         0, //collectionHash
                         localPos,
                         std::move(rdoList),
                         cmat.release(),
                         detEl,
			 persObj->m_time,
			 persObj->m_charge);

  // se the microTPC parameters
  data.setMicroTPC(persObj->m_angle, persObj->m_chisqProb);

  return data;
}

void MMPrepDataCnv_p1::
persToTrans( const Muon::MMPrepData_p1 *persObj, Muon::MMPrepData *transObj,MsgStream & log ) 
{
  *transObj = createMMPrepData (persObj,
				transObj->identify(),
                                transObj->detectorElement(),
                                log);
}

void MMPrepDataCnv_p1::
transToPers( const Muon::MMPrepData *transObj, Muon::MMPrepData_p1 *persObj, MsgStream & /**log*/ )
{
    //log << MSG::DEBUG << "MMPrepDataCnv_p3::transToPers" << endmsg;
    persObj->m_locX     = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat = (transObj->localCovariance())(0,0);
    
    persObj->m_time     = (int) transObj->time();
    persObj->m_charge   = (int) transObj->charge();

    persObj->m_angle     = (float) transObj->angle();
    persObj->m_chisqProb = (float) transObj->chisqProb();

    std::vector<signed char> rdoListPers;
    /// store the rdoList in a vector with the different
    /// with respect to the cluster identifier
    const std::vector<Identifier>& rdoListTrans = transObj->rdoList();
    unsigned int clusIdCompact = transObj->identify().get_identifier32().get_compact();

    for ( const auto& rdo_id : rdoListTrans ) {
      // get the difference with the cluster identifier
      // no need to move by 1 bit as the difference will be small
      signed char rdo_diff = (signed char) rdo_id.get_identifier32().get_compact()-clusIdCompact;
      rdoListPers.push_back(rdo_diff);
    } 
   
    persObj->m_rdoList = rdoListPers; 

}


