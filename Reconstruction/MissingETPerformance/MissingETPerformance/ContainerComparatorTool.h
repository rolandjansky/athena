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
  std::vector<std::vector<std::string> > m_containers_to_compare;

/*
  //vector of pairs of names of MissingETData objects and names of BasicPlotsTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_basictool_pairs;
  //vector of pairs of names of MissingETData objects and names of ResolutionTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_resolution_pairs;
  //vector of pairs of names of MissingETData objects and names of LinearityTools to call execute(data) on
  std::vector<std::pair<std::string, std::string> > data_linearity_pairs;
*/

  //vector of pairs of names of MissingETData objects and names of BasicPlotsTools to call execute(data) on
  std::vector<std::vector<std::string> > m_data_basictool_pairs;
  //vector of pairs of names of MissingETData objects and names of ResolutionTools to call execute(data) on
  std::vector<std::vector<std::string> > m_data_resolution_pairs;
  //vector of pairs of names of MissingETData objects and names of LinearityTools to call execute(data) on
  std::vector<std::vector<std::string> > m_data_linearity_pairs;

  //array of MissingETData
  ToolHandleArray<MissingETData> m_the_data;

  //array of BasicPlotsTools, will call execute on each one as requested, giving it the correct MissingETData object specified by data_basictool_pairs
  ToolHandleArray<BasicPlotsTool> m_basicplots_tools;
  //array of ResolutionTool, will call execute on each one as requested, giving it the correct MissingETData object specified by data_resolution_pairs
  ToolHandleArray<ResolutionTool> m_resolution_tools;
  //array of LinearityTool, will call execute on each one as requested, giving it the correct MissingETData object specified by data_linearity_pairs
  ToolHandleArray<LinearityTool> m_linearity_tools;

  //histograms
  class DiffContainers {
  public:
    DiffContainers() {}
    ~DiffContainers();
    DiffContainers(const std::string &, const std::string &,ITHistSvc * );
    void fillHistos(std::pair<const MissingET *, const MissingET *>);
  private:
    TH1D *m_met;
    TH1D *m_metx;
    TH1D *m_mety;
    TH1D *m_metphi;
    TH1D *m_set;
  };
  std::vector<DiffContainers *> m_diffContainers;

  class VsContainers {
  public:
    VsContainers() {}
    ~VsContainers();
    VsContainers(const std::string &, const std::string &,ITHistSvc * );
    void fillHistos(std::pair<const MissingET *, const MissingET *>);
  private:
    TH2D *m_met;
    TH2D *m_metx;
    TH2D *m_mety;
    TH2D *m_metphi;
    TH2D *m_set;
  };
  std::vector<VsContainers *> m_vsContainers;
};

#endif // CONTAINERCOMPARATORTOOL_H
