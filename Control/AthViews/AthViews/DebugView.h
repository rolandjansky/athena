///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_DEBUGVIEW_H
#define ATHVIEWS_DEBUGVIEW_H 1

#include "AthViews/SimpleView.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "tbb/concurrent_unordered_set.h"

namespace SG {
  class DataProxy;
  class View;
}
class DataObject;

/**
 * @brief Very simple pass-through implementation of IProxyDict 
 *
 * Most of the methods are copypasta and don't do anything
 * Adds extra debugging information to SimpleView
 *
 * @author Ben Wynne - ATLAS
 */
class DebugView : public SimpleView, AthMessaging
{
  public:
    DeclareInterfaceID( DebugView, 2, 0 );

    DebugView() = delete;
    DebugView( std::string const& Name, bool AllowFallThrough = true, std::string const& storeName = "StoreGateSvc" );
    virtual ~DebugView();


    /**
    * @see SimpleView::linkParent
    **/
    virtual void linkParent( const IProxyDict* parent );


    /**
    * @see SimpleView::proxy
    */
    virtual SG::DataProxy* proxy(const CLID& id, const std::string& key) const;

    virtual SG::DataProxy* findProxy( const CLID& id, const std::string& key, const bool allowFallThrough ) const;

    virtual SG::DataProxy* proxy(const void* const pTransient) const;


    /**
    * @see SimpleView::recordObject
    */
    virtual SG::DataProxy* recordObject ( SG::DataObjectSharedPtr<DataObject> obj,
                                          const std::string& key,
                                          bool allowMods,
                                          bool returnExisting);


  protected:

    // List of data loaded via fallthrough
    mutable tbb::concurrent_unordered_set< std::string > m_fallList ATLAS_THREAD_SAFE;
};

#endif // ATHVIEW_DEBUGVIEW_H
