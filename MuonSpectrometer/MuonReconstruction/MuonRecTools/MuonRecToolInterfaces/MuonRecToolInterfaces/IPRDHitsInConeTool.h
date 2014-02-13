/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDHITSINCONETOOLS_IPRDHITSINCONETOOL_H
#define PRDHITSINCONETOOLS_IPRDHITSINCONETOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Trk {
    class PrepRawData;
}

namespace Muon {

static const InterfaceID IID_IPRDHitsInConeTool("IPRDHitsInConeTool", 1, 0);
//<! Interface ID

/** This is the abstract for the interface for the PRDHitsInConeTools tool.  Please
*   see the documentation for PRDHitsInConeTools for usage information.
*/
class IPRDHitsInConeTool : virtual public IAlgTool {
    
    public:
    // Interface definition
    static const InterfaceID& interfaceID(); //<! Interface ID

    // Virtual tool public functions
    /** Virtual declaration of the public HitsInCone function.  Returns a vector
    * of hits falling in a cone in the specified direction.  Parameters of the cone
    * may be tuned using setProperty()
    */
    virtual std::vector<Trk::PrepRawData*> *HitsInCone(float dirX, float dirY, float dirZ) = 0; 
    
    /** Virtual declaration of the public HitsInCone function.  Returns a vector
    * of hits falling in a cone in the specified direction.  Parameters of the cone
    * may be tuned using setProperty()
    */
    virtual std::vector<Trk::PrepRawData*> *HitsInCone(float eta, float phi) = 0;

};

inline const InterfaceID& IPRDHitsInConeTool::interfaceID()
{
    return IID_IPRDHitsInConeTool;
}

}

#endif // PRDHITSINCONETOOLS_IPRDHITSINCONETOOLS_H
