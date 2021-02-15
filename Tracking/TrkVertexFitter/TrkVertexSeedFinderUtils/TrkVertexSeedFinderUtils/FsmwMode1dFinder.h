/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERUTILS_FSMWMODE1DFINDERALGO_H
#define TRKVERTEXSEEDFINDERUTILS_FSMWMODE1DFINDERALGO_H

#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <vector>

namespace Trk
{

  /**
   @class FsmwMode1dFinder

   Algotool which calculates the mode of a 
   unidimenensional distribution using the Fraction of Sample Mode with Weights algorithm, 
   which is an iterative Least Median estimator.

   It's like an iterative "Half Sample Mode", but the fraction you take at each 
   step can be configured by the user (in the job option file).

   Configuration possibilities:
   (1) fraction (default is 50 %)

   Details on this and other kinds of mode finders can be found in Waltenberger's PhD thesis, 
   done collaborating with CMS.

   @author Giacinto.Piacquadio@physik.uni-freiburg.de

   */



  class FsmwMode1dFinder : public AthAlgTool, virtual public IMode1dFinder
  {
  public:
    //default constructor due to Athena interface
    FsmwMode1dFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~FsmwMode1dFinder();

    //obtain the 1d-mode (double) from a list of doubles (unidimensional distribution)
    virtual double getMode(std::vector<DoubleAndWeight>) const;

    virtual double getMode(std::vector<double>) const;


        
  private:

    struct CompareTheTwoDoubleAndWeights {
      int operator () ( const DoubleAndWeight & first, const DoubleAndWeight & second ) const {
	return first.first < second.first;
      }
    };
    struct CompareTheTwoDoubles {
      int operator () ( const double & first, const double & second ) const {
	return first < second;
      }
    };
    
    double m_fraction;
    double m_firstfraction;
  };
}
#endif
