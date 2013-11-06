/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TICTLISTENER_H
#define TICTLISTENER_H

#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "StoreGate/StoreClearedIncident.h"

class StoreGateSvc;


/** @class TICTListener

    Abstract class to make a derive class (tool) aware of the end of event.
    Register the tool in the IncidentListener and call m_clear when the event finishes

    @author Bruno.Lenzi@cern.ch
*/
class TICTListener : virtual public IIncidentListener
{
  public:
    ~TICTListener() {};
    StoreGateSvc*       m_storeGate;
    bool m_listener;
    virtual void m_clear() {};
    
    virtual void initializeListener( IIncidentListener* tool, const std::string name )
    {
      ServiceHandle<IIncidentSvc> iincSvc( "IncidentSvc", name );
      if ( !iincSvc.retrieve().isSuccess())
      {
     std::cout << name << "Unable to retrieve the IncidentSvc" << std::endl;
        m_listener = false;
      }
      else
      {
        iincSvc->addListener( tool, "StoreCleared" );
        m_listener = true;
      }
    };
    
    virtual void handle( const Incident &incident )
    {
      if ( incident.type() == "StoreCleared" )
        if ( const StoreClearedIncident* inc = dynamic_cast<const StoreClearedIncident*> (&incident) )
        {
//       if ( inc->store() == &*evtStore() )
          if ( inc->store() == m_storeGate )
            m_clear();
        }
    };
};

#endif //TICTListener_H
