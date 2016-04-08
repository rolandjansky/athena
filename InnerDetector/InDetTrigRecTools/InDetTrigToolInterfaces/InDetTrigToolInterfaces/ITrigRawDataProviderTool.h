/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
/*
 * "$Id: "
 *
 * author: Jiri Masik
 *
 * Description: the interface for raw data provider tools (all detectors)
 *
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef ITRIGRAWDATAPROVIDERTOOL_H
#define ITRIGRAWDATAPROVIDERTOOL_H

#include <string>

class IRoiDescriptor;

namespace InDet {

  static const InterfaceID IID_ITrigRawDataProviderTool("InDet::ITrigRawDataProviderTool",1,0);

  class ITrigRawDataProviderTool : virtual public IAlgTool {

  public:

    static const InterfaceID& interfaceID();

    /** decode method
     * decodes data in the region specified by the input parameters
     * @param[in] roi            eta and phi position
     * @param[in] etaHalfWidth   eta half width
     * @param[in] phiHalfWidth   phi half width
     */
    virtual StatusCode decode(const IRoiDescriptor *roi) = 0;
  private:
    virtual StatusCode initContainer() = 0;

  };

  inline const InterfaceID& InDet::ITrigRawDataProviderTool::interfaceID(){
    return IID_ITrigRawDataProviderTool;
  }

}

#endif
