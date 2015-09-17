/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
		        KullbackLeiblerComponentDistance.h  -  description
			--------------------------------------------------
begin                : Thursday 24th February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class inherited from IComponentSeparationDistance for
                       determining the distance between components based on the
                       Kullback-Leibler metric
*********************************************************************************/

#ifndef TrkKullbackLeiblerComponentSeparation_H
#define TrkKullbackLeiblerComponentSeparation_H

#include "TrkGaussianSumFilter/IComponentSeparationDistance.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace Trk{

class KullbackLeiblerComponentDistance : public AthAlgTool, virtual public IComponentSeparationDistance{
 
 public:
  
  /** Comstructor with AlgTool parameters */
  KullbackLeiblerComponentDistance(const std::string, const std::string, const IInterface*);

  /** Virtual destructor */
  virtual ~KullbackLeiblerComponentDistance() {};

  /** AlgTool initialisation method */
  StatusCode initialize();

  /** AlgTool finalisation method */
  StatusCode finalize();

  /** Interface definition */
  virtual double operator() (const ComponentParameters&, const ComponentParameters&) const;
 
 private:
  int                                m_outputlevel;                      //!< to cache current output level
  bool                               m_do1D;

};

}

#endif
