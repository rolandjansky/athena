/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_TAUCONSTITUENTGETTER_H
#define PANTAUINTERFACES_ITOOL_TAUCONSTITUENTGETTER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <string>

namespace PanTau{
    class TauConstituent;
}

namespace Analysis {
    class TauJet;
}
#include "xAODTau/TauJet.h"

namespace PanTau {



    static const InterfaceID IID_ITool_TauConstituentGetter("PanTau::ITool_TauConstituentGetter", 1,0);

    /** @class ITool_TauConstituentGetter
        @brief Interface for Tool_TauConstituentGetter
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_TauConstituentGetter : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual StatusCode GetTauConstituents(//const Analysis::TauJet*,
                                                  const xAOD::TauJet* tauJet,
                                                  std::vector<TauConstituent*>& outputList,
                                                  std::string algName) const = 0;
            
    };
    

    inline const InterfaceID& PanTau::ITool_TauConstituentGetter::interfaceID() {
        return IID_ITool_TauConstituentGetter;
    }

}
#endif //PANTAUINTERFACES_ITOOL_TAUCONSTITUENTGETTER_H 
