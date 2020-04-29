/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_G4FieldManagerToolBase_H
#define G4ATLASTOOLS_G4FieldManagerToolBase_H

// Framework includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// G4Atlas includes
#include "G4AtlasInterfaces/IEquationOfMotionTool.h"
#include "G4AtlasInterfaces/IFieldManagerTool.h"
#include "G4AtlasInterfaces/IG4FieldSvc.h"

// Geant4 includes
#include "G4Version.hh"

// Forward declarations
class G4VIntegrationDriver;
class G4MagIntegratorStepper;


/** @class G4FieldManagerToolBase G4FieldManagerToolBase.h "G4AtlasTools/G4FieldManagerToolBase.h"
 *
 *  Tool base class for implementation/retrieval of a G4FieldManager
 *
 *  @author Andrea Dell'Acqua
 *  @date   2015-11-17
 */
class G4FieldManagerToolBase : public extends<AthAlgTool, IFieldManagerTool>
{

  public:
    /// Standard constructor
    G4FieldManagerToolBase(const std::string& type, const std::string& name,
                            const IInterface* parent);

    /// Destructor
    virtual ~G4FieldManagerToolBase(){}

    /// Initialize method
    virtual StatusCode initialize( ) override;

  protected:

#if G4VERSION_NUMBER >= 1040
    /// Common method to construct a driver with a stepper of requested type.
    G4VIntegrationDriver*
    createDriverAndStepper(std::string stepperType, G4MagneticField* field) const;
#endif

#if G4VERSION_NUMBER < 1040
    /// Common method to construct a stepper of requested type.
    G4MagIntegratorStepper*
    getStepper(std::string stepperType, G4MagneticField* field) const;
#endif

    /// Common method to apply configuredfield parameters
    StatusCode setFieldParameters(G4FieldManager* fieldMgr) const;

    /// Handle to the G4 field service
    ServiceHandle<IG4FieldSvc> m_fieldSvc;

    /// @todo TODO why is this duplicated in the g4 field svc?
    bool m_fieldOn;

    /// The type of stepper to use
    std::string m_integratorStepper;

    /// The type of equation of motion to use
    ToolHandle<IEquationOfMotionTool> m_equationOfMotion;
    //std::string m_equationOfMotion;

    /// @name Field parameters
    /// @{
    double m_minEps;
    double m_maxEps;
    double m_deltaChord;
    double m_deltaOneStep;
    double m_deltaIntersection;
    double m_maxStep;
    double m_minStep;
    /// @}

};

#endif // G4ATLASTOOLS_G4FieldManagerToolBase_H
