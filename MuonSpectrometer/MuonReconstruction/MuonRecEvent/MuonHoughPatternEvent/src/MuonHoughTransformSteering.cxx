/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughPattern.h"
#include "MuonHoughPatternEvent/MuonHoughTransformer.h"
#include "MuonHoughPatternEvent/MuonHoughTransformSteering.h"

MuonHoughTransformSteering::MuonHoughTransformSteering(MuonHoughTransformer* houghtransformer)
{
  m_houghtransformer=houghtransformer;
  m_maximum_residu_mm = 1.;
}

MuonHoughTransformSteering::~MuonHoughTransformSteering()
{
  delete m_houghtransformer;
  m_houghtransformer=0;
}

MuonHoughPatternCollection MuonHoughTransformSteering::constructHoughPatterns(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int max_patterns, bool which_segment, int printlevel)const
{
  MuonHoughPatternCollection houghpatterns;
  houghpatterns.reserve(max_patterns);
  std::vector <std::pair <int, int> > maxima = m_houghtransformer->getMaxima(max_patterns); // sector,binnumber , sorted vector
  
  for (unsigned int maximum_number=0; maximum_number<maxima.size(); maximum_number++)
    {
      int binnumber = maxima[maximum_number].second;

      if (binnumber!=-1)
	{
	  int sector = maxima[maximum_number].first;
	  MuonHoughPattern* houghpattern = constructHoughPattern(event,binnumber,residu_mm,residu_grad,sector,which_segment,printlevel);
	  
	  houghpatterns.push_back(houghpattern);
	}
      else 
	{
	  if (printlevel>=4)
	    {
	      std::cout << "binnumber == -1 (no max found), max patterns = " << maximum_number << std::endl;
	    }
	  break;
	}
    }

  // subtract all hits that were just added to a pattern
  //m_houghtransformer->fill(event,true);

  return houghpatterns;
}

MuonHoughPattern* MuonHoughTransformSteering::constructHoughPattern(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int maximum_number, bool which_segment, int printlevel)const
{
  if (printlevel>=3)
    {std::cout << "MuonHoughTransformSteering::constructHoughPattern (start) " << std::endl;}

  MuonHoughPattern* houghpattern = m_houghtransformer->associateHitsToMaximum(event,residu_mm,residu_grad,maximum_number,which_segment,printlevel);
      
  if (printlevel>=3)
    {std::cout << "MuonHoughTransformSteering::constructHoughPattern (end) " << std::endl;}

  return houghpattern;
}

MuonHoughPattern* MuonHoughTransformSteering::constructHoughPattern(const MuonHoughHitContainer* event,std::pair <double,double> coords, double residu_mm,double residu_grad, int sector, bool which_segment, int printlevel)const
{
  MuonHoughPattern* houghpattern = m_houghtransformer->associateHitsToCoords(event,coords,residu_mm,residu_grad,sector,which_segment,printlevel);
  return houghpattern;
}

MuonHoughPattern* MuonHoughTransformSteering::constructHoughPattern(const MuonHoughHitContainer* event,int binnumber, double residu_mm,double residu_grad, int sector, bool which_segment, int printlevel)const
{
  MuonHoughPattern* houghpattern = m_houghtransformer->associateHitsToBinnumber(event,binnumber,residu_mm,residu_grad,sector,which_segment,printlevel);
  return houghpattern;
}


void MuonHoughTransformSteering::fill(const MuonHoughHitContainer* event, bool /**subtract*/)
{
  m_houghtransformer->fill(event);
}

void MuonHoughTransformSteering::resetHisto()
{
  m_houghtransformer->resetHisto();
}
