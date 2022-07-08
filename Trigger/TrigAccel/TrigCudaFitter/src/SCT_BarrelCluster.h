#ifndef __SCT_BARREL_CLUSTER_H__
#define __SCT_BARREL_CLUSTER_H__

#include "SCT_Cluster.h"

class SCT_BarrelCluster : public SCT_Cluster
{
  public:
    SCT_BarrelCluster(const Surface*);
    virtual ~SCT_BarrelCluster(void);

    virtual void setParameters(float* par);
    virtual TrkBaseNode* createDkfNode(void) const;
};
#endif
