// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   BPhysBlindingTool.h
 * @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
 *
 * @brief  Dual-use tool for blinding and unblinding certain float values
 */

// $Id: $
#ifndef BPHYSTOOLS_BPHYSBLINDINGTOOL_H
#define BPHYSTOOLS_BPHYSBLINDINGTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// System include(s):
#include <memory>

// Local includes
#include "BPhysTools/IBPhysBlindingTool.h"
#include "BPhysTools/SimpleEncrypter.h"


// EDM includes
#include "xAODTracking/VertexAuxContainer.h"

namespace xAOD {
  ///
  /// @class  BPhysBlindingToll
  /// @author Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch>
  ///
  /// Dual-use tool for blinding and unblinding certain float values
  /// provided as variables in a container.
  ///
  /// This tool can be used in two ways:
  /// 1. As a tool to blind or unblind arbitrary positive float values
  ///    using doBlind(float val) or doUnblind(float val).
  ///    For this mode to work only the corresponding key
  ///    (JO BlindingKey or UnblindingKey) needs to be set.
  /// 2. As a tool to blind a list of variables (VarToBlindNames)
  ///    for a certain xAOD::VertexContainer's vertices.
  ///    Since this only works for positive float values,
  ///    an the values may be scaled by a factor and an
  ///    offset may be added to the values, specified separately for
  ///    each variable (BlindingFactors, BlindingOffsets).
  ///    In addition a negative sign may be added to the resulting blinded
  ///    value (NegativeSigns) as a convenience.
  ///    If this tool is used for unblinding the same values for
  ///    BlindingFactors, BlindingOffsets and NegativeSigns need to be
  ///    provided.
  ///    Depending on the mode, the BlindingKey or Unblindingkey need
  ///    to be set.
  ///
  ///    @Note: Key pairs may be produced using the createBlindingKeys
  ///           utility.
  ///
  /// Job options:
  ///  - BlindingKey
  ///    Hex string providing the (public) blinding key.
  ///  - UnblindingKey
  ///    Hex string providing the (private) unblinding key.
  ///  - VertexContainerName
  ///    Name of the vertex container to be used
  ///  - BlindingFlag
  ///    Flag to indicate candidates for blinding ("pass_XXXX")
  ///    Blind values for all candidates if empty.
  ///  - VarToBlindNames
  ///    String with list of float variables to blind (delimiter: .)
  ///  - BlindingOffsets
  ///    Offsets applied to values before blinding
  ///    List must have same length as VarToBlindNames or zero.
  ///  - BlindingFactors
  ///    Scale factors applied before blinding
  ///    List must have same length as VarToBlindNames or zero.
  ///  - NegativeSigns
  ///    Flip signs to negative range?
  ///    List must have same length as VarToBlindNames or zero.
  ///
  ///
  class BPhysBlindingTool :
    public asg::AsgTool, virtual public xAOD::IBPhysBlindingTool {
    
    /// Declare the correct constructor for Athena
    ASG_TOOL_CLASS( BPhysBlindingTool, xAOD::IBPhysBlindingTool )
    
   public:
    ///
    /// @brief Regular AsgTool constructor
    BPhysBlindingTool(const std::string& name = "BPhysBlindingTool");
    ///
    /// @brief Method initialising the tool
    virtual StatusCode initialize() override;

    ///
    /// @brief Method finalizing the tool
    virtual StatusCode finalize() override;

    ///
    /// @brief Simply blind one positive float value
    ///
    /// @param[in] val : positive float value to blind.
    ///
    /// @returns Blinded positive float value; same value as input on error.
    virtual float doBlind(const float& val) override;

    ///
    /// @name Methods to be called by user classes
    /// @{
    ///
    /// @brief Simply unblind one positive float value
    ///
    /// @param[in] val : Blinded positive float value.
    ///
    /// @returns Unblinded positive float value; same value as input on error.
    virtual float doUnblind(const float& val) override;

    ///
    /// @brief Simply blind one (positive) float value with corrections
    ///
    /// @param[in] val          : float value to blind.
    /// @param[in] negativeSign : flip sign after blinding
    /// @param[in] offset       : before blinding, shift by offset
    /// @param[in] factor       : before blinding, stretch by factor
    ///
    /// @returns Blinded float value; same value as input on error.
    virtual float doBlind(const float& val,
                          const bool&  negativeSign,
                          const float& offset,
                          const float& factor) override;

    ///
    /// @name Methods to be called by user classes
    /// @{
    ///
    /// @brief Simply unblind one (positive) float value with corrections
    ///
    /// @param[in] val          : Blinded float value.
    /// @param[in] negativeSign : flip sign before unblinding
    /// @param[in] offset       : after unblinding, shift by offset
    /// @param[in] factor       : after unblinding, stretch by factor
    ///
    /// @returns Unblinded positive float value; same value as input on error.
    virtual float doUnblind(const float& val,
                            const bool&  negativeSign,
                            const float& offset,
                            const float& factor) override;

    /// 
    /// @brief Perform blinding of requested variables
    virtual StatusCode doBlind() override;

    /// 
    /// @brief Perform unblinding of requested variables
    virtual StatusCode doUnblind() override;

  protected:
    ///
    /// @name Perform blinding or unblinding action
    ///
    virtual StatusCode doBlindingAction(bool unblind=false);
    ///
    /// @name Utility methods
    /// @{
    ///
    /// @brief Check whether an element is marked as passing a hypothesis.
    /// 
    virtual bool pass(const SG::AuxElement& em, std::string hypo);
    
    ///
    /// @brief Tokenize a string using certain separators
    ///
    virtual std::vector<std::string> getTokens(std::string input,
                                               std::string seperators);
    ///
    /// @brief Convert vector of floats to string
    ///
    virtual std::string vecToString(const std::vector<float>& v) const;
    ///
    /// @brief Convert vector of bools to string
    ///
    virtual std::string vecToString(const std::vector<bool>& v) const;
    ///
    /// @name Cache current event.
    ///
    virtual StatusCode cacheEvent();
    /// @}
    
  protected:      
    ///
    /// @name Job options
    /// @{
    ///
    /// @brief Vertex container name
    std::string m_vertexContainerName;
    ///
    /// @brief List of variables to blind
    ///
    /// (as concatenated string using . as delimiter) 
    std::string m_varToBlindNames;
    ///
    /// @brief Flag to indicate candidates for blinding
    ///
    /// Left empty: Blind values for all candidates.
    std::string m_blindingFlag;
    /// 
    /// @brief Offsets applied to values before blinding
    ///
    /// List must have same length as VarToBlindNames or zero.
    /// Applied before blinding/after unblinding.
    std::vector<float> m_vOffsets;
    ///
    /// @brief Scale factors applied before blinding
    ///
    /// List must have same length as VarToBlindNames or zero.
    /// Applied before blinding/after unblinding.
    std::vector<float> m_vFactors;
    ///
    /// @brief Flip signs to negative range?
    ///
    /// List must have same length as VarToBlindNames or zero.
    /// Applied after blinding/before unblinding.
    std::vector<bool> m_vNegSigns;
    ///
    /// @brief Key for blinding
    std::string m_blindKey;
    ///
    /// @brief Key for unblinding
    std::string m_unblindKey;
    /// @}

    ///
    /// @name Containers
    /// @{
    xAOD::VertexContainer*    m_vtxContainer;    //!
    xAOD::VertexAuxContainer* m_vtxAuxContainer; //!
    /// @}

    ///
    /// @name Event caching
    /// @{
    int m_cachedRun;    //!
    int m_cachedEvent;  //!
    /// @}

    ///
    ///
    /// @name Counters
    /// @{
    long m_eventsForBlindingSeen;       //!
    long m_candidatesForBlindingSeen;   //!
    long m_eventsForUnblindingSeen;     //!
    long m_candidatesForUnblindingSeen; //!
    long m_eventsBlinded;               //!
    long m_candidatesBlinded;           //!
    long m_eventsUnblinded;             //!
    long m_candidatesUnblinded;         //!
    /// @}
  private:
    ///
    /// @brief Vector of variable names
    ///
    std::vector<std::string> m_vVarNames; //!

    ///
    /// @brief Instance of SimpleEncrypter
    ///
    SimpleEncrypter m_senc; //!
    
  }; // class BPhysBlindingTool

} // namespace xAOD

#endif // BPHYSTOOLS_BPHYSBLINDINGTOOL_H
