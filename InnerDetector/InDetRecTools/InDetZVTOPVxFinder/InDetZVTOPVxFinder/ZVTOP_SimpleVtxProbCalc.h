/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

  class ZVTOP_SimpleVtxProbCalc : public extends<AthAlgTool, IZVTOP_SimpleVtxProbCalc>
    {
    public:
      ZVTOP_SimpleVtxProbCalc(const std::string&,const std::string&,const IInterface*);

       /** default destructor */
      virtual ~ZVTOP_SimpleVtxProbCalc ();
      
       /** standard Athena-Algorithm method */
      virtual StatusCode initialize() override;
       /** standard Athena-Algorithm method */
      virtual StatusCode finalize  () override;
      
      virtual double calcVtxProb(double & trk_func_sum, double & trk_func_sum2) const override;
    }; 
} // end of namespace

#endif 
