/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SolenoidTest.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SOLENOIDTEST_H
#define SOLENOIDTEST_H 1

// STL includes
#include <string>
#include <cmath>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// forward declarations
class ITHistSvc;
class TTree;
namespace MagField {
    class IMagFieldSvc;
}

namespace MagField {

    class SolenoidTest : public AthAlgorithm {

    public: 
        SolenoidTest( const std::string& name, ISvcLocator* pSvcLocator );
        virtual ~SolenoidTest();
        StatusCode  initialize();
        StatusCode  execute();
        StatusCode  finalize();

    private: 
        bool checkWithReference();      //!< check current field with reference field
        void getFieldValue();           //!< get Field value either by G4 or by MagFieldSvc
        StatusCode fetchEnvironment();  //!< get environment either for g4 or for magFieldSvc

        ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;        //!< new field svc

        ServiceHandle<ITHistSvc>  m_thistSvc;    //!< the histogram service
        std::string               m_histStream;  //!< THistSvc stream name

        TTree*        m_tree;        //!< the ROOT tree containing the output
        std::string   m_treeName;    //!< name of the Tree object

        double        m_minR;        //!< minimum radius to scan in mm
        double        m_maxR;        //!< maximum radius to scan in mm
        double        m_minZ;        //!< minimum z to scan in mm
        double        m_maxZ;        //!< maximum z to scan in mm

        int           m_stepsR;      //!< the number of steps in R
        int           m_stepsZ;      //!< the number of steps in Z
        int           m_stepsPhi;    //!< the number of steps in phi

        double        m_xyzt[4]{};     //!< stores the current xyzt position
        double        m_field[3]{};    //!< stores the field components
        double        m_fieldZR[3]{};  //!< stores the 2d field components
        double        m_fieldOld[3]{}; //!< stores the old field components
        double        m_deriv[9]{};    //!< stores derivatives
        double        m_derivZR[9]{};  //!< stores derivatives
        double        m_derivOld[9]{}; //!< stores derivatives

        long          m_event;       //!< event counter

        bool          m_useFullField;
        bool          m_useFastField;
        bool          m_useOldField = false;
        bool          m_writeNtuple;
        bool          m_derivatives;

    }; // end class SolenoidTest

} // end namespace MagField

#endif
