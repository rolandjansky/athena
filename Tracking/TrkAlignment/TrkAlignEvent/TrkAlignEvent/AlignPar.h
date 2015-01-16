/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_ALIGNPAR_H
#define TRKALIGNEVENT_ALIGNPAR_H

/** @file AlignPar.h
    @class AlignPar

    @brief AlignPar contains all the information related to an alignment parameter 
    of a particular align module or detector element, including a pointer to the 
    align module/detector element, the value of the alignment parameter with its 
    error, the initial value and error.

    @author Robert Harrington <roberth@bu.edu>
    @date 02/16/2009
*/

#include "TrkAlignEvent/AlignModule.h"
#include "GaudiKernel/MsgStream.h"

namespace Trk {
  
  class AlignPar {

  public:
    
    /** constructor for alignment parameters of AlignModule */
    AlignPar(const AlignModule* alignModule, 
	     AlignModule::TransformParameters paramType);
    
    /** constructor for alignment parameters of TrkDetElementBase */
    AlignPar(const TrkDetElementBase* detElement, 
	     AlignModule::TransformParameters paramType);
    
    virtual ~AlignPar() {}
    
    /** returns the AlignModule */
    const AlignModule* alignModule() const { return m_alignModule; }
    
    /** returns the detElement */
    const TrkDetElementBase* detElement() const { return m_detElement; }
    
    /** returns the type of parameter (i.e. TransX, RotX) */
    AlignModule::TransformParameters paramType() const { return m_paramType; }
    
    /** returns the string representing the type of parameter (i.e. TransX, RotX) */
    std::string dumpType() const;

    /** returns initial parameter and error */
    double initPar() const { return m_initPar; }
    double initErr() const { return m_initErr; }
    
    /** returns final parameter and error */
    double finalPar() const { return m_finalPar; }
    double finalErr() const { return m_finalErr; }
    
    /** returns final parameter and error */
    double par() const { return m_par; }
    double err() const { return m_err; }

    /** returns sigma */
    double sigma() const { return m_sigma; }

    /** returns soft-mode-cut */
    double softCut() const { return m_softCut; }

    /** sets final parameter and error */
    void setPar(double par, double err) { 
      m_par=par; m_err=err; 
      m_finalPar=m_initPar+par; 
      m_finalErr=std::sqrt(m_initErr*m_initErr + err*err); 
    }

    /** index of alignPar in AlignParList owned by AlignModuleList */
    int index() const { return m_index; }

    /** sets initial parameter and error */
    void setInitPar(double par,double err) { m_initPar=par; m_initErr=err; }
    
    /** sets final parameter and error */
    void setFinalPar(double par, double err) { m_finalPar=par; m_finalErr=err; }

    /** sets sigma */
    void setSigma(double sigma) { m_sigma=sigma; }

    /** sets soft-mode-cut */
    void setSoftCut(double softCut) { m_softCut = softCut; }

    /** used by AlignModuleList to set value of index in AlignParList */
    void setIndex(int index) { m_index=index; }

    void setFirstDeriv(double val) { m_firstDeriv=val; }
    void setSecndDeriv(double val) { m_secndDeriv=val; }

    double firstDeriv() const { return m_firstDeriv; }
    double secndDeriv() const { return m_secndDeriv; }

  private:

    const AlignModule*       m_alignModule;
    const TrkDetElementBase* m_detElement;

    AlignModule::TransformParameters m_paramType;

    double m_initPar;
    double m_initErr;
    double m_par;
    double m_err;    
    double m_finalPar;
    double m_finalErr;
    double m_sigma;
    double m_softCut;

    double m_firstDeriv;
    double m_secndDeriv;

    int    m_index;
  };

  /** overload of << operator for MsgStream for debug output */
  MsgStream& operator << (MsgStream& sl, const AlignPar& alignPar);

  inline AlignPar::AlignPar(const AlignModule* alignModule, AlignModule::TransformParameters paramType) : m_alignModule(alignModule), m_detElement(0), m_paramType(paramType), m_initPar(0.), m_initErr(0.), m_par(0.), m_err(0.), m_finalPar(0.), m_finalErr(0.), m_sigma(1.), m_softCut(1.), m_firstDeriv(0.), m_secndDeriv(0.), m_index(-1) {}

  inline AlignPar::AlignPar(const TrkDetElementBase* detElement, AlignModule::TransformParameters paramType) : m_alignModule(0), m_detElement(detElement), m_paramType(paramType), m_initPar(0.), m_initErr(0.), m_par(0.), m_err(0.), m_finalPar(0.), m_finalErr(0.), m_sigma(1.), m_softCut(1.), m_firstDeriv(0.), m_secndDeriv(0.), m_index(-1) {}

}

#endif // TRKALIGNEVENT_ALIGNPAR_H
