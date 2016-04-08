/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUINTERFACES_ITOOL_FEATUREEXTRACTOR_H
#define PANTAUINTERFACES_ITOOL_FEATUREEXTRACTOR_H

#include "GaudiKernel/IAlgTool.h"



namespace PanTau {
    class TauFeature;
    class PanTauSeed;
}


namespace PanTau {
    static const InterfaceID IID_ITool_FeatureExtractor("PanTau::ITool_FeatureExtractor", 1, 0);

    /** @class ITool_FeatureExtractor
        @brief Interface for extracting features from tau seeds.

        @author Christian Limbach
        @author Sebastian Fleischmann
    */
    
    class ITool_FeatureExtractor : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID( ) ;
        
        
        //get the features for an input seed
        virtual StatusCode calculateFeatures(PanTau::PanTauSeed* inSeed) = 0;
        
        
    };

    inline const InterfaceID& PanTau::ITool_FeatureExtractor::interfaceID() {
        return IID_ITool_FeatureExtractor;
    }


}
#endif // PANTAUINTERFACES_ITOOL_FEATUREEXTRACTOR_H
