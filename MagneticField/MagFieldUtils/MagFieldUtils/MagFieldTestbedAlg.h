/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// G4FieldValidationAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDTESTBEDALG_H
#define MAGFIELDTESTBEDALG_H 1

// STL includes
#include <string>
#include <cmath>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IChronoStatSvc.h"

// forward declarations
class ITHistSvc;
class TTree;
class G4Field;
namespace MagField {
  class IMagFieldSvc;
}


namespace MagField {
  /** @class MagFieldTestbedAlg
     */
  class MagFieldTestbedAlg : public AthAlgorithm {
    public: 
      //** Constructor with parameters */
      MagFieldTestbedAlg( const std::string& name, ISvcLocator* pSvcLocator );

      /** Destructor */
      virtual ~MagFieldTestbedAlg();

      /** Athena algorithm's interface method initialize() */
      StatusCode  initialize();

      /** Athena algorithm's interface method execute() */
      StatusCode  execute();

      /** Athena algorithm's interface method finalize() */
      StatusCode  finalize();

    private: 
      bool        checkWithReference();  //!< check current field with reference field
      void        getFieldValue();					   //!< get Field value either by G4 or by MagFieldSvc
      StatusCode  fetchEnvironment();				   //!< get environment either for g4 or for magFieldSvc

      ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;  //!< service to get vanilla field svc

      const G4Field *p_g4field;								//!< field service from G4

      ServiceHandle<ITHistSvc>        m_thistSvc;    //!< the histogram service
      std::string                     m_histStream;  //!< THistSvc stream name

      ServiceHandle<IChronoStatSvc>        m_chronoSvc;    //!< the chrono service

      TTree                          *m_tree;        //!< the ROOT tree containing the output
      std::string                     m_treeName;    //!< name of the Tree object

      bool                            m_generateBox;       //!< generate box
      double                          m_minX;       //!< the minimum x for box generation
      double                          m_halfX;       //!< the half-lenght along x
      double                          m_halfY;       //!< the half-length along y
      double                          m_halfZ;       //!< the half-length along y

      int                             m_stepsX;      //!< the number of steps in X
      int                             m_stepsY;      //!< the number of steps in Y
      int                             m_stepsZ;      //!< the number of steps in Z
      long							  m_numberOfReadings;//!< random readings instead of grid



      // debug
      int m_referenceCount;

      std::string                     m_refFile;     //!< reference field file name
      std::string                     m_refTreeName; //!< TTree object in reference file
      double                          m_absTolerance;//!< absolute tolerance in field against reference
      double                          m_relTolerance;//!< relative tolerance in field against reference

      double                          m_xyzt[4];     //!< stores the current xyzt position
      double                          m_field[3];    //!< stores the field components
      double						  m_deriv[9];	 //!< stores derivatives TODO: currently only useMagFieldSvc = true
      double 						  m_explicitX;   //!< if value to check is given explicitly
      double 						  m_explicitY;   //!< if value to check is given explicitly
      double 						  m_explicitZ;   //!< if value to check is given explicitly

      
      bool                          m_complete;          //!< validation completed already?
      bool							m_useG4Field;        //!< G4
      bool							m_recordReadings;    //!< record readings (for reproducability) or not (for speed tests)
      bool							m_useDerivatives;    //!< only outer detector values will be tested
      bool 							m_onlyCheckSolenoid; //!< only scan solenoid field
      bool							m_coordsAlongBeam;   //!< coordinates on a beams in random directions
      bool							m_explicitCoords;	 //!< give values to check explicitly


  }; 

} // end namespace MagField

#endif
