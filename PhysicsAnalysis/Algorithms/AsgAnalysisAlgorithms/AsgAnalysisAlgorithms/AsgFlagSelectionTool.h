/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/// @author Teng Jian Khoo



#ifndef ASG_ANALYSIS_ALGORITHMS__ASG_FLAG_SELECTION_TOOL_H
#define ASG_ANALYSIS_ALGORITHMS__ASG_FLAG_SELECTION_TOOL_H

#include <AsgTools/AsgTool.h>
#include <PATCore/IAsgSelectionTool.h>
#include <SelectionHelpers/ISelectionAccessor.h>
#include <xAODBase/IParticle.h>
#include <memory>
#include <string>
#include <vector>

namespace CP
{
    /// \brief an \ref IAsgSelectionTool that cuts on char decorations
    ///
    /// For ROOT I/O reasons, chars are commonly used to store boolean
    /// properties, such as flags indicating whether particular criteria
    /// have been passed. This tool provides a conversion of a list
    /// of such decorations, interpreted as bools, into the SelectionType
    /// encoding of TAccept, for use in AnaAlgSequence.
    ///
    /// A list of flags may be provided to the tool, and the output will
    /// simply concatenate them into the TAccept. Each selection criterion
    /// must be passed (no OR behaviour), but the flag logic may be
    /// inverted, such that flags may be encoded as "passX" or !"failX".

    class AsgFlagSelectionTool final
    : public asg::AsgTool, virtual public IAsgSelectionTool
  {
    //
    // public interface
    //

    // Create a proper constructor for Athena
    ASG_TOOL_CLASS( AsgFlagSelectionTool, IAsgSelectionTool )


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AsgFlagSelectionTool (const std::string& name);




    //
    // inherited interface
    //

    virtual StatusCode initialize () override;

    virtual const asg::AcceptInfo& getAcceptInfo( ) const override;

    virtual asg::AcceptData accept( const xAOD::IParticle* part ) const override;



    //
    // private interface
    //

    /// tool properties
    /// \{

    std::vector<std::string> m_selFlags;
    std::vector<bool> m_invertFlags;
    std::vector<std::unique_ptr<ISelectionAccessor> > m_acc_selFlags;

    /// \}


    /// \brief the \ref asg::AcceptInfo we are using
  private:
    asg::AcceptInfo m_accept;
  };
}

#endif
