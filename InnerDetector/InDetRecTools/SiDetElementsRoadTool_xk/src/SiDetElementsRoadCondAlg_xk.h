/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsRoadCondAlg_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsRoadCondAlg_xk_H
#define SiDetElementsRoadCondAlg_xk_H

#include <vector>
#include "GaudiKernel/ICondSvc.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayerVectors_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderBounds.h"

class PixelID;
class SCT_ID;

namespace InDetDD {
  class PixelDetectorManager;
}

namespace InDet {
  /**
     @class SiDetElementsRoadCondAlg_xk
     @author Susumu.Oda@cern.ch
  */

  class SiDetElementsRoadCondAlg_xk : public AthReentrantAlgorithm
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiDetElementsRoadCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiDetElementsRoadCondAlg_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
      
    ///////////////////////////////////////////////////////////////////
    // Private Data
    ///////////////////////////////////////////////////////////////////
    BooleanProperty m_usePIX{this, "usePixel", true, "Flag to use Pixel"};
    BooleanProperty m_useSCT{this, "useSCT",   true, "Flag to use SCT"};
    BooleanProperty m_useDynamicAlignFolders{this, "UseDynamicAlignFolders", false, "Flag to use dynamic alignment folders"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_IBLDistFolderKey{this, "IBLDistFolderKey", "/Indet/IBLDist", "Key of /Indet/IBLDist folder"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_pixelL2FolderKey{this, "PixelL2FolderKey", "/Indet/AlignL2/PIX", "Key of /Indet/AlignL2/PIX folder"};
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    SG::WriteCondHandleKey<SiDetElementsLayerVectors_xk> m_writeKey{this, "WriteKey", "SiDetElementsLayerVectors_xk", "Key of SiDetElementsLayerVectors_xk"};
    StringProperty m_pix{this, "PixManagerLocation", "Pixel", "PIX manager location"};
    StringProperty m_sct{this, "SCTManagerLocation", "SCT", "SCT manager location"};
    const InDetDD::PixelDetectorManager* m_pixmgr;
    ServiceHandle<ICondSvc> m_condSvc;

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////
    void detElementInformation(const InDetDD::SiDetectorElement&, double*);
  };

} // end of name space

#endif // SiDetElementsRoadCondAlg_xk_H
