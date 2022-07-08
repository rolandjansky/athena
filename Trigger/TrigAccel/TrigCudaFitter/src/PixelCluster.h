#ifndef __PIXEL_CLUSTER_H__
#define __PIXEL_CLUSTER_H__

#include "SiCluster.h"

class PixelCluster : public SiCluster
{
  public:
    PixelCluster(const Surface*);
    virtual ~PixelCluster(void);

    double m_m[2];
    double m_cov[2][2];

  public:
    virtual void setParameters(float* par);
    virtual TrkBaseNode* createDkfNode(void) const;
};
#endif
