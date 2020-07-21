/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IFAKE_BKG_TOOL_H
#define IFAKE_BKG_TOOL_H

#include "PATInterfaces/ISystematicsTool.h"
#include "xAODBase/IParticleContainer.h"

#include <string>

class TDirectory;
class TH1;
class TH2;
class TH3;

namespace CP
{

class IFakeBkgSystDescriptor;

/// \brief Top-level interface for the tools providing an estimate of the fake lepton background
/// All the tools, each of them implementing a different method for the estimation, inherit from this interface
class IFakeBkgTool : virtual public CP::ISystematicsTool
{
  public:
 
    /// \brief default value taken by the 'selection' argument of several methods or properties
    /// It indicates how the analysis selects leptons in the signal region
    /// The syntax supported for this arguments is described in the package documentation.
    static constexpr const char* defaultSelection() { return "0!T"; }
    
    /// \brief default value taken by the 'process' argument of several methods or properties
    /// It indicates what should be treated as fake lepton background. 
    /// The syntax supported for this arguments is described in the package documentation.
    static constexpr const char* defaultProcess() { return ">=1F[T]"; }

    /// \brief supply list of leptons / global variables, internal counters incremented
    /// Does not return anything; event weight(s) must be retrieved by subsequent call(s) to getEventWeight() (for tools inheriting from ILinearFakeBkgTool)
    virtual StatusCode addEvent(const xAOD::IParticleContainer& particles, float extraWeight = 1.f) = 0;
    
    /// \brief returns the accumulated fake lepton background yield (or compute it, in the case of the likelihood matrix method), and fills the registered histograms (if any)
    /// The 'selection' and 'process' settings used for these operations can be chosen with the 'Selection' and 'Process' properties.
    /// Only statistical uncertainties can be retrieved; use applySystematicVariation() to obtain uncertainties
    virtual StatusCode getTotalYield(float& yield, float& statErrorUp, float& statErrorDown) = 0;

    /// \brief associates a 1D histogram to the tool, to obtain a binned estimate of the fake lepton background
    /// the 'val' argument points to a variable corresponding to the X axis of the histogram and that needs to be updated before each call to addEvent()
    /// undefined behaviour if the pointers are invalidated during the tool's lifetime
    virtual StatusCode register1DHistogram(TH1* h1, const float *val) = 0;
    
    /// \brief associates a 2D histogram to the tool, to obtain a binned estimate of the fake lepton background
    /// the 'xval' / 'yval' arguments point to variables corresponding to the X/Y axis of the histogram and that need to be updated before each call to addEvent()
    /// undefined behaviour if the pointers are invalidated during the tool's lifetime
    virtual StatusCode register2DHistogram(TH2* h2, const float *xval, const float *yval) = 0;

   /// \brief associates a 3D histogram to the tool, to obtain a binned estimate of the fake lepton background
    /// the 'xval' / 'yval' / 'zval'  arguments point to variables corresponding to the X/Y/Z axis of the histogram and that need to be updated before each call to addEvent()
    /// undefined behaviour if the pointers are invalidated during the tool's lifetime
    virtual StatusCode register3DHistogram(TH3* h3, const float *xval, const float *yval, const float *zval) = 0;

    virtual StatusCode saveProgress(TDirectory* dir) = 0;
    
    /// \brief retrieves an interface to various helper methods to identify what the different SystematicVariations correspond to
    virtual const IFakeBkgSystDescriptor& getSystDescriptor() const = 0;

};

}

#endif
