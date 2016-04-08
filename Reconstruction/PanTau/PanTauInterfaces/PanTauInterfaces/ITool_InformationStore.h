/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_INFORMATIONSTORE_H
#define PANTAUINTERFACES_ITOOL_INFORMATIONSTORE_H
// Gaudi
#include "GaudiKernel/IAlgTool.h"


#include <string>

class eflowObjectContainer;

// namespace Analysis {
//     class TauJetContainer;
//     class TauDetailsContainer;
// }

//! xAOD EDM
#include "xAODTau/TauJetContainer.h"

namespace Rec {
    class TrackParticleContainer;
}


namespace PanTau {



    static const InterfaceID IID_ITool_InformationStore("PanTau::ITool_InformationStore", 1,0);

    /** @class ITool_InformationStore
        @brief Interface for Tool_InformationStore
        @author Christian Limbach (limbach@physik.uni-bonn.de)
    */


    class ITool_InformationStore : virtual public IAlgTool {
        public:
            static const InterfaceID& interfaceID();
            
            virtual StatusCode updateInformation(std::string inputAlg) = 0;
            virtual StatusCode getInfo_Int(std::string varName,     int& value) = 0;
            virtual StatusCode getInfo_Double(std::string varName,  double& value) = 0;
            virtual StatusCode getInfo_VecDouble(std::string varName,  std::vector<double>& value) = 0;
            virtual StatusCode getInfo_String(std::string varName,  std::string& value) = 0;
            virtual StatusCode getInfo_VecString(std::string varName,  std::vector<std::string>& value) = 0;
            
            virtual StatusCode dumpMaps() const = 0;
            
            virtual const eflowObjectContainer*             getContainer_eflowRec() const = 0;
            virtual const xAOD::TauJetContainer*            getContainer_TauRec() const = 0;
            virtual const Rec::TrackParticleContainer*      getContainer_TrackParticle() const = 0;
    };
    

    inline const InterfaceID& PanTau::ITool_InformationStore::interfaceID() {
        return IID_ITool_InformationStore;
    }

}
#endif //PANTAUINTERFACES_ITOOL_INFORMATIONSTORE_H

