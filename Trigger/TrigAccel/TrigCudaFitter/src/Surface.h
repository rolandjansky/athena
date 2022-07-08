#ifndef __SURFACE_H__
#define __SURFACE_H__

#include<iostream>

class Surface
{
  public:
    Surface(float*);
    virtual ~Surface(void);

    friend std::ostream& operator << (std::ostream&, const Surface*);
    void addRow(int i, float* row);
    class TrkPlanarSurface* createDkfSurface(void) const;
    long int generateKey() const;

  private:
    double m_center[3];
    double m_normal[3];
    double m_matrix[3][3];
};
#endif

