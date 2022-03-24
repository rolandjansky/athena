/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiDetElementsRoadCondAlg_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementsRoadCondAlg_xk_H
#define SiDetElementsRoadCondAlg_xk_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "SiDetElementsRoadTool_xk/SiDetElementsLayerVectors_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderBounds.h"

#include <vector>

class PixelID;
class SCT_ID;

namespace InDet {
  /**
     @class SiDetElementsRoadCondAlg_xk

     Prepare a condition object SiDetElementsLayerVectors_xk from
     condition objects PixelDetectorElementCollection and
     SCT_DetectorElementCollection for SiDetElementsRoadMaker_xk
     and stores it in condition store using ReadCondHandle and
     WriteCondHandle. In AthenaMT, detector elements are condition
     objects and this condition algorithm is necessary.

     @author Susumu.Oda@cern.ch
  */

  class SiDetElementsRoadCondAlg_xk : public AthReentrantAlgorithm
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard tool methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiDetElementsRoadCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SiDetElementsRoadCondAlg_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    /// Make this algorithm clonable.
    virtual bool isClonable() const override { return true; };
    //@}

  private:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Properties
    ///////////////////////////////////////////////////////////////////
    //@{
    BooleanProperty m_usePIX{this, "usePixel", true, "Flag to use Pixel"};
    BooleanProperty m_useSCT{this, "useSCT",   true, "Flag to use SCT"};
    BooleanProperty m_ITkGeometry{this, "ITkGeometry", false};
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Condition handles
    /// SG::ReadCondHandleKey is input and SG::WriteCondHandleKey are output.
    ///////////////////////////////////////////////////////////////////
    //@{
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_pixelDetEleCollKey{this, "PixelDetEleCollKey", "PixelDetectorElementCollection", "Key of SiDetectorElementCollection for Pixel"};
    SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    SG::WriteCondHandleKey<SiDetElementsLayerVectors_xk> m_writeKey{this, "WriteKey", "SiDetElementsLayerVectors_xk", "Key of SiDetElementsLayerVectors_xk"};
    //@}


    ///////////////////////////////////////////////////////////////////
    /// @name Private method
    ///////////////////////////////////////////////////////////////////
    //@{
    void detElementInformation(const InDetDD::SiDetectorElement&, double*);
    //@}
  };

} // end of name space

#endif // SiDetElementsRoadCondAlg_xk_H
