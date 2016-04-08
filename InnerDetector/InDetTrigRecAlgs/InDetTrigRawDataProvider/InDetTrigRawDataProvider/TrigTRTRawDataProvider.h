/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/*
 * "$Id: TrigTRTRawDataProvider.h,v 1.14 2009-04-03 12:45:32 masik Exp $"
 *
 * author: Jiri Masik 
 *
 * Description:  fill TRT RDO container with data
 */
////////////////////////////////////////////////////////////////////////////

#ifndef TRIGTRTRAWDATAPROVIDER_H
#define TRIGTRTRAWDATAPROVIDER_H

// Base class

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"
//typedef
#include "InDetRawData/TRT_RDO_Container.h"

#include <string>

class StoreGateSvc;
class IRoiDescriptor;
class TRT_ID;
class ITRT_CablingSvc;
class IROBDataProviderSvc;
class IRegSelSvc;
class ITRTRawDataProviderTool;


namespace InDet {

  class TrigTRTRawDataProvider :  virtual public ITrigRawDataProviderTool,
				  public AthAlgTool {
    
  public:
    
    TrigTRTRawDataProvider(const std::string& type, const std::string& name,
			    const IInterface* parent);

    virtual StatusCode initialize();

    StatusCode decode(const IRoiDescriptor *roi);

    virtual StatusCode finalize();

    virtual ~TrigTRTRawDataProvider();

  private:
    StatusCode initContainer();
    
  private:
    ServiceHandle<IRegSelSvc>           m_regionSelector;     
    ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
    ToolHandle<ITRTRawDataProviderTool> m_rawDataTool;
    ServiceHandle<StoreGateSvc>         m_storeGate;
    ServiceHandle<StoreGateSvc>         m_detStore; 
    ServiceHandle<ITRT_CablingSvc>      m_IdMapping;
    const TRT_ID*                       m_id; 
    //! the RDO container
    TRT_RDO_Container*                  m_container;
    std::string                         m_RDO_Key;
    bool                                m_first_event;

    //
  };

}

#endif


