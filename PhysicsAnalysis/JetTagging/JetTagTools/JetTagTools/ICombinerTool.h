/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_ICOMBINERTOOL_H
#define JETTAGTOOLS_ICOMBINERTOOL_H
/******************************************************
    @class ICombinerTool
    Package : JetTagTools
    Created : January 2005
    Interface class for the CombinerTool AlgTool, it inherits from IAlgTool
    Detailed information about private members and member functions can be found in the actual implementation class
    CombinerTool.

    @author Andreas.Wildauer@cern.ch
********************************************************/

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"
#include <string>
#include <vector>

//class Jet;
//class JetTagInfoBase;

namespace Analysis
{

static const InterfaceID IID_ICombinerTool("Analysis::ICombinerTool", 1, 0);

class ICombinerTool : virtual public IAlgTool {

 public:

       /** Virtual destructor */
       virtual ~ICombinerTool(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_ICombinerTool; };

       /** all tools used same samples (1 signal, N background) to create the LH histograms */
       //virtual std::vector<double> simpleCombine(const JetTagInfoBase*) const=0;
       virtual std::vector<double> simpleCombine(const xAOD::Jet& particleJet, const std::string& infoKey) const=0;

       virtual std::vector<double> simpleCombine(const xAOD::Jet& particleJet,
                                  const std::vector<std::string>& combineTheseTaggers ) const= 0;
                                  

};

}

#endif /// JETTAGTOOLS_ICOMBINERTOOL_H

