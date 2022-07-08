#include "Surface.h"
#include "SiCluster.h"

SiCluster::SiCluster(const Surface* pS) : m_pSurface(pS)
{
}

SiCluster::~SiCluster(void)
{
	delete m_pSurface;
}


