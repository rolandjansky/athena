/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_PIXELDETECTORELEMENTSTATUSTOOLBASE_H
#define PIXELCONDITIONSSERVICES_PIXELDETECTORELEMENTSTATUSTOOLBASE_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "InDetConditionsSummaryService/IDetectorElementStatusTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "InDetIdentifier/PixelID.h"

#include "PixelReadoutGeometry/IPixelReadoutManager.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementStatus.h"

class PixelDetectorElementStatusToolBase: public extends<AthAlgTool, IDetectorElementStatusTool> {
  public:

    PixelDetectorElementStatusToolBase(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~PixelDetectorElementStatusToolBase();
    virtual StatusCode initialize() override;

  protected:
   /** Create a new detector element status element container.
    * Depending on the properties the container may be a copy of an event data or conditions data element status container.
    */
    std::unique_ptr<InDet::SiDetectorElementStatus>
    createDetectorElementStatus(const EventContext& ctx,
                                SG::WriteCondHandle<InDet::SiDetectorElementStatus>* whandle) const;

   const PixelID* m_pixelID {};

  private:
    ServiceHandle<InDetDD::IPixelReadoutManager> m_pixelReadout
       {this, "PixelReadoutManager", "PixelReadoutManager", "Pixel readout manager" };

    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey
       {this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};

    SG::ReadCondHandleKey<InDet::SiDetectorElementStatus> m_pixelDetElStatusCondKey
       {this, "PixelDetElStatusCondDataBaseKey", "" , "Optional conditions data key of an input SiDetectorElementStatus on which the newly created object will be based."};

};


#endif
