#ifndef PROPHECY4FCONTROL_DRM_H
#define PROPHECY4FCONTROL_DRM_H

/* --------------------------------------------------------- */
/*  Prophecy4fMerger.h - Prophecy4f merger with LHE events   */
/*                       from e.g. PowHeg                    */
/*  Formerly called:                                         */
/*  DRM Code for merging POWHEG and Prophecy4f LHEs events   */
/*	M. Duehrrsen, D. Rebuzzi, A. Mueck, A. Salvucci      */
/*	       Second Version: 14 November 2014              */
/*	       Please report any problem to:		     */
/*	         daniela.rebuzzi@cern.ch,                    */
/*	         michael.duehrssen@cern.ch,                  */
/*	         antonio.salvucci@cern.ch		     */
/*               r.d.schaffer@cern.ch                        */
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
#include "TRandom3.h"


class Prophecy4fMerger{

public:
    /* Constructors */
    Prophecy4fMerger();

    /* Destructors */
    virtual ~Prophecy4fMerger();

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
    void setIO(const std::string& powheg,
               const std::string& prophecy4e,
               const std::string& prophecy4mu,
               const std::string& prophecy2e2mu,
               const std::string& outlhe,
               bool debug);

    void setRandomSeed(unsigned long long seed);
    
    /* Merge LHE files */
    void merge();

    /*Verify if POWHEG event has Higgs off-mass shell */
    bool isPHevent(TLorentzVector higgs,
                   TLorentzVector sum_daugh_rest_init);

    /* Set Particles Mass */
    double setParticleMass(int id) const;

    /* Check if File Exists */
    bool fileExists(const std::string& filename);
  
    /* Debug methods */
    void print(const std::string& field);
    void print(const std::string& field, int value);
  
  
protected:
    std::string m_inPowheg;
    std::string m_inProphecy4e;
    std::string m_inProphecy4mu;
    std::string m_inProphecy2e2mu;
    std::string m_outLHE;
    bool m_phEvent;
    bool m_debug;
    TRandom3 m_rand;
    static constexpr double m_deltaM       = 3.;
    static constexpr double m_muonMass     = 0.10566;
    static constexpr double m_electronMass = 0.00051;
    static constexpr double m_tauMass      = 1.777;
    static const long m_electronID      = 11;
    static const long m_muonID          = 13;
    static const long m_tauID           = 15;
    static const long m_zID             = 23;
    static const long m_higgsID         = 25;
    static const long m_photonID        = 22;
    static const long m_neutrinoEl      = 12;
    static const long m_neutrinoMu      = 14;
    static const long m_neutrinoTau     = 16;
  
};
#endif
