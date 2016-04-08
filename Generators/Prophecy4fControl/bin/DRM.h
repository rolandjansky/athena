#ifndef DRM_h
#define DRM_h

/* --------------------------------------------------------- */
/*  DRM Code for merging POWHEG and Prophecy4f LHEs events   */
/*	M. Duehrrsen, D. Rebuzzi, A. Mueck, A. Salvucci      */
/*	       Second Version: 14 November 2014              */
/*	       Please report any problem to:		     */
/*	         daniela.rebuzzi@cern.ch,                    */
/*	         michael.duehrssen@cern.ch,                  */
/*	         antonio.salvucci@cern.ch		     */
/* --------------------------------------------------------- */

/* Basic C++ */
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
//#include <utility>
//#include <exception>
/* ROOT */
#include <TLorentzVector.h>
/* Utils */
#include "LHEF.h"

class DRM{

 public:
  /* Constructors */
  DRM();

  /* Destructors */
  virtual ~DRM();

  /* Methods */

  /* --------------------------------------------------------- */
  /* Transforms PI (given in the frame of PS) into PF (in Lab) */
  /*           N.B. P(1,2,3,4) = (PX,PY,PZ,E); PS(5)=M         */
  /* --------------------------------------------------------- */
  int alulb4(double *ps, double *pi, double *pf);

  /* --------------------------------------------------------- */
  /* Transforms PI (given in Lab) into PF (in rest frame of PS */
  /*           N.B. P(1,2,3,4) = (PX,PY,PZ,E); PS(5)=M         */
  /* --------------------------------------------------------- */
  int alulf4(double *ps, double *pi, double *pf);

  /* ---------------------------------------------------------- */
  /* Transforms PI (given in rest frame of PS) into PF (in Lab) */
  /*           N.B. P(1,2,3,4,5) = (PX,PY,PZ,E,M)               */
  /* -----------------------------------------------------------*/
  int alulob(double *ps, double *pi, double *pf);

  /* ---------------------------------------------------------- */
  /* Transforms PI (given in Lab) into PF (in rest frame of PS) */
  /*           N.B. P(1,2,3,4,5) = (PX,PY,PZ,E,M)               */
  /* ---------------------------------------------------------- */
  int alulof(double *ps, double *pi, double *pf);

  /* ---------------------------------------------- */
  /* C.M. Momentum for decay masses EM0-> EM1 + EM2 */
  /*          Set to -1 below threshold             */
  /* ---------------------------------------------- */
  double alupcm(double em0, double em1, double em2);
  
  int rescms(double *p, double *p1, double *p2, double m1, double m2);
  /* Set Input and Output Files */
  void SetIO(std::string powheg, std::string prophecy,
	     std::string outlhe, bool debug);

  /* Merge LHE files */
  void Merge();

  /*Verify if POWHEG event has Higgs off-mass shell */
  bool IsPHevent(TLorentzVector higgs,
		 TLorentzVector sum_daugh_rest_init);

  /* Set Particles Mass */
  std::pair<double,double> SetParticleMass(long *ID);

  /* Check if File Exists */
  bool FileExists(std::string filename);
  
  /* Debug methods */
  void Print(std::string field);
  void Print(std::string field, int value);
  
 protected:
  std::string m_InPowheg;
  std::string m_InProphecy;
  std::string m_OutLHE;
  bool m_PHEvent;
  bool m_Debug;
  static constexpr double m_DeltaM       = 3.;
  static constexpr double m_MuonMass     = 0.10566;
  static constexpr double m_ElectronMass = 0.00051;
  static constexpr double m_TauMass      = 1.777;
  static const long m_ElectronID      = 11;
  static const long m_MuonID          = 13;
  static const long m_TauID           = 15;
  static const long m_ZID             = 23;
  static const long m_HiggsID         = 25;
  static const long m_PhotonID        = 22;
  static const long m_NeutrinoEl      = 12;
  static const long m_NeutrinoMu      = 14;
  static const long m_NeutrinoTau     = 16;
  
};
#endif
