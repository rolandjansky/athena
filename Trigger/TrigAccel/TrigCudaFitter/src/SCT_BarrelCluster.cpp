#include "Surface.h"
#include "SCT_BarrelCluster.h"
#include "TrkBaseNode.h"
#include "TrkFilteringNodes.h"

SCT_BarrelCluster::SCT_BarrelCluster(const Surface* pS) : SCT_Cluster(pS)
{
	m_m=0.0;m_cov=0.0;
}

SCT_BarrelCluster::~SCT_BarrelCluster(void)
{
}

void SCT_BarrelCluster::setParameters(float* par)
{
	m_m=par[0];m_cov=par[1];
}

TrkBaseNode* SCT_BarrelCluster::createDkfNode(void) const
{
	double cov,m;
	cov=m_cov;
	m=m_m;

	TrkPlanarSurface* pS = m_pSurface->createDkfSurface();
	TrkBaseNode* pN = new TrkClusterNode(pS,200.0f,m,cov);
	return pN;
}
