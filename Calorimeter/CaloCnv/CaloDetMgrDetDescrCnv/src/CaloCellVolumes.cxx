/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloDetMgrDetDescrCnv/CaloCellVolumes.h"

#include "Identifier/Identifier.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
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


CaloCellVolumes::CaloCellVolumes(ISvcLocator* svcLocator,
                                 const CaloCell_ID* calocell_id)
  : m_calocell_id (calocell_id),
    m_svcLocator(svcLocator)
{
  // default values for tube spacings
  m_fcalTubeSpacings[1] = 7.5;
  m_fcalTubeSpacings[2] = 8.179;
  m_fcalTubeSpacings[3] = 9.;

  //Get pointers to the IGeoModelSvc and IRDBAccessSvc
  IGeoModelSvc* geoModelSvc = 0;
  IRDBAccessSvc* rdbAccessSvc = 0; 

  StatusCode sc = m_svcLocator->service("GeoModelSvc",geoModelSvc);

  if(sc.isFailure())
    throw std::runtime_error("CaloCellVolumes error: cannot access GeoModelSvc");

  sc = m_svcLocator->service("RDBAccessSvc",rdbAccessSvc);
  if(sc.isFailure())
    throw std::runtime_error("CaloCellVolumes error: cannot access RDBAccessSvc");

  DecodeVersionKey detectorKey = DecodeVersionKey(geoModelSvc,"LAr");

  IRDBRecordset_ptr cellVolRec = rdbAccessSvc->getRecordsetPtr("LArCellVolumes",detectorKey.tag(),detectorKey.node());
  if(cellVolRec->size()==0)
  {
    cellVolRec = rdbAccessSvc->getRecordsetPtr("LArCellVolumes","LArCellVolumes-00");
    if(cellVolRec->size()==0)
    {
      throw std::runtime_error("CaloCellVolumes error: 0 size of LArCellVolumes recordset");
    }
  }

  // get fcal tube spacings
  IRDBRecordset_ptr fcalModRec = rdbAccessSvc->getRecordsetPtr("FCalMod",detectorKey.tag(),detectorKey.node());
  if(fcalModRec->size()==0)
    fcalModRec = rdbAccessSvc->getRecordsetPtr("FCalMod","FCalMod-00");

  for(unsigned int i=0; i<fcalModRec->size(); i++)
  {
    const IRDBRecord* rec = (*fcalModRec)[i];
    m_fcalTubeSpacings[rec->getInt("FCALSAMPLING")] = rec->getDouble("TUBESPACING");
  }

  std::string LArTag = rdbAccessSvc->getChildTag("LAr",detectorKey.tag(),detectorKey.node());

  m_geometryLayout = "Atlas";
  if(LArTag.find("H8")!=std::string::npos) 
    m_geometryLayout = "H8";
  else if(LArTag.find("H6")!=std::string::npos) 
    m_geometryLayout = "H6";
  else if(LArTag.find("G3")!=std::string::npos)
    m_geometryLayout = "G3";

  // Initialize m_cellVolumes vector
  for(unsigned int ind = 0; ind < cellVolRec->size(); ind++)
  {
    const IRDBRecord* currentRec = (*cellVolRec)[ind];

    int subcalo = currentRec->getInt("SUBCALO");
    int posneg = currentRec->getInt("POSNEG");
    int sampling = currentRec->getInt("SAMPLING");
    int region = currentRec->getInt("REGION");
    int eta = currentRec->getInt("ETA");
    int phi = currentRec->getInt("PHI");

    Identifier channelID;
    if(m_geometryLayout != "H6" || subcalo == CaloCell_ID::LARFCAL) {
      channelID = m_calocell_id->cell_id (subcalo, posneg, sampling, region, eta, phi);
    } else {
      Identifier reg_id =  m_calocell_id->region_id(subcalo, posneg, sampling, region);
      int phimin = m_calocell_id->phi_min(reg_id);
      channelID = m_calocell_id->cell_id (subcalo, posneg, sampling, region, eta, phimin);
    }

    CaloCellVolume cellVol;
    cellVol.channelID = channelID;
    cellVol.volume = currentRec->getDouble("VOLUME");
    m_cellVolumes.push_back(cellVol);
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
  if (m_cellVolumes.size()==0)
    std::cerr << "CaloCellVolumes::CellVolume(). Problem in CaloCellVolumes: no cell volumes...\n"; 
  else
  {
    // compute Identifier of Cell with same eta, but phi=0 and pos side
    // (except for fcal)
    Identifier volId;

    if (!m_calocell_id->is_fcal(cell_id)) 
    {
      int subCalo = m_calocell_id->sub_calo(cell_id);
      int posneg = std::abs (m_calocell_id->pos_neg(cell_id));
      int sampling = m_calocell_id->sampling(cell_id);
      int region = m_calocell_id->region(cell_id);
      int eta = m_calocell_id->eta(cell_id);
    
      if(m_geometryLayout != "H6") {
	volId = m_calocell_id->cell_id(subCalo, posneg, sampling, region, eta, 0);
      } else {
	Identifier reg_id =  m_calocell_id->region_id(subCalo, posneg, sampling, region);
	int phimin = m_calocell_id->phi_min(reg_id);
	volId = m_calocell_id->cell_id(subCalo, posneg, sampling, region, eta, phimin);
      }
    }
    else
      volId = cell_id;

    CaloCellVolumeVector::const_iterator it = 
      std::lower_bound (m_cellVolumes.begin(), m_cellVolumes.end(), volId,
                        CaloCellVolume_Compare());
    if (it != m_cellVolumes.end() && it->channelID == volId)
      return it->volume;
  }

  return 0.;
}

void CaloCellVolumes::print ()
{
  if (m_cellVolumes.size()==0)
    std::cerr << "CaloCellVolumes::print(). No cell volumes...\n";
  else 
  {
    std::cout << "CaloCellVolumes::print(). Number of volumes found: " 
	      << m_cellVolumes.size();
    
    for(unsigned int i = 0; i<m_cellVolumes.size(); i++)
    {
      const CaloCellVolume  vol = m_cellVolumes[i];
      m_calocell_id->print( (Identifier) vol.channelID );
      std::cout<<m_calocell_id->show_to_string( (Identifier) vol.channelID );
      std::cout << "   ==> vol= " << vol.volume << "  mm3\n";
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
