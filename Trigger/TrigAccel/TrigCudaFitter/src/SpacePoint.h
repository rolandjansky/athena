#include <vector>

class SiCluster;

class SpacePoint
{
  public:
    SpacePoint(double, double, double);
    virtual ~SpacePoint(void);

    void addCluster(const SiCluster* pclust);

    std::vector<const SiCluster*> m_clusters;
    double m_x, m_y, m_z, m_r;
};
