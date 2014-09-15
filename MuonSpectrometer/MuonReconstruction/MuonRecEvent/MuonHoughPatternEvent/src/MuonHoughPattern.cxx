/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternEvent/MuonHoughPattern.h"
#include "CxxUtils/sincos.h"

MuonHoughPattern::MuonHoughPattern(int id_number,bool ownhits):MuonHoughHitContainer(ownhits),m_id_number(id_number),m_whichsegment(false),m_ephi(-MuonHough::Pi/2.),m_erphi(0.),m_etheta(MuonHough::Pi/2.), m_ertheta(0.),m_ecurvature(1.),m_maximumhistogram(0.)
{
}

MuonHoughPattern::~MuonHoughPattern()
{
}

void MuonHoughPattern::resetTracksegment()
{
  for (unsigned int i=0; i<m_hit.size(); i++)
    {
      delete m_hit[i];
      m_hit[i]=0;
    }
  
  m_hit.clear();
}

bool MuonHoughPattern::hitInHoughPattern(MuonHoughHit * hit)const // adviced not to use, slow function
{
  for (unsigned int i = 0; i<size(); i++)
    {
      if (m_hit[i]->getId()==hit->getId())
	{
	  return true;
	}
    }

  return false;
}

double MuonHoughPattern::patternLength()const
{
  // takes the first 2 hits and calculates distance and then takes next hit, and calculates from previous 2 hits which 2 are farthest away, etc..
  // also possible to calculate point closest to IP and determine if left/ right to pattern, app. just as fast.

  double max_patternlength = 0;

  if (m_hit.size()>=2)
    {
      double pattern_length1=0,pattern_length2 = 0;
      int hitno1 = 0;
      int hitno2 = 1;

      std::vector <double> diff(3);
      diff[0] = m_hit[hitno1]->getHitx()-m_hit[hitno2]->getHitx();
      diff[1] = m_hit[hitno1]->getHity()-m_hit[hitno2]->getHity();
      diff[2] = m_hit[hitno1]->getHitz()-m_hit[hitno2]->getHitz();
      max_patternlength = m_muonhoughmathutils.abs(diff);
      
      for (unsigned int i=2; i<m_hit.size(); i++)
	{
	  diff[0] = m_hit[hitno1]->getHitx()-m_hit[i]->getHitx();
	  diff[1] = m_hit[hitno1]->getHity()-m_hit[i]->getHity();
	  diff[2] = m_hit[hitno1]->getHitz()-m_hit[i]->getHitz();
	  pattern_length1 = m_muonhoughmathutils.abs(diff);
	  
	  diff[0] = m_hit[hitno2]->getHitx()-m_hit[i]->getHitx();
	  diff[1] = m_hit[hitno2]->getHity()-m_hit[i]->getHity();
	  diff[2] = m_hit[hitno2]->getHitz()-m_hit[i]->getHitz();
	  pattern_length2 = m_muonhoughmathutils.abs(diff);

	  if (pattern_length1 <= max_patternlength && pattern_length2 <= max_patternlength)
	    {
	      // nothing happens..
	    }
	  else if (pattern_length1 > max_patternlength && pattern_length1 >= pattern_length2)
	    {
	      hitno2 = i;
	      max_patternlength = pattern_length1;
	    }
	  else if (pattern_length2 > max_patternlength && pattern_length2 > pattern_length1)
	    {
	      hitno1 = i;
	      max_patternlength = pattern_length2;
	    }
	}
    }
  else {std::cout << "MuonHoughPattern::pattern_size <2" << std::endl;}

  return max_patternlength;
}

double MuonHoughPattern::calculateEZ()const
{
  double ez = 0;
  double count = 0;
  //  double r0 = m_erphi;
  
  for (unsigned int hitno=0; hitno<m_hit.size(); hitno++)
    {
      // for each hit the distance from the hit to x,y line through (0,0) in the xy plane is calcualted and then distance*cos(theta) gives z_hit - z

      double distance = m_muonhoughmathutils.signedDistanceToLine(getHit(hitno)->getHitx(),getHit(hitno)->getHity(),0,m_ephi - MuonHough::Pi);
      
      double z = getHit(hitno)->getHitz() - (distance * std::cos(m_etheta) / std::sin(m_etheta)); // distance * sin (m_etheta) = L  // - sign correct?
      // hough correction in here?

      // straight line fitter?

      ez += z;
      count++;

    }

  ez = ez / count ;

  return ez;
}

void MuonHoughPattern::printHoughPattern()const
{
  if (m_hit.size()==0)
    {
      std::cout <<"MuonHoughPattern::No pattern_Segment" << std::endl;
    }
  
  std::cout << "MuonHoughPattern::Size of MuonHoughPattern: " << m_hit.size() << std::endl;  
  for (unsigned int i=0; i<m_hit.size(); i++)
    {
      std::cout << m_hit[i]->getHitx() << " " << m_hit[i]->getHity() << " " << m_hit[i]->getHitz() << " " << m_hit[i]->getId() << std::endl;
    } 
}

double MuonHoughPattern::getEAngle()const
{
  double eangle=0;
  switch (m_id_number)
    {
    case MuonHough::hough_xy:
      eangle=m_ephi;
      break;
    case MuonHough::hough_rz: case MuonHough::hough_rzcosmics: case MuonHough::hough_rz_rpc: case MuonHough::hough_rz_mdt: case MuonHough::hough_curved_at_a_cylinder:
      eangle=m_etheta;
      break;
    default:
      std::cout << "MuonHoughPattern::no valid id_number" << std::endl;
    }
  return eangle;
}
      
double MuonHoughPattern::getER()const
{
  double er=0;
  switch (m_id_number)
    {
    case MuonHough::hough_xy:
      er=m_erphi;
      break;
    case MuonHough::hough_rz: case MuonHough::hough_rzcosmics: case MuonHough::hough_rz_rpc: case MuonHough::hough_rz_mdt: case MuonHough::hough_curved_at_a_cylinder:
      er=m_ertheta;
      break;
    default:
      std::cout << "MuonHoughPattern::no valid id_number" << std::endl;
    }
  return er;
}

void MuonHoughPattern::setEAngle(double eangle)
{
  switch (m_id_number)
    {
    case MuonHough::hough_xy:
      m_ephi=eangle;
      break;
    case MuonHough::hough_rz: case MuonHough::hough_rzcosmics: case MuonHough::hough_rz_rpc: case MuonHough::hough_rz_mdt: case MuonHough::hough_curved_at_a_cylinder:
      m_etheta=eangle;
      break;
    default:
      std::cout << "MuonHoughPattern::no valid id_number" << std::endl;
    }
}

void MuonHoughPattern::setER(double er)
{
  switch (m_id_number)
    {
    case MuonHough::hough_xy:
      m_erphi=er;
      break;
    case MuonHough::hough_rz: case MuonHough::hough_rzcosmics: case MuonHough::hough_rz_rpc: case MuonHough::hough_rz_mdt: case MuonHough::hough_curved_at_a_cylinder:
      m_ertheta=er;
      break;
    default:
      std::cout << "MuonHoughPattern::no valid id_number" << std::endl;
    }
}

std::vector <double> MuonHoughPattern::getEPos()const
{
  // similar to StandardAlgs::shortestPointOfLineToOrigin3D

  // should be maybe really shortest point, but

  // problem is that there is no starting point on the line (well this calc. pos could be available) 

  std::vector <double> pos(3);

  pos[0] = m_erphi * std::sin(m_ephi);
  pos[1] = -m_erphi * std::cos(m_ephi);

  if (m_etheta != 0)
    {
      pos[2] = m_ertheta / std::sin(m_etheta);
    }
  else 
    {
      pos[2] = 0;
    }
  return pos;
}

std::vector <double> MuonHoughPattern::getEDir()const
{
  std::vector <double> dir(3);

  dir[0]= std::cos(m_ephi)*std::sin(m_etheta);
  dir[1]= std::sin(m_ephi)*std::sin(m_etheta);
  dir[2]= std::cos(m_etheta);

  return dir;
}

void MuonHoughPattern::updateParametersRPhi(bool cosmics)
{
  if (size()<=0) return;

  double sum_x = 0.;
  double sum_y = 0.;

  int hitsno = size();

  for (unsigned int i=0; i<size(); i++)
    {
      sum_x += getHitx(i); //*getOrigWeight(i);
      sum_y += getHity(i); //*getOrigWeight(i);
    }

  // adding interaction point constraint:
  if (cosmics == false || size()==1) hitsno += size();
  
  const double av_x = sum_x / (hitsno+0.);
  const double av_y = sum_y / (hitsno+0.);
  
  double sumx = 0.;
  double sumy = 0.;
  for (unsigned int i=0; i<size(); i++)
    {
      double hitx = getHitx(i);
      double hity = getHity(i);
      double x_offset = hitx - av_x;
      double y_offset = hity - av_y;
      double weight = (x_offset*x_offset + y_offset*y_offset); //*getOrigWeight(i);
      int sign = 1;
      if (cosmics==false) {
	if (x_offset*hitx+y_offset*hity<0) {sign =-1;}
      }
      else {
	if (y_offset<0) {sign =-1;} // assume cosmic muon comes from above
      }
      sumx += weight*sign*x_offset;
      sumy += weight*sign*y_offset;
    }
  // adding interaction point (count size)
  if (cosmics==false) {
    double weight = av_x*av_x + av_y*av_y;
    sumx += size() * weight * av_x;
    sumy += size() * weight * av_y;
  }

  if (std::abs(sumx) < 0.000001 || std::abs(sumy) < 0.000001) {
    // {std::cout << " sum too small to update" << std::endl;}
    return;
  }

  double phi = std::atan2(sumy,sumx);
  if (cosmics == true) {
    if (phi > 0) phi -= MuonHough::Pi; // phi between 0,-Pi for cosmics! 
  }

  //  std::cout << "phi: << : " << phi << std::endl;
  CxxUtils::sincos scphi(phi);
  
  const double r0 = scphi.apply(av_x,-av_y); //av_x * sincosphi[0] - av_y * sincosphi[1];

  setEPhi(phi);
  setERPhi(r0);
}
