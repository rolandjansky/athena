/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAV_AUX	
#define NAV_AUX

class SeedTE
{
        public:
        const HLT::TriggerElement *m_teL1;
        const HLT::TriggerElement *m_teL2;
        unsigned int m_roi;
        SeedTE(const HLT::TriggerElement *teL1, const HLT::TriggerElement *teL2, const unsigned int &roi)
        : m_teL1(teL1), m_teL2(teL2), m_roi(roi)
        {
        }
};

class ConfSeedTE
{
        public:
        TrigConf::HLTTriggerElement *m_te;
        bool m_found;

        ConfSeedTE(TrigConf::HLTTriggerElement *te, const bool found)
        : m_te(te), m_found(found)
        {
        }
};

#endif
