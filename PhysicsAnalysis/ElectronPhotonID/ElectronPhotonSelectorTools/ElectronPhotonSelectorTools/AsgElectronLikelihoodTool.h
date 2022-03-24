/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGELECTRONLIKELIHOODTOOL__
#define __ASGELECTRONLIKELIHOODTOOL__

// Atlas includes
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "PATCore/AcceptData.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODTracking/VertexContainer.h"

class EventContext;

namespace Root {
class TElectronLikelihoodTool;
}

class AsgElectronLikelihoodTool
  : public asg::AsgTool
  , virtual public IAsgElectronLikelihoodTool
{
  ASG_TOOL_CLASS2(AsgElectronLikelihoodTool,
                  IAsgElectronLikelihoodTool,
                  IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgElectronLikelihoodTool(const std::string& myname);

  /** Standard destructor */
  virtual ~AsgElectronLikelihoodTool();

public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize() override;

  // Main methods for IAsgSelectorTool interface

  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo
      and query what cuts are defined before the first object
      is passed to the tool. */
  virtual const asg::AcceptInfo& getAcceptInfo() const override;

  /** The main accept method: using the generic interface */
  virtual asg::AcceptData accept(
    const xAOD::IParticle* part) const override final;

  virtual asg::AcceptData accept(
    const EventContext& ctx,
    const xAOD::IParticle* part) const override final;

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Electron* eg) const override final
  {
    return accept(ctx, eg, -99); // mu = -99 as input will force accept to grab
                                 // the pileup variable from the xAOD object
  }

  /** The main accept method: the actual cuts are applied here */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Egamma* eg) const override final
  {
    return accept(ctx, eg, -99); // mu = -99 as input will force accept to grab
                                 // the pileup variable from the xAOD object
  }

  /** The main accept method: in case mu not in EventInfo online */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Electron* eg,
                                 double mu) const override final;

  /** The main accept method: in case mu not in EventInfo online */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Egamma* eg,
                                 double mu) const override final;

  // Main methods for IAsgCalculatorTool interface
public:

  virtual double calculate(const EventContext& ctx,
                           const xAOD::IParticle* part) const override final;
  /** The main result method: the actual likelihood is calculated here */
  virtual double calculate(const EventContext& ctx,
                           const xAOD::Electron* eg) const override final
  {
    return calculate(
      ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup
                     // variable from the xAOD object
  }
  /** The main result method: the actual likelihood is calculated here */
  virtual double calculate(const EventContext& ctx,
                           const xAOD::Egamma* eg) const override final
  {
    return calculate(
      ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup
                     // variable from the xAOD object
  }

  /** The main result method: the actual likelihood is calculated here */
  virtual double calculate(const EventContext& ctx,
                           const xAOD::Electron* eg,
                           double mu) const override final;

  /** The main result method: the actual likelihood is calculated here */
  virtual double calculate(const EventContext& ctx,
                           const xAOD::Egamma* eg,
                           double mu) const override final;

  /** The result method for multiple outputs:
      only one output is used so return vector of that output */
  virtual std::vector<float> calculateMultipleOutputs(const EventContext &ctx,
                                                      const xAOD::Electron *eg,
                                                      double mu = -99) const override final
  {
    return {static_cast<float>(calculate(ctx, eg, mu))};
  }

  /// Get the name of the current operating point
  virtual std::string getOperatingPointName() const override final;

  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices(const EventContext& ctx) const;

  /// Get the FCal ET for centrality determination (for HI collisions)
  double getFcalEt(const EventContext& ctx) const;

  /// check for FwdElectron
  bool isForwardElectron(const xAOD::Egamma* eg, const float eta) const;

  // Private member variables
private:
  /** Working Point */
  std::string m_WorkingPoint;

  // The input config file.
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TElectronLikelihoodTool* m_rootTool;

  /// Whether to use the PV (not available for trigger)
  bool m_usePVCont;

  /// defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  /// Whether or not to use the CaloSums container in HI events
  bool m_useCaloSumsCont;

  /// defualt FCal ET (when not using CaloSums container, in HI events)
  double m_fcalEtDefault;

  /// The input ROOT file name that holds the PDFs
  std::string m_pdfFileName;

  /// Flag for calo only LH
  bool m_caloOnly = false;

  /// Flag for skip the use of deltaPoverP in  LH computation (like at HLT)
  bool m_skipDeltaPoverP;

  /// Flag to toggle the correction of deltaEta1 for the pear shape distortion of the LAr
  bool m_correctDeltaEta = false;

  ///  read handle key to heavy ion container
  SG::ReadHandleKey<xAOD::HIEventShapeContainer> m_HIESContKey{
    this,
    "CaloSumsContainer",
    "CaloSums",
    "The CaloSums container name"
  };

  ///  read handle key to primary vertex container
  SG::ReadHandleKey<xAOD::VertexContainer> m_primVtxContKey{
    this,
    "primaryVertexContainer",
    "PrimaryVertices",
    "The primary vertex container name"
  };

}; // End: class definition

#endif

