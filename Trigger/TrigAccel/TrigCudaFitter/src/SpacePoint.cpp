#include "SiCluster.h"
#include "SpacePoint.h"

#include <math.h>

SpacePoint::SpacePoint(double x, double y, double z) :
  m_x(x), m_y(y), m_z(z), m_r(0.0)
{
	m_clusters.clear();m_r=sqrt(m_x*m_x+m_y*m_y+m_z*m_z);
}

SpacePoint::~SpacePoint(void)
{
	for(std::vector<const SiCluster*>::iterator it=m_clusters.begin();it!=m_clusters.end();++it)
		delete (*it);
}

void SpacePoint::addCluster(const SiCluster* pclust)
{
	m_clusters.push_back(pclust);
}

