/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "ClusterSplitProbabilityContainerCnv.h"


const std::string ClusterSplitProbabilityContainerCnv::s_name("ClusterSplitProbabilityContainerCnv");
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
ClusterSplitProbabilityContainerCnv::ClusterSplitProbabilityContainerCnv(ISvcLocator* svcloc)
   : ClusterSplitProbabilityContainerCnvBase(svcloc)
{
}


ClusterSplitProbabilityContainer_PERS * ClusterSplitProbabilityContainerCnv::createPersistent( Trk::ClusterSplitProbabilityContainer *transCont)
{
   MsgStream log(msgSvc(), s_name );
   return m_converter.createPersistent( transCont, log );
}

//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
Trk::ClusterSplitProbabilityContainer *ClusterSplitProbabilityContainerCnv::createTransient()
{
    MsgStream log(msgSvc(), s_name );
    static pool::Guid p1_guid( "33129502-20bc-44f9-acff-62e5c3670d46" );

    Trk::ClusterSplitProbabilityContainer *p_collection = nullptr;
    if( compareClassGuid( p1_guid )){
       std::unique_ptr<ClusterSplitProbabilityContainer_PERS> pers( poolReadObject< ClusterSplitProbabilityContainer_PERS >() );
       p_collection = m_converter.createTransient( pers.get(), log );
    }
    else
        throw std::runtime_error( "Unsupported persistent version of Data Collection" );
    return p_collection;
}
