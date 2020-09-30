/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "InDetByteStreamErrors/IDCInDetBSErrContainer.h"

#include "IRegionSelector/IRegSelTool.h"

#include <string>

class IRoiDescriptor;
class PixelID;
class IROBDataProviderSvc;

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

    ToolHandle<IRegSelTool>               m_regionSelector { this, "RegSelTool", "RegSelTool/RegSelTool_Pixel" };     

    ServiceHandle<IROBDataProviderSvc>    m_robDataProvider;
    ToolHandle<IPixelRawDataProviderTool>  m_rawDataTool;
    const PixelID*                        m_id; 

    PixelRDO_Container*                   m_container;

    std::string                           m_RDO_Key;

    IDCInDetBSErrContainer*               m_decodingErrors;
    std::string                           m_decodingErrorsKey;
    //
  };

}

#endif


