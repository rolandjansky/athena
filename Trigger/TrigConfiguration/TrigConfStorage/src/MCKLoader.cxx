/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/MCKLoader.h"
#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include <string>
#include "./DBHelper.h"

using namespace std;

TrigConf::MCKLoader::MCKLoader(StorageMgr& sm) :
    DBLoader("MCKLoader", sm, sm.sessionMgr().createSession())
{}

TrigConf::MCKLoader::~MCKLoader()
{}

bool
TrigConf::MCKLoader::loadMCKlinkedToSMK(unsigned int smk, unsigned int & mck)
{

    unique_ptr< coral::IQuery > q;

    startSession();
    
    try {
        unique_ptr< coral::IQuery > qtmp( m_session.nominalSchema().tableHandle( "MCK_TO_SMK_LINK").newQuery() );
        q = std::move(qtmp);
    }
    catch(coral::TableNotExistingException & ex) {
        TRG_MSG_WARNING("MAM tables not found in this trigger database");
        return false;
    } 
    
    std::string theCondition = "SMK_LINK_SMK = :smId";
    theCondition += string( " AND SMK_LINK_ACTIVE_MCK = 1"       );

    coral::AttributeList bindings;
    bindings.extend<int>("smId");
    bindings[0].data<int>() = smk;

    q->setCondition( theCondition, bindings );

    coral::AttributeList attList;
    attList.extend<int>( "SMK_LINK_MCK" );

    fillQuery(q.get(), attList);

    coral::ICursor& cursor = q->execute();

    int nFound(0);
    while (cursor.next()) {
        ++nFound;
        const coral::AttributeList& row = cursor.currentRow();
        mck = row["SMK_LINK_MCK"].data<int>();
    }
    commitSession();

    if(nFound>1) {
        TRG_MSG_ERROR("Found more than one active MCKs for SMK " << smk);
    } else if(nFound==1) {
        TRG_MSG_INFO("Found active MCK " << mck << " for SMK " << smk);
    } else {
        TRG_MSG_INFO("Found no active MCK for SMK " << smk);
    }
    return nFound==1;
}

bool
TrigConf::MCKLoader::loadReleaseLinkedToMCK(unsigned int mck, std::string & mck_release)
{
    //load athena release (when MCK!=0)
    
    if ( mck == 0 ){
        TRG_MSG_ERROR("No release exists for MCK 0");
        return false;
    }
    
    startSession();
    
    unique_ptr< coral::IQuery > q;
    
    try {
        unique_ptr< coral::IQuery > qtmp( m_session.nominalSchema().tableHandle( "MCK_TABLE").newQuery() );
        q = std::move(qtmp);
    }
    catch(coral::TableNotExistingException & ex) {
        TRG_MSG_WARNING("MAM tables not found in this trigger database");
        return false;
    } 
            
    coral::AttributeList bindings;
    bindings.extend<int>("mck");
    bindings[0].data<int>() = mck;
    
    q->setCondition( "MCK_ID = :mck", bindings );
    
    coral::AttributeList attList;
    attList.extend<std::string>        ( "MCK_ATHENA_VERSION" );
    
    fillQuery(q.get(), attList);
    
    coral::ICursor& cursor = q->execute();
    
    int nFound(0);
    while (cursor.next()) {
        ++nFound;
        const coral::AttributeList& row = cursor.currentRow();         
        mck_release = row["MCK_ATHENA_VERSION"].data<std::string>();
    }
    commitSession();
    
    if(nFound>1) {
        TRG_MSG_ERROR("Found more than one release for MCK " << mck);
    } else if(nFound==1) {
        TRG_MSG_INFO("Found release " << mck_release << " for MCK " << mck);
    } else {
        TRG_MSG_INFO("Found no release for MCK " << mck);
    }
    return nFound==1;

}

