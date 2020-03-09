// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SCTinformation
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for sct strips information
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 19/04/2018 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SCTinformation_h
#define SCTinformation_h

namespace InDet {

  class SCTinformation {
   
    /////////////////////////////////////////////////////////////////////////////////
    // Public methods:
    /////////////////////////////////////////////////////////////////////////////////
   
  public:
    SCTinformation();
    SCTinformation(const InDet::SiCluster* cluster, 
                   const Amg::Vector3D& strip_start, 
                   const Amg::Vector3D& strip_end, 
                   const Amg::Vector3D& vec, 
                   const double& locx);

    SCTinformation(const SCTinformation&) = default;
    virtual ~SCTinformation() = default;
    SCTinformation& operator = (const SCTinformation&) = default;

    const InDet::SiCluster* cluster() const {return m_cluster;}
    const Amg::Vector3D& strip_center () const {return m_center ;}
    const Amg::Vector3D& strip_direction () const {return m_sdir ;}
    const Amg::Vector3D& traj_direction () const {return m_tdir ;}
    const Amg::Vector3D& normal() const {return m_normal;}
    const double&        oneOverStrip() const {return m_oneOverStrip;}
    const double&        locX() const {return m_locX;}

    double step(SCTinformation&) const;
    Amg::Vector3D position(const double& s) const;
    void set(const InDet::SiCluster* CL,
             const Amg::Vector3D& strip_start,
             const Amg::Vector3D& strip_end,
             const Amg::Vector3D& vec,
             const double& locx);


  private:

    const InDet::SiCluster* m_cluster;     // SCT cluster
    Amg::Vector3D m_center;                // Center of strip
    Amg::Vector3D m_sdir;                  // Direction of strip (strip_start-strip_end)
    Amg::Vector3D m_tdir;                  // Direction of trajectory (strip_start+strip_end-2*vertexVec)
    Amg::Vector3D m_normal;                // Normal to strip diretion and trjectory direction plane
    double        m_oneOverStrip;          // Invert length of the strip
    double        m_locX;                  // cluster local X 
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////
  inline SCTinformation::SCTinformation() : m_cluster(nullptr) {}
  
  /////////////////////////////////////////////////////////////////////////////////
  // Constructor with initialisation
  /////////////////////////////////////////////////////////////////////////////////
 
  inline SCTinformation::SCTinformation (const InDet::SiCluster* cluster, 
                                         const Amg::Vector3D& strip_start, 
                                         const Amg::Vector3D& strip_end, 
                                         const Amg::Vector3D& vec, 
                                         const double& locx) {
    this->set(cluster, strip_start, strip_end, vec, locx);
  }


  /////////////////////////////////////////////////////////////////////////////////
  // Step along strip
  /////////////////////////////////////////////////////////////////////////////////

  inline double SCTinformation::step(SCTinformation& In) const
  {
    // this function returns the ratio between the components of the 
    // In normal vector over the strip direction and trajectory direction.
    
    // first evaluate the denominator:
    // a is component of the In normal vector on the strip direction.
    double a = m_sdir.dot(In.m_normal);
    
    // a = 0 if the In.m_normal and m_sdir are perpendicular.
    // If so, return a very big number.
    // Otherwise, evaluate the numerator and return 
    if (a!=0.) return (-m_tdir.dot(In.m_normal)/a);
    else return 10000.;
  }

  inline Amg::Vector3D SCTinformation::position(const double& s) const
  {
    return (m_center+(0.5*s)*m_sdir);
  }
  inline void  SCTinformation::set (const InDet::SiCluster* cluster,
                                    const Amg::Vector3D& strip_start,
                                    const Amg::Vector3D& strip_end,
                                    const Amg::Vector3D& vec, 
                                    const double& locx) {
    m_cluster = cluster;
    m_center  = 0.5*(strip_start+strip_end); 
    m_sdir    = strip_start-strip_end;       
    m_tdir    = 2.*(m_center-vec);           
    m_normal  = m_sdir.cross(m_tdir);        
    m_oneOverStrip = 1./m_sdir.mag();        
    m_locX = locx;
  }
  
} // end of name space

#endif  // SCTinformation_h
