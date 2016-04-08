/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_INPUTCONVERTER_H
#define PANTAUINTERFACES_ITOOL_INPUTCONVERTER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

#include <string>


//! xAOD EDM
#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"

namespace PanTau{
    class TauConstituent;
}

class eflowObject;

namespace Analysis {
    class TauPi0Cluster;
    class TauJet;
}

namespace Rec {
    class TrackParticle;
}




namespace PanTau {



    static const InterfaceID IID_ITool_InputConverter("PanTau::ITool_InputConverter", 1,0);

    /** @class ITool_InputConverter
        @brief Interface for Tool_InputConverter
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_InputConverter : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            
            //PFO Converter (r19+)
            virtual StatusCode ConvertToTauConstituent(xAOD::PFO* pfo,
                                                       PanTau::TauConstituent* &tauConstituent,
                                                       const xAOD::TauJet* tauJet,
                                                       std::string algName) const = 0;
            
            virtual bool       passesPreselectionEnergy(double itsEnergy) const = 0;
            
            //cluster based converter
//             virtual StatusCode ConvertToTauConstituent(eflowObject* efo, TauConstituent* &tauConstituent) const;
            
    };
    

    inline const InterfaceID& PanTau::ITool_InputConverter::interfaceID() {
        return IID_ITool_InputConverter;
    }

}
#endif //PANTAUINTERFACES_ITOOL_INPUTCONVERTER_H 
