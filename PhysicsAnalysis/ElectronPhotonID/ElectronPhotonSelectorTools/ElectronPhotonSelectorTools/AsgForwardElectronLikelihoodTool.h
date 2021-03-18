/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __ASGFORWARDELECTRONLIKELIHOODTOOL__
#define __ASGFORWARDELECTRONLIKELIHOODTOOL__


// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "xAODEgamma/ElectronFwd.h"
#include "PATCore/AcceptData.h"
#include "AsgDataHandles/ReadHandleKey.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

class EventContext;

namespace Root{
  class TForwardElectronLikelihoodTool;
}


class AsgForwardElectronLikelihoodTool : public asg::AsgTool, 
				  virtual public IAsgElectronLikelihoodTool
{
  ASG_TOOL_CLASS2(AsgForwardElectronLikelihoodTool, IAsgElectronLikelihoodTool, IAsgSelectionTool)

public:
  /** Standard constructor */
  AsgForwardElectronLikelihoodTool( const std::string& myname);


  /** Standard destructor */
  virtual ~AsgForwardElectronLikelihoodTool();
public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize() override final;


  // Main methods for IAsgSelectorTool interface
public:

  /** Method to get the plain AcceptInfo.
      This is needed so that one can already get the AcceptInfo 
      and query what cuts are defined before the first object 
      is passed to the tool. */
  virtual const asg::AcceptInfo& getAcceptInfo() const override;

  /** The main accept method: using the generic interface */
  asg::AcceptData accept( const xAOD::IParticle* part ) const override;
  asg::AcceptData accept( const EventContext& ctx, const xAOD::IParticle* part ) const override;

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept( const xAOD::Electron* eg ) const {
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }  
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* eg ) const override {
    return accept (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: the actual cuts are applied here */
  asg::AcceptData accept( const xAOD::Egamma* eg ) const {
    return accept (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }
  asg::AcceptData accept(const EventContext& ctx, const xAOD::Egamma* eg ) const override {
    return accept (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main accept method: in case mu not in EventInfo online */
  asg::AcceptData accept( const xAOD::Electron* eg, double mu ) const;
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Electron* eg, double mu ) const override;

  /** The main accept method: in case mu not in EventInfo online */
  asg::AcceptData accept( const xAOD::Egamma* eg, double mu ) const;
  asg::AcceptData accept( const EventContext& ctx, const xAOD::Egamma* eg, double mu ) const override;

  // Main methods for IAsgCalculatorTool interface
public:
  /** The main result method: the actual likelihood is calculated here */
  double calculate( const xAOD::IParticle* part ) const;
  double calculate( const EventContext &ctx, const xAOD::IParticle* part ) const override;

  /** The main result method: the actual likelihood is calculated here */
    double calculate( const xAOD::Electron* eg ) const {
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }
  double calculate( const EventContext &ctx, const xAOD::Electron* eg ) const override {
    return calculate (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual likelihood is calculated here */
    double calculate( const xAOD::Egamma* eg ) const {
    return calculate (eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }
  double calculate( const EventContext &ctx, const xAOD::Egamma* eg ) const override {
    return calculate (ctx, eg, -99); // mu = -99 as input will force accept to grab the pileup variable from the xAOD object
  }

  /** The main result method: the actual likelihood is calculated here */
  double calculate( const xAOD::Electron* eg, double mu ) const;
  double calculate( const EventContext &ctx, const xAOD::Electron* eg, double mu ) const override;
 
  /** The main result method: the actual likelihood is calculated here */
  double calculate( const xAOD::Egamma* eg, double mu ) const; 
  double calculate( const EventContext &ctx, const xAOD::Egamma* eg, double mu ) const override;

  virtual std::string getOperatingPointName( ) const override;

  // Private methods
private:
  /// Get the number of primary vertices
  unsigned int getNPrimVertices(const EventContext &ctx) const;


  /// Get the name of the current operating point



  // Private member variables
private:
  /* The Working Point */
  std::string  m_WorkingPoint;

  // The input config file.
  std::string m_configFile;

  /** Pointer to the underlying ROOT based tool */
  Root::TForwardElectronLikelihoodTool* m_rootForwardTool;
  

  /// Whether to use the PV (not available for trigger)
  bool m_usePVCont;

  /// defualt nPV (when not using PVCont)
  unsigned int m_nPVdefault;

  ///  read handle key to primary vertex container
  SG::ReadHandleKey<xAOD::VertexContainer> m_primVtxContKey {
    this, "primaryVertexContainer", "PrimaryVertices",
    "The primary vertex container name"};

  /// The input ROOT file name that holds the PDFs
  std::string m_pdfFileName;


}; // End: class definition





#endif

