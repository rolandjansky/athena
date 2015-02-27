/** -*- C++ -*-
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
#include <sstream>
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
    virtual StatusCode initialize();

    void handle(const Incident&) override;

    /** retrieve the bcidCorrection value */
    virtual int dynamicPedestal(int iEta, int layer, int pedestal, int iBCID, float mu);

  protected:
    // Base-Class for a mu-, bcid- and eta-parameterization of dynamic pedestal.
    class IParameterization
    {
    public:
      virtual ~IParameterization() {}
      virtual float get(int iEta, int iBCID, float mu) const = 0;
    };

    // Concrete Implementation for the 1st parameterization (correction as function of mu).
    // It is initialized with two parameter per BCID. The dynamic pedestal is caluclated as
    // d = mu * p1[BCID] + mu^2 = p2[BCID]; where p[BCID] is a the parameters for a bcid of BCID.
    class FirstParameterization : virtual public IParameterization
    {
    public:
      FirstParameterization(const std::vector<float>& p1, const std::vector<float>& p2)
        : m_p1(p1)
        , m_p2(p2) {}
      virtual float get(int iEta, int iBCID, float mu) const;
    private:
      std::vector<float> m_p1;  // list of parameters per bcid
      std::vector<float> m_p2;  // list of parameters per bcid
    };

    // Concrete Implementation for the 2nd parameterization.
    // In this case the correction is calculated as
    // d = mu * p1(eta, bcid) + mu^2*p2(eta, bcid), with
    // p1 = q_0[BCID] + q_1[BCID] * eta + q_2[BCID] * eta**2
    // p2 = q_0^'[BCID] + q_1^'[BCID] * eta + q_2^'[BCID] * eta**2
    // Here, q_0, q_1 and q_2 (as well as the primed q's) are parameters for a bcid of BCID
    class SecondParameterization : virtual public IParameterization
    {
    public:
      SecondParameterization(const std::vector<float>& q0, const std::vector<float>& q1, const std::vector<float>& q2,
                             const std::vector<float>& q0p, const std::vector<float>& q1p, const std::vector<float>& q2p)
        : m_q0(q0), m_q1(q1), m_q2(q2)
        , m_q0p(q0p), m_q1p(q1p), m_q2p(q2p) {}
      virtual float get(int iEta, int iBCID, float mu) const;
    private:
      std::vector<float> m_q0, m_q1, m_q2;
      std::vector<float> m_q0p, m_q1p, m_q2p;
    };

    // Stores the concrete implementation of the BCID correction for each eta-slice (the index of the vector).
    // A lot of the eta-slices will share the same SecondParameterization object.
    // However, storing pointers to identical objects multiple times comes at the cost of ~ 2 * 50 * 4 Byte = 400 Bytes
    // of duplicated pointers in memory and saves expensive look-ups.
    // The first entry of the array is for trains after short gaps, the second for trains after long gaps.
    std::array<std::vector<IParameterization*>, 2> m_emParameterizations;
    std::array<std::vector<IParameterization*>, 2> m_hadParameterizations;

    // Since the above vectors store some pointer twice or more often, we need to keep track of the
    // objects that were genuinely allocated to avoid double-freeing memory
    std::vector<IParameterization*> m_allocatedParameterizations;
    
  private:
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
    void parseInputFile(const std::string& fileName, std::vector<IParameterization*>& params);

    static const unsigned s_nElements = 33;
    static const unsigned s_nBCIDPerTrain = 74; // actually 72 filled BCIDs + one before and one after
  }; // end of class L1DynamicPedestalProviderTxt
} // end of namespace

#endif // TRIGT1CALOTOOLS_L1DYNAMICPEDESTALPYTHON_H
