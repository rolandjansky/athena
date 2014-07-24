/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  class DummyAnnealingMaker : public AthAlgTool, virtual public IVertexAnnealingMaker
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

   /**
    * Default constructor due to Athena interface
    */
    DummyAnnealingMaker(const std::string& t, const std::string& n, const IInterface*  p);
    
   /**
    * Destructor
    */
    virtual ~DummyAnnealingMaker();

   /**
    * Meaningless
    */
    void reset();
   
   /**
    * Meaningless
    */
    void anneal();
    
   /**
    * Returns 0.5
    */
    double getWeight(double chisq) const;

   /**
    * Always true
    */
    bool isEquilibrium() const 
    {
      return true;
    };
    
   /**
    * Returns 0.
    */
    double actualTemp() const
    {
      return 0;
    };
   
   /**
    * Returns 0.5
    */
    double getWeight(double chisq,const std::vector<double>& allchisq) const;
    
   private:
      
  };
}
#endif
