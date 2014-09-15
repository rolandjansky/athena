/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughHisto2DContainer.h"
#include <iostream>

MuonHoughHisto2DContainer::MuonHoughHisto2DContainer()
{
}

MuonHoughHisto2DContainer::~MuonHoughHisto2DContainer()
{
}

std::pair <int,int> MuonHoughHisto2DContainer::getMaximumBinnumber(unsigned int maximum_number,bool which_segment,int printlevel)const
{
  // does only work for maximum_number==0!
  if (maximum_number!=0)
    {
      std::cout << "WARNING: possibly maximum taken which is not " << maximum_number+1 << "rd maximum" << std::endl; 
    }

  double maximum = 0.;
  int maxid = -1;
  int maxbin = -1;

  //  std::vector < std::pair <int,double> > maxima_vector;

  for (int histoid=0; histoid<size(); histoid++)
    {
      std::pair <int, double> histomax = getHisto(histoid)->getMaximumBin(0,which_segment,printlevel); // binnumber and value of the area of the bin

      //maxima_vector.push_back(histomax);

      if (histomax.second > maximum) { maximum = histomax.second; maxid = histoid; maxbin = histomax.first;}
      
    }

  //  maxima_vector.nth_element();

  std::pair <int,int> coordsmaximum (maxid, maxbin);

  return coordsmaximum;
  
}

void MuonHoughHisto2DContainer::reset()
{
  for (int histoid=0; histoid<size(); histoid++)
    {
      getHisto(histoid)->reset();
    }
}
