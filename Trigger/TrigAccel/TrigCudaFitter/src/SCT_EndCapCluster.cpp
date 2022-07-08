#include "Surface.h"
#include "SCT_EndCapCluster.h"
#include "TrkBaseNode.h"
#include "TrkFilteringNodes.h"

SCT_EndCapCluster::SCT_EndCapCluster(const Surface* pS) : SCT_Cluster(pS)
  , m_R(0)
{
	m_m=0.0;m_cov=0.0;
}

SCT_EndCapCluster::~SCT_EndCapCluster(void)
{
}

void SCT_EndCapCluster::setParameters(float* par)
{
	m_R=par[0];
	m_m=par[1];
	m_cov=par[2];
}

TrkBaseNode* SCT_EndCapCluster::createDkfNode(void) const
{
	double cov,m,R;
	cov=m_cov;
	m=m_m;
	R=m_R;

	TrkPlanarSurface* pS = m_pSurface->createDkfSurface();
	TrkBaseNode* pN = new TrkEndCapClusterNode(pS,200.0f,R,m,cov);
	return pN;
}
