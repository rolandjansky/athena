/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FATRASRECOTOOLS_ISF_PRDGENERALTRUTHTRAJECTORYSORTERID_H
#define ISF_FATRASRECOTOOLS_ISF_PRDGENERALTRUTHTRAJECTORYSORTERID_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkTruthTrackInterfaces/IPRD_TruthTrajectoryManipulator.h"


namespace iFatras {
    class ISF_PRDGeneralTruthTrajectorySorterID : public AthAlgTool, virtual public Trk::IPRD_TruthTrajectoryManipulator {

    public:
        //** Constructor with parameters */
        ISF_PRDGeneralTruthTrajectorySorterID( const std::string& t, const std::string& n, const IInterface* p );
        /** Tool Destructor */
        ~ISF_PRDGeneralTruthTrajectorySorterID() {}

        // Athena algtool's Hooks
        StatusCode  initialize();
        StatusCode  finalize();

        virtual bool manipulateTruthTrajectory( Trk::PRD_TruthTrajectory &) const;

    };
}

#endif // ISF_FATRASRECOTOOLS_ISF_PRDGENERALTRUTHTRAJECTORYSORTERID_H
