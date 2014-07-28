// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_MU_MUROADS_H
#define PHYSICS_MU_MUROADS_H


class MuRoadsSvc;

class MuRoads {

public:
    typedef float (*Roads)[12][8];
    typedef float (*MaxRoads)[12];

public:
    ~MuRoads(void);

public:
//#ifdef LVL2_STANDALONE
//    static const MuRoads* instance(void);
//#endif

    Roads roads(void) const;
    MaxRoads max_roads(void) const;

private:
    MuRoads(float si = 0, float sm = 0, float so = 0);

//private:
//    static MuRoads* s_instance;

private:
    float m_roads[2][12][8];
    float m_max_roads[2][12];

    friend class MuRoadsSvc;

};

inline MuRoads::Roads
MuRoads::roads(void) const
{
   // CID 22905: CAST_TO_QUALIFIED_TYPE
   // return (const MuRoads::Roads) m_roads;
   return (MuRoads::Roads) m_roads;
}

inline MuRoads::MaxRoads
MuRoads::max_roads(void) const
{
   // CID 22906: CAST_TO_QUALIFIED_TYPE
   // return (const MuRoads::MaxRoads) m_max_roads;
   return (MuRoads::MaxRoads) m_max_roads;
}

#endif // PHYSICS_MU_MUROAD_H
