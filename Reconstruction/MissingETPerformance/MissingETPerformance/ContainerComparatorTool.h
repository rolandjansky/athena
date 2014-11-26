/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONTAINERCOMPARATORTOOL_H 
#define CONTAINERCOMPARATORTOOL_H 

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "MissingETPerformance/MissingETData.h"
#include "MissingETPerformance/JetVariablesTool.h"
#include "MissingETPerformance/BasicPlotsTool.h"
#include "MissingETPerformance/ResolutionTool.h"
#include "MissingETPerformance/LinearityTool.h"

#include <string>

#include "TH1.h"
#include "TH2.h"

static const InterfaceID IID_ContainerComparatorTool("ContainerComparatorTool", 1, 0);

class ContainerComparatorTool : public AthAlgTool {

public:

  ContainerComparatorTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  /** AlgTool and IAlgTool interface methods */
  static const InterfaceID& interfaceID( ) { return IID_ContainerComparatorTool; };

  StatusCode CBNT_initialize();

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  /** finalization */
  virtual StatusCode finalize();
  virtual StatusCode execute(MissingETData *data);

protected:

   /** Standard destructor */
   virtual ~ContainerComparatorTool( );

 private:
  StatusCode comparatorPlots(MissingETData *data);

  /** a handle on the Hist/TTree registration service */
  ITHistSvc * m_thistSvc;

  //vector of vectors - each vector is a vector of container names to compare
  std::vector<std::vector<std::string> > containers_to_compare;

/*
  //vector of pairs of names of MissingETData objects and names of BasicPlotsTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_basictool_pairs;
  //vector of pairs of names of MissingETData objects and names of ResolutionTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_resolution_pairs;
  //vector of pairs of names of MissingETData objects and names of LinearityTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_linearity_pairs;
*/

  //vector of pairs of names of MissingETData objects and names of BasicPlotsTools to call execute(data) on
  std::vector<std::vector<std::string> > data_basictool_pairs;
  //vector of pairs of names of MissingETData objects and names of ResolutionTools to call execute(data) on
  std::vector<std::vector<std::string> > data_resolution_pairs;
  //vector of pairs of names of MissingETData objects and names of LinearityTools to call execute(data) on
  std::vector<std::vector<std::string> > data_linearity_pairs;

  //array of MissingETData
  ToolHandleArray<MissingETData> the_data;

  //array of BasicPlotsTools, will call execute on each one as requested, giving it the correct MissingETData object specified by data_basictool_pairs
  ToolHandleArray<BasicPlotsTool> basicplots_tools;
  //array of ResolutionTool, will call execute on each one as requested, giving it the correct MissingETData object specified by data_resolution_pairs
  ToolHandleArray<ResolutionTool> resolution_tools;
  //array of LinearityTool, will call execute on each one as requested, giving it the correct MissingETData object specified by data_linearity_pairs
  ToolHandleArray<LinearityTool> linearity_tools;

  //histograms
  class _diffContainers {
  public:
    _diffContainers() {}
    ~_diffContainers();
    _diffContainers(const std::string &, const std::string &,ITHistSvc * );
    void fillHistos(std::pair<const MissingET *, const MissingET *>);
  private:
    TH1D *met;
    TH1D *metx;
    TH1D *mety;
    TH1D *metphi;
    TH1D *set;
  };
  std::vector<_diffContainers *> diffContainers;

  class _vsContainers {
  public:
    _vsContainers() {}
    ~_vsContainers();
    _vsContainers(const std::string &, const std::string &,ITHistSvc * );
    void fillHistos(std::pair<const MissingET *, const MissingET *>);
  private:
    TH2D *met;
    TH2D *metx;
    TH2D *mety;
    TH2D *metphi;
    TH2D *set;
  };
  std::vector<_vsContainers *> vsContainers;
};

#endif // CONTAINERCOMPARATORTOOL_H
