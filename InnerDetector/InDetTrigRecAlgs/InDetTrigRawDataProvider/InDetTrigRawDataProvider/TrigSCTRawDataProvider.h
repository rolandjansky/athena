/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"

#include "SCT_Cabling/ISCT_CablingTool.h"
#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"

#include "IRegionSelector/IRegSelTool.h"

#include <string>

class TrigRoiDescriptor;
class SCT_ID;
class IROBDataProviderSvc;
class MsgStream;
class IRoiDescriptor;
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
    ToolHandle<IRegSelTool>             m_regionSelector { this, "RegSelTool", "RegSelTool/RegSelTool_SCT" };     
    ServiceHandle<IROBDataProviderSvc>  m_robDataProvider;
    ToolHandle<ISCTRawDataProviderTool> m_rawDataTool{this, "RawDataTool", "SCTRawDataProviderTool"};
    ToolHandle<ISCT_CablingTool>        m_cablingTool{this, "SCT_CablingTool", "SCT_CablingTool", "Tool to retrieve SCT Cabling"};
    const SCT_ID*                       m_id; 
    //! the RDO container
    SCT_RDO_Container*                  m_container;
    IDCInDetBSErrContainer*             m_bsErrCont;
    std::string                         m_RDO_Key;
    std::string                         m_bsErrCont_Key;

    //
  };

}

#endif


