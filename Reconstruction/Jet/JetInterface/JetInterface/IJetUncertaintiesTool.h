/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_IJETUNCERTAINTIESTOOL_H
#define JETUNCERTAINTIES_IJETUNCERTAINTIESTOOL_H

#include "AsgTools/IAsgTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include <string>
class TString;

class IJetUncertaintiesTool : virtual public asg::IAsgTool
{
    // Interface declaration
    ASG_TOOL_INTERFACE(IJetUncertaintiesTool)

    public:
        // Retrieve tool information
        virtual std::string getName()         const = 0;
        virtual std::string getJetDef()       const = 0;
        virtual std::string getMCType()       const = 0;
        virtual std::string getConfigFile()   const = 0;
        virtual std::string getPath()         const = 0;
        virtual std::string getAnalysisFile() const = 0;
        virtual float       getRefMu()        const = 0;
        virtual float       getRefNPV()       const = 0;

        // Retrieve component information
        virtual size_t      getNumComponents() const = 0;
        virtual size_t      getComponentIndex(const std::string& name) const = 0;
        virtual size_t      getComponentIndex(const TString& name)     const = 0;
        virtual std::string getComponentName(const size_t index)       const = 0;
        virtual std::string getComponentDesc(const size_t index)       const = 0;

        // Retrieve component scaling information
        virtual bool getComponentScalesFourVec(const size_t index) const = 0;
        virtual bool getComponentScalesPt(const size_t index)      const = 0;
        virtual bool getComponentScalesMass(const size_t index)    const = 0;
        virtual bool getComponentScalesD12(const size_t index)     const = 0;
        virtual bool getComponentScalesD23(const size_t index)     const = 0;
        virtual bool getComponentScalesTau21(const size_t index)   const = 0;
        virtual bool getComponentScalesTau32(const size_t index)   const = 0;

        // Retrieve uncertainty and validity information for a given component
        virtual bool   getValidity(size_t index, const xAOD::Jet& jet) const = 0;
        virtual bool   getValidity(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;
        virtual double getUncertainty(size_t index, const xAOD::Jet& jet) const = 0;
        virtual double getUncertainty(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;
        virtual bool   getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet) const = 0;
        virtual bool   getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;


};

#endif
