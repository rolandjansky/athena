/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICalibrationDataInterfaceTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IANALYSISCALIBRATIONDATAINTERFACETOOL_H
#define IANALYSISCALIBRATIONDATAINTERFACETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <string>

class Jet;

namespace Analysis 
{

  static const InterfaceID IID_ICalibrationDataInterfaceTool("Analysis::ICalibrationDataInterfaceTool", 1, 0);

  class ICalibrationDataInterfaceTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() {
      return IID_ICalibrationDataInterfaceTool;
    }

    /** retrieve either the total uncertainty or only the statistical or systematic components */
    enum Uncertainty { None, Total, Statistical, Systematic };

    // declarations of the interface-defining member functions

    /** efficiency retrieval */
    virtual std::pair<double, double> getEfficiency (const Jet& jet, const std::string& label,
						     const std::string& OP, Uncertainty unc = None) const = 0;

    /** "MC" efficiency retrieval */
    virtual std::pair<double, double> getMCEfficiency (const Jet& jet, const std::string& label,
						       const std::string& OP, Uncertainty unc = None) const = 0;

    /** efficiency scale factor retrieval */
    virtual std::pair<double, double> getScaleFactor (const Jet& jet, const std::string& label,
						      const std::string& OP, Uncertainty unc = None) const = 0;

  };

} // end of namespace

#endif 
