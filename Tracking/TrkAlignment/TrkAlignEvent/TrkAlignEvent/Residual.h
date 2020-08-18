/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKALIGNEVENT_RESIDUAL_H
#define TRKALIGNEVENT_RESIDUAL_H

/**
   @file Residual.h
   @class Residual

   @brief Residual is a class that stores the residual, error, and type of residual

   @author Robert Harrington <roberth@bu.edu>
   @date 8/29/08
*/

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkAlignEvent/AlignResidualType.h"
#include <cmath>

namespace Trk {

  class Residual {

  public:
    Residual(AlignResidualType resType, AlignMesType mesType, ParamDefs param, double residual, double errSq);
    ~Residual() = default;

    /** returns residual type */
    AlignResidualType residualType() const;

    /** returns measurement type of the residual */
    AlignMesType mesType() const;

    /** returns residual */
    double residual() const;

    /** returns error */
    double error() const;

    /** returns squared error */
    double errSq() const;

    /** returns normalized residual */
    double residualNorm() const;

    /** returns the ParamDef */
    ParamDefs param() const;

  private:
    AlignResidualType m_resType; // type of the residual
    AlignMesType m_mesType; // TSOS type for the residual
    ParamDefs    m_param;
    double       m_residual;
    double       m_errSq;
    double       m_residualNorm;

  }; // end class


  inline double Residual::residual() const { return m_residual; }

  inline double Residual::error() const { return std::sqrt(m_errSq); }
  
  inline double Residual::errSq() const { return m_errSq; }

  inline double Residual::residualNorm() const { return m_residualNorm; }

  inline AlignResidualType Residual::residualType() const { return m_resType; }

  inline AlignMesType Residual::mesType() const { return m_mesType; }
}

#endif // TRKALIGNEVENT_RESIDUAL_H
