/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"

#include "IRegionSelector/IRegSelTool.h"

#include <string>

class IRoiDescriptor;
class TRT_ID;
class ITRT_CablingSvc;
class IROBDataProviderSvc;
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
    ToolHandle<IRegSelTool>             m_regionSelector { this, "RegSelTool", "RegSetTool/RegSelTool_TRT" };     
    ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
    ToolHandle<ITRTRawDataProviderTool> m_rawDataTool;
    ServiceHandle<ITRT_CablingSvc>      m_IdMapping;
    ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_bsErrorSvc;
    const TRT_ID*                       m_id; 
    //! the RDO container
    TRT_RDO_Container*                  m_container;
    std::string                         m_RDO_Key;

    //
  };

}

#endif


