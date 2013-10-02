/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrkParametersManipulator, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEVENTUTILS_TRACKPARAMETERSMANIPULATOR_H
#define TRKEVENTUTILS_TRACKPARAMETERSMANIPULATOR_H

//Trk
//#include "TrkParametersBase/ParametersT.h"
//#include "TrkEventPrimitives/Charged.h"
//#include "TrkEventPrimitives/DefinedParameter.h"

namespace Trk {

  class ErrorMatrix;

  /** @class TrkParametersManipulator
       A class to inherit from to be allowed to do coherent manipulations on 
       Trk::TrackParameters and inherited classes

      @author Andreas.Salzburger@cern.ch
    */


   class TrkParametersManipulator {
      
       public:
         /** Default constructor */
         TrkParametersManipulator(){}
      
         /** Virtual destructor*/
         ~TrkParametersManipulator(){}

         /**full manipulate method - the pointer is passed through, usage:

              TrackParameters* tp = createTrackParameters(...);
              CLHEP::HepVector*    newPars  = updateParameters(...);
              ErrorMatrix*  newEm    = updateErrorMatrix(...);
              tp = manipulator.manipulate(tp, newPars, newEm);

             is the valid usage! 
         const ParametersT<Charged>* manipulateParameters(const ParametersT<Charged>* tp,
                                                          CLHEP::HepVector* hvec,
                                                          const ErrorMatrix* em) const;

           */

         /**single manipulate method - the pointer is passed through, usage:

              TrackParameters* tp           = createTrackParameters(...);
              DefinedParameter newAngle     = updateParameters(...);
              ErrorMatrix*  newEm           = 
              tp = manipulator.manipulate(tp, newAngle, newEm);

             is the valid usage! 
         const ParametersT<Charged>* manipulateParameter(const ParametersT<Charged>* tp,
                                                        DefinedParameter dpar,
                                                        const ErrorMatrix* em) const;

           */

  };

/*
inline const ParametersT<Charged>* TrkParametersManipulator::manipulateParameters(const ParametersT<Charged>* tp,
                                                                                  CLHEP::HepVector* hvec,
                                                                                  const ErrorMatrix*em) const
{
    if (!tp && !em) return tp;
     tp->manipulateParameters(hvec,em);
    return tp;
}


inline const ParametersT<Charged>* TrkParametersManipulator::manipulateParameter(const ParametersT<Charged>* tp,
                                                                                 DefinedParameter dpar,
                                                                                 const ErrorMatrix*em) const
{
     tp->manipulateParameter(dpar,em);
    return tp; 
}
*/

}


#endif // TRKEVENTUTILS_TRACKPARAMETERSMANIPULATOR_H
