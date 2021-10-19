/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCellVolumes.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

struct CaloCellVolume_Compare
{
  bool operator() (const CaloCellVolume& a, const CaloCellVolume& b)
  { return a.channelID < b.channelID; }
  bool operator() (const CaloCellVolume& a, Identifier b)
  { return a.channelID < b; }
};


CaloCellVolumes::CaloCellVolumes(ISvcLocator* svcLocator
                                 , const CaloCell_ID* calocell_id)
  : m_calocell_id(calocell_id)
{
  // Retrieve input parameters from the database
  IGeoModelSvc*  geoModel{nullptr};
  IGeoDbTagSvc*  geoDbTagSvc{nullptr};
  IRDBAccessSvc* rdbAccess{nullptr};

  if(svcLocator->service("GeoModelSvc",geoModel) == StatusCode::FAILURE)
    throw std::runtime_error("CellVolumes error: cannot access GeoModelSvc");
  
  if(svcLocator->service("GeoDbTagSvc",geoDbTagSvc) == StatusCode::FAILURE)
    throw std::runtime_error("CellVolumes error: cannot access GeoDbTagSvc");

  if(svcLocator->service(geoDbTagSvc->getParamSvcName(),rdbAccess) == StatusCode::FAILURE)
    throw std::runtime_error("CellVolumes error: cannot access RDBAccessSvc");

  std::string larKey, larNode;
  if(geoDbTagSvc->getSqliteReader()==nullptr) {
    DecodeVersionKey detectorKey = DecodeVersionKey(geoModel,"LAr");
    larKey = detectorKey.tag();
    larNode = detectorKey.node();
  }

  IRDBRecordset_ptr cellVolRec = rdbAccess->getRecordsetPtr("LArCellVolumes",larKey,larNode);
  if(cellVolRec->size()==0) {
    cellVolRec = rdbAccess->getRecordsetPtr("LArCellVolumes","LArCellVolumes-00");
    if(cellVolRec->size()==0) {
      throw std::runtime_error("CaloCellVolumes error: 0 size of LArCellVolumes recordset");
    }
  }

  // get fcal tube spacings
  IRDBRecordset_ptr fcalModRec = rdbAccess->getRecordsetPtr("FCalMod",larKey,larNode);
  if(fcalModRec->size()==0) {
    fcalModRec = rdbAccess->getRecordsetPtr("FCalMod","FCalMod-00");
    if(fcalModRec->size()==0) {
      throw std::runtime_error("CaloCellVolumes error: 0 size of FCalMod recordset");
    }
  }

  for(const IRDBRecord* rec : *fcalModRec) {
    m_fcalTubeSpacings[rec->getInt("FCALSAMPLING")] = rec->getDouble("TUBESPACING");
  }
  
  m_geometryLayout = "Atlas";
  if(geoDbTagSvc->getSqliteReader()==nullptr) {
    std::string LArTag = rdbAccess->getChildTag("LAr",larKey,larNode);
    if(LArTag.find("H8")!=std::string::npos) {
      m_geometryLayout = "H8";
    }
    else if(LArTag.find("H6")!=std::string::npos) {
      m_geometryLayout = "H6";
    }
    else if(LArTag.find("G3")!=std::string::npos) {
      m_geometryLayout = "G3";
    }
  }
  
  // Initialize m_cellVolumes vector
  for(const IRDBRecord* rec : *cellVolRec) {
    
    int subcalo = rec->getInt("SUBCALO");
    int posneg = rec->getInt("POSNEG");
    int sampling = rec->getInt("SAMPLING");
    int region = rec->getInt("REGION");
    int eta = rec->getInt("ETA");
    int phi = rec->getInt("PHI");
    
    Identifier channelID;
    if(m_geometryLayout != "H6" || subcalo == CaloCell_ID::LARFCAL) {
      channelID = m_calocell_id->cell_id (subcalo, posneg, sampling, region, eta, phi);
    } 
    else {
      Identifier reg_id =  m_calocell_id->region_id(subcalo, posneg, sampling, region);
      int phimin = m_calocell_id->phi_min(reg_id);
      channelID = m_calocell_id->cell_id (subcalo, posneg, sampling, region, eta, phimin);
    }

    CaloCellVolume cellVol;
    cellVol.channelID = channelID;
    cellVol.volume = rec->getDouble("VOLUME");
    m_cellVolumes.push_back(cellVol);
  }
  
  if (m_cellVolumes.size()==0) {
    throw std::runtime_error("CaloCellVolumes error: 0 volumes have been built!");
  }
  
  std::sort (m_cellVolumes.begin(), m_cellVolumes.end(),
             CaloCellVolume_Compare());
}

CaloCellVolumes::~CaloCellVolumes()
{
  m_cellVolumes.clear();
}

double CaloCellVolumes::CellVolume(Identifier cell_id)
{
  // compute Identifier of Cell with same eta, but phi=0 and pos side
  // (except for fcal)
  Identifier volId;
  
  if (!m_calocell_id->is_fcal(cell_id)) {
    int subCalo = m_calocell_id->sub_calo(cell_id);
    int posneg = std::abs (m_calocell_id->pos_neg(cell_id));
    int sampling = m_calocell_id->sampling(cell_id);
    int region = m_calocell_id->region(cell_id);
    int eta = m_calocell_id->eta(cell_id);
    
    if(m_geometryLayout != "H6") {
      volId = m_calocell_id->cell_id(subCalo, posneg, sampling, region, eta, 0);
    } 
    else {
      Identifier reg_id =  m_calocell_id->region_id(subCalo, posneg, sampling, region);
      int phimin = m_calocell_id->phi_min(reg_id);
      volId = m_calocell_id->cell_id(subCalo, posneg, sampling, region, eta, phimin);
    }
  }
  else {
    volId = cell_id;
  }

  CaloCellVolumeVector::const_iterator it = 
    std::lower_bound (m_cellVolumes.begin(), m_cellVolumes.end(), volId,
		      CaloCellVolume_Compare());
  if (it != m_cellVolumes.end() && it->channelID == volId) return it->volume;

  return 0.;
}

void CaloCellVolumes::print ()
{
  if (m_cellVolumes.size()==0) {
    std::cerr << "CaloCellVolumes::print(). No cell volumes..." << std::endl;
  }
  else {
    std::cout << "CaloCellVolumes::print(). Number of volumes found: " 
	      << m_cellVolumes.size() << std::endl;
    
    for(const CaloCellVolume& vol : m_cellVolumes) {
      m_calocell_id->print( (Identifier) vol.channelID );
      std::cout << m_calocell_id->show_to_string( (Identifier) vol.channelID );
      std::cout << "   ==> vol= " << vol.volume << "  mm3" << std::endl;
    }
  }
}

double CaloCellVolumes::getFcalTubeSpacing(int sampling)
{
  TubeSpacingMap::const_iterator it = m_fcalTubeSpacings.find(sampling);
  if(it==m_fcalTubeSpacings.end())
    throw std::runtime_error("CaloCellVolumes::getFcalTubeSpacing error: wrong sampling provided");

  return (*it).second;
}
