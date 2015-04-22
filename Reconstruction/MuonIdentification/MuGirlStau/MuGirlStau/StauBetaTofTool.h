/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// StauBetaTofTool.h
//   Header file for class MuonPattern
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUON_StauBetaTofTool_H
#define MUON_StauBetaTofTool_H

/**
 @class StauBetaTofTool

 Interface class for tools calculating time reference points
 in space and time to be used by Tracking algorithms
 for events where particles are not timed in with the atlas clock
 (slow particles) or do not originate from the interaction point (cosmics).

 @author shlomit.tarem@cern.ch
 */

#include "MuGirlStau/IStauBetaTofTool.h"

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

namespace MuGirlNS
{
    class StauBetaTofTool: public extends1<AthAlgTool, IStauBetaTofTool>
    {
    public:
        /**AlgTool constructor */
        StauBetaTofTool(const std::string&, const std::string&, const IInterface*);

        /**Virtual destructor*/
        virtual ~StauBetaTofTool()
        {
        }

        /** AlgTool initailize method.*/
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** Calculate time of flight from global position  */
        virtual double timeOfFlight(const Identifier& id, const Amg::Vector3D& pos) const;
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        virtual void setBeta(double beta = 1.)
        {
            m_beta = beta;
        }

        void setTTrack(double tTrack = 0)
        {
            m_tTrack = tTrack;
        }

        void setTShift(double tShift = 0)
        {
            m_tShift = tShift;
        }
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    private:
        double m_inverseSpeedOfLight;
        double m_beta;
        double m_tTrack;
        double m_tShift;
    };
} // namespace MuGirlNS

#endif

