/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1NSWSimTools/SectorTriggerCandidate.h"
#include "TrigT1NSWSimTools/SingleWedgePadTrigger.h"
#include "TrigT1NSWSimTools/PadOfflineData.h"
#include <cassert>
#include <sstream>

namespace NSWL1{

    SectorTriggerCandidate::SectorTriggerCandidate(const SingleWedgePadTrigger &inner,
                            const SingleWedgePadTrigger &outer){
        m_wedgeTrigs.push_back(inner);
        m_wedgeTrigs.push_back(outer);
    }

    SectorTriggerCandidate::SectorTriggerCandidate(const SingleWedgePadTrigger &innerOrOuterInTransition){
        m_wedgeTrigs.push_back(innerOrOuterInTransition);
    }

    Polygon SectorTriggerCandidate::triggerRegion3() const{
        assert(m_wedgeTrigs.size()>0); // cannot have any trig with any wedge
        bool hasBothInnerOuter(m_wedgeTrigs.size()>1);
        if(hasBothInnerOuter){
            const SingleWedgePadTrigger &inner = m_wedgeTrigs[0];
            const SingleWedgePadTrigger &outer = m_wedgeTrigs[1];
            Polygon inner_overlap=SingleWedgePadTrigger::padOverlap3(inner.pads());
            Polygon outer_overlap=SingleWedgePadTrigger::padOverlap3(outer.pads());
            //project the outer overlap into first one's plane
            float Z1=outer.pads()[0]->m_cornerXyz[1][2];
            float Z0=inner.pads()[0]->m_cornerXyz[1][2];
            //write the outer overlap in Z0 and overlap inner+outer to calculate the overall overlap laplaplap
    
            //mproject ? from Z1->Z0
            return largestIntersection(inner_overlap,Project(outer_overlap,Z1,Z0));
            //end of S.I
        } 
        else{
            return SingleWedgePadTrigger::padOverlap3(m_wedgeTrigs[0].pads());
        }
    }

}
