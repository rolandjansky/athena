/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class TRT_DetElementsRoadCondAlg_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef TRT_DetElementsRoadCondAlg_xk_H
#define TRT_DetElementsRoadCondAlg_xk_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsRoadData_xk.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include <vector>

namespace InDet {
  /**
     @class TRT_DetElementsRoadCondAlg_xk
     @author Edson.Carquin.Lopez@cern.ch
  */

  class TRT_DetElementsRoadCondAlg_xk : public AthReentrantAlgorithm
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    TRT_DetElementsRoadCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TRT_DetElementsRoadCondAlg_xk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
    /** Make this algorithm clonable. */
    virtual bool isClonable() const override { return true; };

    void printStraw(const InDetDD::TRT_BaseElement * elementCS, unsigned int strawNum) const;

  private:
      
    ///////////////////////////////////////////////////////////////////
    // Private Data
    ///////////////////////////////////////////////////////////////////
    SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{this, "TRTDetEleContKey", "TRT_DetElementContainer", "Key of TRT_DetElementContainer"};
    SG::WriteCondHandleKey<TRT_DetElementsRoadData_xk> m_writeKey{this, "WriteKey", "TRT_DetElementsRoadData_xk", "Key of TRT_DetElementsRoadData_xk"};

    ///////////////////////////////////////////////////////////////////
    // Methods
    ///////////////////////////////////////////////////////////////////
    void detElementInformation(const InDetDD::TRT_BaseElement&, double*);
  };

} // end of name space

#endif // TRT_DetElementsRoadCondAlg_xk_H
