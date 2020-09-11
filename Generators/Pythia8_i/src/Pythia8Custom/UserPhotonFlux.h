#ifndef MAIN70EXAMPLE__H
#define MAIN70EXAMPLE__H

#include "Pythia8_i/IPythia8Custom.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "Pythia8/Pythia.h"

using namespace Pythia8;

// Photon flux from lead-ions. Integrated over impact parameters > 2*r_Pb.
// Suitable for photo-nuclear processes but not for photon-photon.
// This should be considered as an experimental setup and used with caution.


// Photon flux from leptons, corresponds to internal Lepton2gamma.

class Lepton2gamma2 : public PDF {

public:

  // Constructor.
  Lepton2gamma2(int idBeamIn) : PDF(idBeamIn) {}

  // Update the photon flux.
  void xfUpdate(int , double x, double Q2) {
    xgamma = 0.5 * 0.007297353080 / M_PI * (1. + pow2(1. - x)) / Q2;
  }
};

class Nucleus2gamma2 : public PDF {

public:

  // Constructor.
  //defaults for m_Z m_min_b=2*nuclear radius are for Pb208 (Z=82, R=6.62fm)
  Nucleus2gamma2(int idBeamIn) : PDF(idBeamIn),
				m_Z(82.),
				m_min_b(13.24),
				m_min_x(-1)
  {}

  // Update the photon flux.
  void xfUpdate(int , double x, double ) {


    if(x < m_min_x)
    {
      xgamma=0;
      return;
    }
    double xi = x * M_NUCLEON_EFF * m_min_b / HBAR_TIMES_C;
    double bK0 = besselK0(xi);
    double bK1 = besselK1(xi);
    double intB = xi * bK1 * bK0 - 0.5 * pow2(xi) * ( pow2(bK1) - pow2(bK0) );
    xgamma = 2. * ALPHA_EM * pow2(m_Z) / M_PI * intB;
  }
  void setZ(double in_Z) {m_Z=in_Z;};
  void setMinB(double in_min_b) {m_min_b=in_min_b;};
  void setMinX(double in_min_x) {m_min_x=in_min_x;};
private:
  double m_Z;
  double m_min_b;
  double m_min_x;


  //GeV equivalent of 1 amu, e.g. mass of nucleon bound in nucleus (not free) = 0.9314 GeV 
  static constexpr double M_NUCLEON_EFF=0.9315;
  static constexpr double ALPHA_EM=7.2973525664e-3;
  static constexpr double HBAR_TIMES_C=0.1973269788; // in GeV x fm

};

/** Tool to implement Pythi8 main70 functionality */
class UserPhotonFlux: public AthAlgTool, virtual public IPythia8Custom {
  
  public:
  
  /** AlgTool style constructor */
  UserPhotonFlux(const std::string&,const std::string&,const IInterface*);
  
  /** Destructor */
  virtual ~UserPhotonFlux(){};
  
  /** AlgTool initialize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();
  
  /** Update the pythia event 
  Probably not used for this application
  */
  StatusCode ModifyPythiaEvent(Pythia8::Pythia& pythia) const;

  /** Return how much the cross section is modified.
   *  Probably not used for this application */
  virtual double CrossSectionScaleFactor() const;

  StatusCode InitializePythiaInfo(Pythia8::Pythia& ) const;

 private:

  int m_process;
  double m_flux_Z;
  double m_flux_min_b;
  double m_flux_min_x;
};

#ifdef PYTHIA_VERSION_INTEGER
  #if PYTHIA_VERSION_INTEGER > 8300
    typedef shared_ptr<PDF> PDFPtr;
    typedef shared_ptr<Nucleus2gamma2> Nucleus2gammaPtr;
    #define PNEW(X, Y) make_shared<X>(Y);
  #else
    typedef Nucleus2gamma2* Nucleus2gammaPtr;
    typedef PDF* PDFPtr;
    #define PNEW(X, Y) new X(Y);
  #endif
#else
  typedef Nucleus2gamma2* Nucleus2gammaPtr;
  typedef PDF* PDFPtr;
  #define PNEW(X, Y) new X(Y);
#endif


#endif
