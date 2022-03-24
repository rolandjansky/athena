/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrkVertexFitterValidationUtils/TrkPriVxPurity.h"

namespace Trk
{
 TrkPriVxPurity::TrkPriVxPurity(int ntrk, const std::vector<double>& puWeights, 
                                const std::vector<double>& noTruth, 
                                int brLinks, const std::vector<double>& inWeights, 
		                const std::vector<double>& outWeights):
				m_fittedTracks(ntrk), m_puWeights(puWeights),
				m_noTruth(noTruth),m_brokenLinks(brLinks), 
				m_inWeights(inWeights),
				m_outWeights(outWeights)
 {}


//dump methods
 MsgStream& TrkPriVxPurity::dump(MsgStream& sl) const
 {
  sl<<"***** Primary Vertex Purity Info *****"<<std::endl;
  sl<<"Total number of fitted tracks "<<m_fittedTracks<<std::endl;
  sl<<"Number of track with internally broken links "<<m_brokenLinks<<std::endl;
  if(!m_inWeights.empty())
  {
   sl<<"The weights of inlier tracks:  ";
   std::vector<double>::const_iterator i =  m_inWeights.begin();
   for(;i!=m_inWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  if(!m_outWeights.empty())
  { 
   sl<<"The weights of outlier tracks:  ";
   std::vector<double>::const_iterator i =  m_outWeights.begin();
   for(;i!=m_outWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }

  if(!m_puWeights.empty())
  {
   sl<<"The weights of tracks from PileUp:  ";
   std::vector<double>::const_iterator i =  m_puWeights.begin();
   for(;i!=m_puWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  if(!m_noTruth.empty())
  {
   sl<<"The weights of tracks w/o truth matching:  ";
   std::vector<double>::const_iterator i =  m_noTruth.begin();
   for(;i!=m_noTruth.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  return sl; 
 }// end of msg stream dump method

 std::ostream& TrkPriVxPurity:: dump(std::ostream& sl) const
 {
 sl<<"***** Primary Vertex Purity Info *****"<<std::endl;
  sl<<"Total number of fitted tracks "<<m_fittedTracks<<std::endl;
  sl<<"Number of track with internally broken links "<<m_brokenLinks<<std::endl;
  if(!m_inWeights.empty())
  {
   sl<<"The weights of inlier tracks:  ";
   std::vector<double>::const_iterator i =  m_inWeights.begin();
   for(;i!=m_inWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  if(!m_outWeights.empty())
  { 
   sl<<"The weights of outlier tracks:  ";
   std::vector<double>::const_iterator i =  m_outWeights.begin();
   for(;i!=m_outWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }

  if(!m_puWeights.empty())
  {
   sl<<"The weights of tracks from PileUp:  ";
   std::vector<double>::const_iterator i =  m_puWeights.begin();
   for(;i!=m_puWeights.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  if(!m_noTruth.empty())
  {
   sl<<"The weights of tracks w/o truth matching:  ";
   std::vector<double>::const_iterator i =  m_noTruth.begin();
   for(;i!=m_noTruth.end();++i) sl<<*i<<" ";
   sl<<std::endl;
  }
  
  return sl; 
 }//end of ostream dump method

}//end of namespace definitions
