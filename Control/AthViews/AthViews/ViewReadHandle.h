///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHVIEWS_VIEWREADHANDLE_H
#define ATHVIEWS_VIEWREADHANDLE_H

#include "StoreGate/ReadHandle.h"
#include "AthViews/View.h"

namespace SG {

  template <class T>
  class ViewReadHandle : public ReadHandle<T> {

    public:

      // Constructor making a new (non-const) local view from const input
      ViewReadHandle (const ReadHandleKey<T>& key, const View* view)
      : ReadHandle<T>(key)
      , m_view( new View( view ) )
      {
        StatusCode sc = this->setProxyDict( m_view );
        if ( !sc.isSuccess() ) this->reset();
      };

      // Constructor using non-const input view
      ViewReadHandle (const ReadHandleKey<T>& key, View* view)
      : ReadHandle<T>(key)
      {
        StatusCode sc = this->setProxyDict( view );
        if ( !sc.isSuccess() ) this->reset();
      };

      // Constructor making a new (non-const) local view from const input
      ViewReadHandle(const ReadHandleKey<T>& key, const EventContext& ctx, const View* view)
      : ReadHandle<T>(key, ctx)
      , m_view( new View( view ) )
      {
        StatusCode sc = this->setProxyDict( m_view );
        if ( !sc.isSuccess() ) this->reset();
      };

      // Constructor using non-const input view
      ViewReadHandle(const ReadHandleKey<T>& key, const EventContext& ctx, View* view)
      : ReadHandle<T>(key, ctx)
      {
        StatusCode sc = this->setProxyDict( view );
        if ( !sc.isSuccess() ) this->reset();
      }

      // Destructor for local view
      ~ViewReadHandle()
      {
        if ( m_view ) delete m_view;
      };

    private:
      View* m_view{nullptr};
  };
} // EOF SG namespace

template<typename T>
SG::ReadHandle<T> makeHandle( const SG::View* view , const SG::ReadHandleKey<T>& rhKey, const EventContext& context ) {
  return SG::ViewReadHandle<T>( rhKey, context, view );
}

#endif
