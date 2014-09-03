/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventAthenaPool/CscRawData_p2.h"
/*
void	CscRawData_p2::fillPersistent(const CscRawData* data, MsgStream &)
{
   m_address	= data->getAddress();
   m_id      	= data->identify();
   m_dpuID   	= data->dpuID(); 
   m_amps    	= data->getSamples();
}


void	CscRawData_p2::fillTransient(CscRawData* data, MsgStream &) const
{
   data->m_address 	= m_address;
   data->m_id  		= m_id;
   data->m_dpuID   	= m_dpuID;
   data->m_amps    	= m_amps;
}



CscRawData*
CscRawData_p2::createTransient(MsgStream &log) const
{
   CscRawData*	trans = new CscRawData();
   fillTransient(trans, log);
   return trans;
}


CscRawData_p2*
CscRawData_p2::createPersistent(const CscRawData* transObj, MsgStream &log)
{0
   CscRawData_p2*	pers = new CscRawData_p2();
   pers->fillPersistent(transObj, log);
   return pers;
}
*/


//#include <iostream>
//using namespace std;

//std::ostream& operator << ( std::ostream& sl, const  CscRawData_p2& data)
//{
   // sl << "CscRawData ["<< "ID=" << data.identify()
   //    << ", address=" << data.getAddress()
   //    << ", dpuID=" <<  data.dpuID()
   //    << ", amps=[";
   // for( unsigned i=0; i<data.getSamples().size(); i++ ) {
   //    sl << data.getSamples()[i] << ",";
   // }
   // sl << "]";
//   return sl;
//}




