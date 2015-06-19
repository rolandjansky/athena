/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetDescr/LArCellVolumes.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include <iostream>
#include <iomanip>
#include <algorithm>


namespace {

struct LArCellVolumeDB_Compare
{
  bool operator() (const LArCellVolumeDB_t& a, const LArCellVolumeDB_t& b)
  { return a.db_channelID < b.db_channelID; }
  bool operator() (const LArCellVolumeDB_t& a, Identifier b)
  { return a.db_channelID < b; }
};

} // anonymous namespace



LArCellVolumes::LArCellVolumes(const IRDBRecordset* cellVolRec)
{
  // Get Id Helper
  m_calocell_id = CaloIdManager::instance()->getCaloCell_ID();

  // Initialize m_cellVolume vector
  for(unsigned int ind = 0; ind < cellVolRec->size(); ind++)
  {
    const IRDBRecord* currentRec = (*cellVolRec)[ind];

    int subcalo = currentRec->getInt("SUBCALO");
    int posneg = currentRec->getInt("POSNEG");
    int sampling = currentRec->getInt("SAMPLING");
    int region = currentRec->getInt("REGION");
    int eta = currentRec->getInt("ETA");
    int phi = currentRec->getInt("PHI");

    Identifier channelID = m_calocell_id->cell_id
      (subcalo, posneg, sampling, region, eta, phi);

    LArCellVolumeDB_t cellVol;
    cellVol.db_channelID = channelID;
    cellVol.db_volume = currentRec->getDouble("VOLUME");
    m_cellVolume.push_back(cellVol);
  }

  std::sort (m_cellVolume.begin(), m_cellVolume.end(),
             LArCellVolumeDB_Compare());
}

LArCellVolumes::~LArCellVolumes()
{
  m_cellVolume.clear();
}

double 
LArCellVolumes::CellVolume ( Identifier cell_id )
{
  if (m_cellVolume.size()==0)
    std::cerr << "LArCellVolumes::CellVolume(). Problem in LArCellVolumes: no cell volumes...\n"; 
  else
  {
  //log << MSG::INFO << "Looking for volume for cell :" << endreq;
  //m_calocell_id->print(cell_id);

  // compute Identifier of Cell with same eta, but phi=0 and pos side
  // (except for fcal)
    Identifier volId;

    if (!m_calocell_id->is_fcal(cell_id)) {
      int subCalo = m_calocell_id->sub_calo(cell_id);
      int posneg = std::abs (m_calocell_id->pos_neg(cell_id));
      int sampling = m_calocell_id->sampling(cell_id);
      int region = m_calocell_id->region(cell_id);
      int eta = m_calocell_id->eta(cell_id);
    
      volId = m_calocell_id->cell_id
	(subCalo, posneg, sampling, region, eta, 0);
    }
    else
      volId = cell_id;

  //m_calocell_id->print( (Identifier) volId );

    LArCellVolumeDB::const_iterator it = 
      std::lower_bound (m_cellVolume.begin(), m_cellVolume.end(), volId,
                        LArCellVolumeDB_Compare());
    if (it != m_cellVolume.end() && it->db_channelID == volId)
      return it->db_volume;
  }

  return 0.;
}

void
LArCellVolumes::print ()
{
  if (m_cellVolume.size()==0)
    std::cerr << "LArCellVolumes::print(). No cell volumes...\n";
  else 
    {
      std::cout << "LArCellVolumes::print(). Number of volumes found: " 
		<< m_cellVolume.size();
      
      for ( unsigned int i = 0; i< m_cellVolume.size(); i++)
	{
	  const LArCellVolumeDB_t  vol = m_cellVolume[i];
	  m_calocell_id->print( (Identifier) vol.db_channelID );
	  std::cout << "   ==> vol= " 
		    << vol.db_volume << "  mm3\n";
	}
    }
}
