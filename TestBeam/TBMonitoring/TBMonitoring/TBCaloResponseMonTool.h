/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBMONITORING_TBCALORESPONSEMONTOOL_H
#define TBMONITORING_TBCALORESPONSEMONTOOL_H

/*! \brief monitors calorimeter response 
 *
 * The \c TBCaloResponseMonTool produces total energy sum distributions as well
 * as for individual calorimeter modules and samplings. In addition, the 
 * variation of the signal as function of the impact position is measured.
 *
 * \author Peter Loch <loch@physics.arizona.edu>
 * \date June 17, 2004 - first implementation
 */  

#include "AthenaMonitoring/MonitorToolBase.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include "TBEvent/TBBPC.h"
#include "TBUtils/TBProfiler.h"
#include "TBUtils/TB2DProfiler.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include <string>
#include <map>
#include <vector>


class StoreGateSvc;

class TBCaloResponseMonTool: public MonitorToolBase
{

 public:

  /*! \brief key types for look up tables */
  typedef CaloSampling::CaloSample                      sample_key_t;
  typedef CaloCell_ID::SUBCALO                          module_key_t;
  typedef std::string                                   bpc_key_t;

  /*! \brief data type */
  typedef std::pair<double,double>                      bpc_data_t;
  typedef std::map<bpc_key_t,bpc_data_t>                bpc_store_t;

 /*! \brief histogram look up tables */
  typedef std::map<bpc_key_t,AIDA::IHistogram1D*>             bpc_1dstore_t;
  typedef std::map<bpc_key_t,AIDA::IHistogram2D*>             bpc_2dstore_t;
  typedef std::map<module_key_t,AIDA::IHistogram1D*>          module_1dstore_t;
  typedef std::map<sample_key_t,AIDA::IHistogram1D*>          sample_1dstore_t;
  typedef std::map<module_key_t,AIDA::IHistogram2D*>          module_2dstore_t;
  typedef std::map<sample_key_t,AIDA::IHistogram2D*>          sample_2dstore_t;
  typedef std::map<bpc_key_t,module_1dstore_t>          bpc_module_1dstore_t;
  typedef std::map<bpc_key_t,module_2dstore_t>          bpc_module_2dstore_t;
  typedef std::map<bpc_key_t,sample_1dstore_t>          bpc_sample_1dstore_t;
  typedef std::map<bpc_key_t,sample_2dstore_t>          bpc_sample_2dstore_t;

  /*! \brief stores */
  typedef std::map<bpc_key_t,TBProfiler<double>* >      bpc_1dprofile_t;
  typedef std::map<bpc_key_t,TB2DProfiler<double>* >    bpc_2dprofile_t;

  typedef std::map<module_key_t,TBProfiler<double>* >   module_1dprofile_t;
  typedef std::map<module_key_t,TB2DProfiler<double>* > module_2dprofile_t;

  typedef std::map<sample_key_t,TBProfiler<double>* >   sample_1dprofile_t;
  typedef std::map<sample_key_t,TB2DProfiler<double>* > sample_2dprofile_t;

  typedef std::map<bpc_key_t,module_1dprofile_t>        bpc_module_1dprofile_t;
  typedef std::map<bpc_key_t,module_2dprofile_t>        bpc_module_2dprofile_t;

  typedef std::map<bpc_key_t,sample_1dprofile_t>        bpc_sample_1dprofile_t;
  typedef std::map<bpc_key_t,sample_2dprofile_t>        bpc_sample_2dprofile_t;

  /*! \brief \c AlgTool constructor */
  TBCaloResponseMonTool(const std::string& type,
			const std::string& name,
			const IInterface* parent);
    

  virtual ~TBCaloResponseMonTool();

  virtual StatusCode initialize();
    
  virtual StatusCode bookHists();
  virtual StatusCode fillHists();
  virtual StatusCode finalHists();

  virtual StatusCode bookMyHists();

 protected:

  StoreGateSvc* m_storeGate;

  /*! \brief properties */
  // in base class !!!  std::string m_path;
  std::string m_bpcContainerName;
  std::string m_caloCellName;

  std::vector<std::string> m_caloNames;
  std::vector<std::string> m_samplingNames;

  unsigned int m_eBins;
  double       m_eLow, m_eHigh;
  unsigned int m_xBins;
  double       m_xLow, m_xHigh;
  unsigned int m_yBins;
  double       m_yLow, m_yHigh;
  unsigned int m_etaBins;
  double       m_etaLow, m_etaHigh;
  unsigned int m_phiBins;
  double       m_phiLow, m_phiHigh;
  double       m_eUnit, m_lUnit, m_aUnit;
  unsigned int m_minEventsInBin;

  /*! \brief internal helpers */
  //  StatusCode bookSampleHists(CaloSampling::CaloSample& theSample);
  //  StatusCode bookModuleHists(CaloSampling::SUBCALO&    theModule);
  StatusCode setupAction();

  /*! \brief stores */
  const CaloCell_ID* m_caloCellHelper;

  double m_pi, m_twoPi;

  std::vector<std::string>              m_bpcNames;
  std::vector<CaloCell_ID::SUBCALO>     m_caloIndices;
  std::vector<CaloSampling::CaloSample> m_samplingIndices;

  bool fillProfile1D(const TBProfiler<double>& aProfile,
		     AIDA::IHistogram1D&             aHist) const;
  bool fillProfile2D(const TB2DProfiler<double>& aProfile,
		     AIDA::IHistogram2D&               aHist) const;
  
 private:

  /*! \brief total energy plots */
  AIDA::IHistogram1D* m_totalE;        
  bpc_1dstore_t m_totalEvsBPCx;
  bpc_1dstore_t m_totalEvsBPCy;
  bpc_2dstore_t m_totalEinBPCxVsBPCy;

  bpc_1dprofile_t m_totalEProfileX;
  bpc_1dprofile_t m_totalEProfileY;
  bpc_2dprofile_t m_totalEProfileXY;
 
 /*! energy sums in modules */
  module_1dstore_t     m_caloE;
  bpc_module_1dstore_t m_caloEvsBPCx;
  bpc_module_1dstore_t m_caloEvsBPCy;
  bpc_module_2dstore_t m_caloEinBPCxVsBPCy;

  bpc_module_1dprofile_t m_caloEProfileX;
  bpc_module_1dprofile_t m_caloEProfileY;
  bpc_module_2dprofile_t m_caloEProfileXY;

  /*! energy sums in samplings */
  sample_1dstore_t     m_layerE;
  bpc_sample_1dstore_t m_layerEvsBPCx;
  bpc_sample_1dstore_t m_layerEvsBPCy;
  bpc_sample_2dstore_t m_layerEinBPCxVsBPCy;

  bpc_sample_1dprofile_t m_layerEProfileX;
  bpc_sample_1dprofile_t m_layerEProfileY;
  bpc_sample_2dprofile_t m_layerEProfileXY;

  /*! cell signals */
  //  IHistogram1D*                                    m_cellE;
  //  IHistogram2D*                                    m_cellEinEtaVsPhi;
  //  std::map<CaloSampling::CaloSample,IHistogram1D*> m_layerCellE;
  //  std::map<CaloSampling::CaloSample,IHistogram2D*> m_layerCellEinEtaVsPhi;
};
#endif
