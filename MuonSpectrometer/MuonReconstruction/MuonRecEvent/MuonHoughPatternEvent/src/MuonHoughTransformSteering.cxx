/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughPattern.h"
#include "MuonHoughPatternEvent/MuonHoughTransformer.h"
#include "MuonHoughPatternEvent/MuonHoughTransformSteering.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

MuonHoughTransformSteering::MuonHoughTransformSteering(MuonHoughTransformer* houghtransformer)
{
  m_houghtransformer=houghtransformer;
  m_maximum_residu_mm = 1.;
}

MuonHoughTransformSteering::~MuonHoughTransformSteering()
{
  delete m_houghtransformer;
  m_houghtransformer=nullptr;
}

MuonHoughPatternCollection MuonHoughTransformSteering::constructHoughPatterns(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int max_patterns, bool which_segment, int printlevel)const
{
  MsgStream log(Athena::getMessageSvc(),"MuonHoughTransformSteering::constructHoughPatterns");
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
	  if (printlevel>=4 || log.level()<=MSG::VERBOSE)
	    {
	      log << MSG::VERBOSE << "binnumber == -1 (no max found), max patterns = " << maximum_number << endmsg;
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
  MsgStream log(Athena::getMessageSvc(),"MuonHoughTransformSteering::constructHoughPattern");
  if (printlevel>=3 || log.level()<=MSG::DEBUG)
    {log << MSG::DEBUG << "MuonHoughTransformSteering::constructHoughPattern (start) " << endmsg;}
  
  MuonHoughPattern* houghpattern = m_houghtransformer->associateHitsToMaximum(event,residu_mm,residu_grad,maximum_number,which_segment,printlevel);
  
  if (printlevel>=3 || log.level()<=MSG::DEBUG)
    {log << MSG::DEBUG << "MuonHoughTransformSteering::constructHoughPattern (end) " << endmsg;}
  
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
