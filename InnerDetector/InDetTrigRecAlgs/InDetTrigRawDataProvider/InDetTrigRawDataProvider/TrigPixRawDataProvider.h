/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/*
 * "$Id: TrigPixRawDataProvider.h,v 1.13 2009-02-20 19:28:31 masik Exp $"
 *
 * author: Jiri Masik 
 *
 * Description:  fill Pixel RDO container with data
 */
////////////////////////////////////////////////////////////////////////////

#ifndef TRIGPIXRAWDATAPROVIDER_H
#define TRIGPIXRAWDATAPROVIDER_H

// Base class

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"

//typedef
#include "InDetRawData/PixelRDO_Container.h"


#include <string>

class StoreGateSvc;
class IRoiDescriptor;
class PixelID;
class IPixelCablingSvc;
class IROBDataProviderSvc;
class IRegSelSvc;
class IPixelRawDataProviderTool;

namespace InDet {
  class TrigPixRawDataProvider : virtual public ITrigRawDataProviderTool,
				 public AthAlgTool {
  public:

    TrigPixRawDataProvider(const std::string& type, const std::string& name,
			    const IInterface* parent);

    virtual StatusCode initialize();

    StatusCode decode(const IRoiDescriptor *roi);

    virtual StatusCode finalize();

    virtual ~TrigPixRawDataProvider();

  private:
    StatusCode initContainer();

    
  private:
    ServiceHandle<IRegSelSvc>             m_regionSelector;     
    ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;
    ToolHandle<IPixelRawDataProviderTool>  m_rawDataTool;
    ServiceHandle<StoreGateSvc>           m_storeGate;
    ServiceHandle<StoreGateSvc>           m_detStore; 
    ServiceHandle<IPixelCablingSvc>       m_IdMapping;
    const PixelID*                        m_id; 

    PixelRDO_Container*                   m_container;
    std::string                           m_RDO_Key;

    //
  };

}

#endif


