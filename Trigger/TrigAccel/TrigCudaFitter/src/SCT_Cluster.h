#ifndef __SCT_CLUSTER_H__
#define __SCT_CLUSTER_H__

#include "SiCluster.h"

class SCT_Cluster : public SiCluster
{
  public:
    SCT_Cluster(const Surface*);
    virtual ~SCT_Cluster(void);

    virtual void setParameters(float* par) = 0;
    virtual TrkBaseNode* createDkfNode(void) const = 0;

    double m_m;
    double m_cov;
};

#endif
