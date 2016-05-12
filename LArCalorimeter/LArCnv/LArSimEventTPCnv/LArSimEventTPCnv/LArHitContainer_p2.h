/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITCONTAINER_P2_H
#define LARHITCONTAINER_P2_H
/**
@class LArHitContainer_p2
@brief Persistent represenation of a LArHitContainer,
@author: Ilija Vukotic
*/
#include <vector>
#include <string>

class LArHitContainer_p2   
{
public:

    /// Default constructor
    LArHitContainer_p2 ();

    // Accessors
    const std::string&  name() const; 
    //private:
	std::vector<unsigned int>   m_channelHash; 
	std::vector<unsigned int>	m_energy; // 18 bits compressed
	std::vector<unsigned short>	m_time;	  // store as unsigned short
	std::string              	m_name;
};


// inlines

inline LArHitContainer_p2::LArHitContainer_p2 () {}

inline const std::string& LArHitContainer_p2::name() const
{return m_name;}

#endif
