/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_COMBINERTOOL_H
#define JETTAGTOOLS_COMBINERTOOL_H

/******************************************************
    @class CombinerTool
    Helper class for likelihood calculations

    Package : JetTagTools 
    Created : January 2005

    @author andreas.wildauer@cern.ch
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "JetTagTools/ICombinerTool.h"
#include <string>
#include <vector>
#include "xAODJet/Jet.h"

//class Jet;
class JetTagInfoBase;
//namespace xAOD { class Jet; }

namespace Analysis
{

class CombinerTool : public AthAlgTool, virtual public ICombinerTool
{
 public:

  CombinerTool(const std::string&,const std::string&,const IInterface*);
  virtual ~CombinerTool();

  /** AlgTool initailize method */
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  /** all tools used same samples (1 signal, N background) to create the LH histograms */
  /** Version 1: take only one ITagInfo
  This takes an ITagInfo object created by ONE tag only and calculated the likelihood. Practical if you want
  to calculate the LH of a single tag (remember only the probability is stored in the info objects)
  */
  //virtual std::vector<double> simpleCombine(const JetTagInfoBase*) const;
  virtual std::vector<double> simpleCombine(const xAOD::Jet& particleJet, const std::string& infoKey) const;
  
  /** Version 2: take only one ITagInfo
  Use a subset of all used tag tools. Specify the subset by puting their instance names into
  the vector: combineTheseTaggers.
  */
  virtual std::vector<double> simpleCombine(const xAOD::Jet& particleJet,
                             const std::vector<std::string>& combineTheseTaggers ) const;

};

}
#endif /// JETTAGTOOLS_COMBINERTOOL_H

