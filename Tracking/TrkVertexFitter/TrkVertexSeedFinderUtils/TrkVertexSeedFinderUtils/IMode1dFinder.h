/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMode1dFinder.h, (c) ATLAS Detector software 2006
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXSEEDFINDERUTILS_IMODE1DFINDER_H
#define TRKVERTEXSEEDFINDERUTILS_IMODE1DFINDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include <vector>


namespace Trk
{

  static const InterfaceID IID_IMODE1DFINDER("IMode1dFinder", 1, 0);

  /**
   @class IMode1dFinder

   Interface class for the algotool which calculates the mode of a 
   unidimensional distribution.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */


  class IMode1dFinder : virtual public IAlgTool {

     public:
       /** Virtual destructor */
       virtual ~IMode1dFinder(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IMODE1DFINDER; };
       
       virtual double getMode(std::vector<Trk::DoubleAndWeight>) const =0;

       virtual double getMode(std::vector<double>) const = 0;
       

  };
}

#endif

