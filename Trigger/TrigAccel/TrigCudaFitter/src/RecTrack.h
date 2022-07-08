#ifndef __RECTRACK_H__
#define __RECTRACK_H__

#include <vector>

class BFieldPoint;
class SpacePoint;
class DkfTrack;

class RecTrack
{
  public:
    RecTrack(int id);
    virtual ~RecTrack(void);
    void setInitialParameters(float* par);
    void addBFieldPoint(double x, double y, double z, float* B);
    void addSpacePoint(const SpacePoint* psp);

  private:
    int m_algoId;
    double m_params[5];
    std::vector<const BFieldPoint*> m_fieldMap;
    std::vector<const SpacePoint*> m_spacepoints;

  public:
    const std::vector<const SpacePoint*>& getSpacePoints(void) const;
    const double* getParameters(void) const;
    int getAlgoId(void) const;
    const BFieldPoint* getField(int i) const;
    void sortSpacePoints(void);
};
#endif
