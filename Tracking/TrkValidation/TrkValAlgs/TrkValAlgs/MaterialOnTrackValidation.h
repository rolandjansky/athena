/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialOnTrackValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MATERIALONTRACKVALIDATION_H
#define MATERIALONTRACKVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>

// Maximal number of updates
#define MAXUPDATES 100

class Identifier;
class TTree;

namespace Trk {

  /** @class MaterialOnTrackValidation

      This is for the Doxygen-Documentation.
      Please delete these lines and fill in information about
      the Algorithm!
      Please precede every member function declaration with a
      short Doxygen comment stating the purpose of this function.

      @author  Markus Haider <markus.haider@student.uibk.ac.at>
  */  

class MaterialOnTrackValidation : public AthAlgorithm {

    public:

      /** Standard Athena-Algorithm Constructor */
      MaterialOnTrackValidation(const std::string& name, ISvcLocator* pSvcLocator);
      /** Default Destructor */
      ~MaterialOnTrackValidation();

      /** standard Athena-Algorithm method */
      StatusCode          initialize();
      /** standard Athena-Algorithm method */
      StatusCode          execute();
      /** standard Athena-Algorithm method */
      StatusCode          finalize();

    private:

      /** properties from JobOptions: */
      std::string    m_inputTrackCollection;

      /** booking the tree */
      void bookTree();
      TTree*  m_Tree;

      /** variables */
      mutable int     m_updates;                //!< number of updates, for max value MAXUPDATES in line 14
      mutable float   m_positionX[MAXUPDATES];  //!< x position of the update
      mutable float   m_positionY[MAXUPDATES];  //!< y position of the update
      mutable float   m_positionZ[MAXUPDATES];  //!< z position of the update
      mutable float   m_positionR[MAXUPDATES];  //!< r value of the material update
      mutable float   m_momentum[MAXUPDATES];   //!< momentum before the update is applied
      mutable float   m_momentumChange[MAXUPDATES];      //!< change of the momentum
      mutable float   m_traversedPathInX0[MAXUPDATES];   //!< accumulated traversed path in rlength
      mutable double  m_currentPathInX0;                 //!< accumulation variable
      mutable float   m_traversedStepInX0[MAXUPDATES];   //!< the step path in x0
      mutable double  m_eta[MAXUPDATES];                 //!< eta
      mutable double  m_stepInX0;                        //!< traversed thickness in radiation length

};

} // end of namespace

#endif 
