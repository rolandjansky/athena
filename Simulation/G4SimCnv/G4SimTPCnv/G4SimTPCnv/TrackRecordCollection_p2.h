/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKRECORDCOLLECTION_P2_H
#define TRACKRECORDCOLLECTION_P2_H
 
/*
 
Persistent represenation of TrackRecordCollection
Author: Davide Costanzo
 
*/
 
#include <vector>
#include <string>
#include "G4SimTPCnv/TrackRecord_p1.h"
 
 
class TrackRecordCollection_p2
{
public:
    /// typedefs
    typedef std::vector<TrackRecord_p1> HitVector;
    typedef HitVector::const_iterator const_iterator;
    typedef HitVector::iterator       iterator;
                                                                                                                                                            
 
    /// Default constructor
    TrackRecordCollection_p2 ();
 
    // Accessors
    const std::string&  name() const;
    const HitVector&    getVector() const;
private:
    std::vector<TrackRecord_p1>   m_cont;
    std::string m_name;
};
 
 
// inlines
 
inline
TrackRecordCollection_p2::TrackRecordCollection_p2 () {}
 
inline
const std::string&
TrackRecordCollection_p2::name() const
{return m_name;}
 
inline
const std::vector<TrackRecord_p1>&
TrackRecordCollection_p2::getVector() const
{return m_cont;}
 
#endif

