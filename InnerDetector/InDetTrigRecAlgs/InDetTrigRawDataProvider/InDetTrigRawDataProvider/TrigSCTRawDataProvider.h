/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/*
 * "$Id: TrigSCTRawDataProvider.h,v 1.12 2009-01-15 11:20:30 masik Exp $"
 *
 * author: Jiri Masik 
 *
 * Description:  fill SCT RDO container with data
 */
////////////////////////////////////////////////////////////////////////////

#ifndef TRIGSCTRAWDATAPROVIDER_H
#define TRIGSCTRAWDATAPROVIDER_H

// Base class

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"

//typedef
#include "InDetRawData/SCT_RDO_Container.h"

#include <string>

class StoreGateSvc;
class TrigRoiDescriptor;
class SCT_ID;
class IRegSelSvc;
class IROBDataProviderSvc;
class ISCT_CablingSvc;
class MsgStream;
class IRoiDescriptor;
class ISCTRawDataProviderTool;
class Incident;

namespace InDet {
  class TrigSCTRawDataProvider :  virtual public ITrigRawDataProviderTool,
				  public AthAlgTool,
				  virtual public IIncidentListener {
    
  public:
    
    TrigSCTRawDataProvider(const std::string& type, const std::string& name,
			    const IInterface* parent);

    virtual StatusCode initialize();

    StatusCode decode(const IRoiDescriptor *roi);

    virtual StatusCode finalize();

    virtual ~TrigSCTRawDataProvider();

    virtual void handle(const Incident& inc);    //on beginEvent clean up the collections

  private:
    StatusCode initContainer();
    
  private:
    ServiceHandle<IRegSelSvc>           m_regionSelector;     
    ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
    ToolHandle<ISCTRawDataProviderTool> m_rawDataTool;
    ServiceHandle<StoreGateSvc>         m_storeGate;
    ServiceHandle<StoreGateSvc>         m_detStore; 
    ServiceHandle<ISCT_CablingSvc>      m_cablingSvc;
    const SCT_ID*                       m_id; 
    //! the RDO container
    SCT_RDO_Container*                  m_container;
    std::string                         m_RDO_Key;

    //
  };

}

#endif


