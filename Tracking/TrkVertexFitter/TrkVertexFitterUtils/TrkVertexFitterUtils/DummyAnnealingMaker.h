/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXADAPIVEFITTERTOOLS_DUMMYANNEALINGMAKER_H
#define TRKVERTEXADAPIVEFITTERTOOLS_DUMMYANNEALINGMAKER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkVertexFitterInterfaces/IVertexAnnealingMaker.h"


/** 
 *  @class Trk::DummyAnnealingMaker
 *
 *  This class implements an annealing maker
 *  but it's a DUMMY one! weight is always 0.5!
 *  Class required for technical reasons only.
 *  Most of methods are therefore menaningless.
 * @author N. Giacinto Piacquadio (for the Freiburg Group)
 */

namespace Trk
{

  class DummyAnnealingMaker : public extends<AthAlgTool, IVertexAnnealingMaker>
  {
  public:
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

   /**
    * Default constructor due to Athena interface
    */
    using base_class::base_class;
    
   /**
    * Meaningless
    */
    virtual void reset(AnnealingState& state) const override;
   
   /**
    * Meaningless
    */
    virtual void anneal(AnnealingState& state) const override;
    
   /**
    * Returns 0.5
    */
    virtual double getWeight(const AnnealingState& state,
                             double chisq) const override;

   /**
    * Always true
    */
    virtual bool isEquilibrium(const AnnealingState& /*state*/) const  override
    {
      return true;
    };
    
   /**
    * Returns 0.
    */
    virtual double actualTemp(const AnnealingState& /*state*/) const override
    {
      return 0;
    };
   
   /**
    * Returns 0.5
    */
    virtual double getWeight(const AnnealingState& state,
                             double chisq,
                             const std::vector<double>& allchisq) const override;
  };
}
#endif
