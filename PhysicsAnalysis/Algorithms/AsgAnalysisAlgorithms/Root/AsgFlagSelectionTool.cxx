/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 */

/// @author Teng Jian Khoo



//
// includes
//

#include <AsgAnalysisAlgorithms/AsgFlagSelectionTool.h>

#include <cmath>

//
// method implementations
//

namespace CP
{
    AsgFlagSelectionTool ::
    AsgFlagSelectionTool (const std::string& name)
    : AsgTool (name)
    {
      declareProperty ("selectionFlags", m_selFlags, "list of flags to use as selection criteria");
      declareProperty ("invertFlags",    m_invertFlags, "toggles for inverting the selection (index-parallel to selectionFlags)");
    }



    StatusCode AsgFlagSelectionTool ::
    initialize ()
    {
      if(m_invertFlags.size()!=m_selFlags.size() && !m_invertFlags.empty()) {
          ATH_MSG_ERROR("Property invertFlags has different size to selectionFlags. Please check your configuration");
          return StatusCode::FAILURE;
      }
      // Could also warn if there are fewer values, but we don't have to force users to set where irrelevant.
      // Maybe warn unless the size is 0, in which case assume all default?

      for(size_t index=0; index<m_selFlags.size(); ++index) {
          const std::string& thisflag = m_selFlags[index];
          if (thisflag.empty()) {
              ATH_MSG_ERROR("Empty string passed as selection flag!");
              return StatusCode::FAILURE;
          } else {
              // Extend m_invertFlags until the size matches m_selectionFlags
              // Only done in the case that m_invert was empty
              if(m_invertFlags.size()<index+1) {m_invertFlags.push_back(false);}
              std::string doInvertStr = m_invertFlags[index] ? "!" : "";
              m_accept.addCut (doInvertStr + thisflag, doInvertStr + thisflag);
              std::unique_ptr<ISelectionAccessor> accessor;
              ATH_CHECK (makeSelectionAccessor (thisflag, accessor, true));
              m_acc_selFlags.push_back (std::move (accessor));
          }
      }

      return StatusCode::SUCCESS;
    }



    const asg::AcceptInfo& AsgFlagSelectionTool ::
    getAcceptInfo () const
    {
      return m_accept;
    }



    asg::AcceptData AsgFlagSelectionTool ::
    accept (const xAOD::IParticle *particle) const
    {
      asg::AcceptData accept (&m_accept);
      for(std::size_t cutIndex=0; cutIndex<m_accept.getNCuts(); ++cutIndex) {
          // Test against the opposite of the invert value
          bool testval = !m_invertFlags[cutIndex];
          ATH_MSG_VERBOSE("Now testing flag \"" << m_selFlags[cutIndex] << "\" requiring value " << testval);
          accept.setCutResult (cutIndex, m_acc_selFlags[cutIndex]->getBool (*particle)==testval);
      }

      return accept;
    }
}

