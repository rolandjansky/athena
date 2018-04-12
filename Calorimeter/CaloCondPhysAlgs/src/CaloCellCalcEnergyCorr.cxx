///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CaloCellCalcEnergyCorr.cxx 
// Implementation file for class CaloCellCalcEnergyCorr
/////////////////////////////////////////////////////////////////// 

// CaloCondPhysAlgs includes
#include "CaloCondPhysAlgs/CaloCellCalcEnergyCorr.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/ToolHandle.h"

#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"

#include "LArHV/LArHVManager.h"

#include "LArCabling/LArHVCablingTool.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CaloCondBlobObjs/CaloCondBlobFlt.h"

#include "CoolKernel/types.h"
#include "CoolKernel/Record.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CaloCondPhysAlgs/LArHVMapTool.h"

CaloCellCalcEnergyCorr::CaloCellCalcEnergyCorr( const std::string& name, 
						ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_larem_id(0),
  m_larhec_id(0),
  m_larfcal_id(0)
{
  declareProperty("Folder",m_folder="/LAR/CellCorrOfl/EnergyCorr");
  std::vector<int> ivec; ivec.push_back(-1);
  declareProperty("CaloSamples",m_calosample=ivec);
  std::vector<float> fvec; fvec.push_back(1.0);
  declareProperty("SampleValues",m_value=fvec);
  std::vector<int> iivec; iivec.push_back(-1);
  declareProperty("HVLines",m_hvlines=iivec);
  std::vector<float> ffvec; ffvec.push_back(1.0);
  declareProperty("HVvalues",m_hvvalue=ffvec);
}


CaloCellCalcEnergyCorr::~CaloCellCalcEnergyCorr()
{}


StatusCode CaloCellCalcEnergyCorr::initialize()
{
  if(m_calosample.size() != m_value.size() ) {
     ATH_MSG_ERROR( "CaloSamples and SampleValues vectors not equal length !!! " );
     return StatusCode::FAILURE;
  }
  if(m_hvlines.size() != m_hvvalue.size() ) {
     ATH_MSG_ERROR( "HVLines and HVvalues vectors not equal length !!! " );
     return StatusCode::FAILURE;
  }

// retrieve LArEM id helpers

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );

  m_larem_id   = m_caloIdMgr->getEM_ID();
  m_larhec_id   = m_caloIdMgr->getHEC_ID();
  m_larfcal_id   = m_caloIdMgr->getFCAL_ID();

//  retrieve CaloDetDescrMgr 
  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  CHECK(m_hvtool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode CaloCellCalcEnergyCorr::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode CaloCellCalcEnergyCorr::execute()
{  
  CHECK(m_hvtool->execute());
  return StatusCode::SUCCESS;
}

StatusCode CaloCellCalcEnergyCorr::stop()
{  

  const CaloCell_ID*    calocell_id;	
  CHECK(detStore()->retrieve(calocell_id,"CaloCell_ID"));

  ToolHandle<LArHVCablingTool> HVcabling;
  if(m_hvlines.size()>=1 && m_hvlines[0]>0) CHECK(HVcabling.retrieve());

  IdentifierHash hashMin,hashMax;
  if(m_calosample.size()==1 && m_calosample[0] < 0 ) {
     calocell_id->calo_cell_hash_range(CaloCell_ID::TILE, hashMin,hashMax);
  } else {
     int maxsubcalo=-1;
     for (int i=CaloCell_ID::PreSamplerB; i<=CaloCell_ID::EME3; ++i) {
        if (std::find(m_calosample.begin(), m_calosample.end(), i) != m_calosample.end()) maxsubcalo = CaloCell_ID::LAREM;
     }
     for (int i=CaloCell_ID::HEC0; i<=CaloCell_ID::HEC3; ++i) {
        if (std::find(m_calosample.begin(), m_calosample.end(), i) != m_calosample.end()) maxsubcalo = CaloCell_ID::LARHEC;
     }
     for (int i=CaloCell_ID::FCAL0; i<=CaloCell_ID::FCAL2; ++i) {
        if (std::find(m_calosample.begin(), m_calosample.end(), i) != m_calosample.end()) maxsubcalo = CaloCell_ID::LARFCAL;
     }
     for (int i=CaloCell_ID::TileBar0; i<=CaloCell_ID::TileExt2; ++i) {
        if (std::find(m_calosample.begin(), m_calosample.end(), i) != m_calosample.end()) maxsubcalo = CaloCell_ID::TILE;
     }
     if(maxsubcalo < 0 ) {
        ATH_MSG_ERROR( "Wrong CaloSamples vector " << m_calosample );
        return StatusCode::FAILURE;
     } else {
        calocell_id->calo_cell_hash_range(maxsubcalo, hashMin,hashMax);
     }
  }

  ATH_MSG_INFO(  "Working on hash range 0 to " << hashMax );

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("CaloCondBlob16M","blob");// , cool::StorageType::Blob16M);
  AthenaAttributeList* attrList=new AthenaAttributeList(*spec);
  coral::Blob& blob=(*attrList)["CaloCondBlob16M"].data<coral::Blob>();
  CaloCondBlobFlt* flt= CaloCondBlobFlt::getInstance(blob);
  spec->release(); // deletes spec
  // cppcheck-suppress memleak
  spec = nullptr;

  //Blob Defintion Vector
  std::vector<std::vector<float> > defVec;
  defVec.push_back(std::vector<float>(1,1));
  flt->init(defVec,hashMax,1);

  CHECK(detStore()->record(attrList,m_folder));

  std::vector<float> setVec(1,1);
  unsigned nSet=0;
  unsigned nSetHV=0;
  for(unsigned h=0;h<hashMax;++h) {
    float value=1.0;
    std::vector<int>::const_iterator pos;
    if ((pos = std::find(m_calosample.begin(), m_calosample.end(), calocell_id->calo_sample(h))) != m_calosample.end()) {
       // find a position in m_calosample vector, and use this value from m_values vector
        const std::vector<float>::size_type idx = pos - m_calosample.begin();
	value=m_value[idx];
	++nSet;
        // check if we have also HVLine for this cell
        if(m_hvlines.size()>=1 && m_hvlines[0]>0) {
           Identifier offId=calocell_id->cell_id(h);
           std::vector<int> hvlineId = m_hvtool->GetHVLines(offId);
           //std::cout<<offId.getString()<<" : ";
           int nfound=0;
           float hvval=-1;
           std::vector<int>::const_iterator poshv;
           for(unsigned i=0; i<hvlineId.size(); ++i) {
              if ((poshv=std::find(m_hvlines.begin(), m_hvlines.end(), hvlineId[i])) != m_hvlines.end()) {
                 if (hvval<0) hvval = m_hvvalue[poshv - m_hvlines.begin()];
                 if (m_hvvalue[poshv - m_hvlines.begin()] == hvval ) ++nfound;
              }
           }
           if(nfound == (int)hvlineId.size()) { // All lines of this channel has the same corr., apply
              value *= hvval;
              ++nSetHV;
           }
        }
    }
    //std::cout << h << " " << value << std::endl;
    setVec[0]=value;
    flt->setData(h,0,setVec);
  }//end loop over hash

  ATH_MSG_INFO( "Found " << nSet << " channels which have a sample correction. " );
  ATH_MSG_INFO( "Found " << nSetHV << " channels which have a HV correction. " );
  
  return StatusCode::SUCCESS;
}

/*
std::vector<int> CaloCellCalcEnergyCorr::GetHVLines(const Identifier& id) {
  std::set<int> hv;

  // LAr EMB
  if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id)>0) {
    if (abs(m_larem_id->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement) std::abort();
      const EMBCellConstLink cell = embElement->getEMBCell();
      unsigned int nelec = cell->getNumElectrodes();
      for (unsigned int i=0;i<nelec;i++) {
        const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
        for (unsigned int igap=0;igap<2;igap++) hv.insert(electrode->hvLineNo(igap));
      }
    } else { // LAr EMEC
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement) std::abort();
      const EMECCellConstLink cell = emecElement->getEMECCell();
      unsigned int nelec = cell->getNumElectrodes();
      for (unsigned int i=0;i<nelec;i++) {
        const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
        for (unsigned int igap=0;igap<2;igap++) hv.insert(electrode->hvLineNo(igap));
      }
    }
  } else if (m_larhec_id->is_lar_hec(id)) { // LAr HEC
    const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!hecElement) std::abort();
    const HECCellConstLink cell = hecElement->getHECCell();
    unsigned int nsubgaps = cell->getNumSubgaps();
    for (unsigned int igap=0;igap<nsubgaps;igap++) {
      const HECHVSubgapConstLink subgap = cell->getSubgap(igap);
      hv.insert(subgap->hvLineNo());
    }
  } else if (m_larfcal_id->is_lar_fcal(id)) { // LAr FCAL
    const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!fcalElement) std::abort();
    const FCALTile* tile = fcalElement->getFCALTile();
    unsigned int nlines = tile->getNumHVLines();
    unsigned int nlines_found=0;
    for (unsigned int i=0;i<nlines;i++) {
      const FCALHVLineConstLink line = tile->getHVLine(i);
      if (line) nlines_found++;
    }
    if ( nlines_found>0 ) {
      for (unsigned int i=0;i<nlines;i++) {
        const FCALHVLineConstLink line = tile->getHVLine(i);
        if (!line) continue;
        hv.insert(line->hvLineNo());
      }
    }
  } else if (m_larem_id->is_lar_em(id) && m_larem_id->sampling(id)==0) { // Presamplers
    if (abs(m_larem_id->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement) std::abort();
      const EMBCellConstLink cell = embElement->getEMBCell();
      const EMBPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule();
      for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
    } else {
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement) std::abort();
      const EMECCellConstLink cell = emecElement->getEMECCell();
      const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule ();
      for (unsigned int igap=0;igap<2;igap++) hv.insert(hvmodule->hvLineNo(igap));
    }
  }

  std::vector<int> hvlines;
  for (std::set<int>::iterator i=hv.begin();i!=hv.end();++i) hvlines.push_back(*i);
  return hvlines;
}
*/

