#include "CaloConditions/CaloCellPedShift.h"


CaloCellPedShift::CaloCellPedShift(const CaloCell_ID* cellid) {

  IdentifierHash tileMinHash, tileMaxHash;
  cellid->calo_cell_hash_range (CaloCell_ID::TILE,tileMinHash,tileMaxHash);
  
  for (auto& vec :  m_larValues) {
    vec.resize(tileMinHash,0.0);
  }

  for (auto& vec :  m_tileValues) {
    vec.resize(tileMaxHash-tileMinHash,0.0);
  }

  m_tileMinHash=tileMinHash;
}

//Setter
void CaloCellPedShift::setPedShift(const IdentifierHash hash, const int gain, const float value) {
  if (hash<m_tileMinHash) {
    m_larValues[gain][hash]=value;
  }
  else {
    m_tileValues[gain][hash-m_tileMinHash]=value;
  }
}
