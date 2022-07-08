#ifndef __SI_CLUSTER_H__
#define __SI_CLUSTER_H__

class TrkBaseNode;
class Surface;

class SiCluster
{
  public:
    SiCluster(const Surface*);
    virtual ~SiCluster(void);

    virtual void setParameters(float* par) = 0;
    virtual TrkBaseNode* createDkfNode(void) const = 0;

  protected:
    const Surface* m_pSurface;
};

#endif

