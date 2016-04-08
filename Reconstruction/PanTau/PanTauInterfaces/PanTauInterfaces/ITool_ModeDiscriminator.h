/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_MODEDISCRIMINATOR_H
#define PANTAUINTERFACES_ITOOL_MODEDISCRIMINATOR_H

#include "GaudiKernel/IAlgTool.h"


//class Jet;
namespace PanTau {
    class PanTauSeed;
}


namespace PanTau {

    static const InterfaceID IID_ITool_ModeDiscriminator("PanTau::ITool_ModeDiscriminator", 1, 0);

    /** @class ITool_ModeDiscriminator
        @brief Interface for PID from tau seeds.

        @author Sebastian Fleischmann
        @author Christian Limbach
    */
    class ITool_ModeDiscriminator : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID( ) ;

            virtual double getModeLikeliness(PanTau::PanTauSeed* inSeed, bool& wasSuccessful) = 0;
            virtual double getResponse(PanTau::PanTauSeed* inSeed, bool& isOK) = 0;
    };

    
    inline const InterfaceID& PanTau::ITool_ModeDiscriminator::interfaceID() {
        return IID_ITool_ModeDiscriminator;
    }


}
#endif // PANTAUINTERFACES_ITAUDISCRIMINANTTOOL_H
