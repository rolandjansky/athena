/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYSORTERID_H
#define INDETTRUTHTOOLS_PRD_TRUTHTRAJECTORYSORTERID_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryManipulator.h"


namespace InDet {
    class PRD_TruthTrajectorySorterID : public AthAlgTool, virtual public Trk::IPRD_TruthTrajectoryManipulator {

    public:
        //** Constructor with parameters */
        PRD_TruthTrajectorySorterID( const std::string& t, const std::string& n, const IInterface* p );
        /** Tool Destructor */
        ~PRD_TruthTrajectorySorterID() {}

        // Athena algtool's Hooks
        StatusCode  initialize();
        StatusCode  finalize();

        virtual bool manipulateTruthTrajectory( Trk::PRD_TruthTrajectory &) const;

    };
}

#endif
