/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetModifiedMassDrop.h

#ifndef JetModifiedMassDrop_H
#define JetModifiedMassDrop_H

// Joe Taenzer
// October 2016
//
// Tool to apply the modified mass drop tagger (mMDT) and groom jets by filtering with mMDT
// mMDT is described in this paper: arXiv:1307.0007
// See also: http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/ModifiedMassDropTagger.hh       

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetGroomer.h"
#include "JetInterface/IJetFromPseudojet.h"
#include "AsgTools/ToolHandle.h"

namespace JetRecGroomingParams{
  enum SymmetryMeasure : int;
  enum RecursionChoice : int;
}


class JetModifiedMassDrop
: public asg::AsgTool,
//  public fastjet::contrib::RecursiveSymmetryCutBase,
  virtual public IJetGroomer {
ASG_TOOL_CLASS(JetModifiedMassDrop, IJetGroomer)

public:

  // Equivalent enums to those defined in RecursiveSymmetryToolBase.hh
  // Redefined here so they can be used in the python configuration
  // See: http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/RecursiveSymmetryCutBase.hh
  /// an enum of the different (a)symmetry measures that can be used
  enum SymmetryMeasure{scalar_z, ///< \f$ \min(p_{ti}, p_{tj})/(p_{ti} + p_{tj}) \f$
                              vector_z, ///< \f$ \min(p_{ti}, p_{tj})/p_{t(i+j)} \f$
                              y         ///  \f$ \min(p_{ti}^2,p_{tj}^2) \Delta R_{ij}^2 / m_{ij}^2 \f$
                       
  };

  /// an enum for the options of how to choose which of two subjets to recurse into
  enum RecursionChoice{larger_pt, ///< choose the subjet with larger \f$ p_t \f$
                              larger_mt, ///< choose the subjet with larger \f$ m_t \equiv (m^2+p_t^2)^{\frac12}] \f$
                              larger_m   ///  choose the subjet with larger mass (deprecated)
  };

  // Ctor.
  JetModifiedMassDrop(std::string name);

  // Dtor.
  ~JetModifiedMassDrop();

  // Initilization.
  StatusCode initialize();

  // Groom a jet and add result to a container.
  int groom(const xAOD::Jet& jin, xAOD::JetContainer& jout) const;

  // Dump to log.
  void print() const;

private:  // data

  // Job options.
  float m_zcut;                         // Cut on the symmetry measure
  float m_mu;                           // Mass drop, mu = m_heavy/m_parent
  bool m_doFilt;                        // Filter the jet after mass drop grooming
  double m_filtR;                       // Filter radius to use
  int m_filtNSub;                       // Number of subjets to keep in the filter

  // Symmetry measure to use, either y or scalar_z, default is y
  int m_sym;
  // Recursion choice, larger_pt or larger_mt, default is larger_pt
  int m_recursion;

  // JetRecGroomingParams::SymmetryMeasure test_sym;
  // JetRecGroomingParams::RecursionChoice test_rec;

  ToolHandle<IJetFromPseudojet> m_bld;  // Tool to build jets.

};

#endif
