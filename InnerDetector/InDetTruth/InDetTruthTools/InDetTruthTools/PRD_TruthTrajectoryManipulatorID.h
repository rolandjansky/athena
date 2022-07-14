/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYMANIPULATORID_H
#define INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYMANIPULATORID_H

///////////////////////////////////////////////////////////////////
// PRD_TruthTrajectoryManipulatorID.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryManipulator.h"

class AtlasDetectorID;

namespace InDet {

    /** @class PRD_TruthTrajectoryManipulatorID
    
        --- description what this too does ! ----
        
        
        @author : roland Jansky < email addy >
    
    */
    class PRD_TruthTrajectoryManipulatorID : public AthAlgTool, virtual public Trk::IPRD_TruthTrajectoryManipulator {

    public:
        //** Constructor with parameters */
        PRD_TruthTrajectoryManipulatorID( const std::string& t, const std::string& n, const IInterface* p );
        
        /** Tool Destructor */
        ~PRD_TruthTrajectoryManipulatorID() {}

        // Athena algtool's Hooks
        StatusCode  initialize();
        StatusCode  finalize();

        /** Interface method from IPRD_TruthTrajectoryManipulator */
        virtual bool manipulateTruthTrajectory( Trk::PRD_TruthTrajectory &) const;

    private:
        /**ID pixel helper*/
        const AtlasDetectorID* m_atlasId{};
        Gaudi::Property<int>    m_simBarcodeOffset{this, "SimBarcodeOffset", 200000};
    };
}

#endif
