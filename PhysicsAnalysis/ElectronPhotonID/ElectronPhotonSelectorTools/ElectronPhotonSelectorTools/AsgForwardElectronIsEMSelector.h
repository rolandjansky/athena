/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGFORWARDELECTRONISEMSELECTOR__
#define __ASGFORWARDELECTRONISEMSELECTOR__

/**
   @class AsgElectronIsEMSelector
   @brief Tool to select objects.

   @author Jovan Mitrevski (UCSC) Karsten Koeneke (CERN)
   @date   Dec 2011 - Fab 2012

   Based on egammaElectronCutIDTool, by F. Derue.

   11-MAR-2014, convert to ASGTool (Jovan Mitrevski)

*/

// Atlas includes
#include "AsgTools/AsgTool.h"

// Include the interfaces
#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"

#include "AsgDataHandles/ReadHandleKey.h"
#include "xAODTracking/VertexContainer.h"

#include <string>

class EventContext;

namespace Root {
class TForwardElectronIsEMSelector;
}

class AsgForwardElectronIsEMSelector
  : public asg::AsgTool
  , virtual public IAsgForwardElectronIsEMSelector
{

  ASG_TOOL_CLASS3(AsgForwardElectronIsEMSelector,
                  IAsgForwardElectronIsEMSelector,
                  IAsgEGammaIsEMSelector,
                  IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgForwardElectronIsEMSelector(const std::string& myname);

  /** Standard destructor */
  virtual ~AsgForwardElectronIsEMSelector();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize() override final;

  // Main methods for IAsgSelectionTool interface

  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo
      and query what cuts are defined before the first object
      is passed to the tool. */
  virtual const asg::AcceptInfo& getAcceptInfo() const override final;

  /** Accept with generic interface */
  virtual asg::AcceptData accept(
    const xAOD::IParticle* part) const override final;
  virtual asg::AcceptData accept(
    const EventContext& ctx,
    const xAOD::IParticle* part) const override final;

  /** Accept with Egamma objects */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Egamma* part) const override final;

  /** Accept with Photon objects */
  virtual asg::AcceptData accept(const EventContext& ctx,
                                 const xAOD::Photon* part) const override final;

  /** Accept with Electron objects */
  virtual asg::AcceptData accept(
    const EventContext& ctx,
    const xAOD::Electron* part) const override final;
  /** Method to get the operating point */
  virtual std::string getOperatingPointName() const override final;

  // The main execute method
  virtual StatusCode execute(const EventContext& ctx,
                             const xAOD::Egamma* eg,
                             unsigned int& isEM) const override final;
  // Private member variables
private:
  unsigned int getNPrimVertices(const EventContext& ctx) const;

  unsigned int calocuts_electrons(const xAOD::Egamma* eg,
                                  float eta2,
                                  float nvtx,
                                  unsigned int iflag) const;

  /** Working Point */
  std::string m_WorkingPoint;

  /** Config File */
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TForwardElectronIsEMSelector* m_rootForwardTool;

  /// Whether to use the PV (not available for trigger)
  bool m_usePVCont;

  // defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  ///  read handle key to primary vertex container
  SG::ReadHandleKey<xAOD::VertexContainer> m_primVtxContKey{
    this,
    "primaryVertexContainer",
    "PrimaryVertices",
    "The primary vertex container name"
  };

}; // End: class definition

#endif

