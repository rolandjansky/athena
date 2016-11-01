/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _VrtSecInclusive_IntersectionPos_H
#define _VrtSecInclusive_IntersectionPos_H


// Normal STL and physical vectors
#include <vector>
#include <string>

class TTree;

namespace VKalVrtAthena {

  //------------------------------------------------------------
  class IntersectionPos {
  public:
    IntersectionPos(){};
    virtual ~IntersectionPos(){};
    virtual void clear() = 0;
    virtual void setBranchAddress( TTree*, const char* collectionName = "RecoTrk" ) = 0;
    virtual int bec() = 0;
    virtual const std::string& name() = 0;
    virtual std::vector<double>* x() = 0;
    virtual std::vector<double>* y() = 0;
    virtual std::vector<double>* z() = 0;
    enum { barrel, endcap };
  };
    
    
  //------------------------------------------------------------
  class IntersectionPos_barrel : public IntersectionPos {
  public:
    IntersectionPos_barrel(const char* name, const double& r);
    virtual ~IntersectionPos_barrel();
    void clear();
    void setBranchAddress( TTree*, const char* collectionName = "RecoTrk" );
    int bec();
    inline const std::string& name() { return m_name; }
    inline std::vector<double>* x() { return m_x; }
    inline std::vector<double>* y() { return m_y; }
    inline std::vector<double>* z() { return m_z; }
    inline const double& radius() { return m_radius; }
  private:
    std::string m_name;
    std::vector<double>* m_x;
    std::vector<double>* m_y;
    std::vector<double>* m_z;
    const double m_radius;
  };
  

  //------------------------------------------------------------
  class IntersectionPos_endcap : public IntersectionPos {
  public:
    IntersectionPos_endcap(const char* name, const double& zpos, const double& rmin, const double& rmax);
    virtual ~IntersectionPos_endcap();
    void clear();
    void setBranchAddress( TTree*, const char* collectionName = "RecoTrk" );
    int bec();
    inline const std::string& name() { return m_name; }
    inline std::vector<double>* x() { return m_x; }
    inline std::vector<double>* y() { return m_y; }
    inline std::vector<double>* z() { return m_z; }
    inline const double& zpos() { return m_zpos; }
    inline const double& rmin() { return m_rmin; }
    inline const double& rmax() { return m_rmax; }
  private:
    std::string m_name;
    std::vector<double>* m_x;
    std::vector<double>* m_y;
    std::vector<double>* m_z;
    const double m_zpos;
    const double m_rmin;
    const double m_rmax;
  };
    
}  // end of namespace bracket


#endif /* _VrtSecInclusive_IntersectionPos_H */
