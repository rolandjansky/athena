/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JPSIUPSILONCOMMON
#define JPSIUPSILONCOMMON

#include <vector>
#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include <algorithm>
#include "xAODTracking/VertexContainer.h"
namespace xAOD{
   class BPhysHelper;
}

namespace Analysis {

   class JpsiUpsilonCommon{
    public:

        static double getPt(const xAOD::TrackParticle*, const xAOD::TrackParticle*);
        static double getPt(const xAOD::TrackParticle* trk1, const xAOD::TrackParticle* trk2, const xAOD::TrackParticle* trk3);
        static double getPt(const xAOD::TrackParticle*, const xAOD::TrackParticle*, const xAOD::TrackParticle*, const xAOD::TrackParticle*);
        static bool   isContainedIn(const xAOD::TrackParticle*, const std::vector<const xAOD::TrackParticle*>&);
        static bool   isContainedIn(const xAOD::TrackParticle*, const xAOD::MuonContainer*);
        static bool   cutRangeOR(const std::vector<double> &values, double min, double max);
        static bool   cutRange(double value, double min, double max);
        static bool   cutAcceptGreaterOR(const std::vector<double> &values, double min);
        static bool   cutAcceptGreater(double value, double min);
        static const xAOD::Vertex* ClosestPV(xAOD::BPhysHelper&, const xAOD::VertexContainer*);
        template< size_t N>
        static bool isContainedIn(const xAOD::TrackParticle*, const std::array<const xAOD::TrackParticle*, N>& );
   };

template< size_t N>
bool JpsiUpsilonCommon::isContainedIn(const xAOD::TrackParticle* t, const std::array<const xAOD::TrackParticle*, N>& cont )
 {
    return std::find(cont.begin(), cont.end(), t);
 }
}

#endif

