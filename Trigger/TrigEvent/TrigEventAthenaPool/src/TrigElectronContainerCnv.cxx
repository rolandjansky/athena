/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigElectronContainerCnv.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"

//create persistent
TrigElectronContainer_PERS* TrigElectronContainerCnv::createPersistent(TrigElectronContainer* transCont) 
{
    MsgStream mlog(msgSvc(), "TrigElectronContainerConverter" );
    mlog << MSG::DEBUG << "TrigElectronContainerCnv::createPersistent" << endmsg;

    TrigElectronContainer_PERS *persObj = m_converter.createPersistent( transCont, mlog );

    return persObj;
}

//create transient
TrigElectronContainer* TrigElectronContainerCnv::createTransient() 
{
    MsgStream mlog(msgSvc(), "TrigElectronContainerConverter" );
    mlog << MSG::DEBUG << "TrigElectronContainerCnv::createTransient" << endmsg;

    static const pool::Guid p3_guid("F2E0066A-3BC5-44F2-A18C-57C63481988D");
    static const pool::Guid tlp2_guid("2EDB14B9-0B61-4014-90C2-20AEB7AAFEBE");
    static const pool::Guid tlp1_guid("8831D2A9-F4B6-40BE-97C1-4BD7F9468267");
    static const pool::Guid p0_guid2("2F97E0FB-7C93-4616-B322-1A01BF65D331");
    static const pool::Guid p0_guid1("EA6EA1A5-16FC-4DBF-896E-D933B25E65E0");

    if( compareClassGuid( p3_guid ) ){
       std::unique_ptr< TrigElectronContainer_p3 > col_vect( poolReadObject< TrigElectronContainer_p3 >() );
       //         std::cout << "Reading IMFC p3" << std::endl;
       return m_converter.createTransient( col_vect.get(), mlog ) ;

    } else if ( compareClassGuid(tlp2_guid) ) {

      std::unique_ptr< TrigElectronContainer_tlp2 >   col_vect( poolReadObject< TrigElectronContainer_tlp2 >() );
      return m_converter_tlp2.createTransient( col_vect.get(), mlog );
      
    } else if ( compareClassGuid(tlp1_guid) ) {

      std::unique_ptr< TrigElectronContainer_tlp1 >   col_vect( poolReadObject< TrigElectronContainer_tlp1 >() );
      return m_converter_tlp1.createTransient( col_vect.get(), mlog );
      
    } else if ( compareClassGuid(p0_guid1) || compareClassGuid(p0_guid2) ) {
      
      // old version from before TP separation, just return it
      return poolReadObject<TrigElectronContainer>();
      
    }  else {
      
      throw std::runtime_error("Unsupported persistent version of Data container");
      
    }

}
