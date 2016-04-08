/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONTESTS_HIT
#define PERFMONTESTS_HIT 1
namespace PerfMonTest{
  class IHit {
  public:
    virtual ~IHit() {}
  private:
    virtual unsigned int id()=0;
  };

  class DHit : public IHit{
  public: 
    DHit(): 
      IHit(), m_x(0), m_y(0), m_id(0) {}
    //DHit(const DHit& h):
      //IHit(), m_x(h.m_x), m_y(h.m_y), m_id(h.m_id) {}
    DHit(double x, double y, unsigned int id) :
      IHit(), m_x(x), m_y(y), m_id(id) {}
    virtual unsigned int id() { return m_id; }
    //private:
    double m_x;
    double m_y;
    unsigned int m_id;    
  };

  class FHit : public IHit {
  public: 
    FHit():
      IHit(), m_x(0), m_y(0), m_id(0) {}
    //FHit(const FHit& h):
    //  IHit(), m_x(h.m_x), m_y(h.m_y), m_id(h.m_id) {}
    FHit(float x, float y, unsigned short id) :
      IHit(), m_x(x), m_y(y), m_id(id) {}
    virtual unsigned int id() { return m_id; }
    //private:
    float m_x;
    float m_y;
    unsigned short m_id;    
  };

}
#endif
