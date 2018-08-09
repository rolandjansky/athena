/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    SCTinformation(const InDet::SiCluster*,const Amg::Vector3D&,const Amg::Vector3D&,const Amg::Vector3D&);
    SCTinformation(const SCTinformation&);
    virtual ~SCTinformation()                 ;
    SCTinformation& operator  = (const SCTinformation&);

    const InDet::SiCluster* cluster() const {return m_CL;}
    const Amg::Vector3D& a () const {return m_A ;}
    const Amg::Vector3D& q () const {return m_Q ;}
    const Amg::Vector3D& s () const {return m_S ;}
    const Amg::Vector3D& qs() const {return m_QS;}
    const double&        qm() const {return m_QM;}

    double step(SCTinformation&) const;
    Amg::Vector3D position(double&) const;

  protected:

    const InDet::SiCluster* m_CL;
    Amg::Vector3D m_A  ;
    Amg::Vector3D m_Q  ;
    Amg::Vector3D m_S  ;
    Amg::Vector3D m_QS ;
    double        m_QM ;
  };


  /////////////////////////////////////////////////////////////////////////////////
  // Inline methods
  /////////////////////////////////////////////////////////////////////////////////

  inline SCTinformation::SCTinformation ()
  {
  }

  inline SCTinformation& SCTinformation::operator = 
    (const SCTinformation& sp) 
    {
      if(&sp!=this) {

        m_CL = sp.m_CL;
        m_A  = sp.m_A ;
        m_Q  = sp.m_Q ;
        m_S  = sp.m_S ;
        m_QS = sp.m_QS;
        m_QM = sp.m_QM;
      }
      return(*this);
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Constructor with initialisation
  /////////////////////////////////////////////////////////////////////////////////
 
  inline  SCTinformation::SCTinformation
    (const InDet::SiCluster* CL,const Amg::Vector3D& a,const Amg::Vector3D& b,const Amg::Vector3D& vec) 
  {
    m_CL = CL            ;
    m_A  = .5*(a+b)      ; // Center of strip
    m_Q  = a-b           ; // Direction of strip      a-b
    m_S  = 2.*(m_A-vec)  ; // Direction of trajectory a+b-2*vertexVec
    m_QS = m_Q.cross(m_S); // Normal to strip diretion and trjectory direction plane
    m_QM = 1./m_Q.mag()  ; // Invert length of the strip
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Copy constructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SCTinformation::SCTinformation (const SCTinformation& sp)
    {
      *this = sp;
    }

  /////////////////////////////////////////////////////////////////////////////////
  // Destructor
  /////////////////////////////////////////////////////////////////////////////////

  inline SCTinformation::~SCTinformation() {}

  /////////////////////////////////////////////////////////////////////////////////
  // Step along strip
  /////////////////////////////////////////////////////////////////////////////////

  inline double SCTinformation::step(SCTinformation& In) const
  {
    double a = m_Q.dot(In.m_QS); 
    if(a!=0.) return (-m_S.dot(In.m_QS)/a); else return 10000.;
  }

  inline Amg::Vector3D SCTinformation::position(double& s) const
  {
    return (m_A+(.5*s)*m_Q);
  }
 
} // end of name space

#endif  // SCTinformation_h
