/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef CALOUTILS_CALOCLUSTEROVERLAPHELPER_H
#define CALOUTILS_CALOCLUSTEROVERLAPHELPER_H

#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <vector>
#include <iostream>
#include "CxxUtils/sincos.h"

struct CaloClusterOverlapHelper 
{
  typedef std::vector<const CaloCell*> cell_list_t;

  /*! @brief Get list of overlapping cells from two clusters */
  static bool getOverlap(const xAOD::CaloCluster* pClus1,
			 const xAOD::CaloCluster* pClus2,
			 double pl,
			 double pr,
			 cell_list_t& cellList);
  /*! @brief Get list of overlapping cells from cell list and cluster */
  static bool getOverlap(const cell_list_t& cells,
			 const xAOD::CaloCluster* pClus,
			 double pl,
			 double pr,
			 cell_list_t& cellList); 

  /*! @brief Get energy fractions for two clusters */
  static bool getOverlap(const xAOD::CaloCluster* pClus1,
			 const xAOD::CaloCluster* pClus2,
			 double pl,
			 double pr,
			 double& ef1,
			 double& ef2);
  /*! @brief Get energy fraction for cell list and cluster */
  static bool getOverlap(const cell_list_t& cells,
			 const xAOD::CaloCluster* pClus,
			 double pl,
			 double pr,
			 double& ef1,
			 double& ef2);
  /*! @brief Get shared energy between two clusters */
  static bool getOverlap(const xAOD::CaloCluster* pClus1,
			 const xAOD::CaloCluster* pClus2,
			 double pl,
			 double pr,
			 double& eShared);
  /*! @brief Get shared energy between list of cells and cluster */
  static bool getOverlap(const cell_list_t& cells,
			 const xAOD::CaloCluster* pClus,
			 double pl, 
			 double pr,
			 double& eShared);
  /*! @brief Get number of cells shared between two clusters */ 
  static bool getOverlap(const xAOD::CaloCluster* pClus1,
			 const xAOD::CaloCluster* pClus2,
			 double pl,
			 double pr,
			 unsigned int& nCells);
  /*! @ brief get number of cells shared between cell list and cluster */
  static bool getOverlap(const cell_list_t& cells,
			 const xAOD::CaloCluster* pClus,
			 double pl,
			 double pr,
			 unsigned int& nCells);
  // direction overlap
  static bool isNear(const xAOD::CaloCluster* pClus1,
		     const xAOD::CaloCluster* pClus2,
		     double deltaR);
  // point and line stuff
  class CaloPoint
  {
  public:
    CaloPoint(double x=0.,double y=0.,double z=0.): m_x(x), m_y(y), m_z(z) { };
    virtual ~CaloPoint() { };

        virtual double x() const { return m_x; }
    virtual double y() const { return m_y; }
    virtual double z() const { return m_z; }

    virtual double distance(const CaloPoint& a) const
    {
      return sqrt( (m_x-a.m_x)*(m_x-a.m_x) +
                   (m_y-a.m_y)*(m_y-a.m_y) +
                   (m_z-a.m_z)*(m_z-a.m_z) );
    }
  protected:
    double m_x;
    double m_y;
    double m_z;

  };
  class CaloLine
  {
  public:
    CaloLine() : 
      m_pt(CaloPoint(0.,0.,0.)), 
      m_dirX(0.), m_dirY(0.0), m_dirZ(0.0)
    { };
    CaloLine(const CaloPoint& aPoint, const CaloPoint& bPoint)
    {
      m_pt   = aPoint;
      const double r(aPoint.distance(bPoint));
      if ( r > 0 )
        {
          const double inv_r = 1./r;
          m_dirX = (bPoint.x()-aPoint.x())*inv_r;
          m_dirY = (bPoint.y()-aPoint.y())*inv_r;
          m_dirZ = (bPoint.z()-aPoint.z())*inv_r;
        }
      else
        {
          m_dirX = 0.;
          m_dirY = 0.;
          m_dirZ = 0.;
        }
      
    }

    virtual ~CaloLine() { };

    virtual double cosAlpha() const { return m_dirX; }
    virtual double cosBeta()  const { return m_dirY; }
    virtual double cosGamma() const { return m_dirZ; }

    virtual double getAlong(const CaloPoint& aPoint)
    {
      return 
        this->cosAlpha()*(aPoint.x()-m_pt.x()) +
        this->cosBeta() *(aPoint.y()-m_pt.y()) +
        this->cosGamma()*(aPoint.z()-m_pt.z());
    }
    
    virtual double getPerp(const CaloPoint& aPoint)
    {
      double t(this->getAlong(aPoint));
      return 
        sqrt(
             ( aPoint.x() - m_pt.x() - this->cosAlpha()*t ) *
             ( aPoint.x() - m_pt.x() - this->cosAlpha()*t ) +
             ( aPoint.y() - m_pt.y() - this->cosBeta() *t ) *
             ( aPoint.y() - m_pt.y() - this->cosBeta() *t ) +
             ( aPoint.z() - m_pt.z() - this->cosGamma()*t ) *
             ( aPoint.z() - m_pt.z() - this->cosGamma()*t ) 
            );
    }

  private:
    CaloPoint  m_pt;
    double m_dirX;
    double m_dirY;
    double m_dirZ;
  };

protected:
  /**/
  template<typename ITER>
  static cell_list_t getOverlapCells(ITER fCell,
				     ITER lCell,
				     const xAOD::CaloCluster* pClus,
				     double pl,double pr);
};

template<typename ITER>
CaloClusterOverlapHelper::cell_list_t 
CaloClusterOverlapHelper::getOverlapCells(ITER fCell,
					  ITER lCell,
					  const xAOD::CaloCluster* pClus,
					  double pl,
					  double pr)
{
  cell_list_t cellList;
  // ----- FIXME!!
  /*CaloLine l(CaloPoint(pClus->getMomentValue(CaloClusterMoment::CENTER_X),
		       pClus->getMomentValue(CaloClusterMoment::CENTER_Y),
		       pClus->getMomentValue(CaloClusterMoment::CENTER_Z)),
	     CaloPoint(0.,0.,0.));
  */
  // -----
  double rad;//(pr*sqrt(pClus->getMomentValue(CaloxAOD::CaloCluster::Moment::SECOND_R)));
  pClus->retrieveMoment(xAOD::CaloCluster::SECOND_R,rad);
  rad*=pr;
    
  double lam;//(pl*sqrt(pClus->getMomentValue(CaloxAOD::CaloCluster::Moment::SECOND_LAMBDA)));
  pClus->retrieveMoment(xAOD::CaloCluster::SECOND_LAMBDA,lam);
  lam*=pl;

  //use new topo cluster moment in rel15 center_mag = sqrt(CENTER_X^2+CENTER_Y^2+CENTER_Z^2) 
  //std::cout << "CENTER_MAG" << pClus->getMomentValue(CaloClusterMoment::CENTER_MAG) <<std::endl;
  // Recalculate here x,y,z of the Cluster 
  double clus_theta = 2 * atan(exp(-pClus->eta()));
  
  double center_mag = 0;
  (void)pClus->retrieveMoment(xAOD::CaloCluster::CENTER_MAG, center_mag);
  
  CxxUtils::sincos sc_phi(pClus->phi());
  CxxUtils::sincos sc_theta(clus_theta);

  //double clus_x = pClus->getMomentValue(xAOD::CaloCluster::CENTER_MAG) * sin(clus_theta) * cos( pClus->phi());
  double clus_x= center_mag * sc_theta.sn * sc_phi.cs;
  //double clus_y = pClus->getMomentValue(xAOD::CaloCluster::CENTER_MAG) * sin(clus_theta) * sin( pClus->phi());
  double clus_y = center_mag * sc_theta.sn * sc_phi.sn;
  //double clus_z = pClus->getMomentValue(xAOD::CaloCluster::CENTER_MAG) * cos(clus_theta);
  double clus_z = center_mag * sc_theta.cs;

  // Recalculate here CaloLine
  CaloLine l(CaloPoint(clus_x, clus_y, clus_z),
	     CaloPoint(0.,0.,0.));
    
  /* for debugging  
  std::cout << "CaloClusterOverlapHelper::getOverlapCells(...): "
	    << "rad = "   << rad
	    << ", lam = " << lam
	    << ", topo cluster (x,y,z) = ("
	    << pClus->getMomentValue(CaloClusterMoment::CENTER_X)
	    << ","
	    << pClus->getMomentValue(CaloClusterMoment::CENTER_Y)
	    << ","
	    << pClus->getMomentValue(CaloClusterMoment::CENTER_Z)
	    << ")"
	    << std::endl;

  std::cout << "clus_x = " << clus_x <<std::endl;
  std::cout << "clus_y = " << clus_y <<std::endl;
  std::cout << "clus_z = " << clus_z <<std::endl;
  */
  
  for ( ; fCell != lCell; fCell++ )
    {
      CaloPoint p((*fCell)->x(),
		  (*fCell)->y(),
		  (*fCell)->z());
      double lambda(l.getAlong(p));
      double radial(l.getPerp(p));
      /* std::cout << "CaloClusterOverlapHelper::getOverlapCells(...): "
	      << "CaloCell (x,y,z) = ("
	      << (*fCell)->x() << ","
	      << (*fCell)->y() << ","
	      << (*fCell)->z() << "), lambda/threshold = "
	      << lambda << "/" << lam << ", radial/threshold = " 
	      << radial << "/" << rad << std::endl;
      */
		  
      if ( fabs(lambda) < lam && radial < rad )
	{
	  cellList.push_back(*fCell);
	}
    }
  return cellList;
}
#endif
