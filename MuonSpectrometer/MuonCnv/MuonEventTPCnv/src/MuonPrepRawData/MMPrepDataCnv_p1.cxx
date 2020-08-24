/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPrepRawData/MMPrepData.h"
#include "MuonEventTPCnv/MuonPrepRawData/MMPrepDataCnv_p1.h"

/// function used to create an MMPrepDataObject when converting Pers to trans
Muon::MMPrepData
MMPrepDataCnv_p1::
createMMPrepData ( const Muon::MMPrepData_p1 *persObj,
		   Identifier clusId,
                   const MuonGM::MMReadoutElement* detEl,
                   MsgStream & /**log*/ ) 
{
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

  auto cmat = std::make_unique<Amg::MatrixX>(1,1);
  (*cmat)(0,0) = static_cast<double>(persObj->m_errorMat);

  Muon::MMPrepData data (clusId, 
                         0, //collectionHash
                         localPos,
                         std::move(rdoList),
                         cmat.release(),
                         detEl,
			 persObj->m_time,
			 persObj->m_charge,
			 persObj->m_driftDist,
			 persObj->m_stripNumbers,
			 persObj->m_stripTimes,
			 persObj->m_stripCharges);

  // se the microTPC parameters
  data.setMicroTPC(persObj->m_angle, persObj->m_chisqProb);
  // set the drift distances
  data.setDriftDist(persObj->m_stripDriftDist,persObj->m_stripDriftErrors_0_0, persObj->m_stripDriftErrors_1_1);
  data.setAuthor(static_cast<Muon::MMPrepData::Author>(persObj->m_author));

  return data;
}

void MMPrepDataCnv_p1::
persToTrans( const Muon::MMPrepData_p1 *persObj, Muon::MMPrepData *transObj,MsgStream & log ) 
{
  *transObj = createMMPrepData (persObj,
				transObj->identify(),
                                transObj->detectorElement(),
                                log);
  transObj->setAuthor(static_cast<Muon::MMPrepData::Author>(persObj->m_author));
}

void MMPrepDataCnv_p1::
transToPers( const Muon::MMPrepData *transObj, Muon::MMPrepData_p1 *persObj, MsgStream & log )
{
    persObj->m_locX     = transObj->localPosition()[Trk::locX];
    persObj->m_errorMat = (transObj->localCovariance())(0,0);
    
    persObj->m_time     = (short int) transObj->time();
    persObj->m_charge   = (int) transObj->charge();
    persObj->m_driftDist   = (float) transObj->driftDist();

    persObj->m_angle     = (float) transObj->angle();
    persObj->m_chisqProb = (float) transObj->chisqProb();

    persObj->m_stripNumbers = transObj->stripNumbers();
    persObj->m_stripTimes   = transObj->stripTimes();
    persObj->m_stripCharges = transObj->stripCharges();

    persObj->m_stripDriftDist   = transObj->stripDriftDist();
    persObj->m_stripDriftErrors_0_0 = transObj->stripDriftErrors_0_0();
    persObj->m_stripDriftErrors_1_1 = transObj->stripDriftErrors_1_1();

    persObj->m_author = static_cast<int>(transObj->author());

    /// store the rdoList in a vector with the difference with respect to the 32-bit cluster identifier
    Identifier32::value_type clusIdCompact = transObj->identify().get_identifier32().get_compact(); // unsigned int
    std::vector<signed char> rdoListPers;
    const std::vector<Identifier>& rdoListTrans = transObj->rdoList();
    for ( const auto& rdo_id : rdoListTrans ) {
      // get the difference with respect to the 32-bit cluster identifier
      // (this only works if the absolute value of the difference is smaller than 128)
      Identifier32::value_type rdoIdCompact = rdo_id.get_identifier32().get_compact(); // unsigned int
      int diff = (int)(rdoIdCompact-clusIdCompact);
      if (abs(diff)>SCHAR_MAX) log << MSG::WARNING << "Difference between cluster and rdo Identifier (" << diff << ") larger than what can be stored (" << SCHAR_MAX << ")" << endmsg;
      rdoListPers.push_back((signed char)diff);
    } 
   
    persObj->m_rdoList = rdoListPers; 

}


