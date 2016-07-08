/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ZVTOP_SimpleVtxProbCalc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETZVTOP_SIMPLEVTXPROBCALC_H
#define INDETZVTOP_SIMPLEVTXPROBCALC_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetZVTOPVxFinder/IZVTOP_SimpleVtxProbCalc.h"


namespace InDet 
{

  /** @class ZVTOP_SimpleVtxProbCalc 
      
      @author  Tatjana Lenz <tatjana.lenz@cern.ch>
  */  

  class ZVTOP_SimpleVtxProbCalc : virtual public IZVTOP_SimpleVtxProbCalc, public AthAlgTool
    {
    public:
      ZVTOP_SimpleVtxProbCalc(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_SimpleVtxProbCalc ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
      double calcVtxProb(double & trk_func_sum, double & trk_func_sum2);
    private:

     
      
    }; 
} // end of namespace

#endif 
