/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ElementTableCnv.h"
#include "GaudiKernel/MsgStream.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

ElementTableCnv::ElementTableCnv( ISvcLocator *svcloc ) :
 ElementTableCnvBase(svcloc),
 m_msgSvc( msgSvc() )
 {}


ElementTable_PERS*
ElementTableCnv::createPersistentWithKey (Trk::ElementTable *etTrans,
                                          const std::string& key)
{
   MsgStream log (m_msgSvc, "ElementTableCnv: " + key);

   // call to the converter
   ElementTable_PERS * etPers = m_TPConverter.createPersistent(etTrans, log );
 
   return etPers;

}//end of create persistent method


Trk::ElementTable*
ElementTableCnv::createTransientWithKey (const std::string& key)
{
   MsgStream log (m_msgSvc, "ElementTableCnv: " + key);

   static const pool::Guid p1_guid( "B157B642-94C0-11E3-B1C2-02163E00A511" );

   Trk::ElementTable* tCollection = nullptr;
   if( compareClassGuid( p1_guid ) ) {

      std::unique_ptr< ElementTable_PERS >  p_coll( poolReadObject< ElementTable_PERS >() );
      tCollection = m_TPConverter.createTransient( p_coll.get(), log );
   }
 
   return tCollection;

}
