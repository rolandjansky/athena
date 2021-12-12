/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusterSplitProb.h
//   Header file for class PixelClusterSplitProb
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_PIXELCLUSTERSPLITPROB_H
#define TRKPREPRAWDATA_PIXELCLUSTERSPLITPROB_H

#include <vector>

namespace InDet {
    /** @class PixelClusterSplitProb 

        return object of the IPixelClusterSplitProbTool
            
        @author Andreas.Salzburger@cern.ch  
    */

  class PixelClusterSplitProb {
    
  public :
    /** constructor */
    PixelClusterSplitProb(double splitProbability) 
    {
      m_splitProbabilities.push_back(splitProbability);
    }
    
    /** constructor */
    PixelClusterSplitProb(const std::vector<double>& splitProbabilities) : 
      m_splitProbabilities(splitProbabilities)
    {}

    /** copy constructor */
    PixelClusterSplitProb(const PixelClusterSplitProb& pcp) : 
      m_splitProbabilities(pcp.m_splitProbabilities)
    {}
    
    /** assignment */
    PixelClusterSplitProb & operator=(const PixelClusterSplitProb & rhs){
    	if (&rhs == this) return *this;
    	m_splitProbabilities = rhs.m_splitProbabilities;
    	return *this;
    }
    
    /** destructor */
    ~PixelClusterSplitProb(){}
    
    /** return method : total split probability */
    double splitProbability(unsigned int nParticles=2) const;
 
    /** return method : numberOfProbabilitiesStored*/
    unsigned int getHighestSplitMultiplicityStored() const;


  private :    
    //i element of vector describes probability for splitting into i+1 particles
    std::vector<double>  m_splitProbabilities;
    
  };    
    
  inline double PixelClusterSplitProb::splitProbability(unsigned int nParticles) const 
  {

    if (nParticles<2) return -100;
    if (m_splitProbabilities.size()<nParticles-1) return -99;
    return m_splitProbabilities[nParticles-2];
  }

  inline unsigned int PixelClusterSplitProb::getHighestSplitMultiplicityStored() const
  {
    return m_splitProbabilities.size()+1;
  }
  
}



#endif
