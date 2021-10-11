/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMCSYM_H
#define LARRAWCONDITIONS_LARMCSYM_H


/** Helper class to handle z-phi symmetry of calibration constants in MC
 */

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier32.h"
#include "Identifier/IdentifierHash.h"
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

class LArMCSym
{
public:
  LArMCSym() = delete;


  /**
   * @brief Constructor.
   * @brief onlId Online ID helper.
   * @brief caloId Offline ID helper.
   * @brief oflHashtoSymOnl Symmetric HWID corresponding to each offline hash.
   * @brief onlHashtoSymOnl Symmetric HWID corresponding to each online hash.
   * @brief symIds List of all symmetric HWIDs.  MUST BE SORTED.
   */
  LArMCSym(const LArOnlineID* onlId, 
	   const CaloCell_ID* caloId,
	   std::vector<HWIdentifier>&& oflHashtoSymOnl,
	   std::vector<HWIdentifier>&& onlHashtoSymOnl,
	   std::vector<HWIdentifier>&& symIds
	   );


  /// Return for an index if there's no valid symmetric identifier.
  static constexpr size_t NO_INDEX = static_cast<size_t> (-1);
   

  /**
   * @brief Find the symmetric HWID for an offline cell identifier.
   *        May return an invalid identifier.
   */
  HWIdentifier ZPhiSymOfl(const Identifier notSymOffId) const {
    if (m_caloCellID->is_tile(notSymOffId)) return HWIdentifier();
    const IdentifierHash h=m_caloCellID->calo_cell_hash(notSymOffId);
    return ZPhiSymOfl(h);
  }


  /**
   * @brief Find the symmetric HWID for an offline cell hash.
   *        May return an invalid identifier.
   */
  HWIdentifier ZPhiSymOfl(const IdentifierHash notSymOffHash) const {
    assert(notSymOffHash < m_oflHashtoSymOnl.size());
    return HWIdentifier (m_oflHashtoSymOnl[notSymOffHash].first);
  }
  

  /**
   * @brief Find the index in symIds() of the symmetric HWID for
   *        an offline cell identifier.
   *        Returns NO_INDEX if no symmetric identifier is defined.
   */
  size_t ZPhiSymOflIndex(const Identifier notSymOffId) const {
    if (m_caloCellID->is_tile(notSymOffId)) return NO_INDEX;
    const IdentifierHash h=m_caloCellID->calo_cell_hash(notSymOffId);
    return ZPhiSymOflIndex(h);
  }

  /**
   * @brief Find the index in symIds() of the symmetric HWID for
   *        an offline cell hash.
   *        Returns NO_INDEX if no symmetric identifier is defined.
   */
  size_t ZPhiSymOflIndex(const IdentifierHash notSymOffHash) const {
    assert(notSymOffHash < m_oflHashtoSymOnl.size());
    return m_oflHashtoSymOnl[notSymOffHash].second;
  }
  

  /**
   * @brief Find the symmetric HWID for an online channel identifier.
   *        May return an invalid identifier.
   */
  HWIdentifier ZPhiSymOnl(const HWIdentifier notSymOnlId) const {
    const IdentifierHash h=m_onlineID->channel_Hash(notSymOnlId);
    return ZPhiSymOnl(h);
  }


  /**
   * @brief Find the symmetric HWID for an online channel hash.
   *        May return an invalid identifier.
   */
  HWIdentifier ZPhiSymOnl(const IdentifierHash notSymOnlHash) const {
    assert(notSymOnlHash < m_onlHashtoSymOnl.size());
    return HWIdentifier (m_onlHashtoSymOnl[notSymOnlHash].first);
  }


  /**
   * @brief Find the index in symIds() of the symmetric HWID for
   *        an online channel identifier.
   *        Returns NO_INDEX if no symmetric identifier is defined.
   */
  size_t ZPhiSymOnlIndex(const HWIdentifier notSymOnlId) const {
    const IdentifierHash h=m_onlineID->channel_Hash(notSymOnlId);
    return ZPhiSymOnlIndex(h);
  }


  /**
   * @brief Find the index in symIds() of the symmetric HWID for
   *        an online channel hash.
   *        Returns NO_INDEX if no symmetric identifier is defined.
   */
  size_t ZPhiSymOnlIndex(const IdentifierHash notSymOnlHash) const {
    assert(notSymOnlHash < m_onlHashtoSymOnl.size());
    return m_onlHashtoSymOnl[notSymOnlHash].second;
  }


  /**
   * @brief Return the list of symmetric hashes.
   */
  const std::vector<HWIdentifier>& symIds() const {
    return m_symIds;
  }


 private:
  typedef std::pair<Identifier32::value_type, uint32_t> IdPair_t;
  std::vector<IdPair_t>
  initIndices (const std::vector<HWIdentifier>& ids,
               const std::vector<HWIdentifier>& symIds) const;


  const LArOnlineID* m_onlineID;
  const CaloCell_ID* m_caloCellID;
  const std::vector<IdPair_t> m_oflHashtoSymOnl;
  const std::vector<IdPair_t> m_onlHashtoSymOnl;
  const std::vector<HWIdentifier> m_symIds;

};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArMCSym , 33771274 , 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( LArMCSym, 159334782 );

#endif 
