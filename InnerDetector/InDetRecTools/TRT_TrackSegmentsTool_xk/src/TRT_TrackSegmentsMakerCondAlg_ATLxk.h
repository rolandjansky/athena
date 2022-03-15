/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_TrackSegmentsMakerCondAlg_ATLxk
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_TrackSegmentsMakerCondAlg_ATLxk_H
#define TRT_TrackSegmentsMakerCondAlg_ATLxk_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "TRT_TrackSegmentsTool_xk/TRT_TrackSegmentsToolCondData_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderBounds.h"

#include <vector>

namespace InDet {
  /**
     @class TRT_TrackSegmentsMakerCondAlg_ATLxk
     @author Edson.Carquin.Lopez@cern.ch
  */

  class TRT_TrackSegmentsMakerCondAlg_ATLxk : public AthReentrantAlgorithm
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    TRT_TrackSegmentsMakerCondAlg_ATLxk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TRT_TrackSegmentsMakerCondAlg_ATLxk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };

    void printStraw(const InDetDD::TRT_BaseElement * elementCS, unsigned int strawNum) const;

  private:
      
    ///////////////////////////////////////////////////////////////////
    // Private Data
    ///////////////////////////////////////////////////////////////////

    const TRT_ID                         * m_trtid{}           ;
    
    std::string                            m_fieldmode       ; // jobOption: Magnetic field mode
    ToolHandle<Trk::IPropagator>           m_propTool        ; // Propagator            tool
    Trk::MagneticFieldProperties           m_fieldprop       ; // Magnetic field properties
    float                                  m_pTmin           ;
    float                                  m_Psi128{}          ;
    int                                    m_nMom            ; // number momentum  channel
    float                                  m_A{}               ;
    float                                  m_Psi{}             ;
    int                                    m_nPhi{}            ; // number azimuthal channel

    void magneticFieldInit();

    SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"};
    SG::WriteCondHandleKey<TRT_TrackSegmentsToolCondData_xk> m_writeKey{this, "WriteKey", "TRT_TrackSegmentsToolCondData_xk", "Key of TRT_TrackSegmentsToolCondData_xk"};

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////
    void detElementInformation(const InDetDD::TRT_BaseElement&, double*);
  };

} // end of name space

#endif // TRT_TrackSegmentsMakerCondAlg_ATLxk_H
