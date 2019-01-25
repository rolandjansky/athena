/** -*- C++ -*-*/
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/* 
 * @file TrigT1CaloTools/L1DynamicPedestalProviderTxt.h
 * @author V. Scharf <vscharf@kip.uni-heidelberg.de>
 * @date June 2014
 * @brief Tool to retrieve dynamic pedestal values from a text file.
 */

#ifndef TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPYTHON_H
#define TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPYTHON_H

#include "TrigT1CaloToolInterfaces/IL1DynamicPedestalProvider.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <array>
#include <cstdint> // for guaranteed size-types
#include <memory>
#include <stdexcept>
#include <string>
#include <utility> // std::pair
#include <vector>

namespace Trig { class IBunchCrossingTool; }

namespace LVL1
{
  /**
   * @class ParseException
   */
  class ParseException : public std::runtime_error
  {
  public:
    ParseException(const std::string& what) : std::runtime_error(what)
    {}
  };

  /** 
   * @class L1DynamicPedestalProviderTxt
   * @brief Tool to retrieve dynamic pedestal values from a text file.
   *
   * This tool retrieves the dynamic pedestal value given the trigger tower location and the bcid
   * and mu value of the event.
   * The correction is parameterized as a function of eta, BCID and mu. To reduce the number of 
   * parameters necessary the eta dependence is parameterized in a so-called "second parameterization"
   * for large parts of the detector.
   * The parameters are read from a text file.
   */  
  class L1DynamicPedestalProviderTxt : public AthAlgTool,
                                       virtual public IL1DynamicPedestalProvider,
                                       virtual public IIncidentListener
  {
  public:
    /** constructor */
    L1DynamicPedestalProviderTxt(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~L1DynamicPedestalProviderTxt();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    void handle(const Incident&) override;

    /** retrieve the bcidCorrection value */
    virtual int dynamicPedestal(int iEta, int layer, int pedestal, int iBCID, float mu) override;

    // forward declaration for a function that evaluates the correction as function of mu
    class ParamFunc;
  private:
    
    // Stores the the correction for each eta-slice (the "outer" index of the vector) and bcid (the "inner" index).
    // The first entry of the array is for trains after short gaps, the second for trains after long gaps.
    std::array<std::vector<std::vector<std::unique_ptr<ParamFunc>>>, 2> m_emParameterizations;
    std::array<std::vector<std::vector<std::unique_ptr<ParamFunc>>>, 2> m_hadParameterizations;
    
    //// properties ////
    ToolHandle<Trig::IBunchCrossingTool> m_bunchCrossingTool;

    std::string m_inputFileEMShort;
    std::string m_inputFileHADShort;
    std::string m_inputFileEMLong;
    std::string m_inputFileHADLong;

    // maps the BCID (index) to the distance from the head of the train (after short or long gap)
    // bool: long-gap train (true); short-gap train (false)
    std::vector<std::pair<bool, int16_t>> m_distanceFromHeadOfTrain;

    // fills the vector above with data from the BeamIntensityPattern
    void parseBeamIntensityPattern();

    // parses the input file
    void parseInputFile(const std::string& fileName, std::vector<std::vector<std::unique_ptr<ParamFunc>>>& params);

    static const unsigned s_nElements = 33;
    static const unsigned s_nBCIDPerTrain = 74; // actually 72 filled BCIDs + one before and one after
  }; // end of class L1DynamicPedestalProviderTxt
} // end of namespace

#endif // TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPYTHON_H
