/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data object associated with an InDetRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************
 An object of this class stores the simulation information associated with 
 a RawData object, in two data members. 

 One is an integer, the "simulation data word", in which is packed 
 information summarizing the digitization. Its interpretation may depend on 
 which of the three subdetectors is concerned, but will typically contain 
 bit-flags for "noise", "lost in readout", etc.

 The other consists of a number (normally only one) of  pair<int, float> 
 specifying the barcode and energy (charge) deposited by a charged particle, 
 or rather that part of the charge attributed to the corresponding RDO.
 Encoding and decoding methods will be supplied outside this class.

 The implementation emphasizes compactness in memory, at the expense of speed,
 because objects of this class will be persistent, but will not be accessed
 in real production running. 
 
 ***************************************************************************/

// $Id: InDetSimData.h,v 1.15 2004-07-08 20:57:03 costanzo Exp $

#ifndef INDETSIMDATA_InDetSimData_H
# define INDETSIMDATA_InDetSimData_H

//#include "InDetSimData/PixelSimHelper.h"
#include <utility>
#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"

class PixelSimHelper;

class InDetSimData
{
    friend class PixelSimHelper;
    friend class SCT_SimHelper;
    friend class TRT_SimHelper;
    
public:
    typedef std::pair<HepMcParticleLink, float> Deposit; // A particle link, and the 
    // energy (charge) which its hits contribute to the current RDO.
    InDetSimData();
    InDetSimData (const std::vector< Deposit >& deposits, int simDataWord = 0);
    InDetSimData (std::vector< Deposit >&& deposits, int simDataWord = 0);
    InDetSimData (const InDetSimData& other);
    InDetSimData &operator=(const InDetSimData& other);
    InDetSimData &operator=(InDetSimData&& other) noexcept;
    virtual ~InDetSimData();
    int word() const;                           // Get the packed simdata word
    void deposits(std::vector<Deposit>& deposits) const; // Get the Deposits
    //std::vector< Deposit > getdeposits() const; // for some reason I can't get this to return by reference! It won't compile. EJWM 
    const std::vector< Deposit >& getdeposits() const;

private:
    int m_word;   
    //    Deposit* m_p_deposits; but use vector meantime, needs more work
    std::vector<Deposit> m_deposits;
};

inline int InDetSimData::word() const
{
    return m_word & 0x1fffffff;
}

inline const std::vector< InDetSimData::Deposit >& InDetSimData::getdeposits() const
{
  return m_deposits;
}

inline void InDetSimData::deposits(std::vector< InDetSimData::Deposit>& deposits) const
{
  // I really don't understand what the point of this is ... it does EXACTLY the same as the getdeposits() method, just in a different way. EJWM
    deposits = m_deposits;
    return;
}

#endif // INDETSIMDATA_InDetSimData_H
