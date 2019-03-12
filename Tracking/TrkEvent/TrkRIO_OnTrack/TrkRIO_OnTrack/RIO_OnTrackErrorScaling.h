/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _RIO_OnTrackErrorScaling_H_
#define _RIO_OnTrackErrorScaling_H_

#include <vector>
#include "EventPrimitives/EventPrimitives.h"

#include "AthenaKernel/CLASS_DEF.h"

class RIO_OnTrackErrorScaling
{
public:
  RIO_OnTrackErrorScaling();
  virtual ~RIO_OnTrackErrorScaling();

  // CLID of the actual implementation
  virtual CLID clid() const;

  std::vector<std::vector<double> >       &params()       { return m_params; }
  const std::vector<std::vector<double> > &params() const { return m_params; }

  virtual bool postProcess()  {return true; }

protected :
  /// Convenience function to check whether the number of parameters is correct.
  /// @param label a label for the error scaling class in case an error is issued.
  /// @param n_paramter_sets the number of parameter sets
  /// @param param_names the names of the parameter sets
  /// @param n_paramters the number of parameters per set.
  void checkParameters(const char *label, unsigned int n_paramter_sets, const char* const* param_names, unsigned int n_paramters) const;
private:
  std::vector<std::vector<double> > m_params;
};

CLASS_DEF( RIO_OnTrackErrorScaling, 167367682 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( RIO_OnTrackErrorScaling, 1528464940 );


#endif
