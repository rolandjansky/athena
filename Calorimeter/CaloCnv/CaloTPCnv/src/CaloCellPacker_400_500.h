// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellPacker_400_500.h,v 1.3 2009-03-19 01:42:14 ssnyder Exp $

/**
 * @file CaloTPCnv/src/CaloCellPacker_400_500.h
 * @author scott snyder, from earlier code by Ilija Vukotic and Sven Menke
 * @date Jan 2009
 * @brief Calo cell packer/unpacker v400/500.
 *
 * This class handles both versions 400 and 500.
 */


#ifndef CALOCELLPACKER_400_500_H
#define CALOCELLPACKER_400_500_H


#include "CaloCellPackerUtils.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArRecEvent/LArCell.h"
#include "TileEvent/TileCell.h"
#include "AthAllocators/DataPool.h"
#include "CaloEvent/CaloCompactCellContainer.h"
class CaloCellContainer;
class CaloCell;
class CaloCellPacker_400_500_test;

/**
 * @brief Calo cell packer/unpacker v400/500.
 *
 * This class handles packing and unpacking of calorimeter cells
 * into a CaloCompactCellContainer.  This holds data as a vector
 * of integers, but for our purposes, we treat it as an array
 * of 16-bit values.  We use the special iterator-like objects
 * that that class defines for access.
 *
 * A summary of the format is given here.  Most constants and bit assignments
 * are specified within the method @c init_header().  Though some of them
 * are spelled out here, the possibility is left open that they can change.
 * The constants are stored in a header along with the data; for reading,
 * those stored constants are the ones used to interpret the data.
 *
 * The difference between versions 400 and 500 is the inclusion
 * of quality and provenance data.
 * Quality is like a chi**2 comparing the observed pulse shape
 * (with 5 samples) to the expected one. It is different cell by cell,
 * event by event (it is like energy and time from this point of view)
 *
 * The idea to store provenance is to know when reading back the ESD
 * how the energy was estimated.
 * For instance, when doing the OFC iteration method on the cosmics on
 * high energy cell there is one bit which tells if the iteration
 * converges or not and this is useful when analyzing back the event
 * (and this bit can vary from event to event for the same cell).
 *
 * The packed data start with an instance of either @c header400
 * or @c header500, containing
 * the packing constants as well as cell/sequence counts.  Note that the
 * first word is the length of the header, in units of @c int.
 *
 * Following the header is a set of @e sequences, each containing
 * a range of cells with consecutive hash IDs.  The cells are stored
 * in hash order, which implies that the subcalorimeters are strictly
 * in the order LAREM, LARHEC, LARFCAL, TILE.  A single sequence
 * contains cells from only a single subcalorimeter.
 *
 * Each sequence starts with a 32-bit value; where the high word
 * is stored first, and the low word second.  (Note that this is
 * @e opposite from the order in which the x86 would naturally store
 * the data.)  The lower 18 bits
 * of this give the hash value of the first cell in the sequence
 * (others follow sequentially), and the upper 14 bits give the number
 * of cells in the sequence.  (Note: if the number of cells in a sequence
 * is more than can be represented in 14 bits, the sequence is simply
 * split into several.)  Following this is the information for the cells
 * themselves.
 *
 * For LAr cells, there is a 16-bit word, containing, starting
 * from the low bit, 12 bits of energy, one sign bit for energy,
 * two bits for the gain, and one bit for the quality.  The quality
 * bit is 1 for good quality and 0 for bad quality.  The gain bits
 * are one of:
 *     0 ENLOW   for LARLOWGAIN
 *     1 ENMED   for LARMEDIUMGAIN, unless cell is in the HEC with e < e1_high
 *     2 ENHIG   for LARHIGHGAIN, unless cell is not in the HEC with e< e1_high
 *     3 EHHIG   for LARMEDIUMGAIN if cell is in the HEC with e < e1_high or
 *               for LARHIGHGAIN if cell is not in the HEC with e < e1_high
 *
 * The maximum energy is taken to be 50 GeV (e1_high) for EHHIG and 3.2 TeV
 * otherwise.  We take the cube root of the absolute value of the energy,
 * convert it into a fraction of fullscale (as a cube root), and store
 * that fraction as a fixed-point number.
 *
 * If the quality is good, this is followed by two additional 16-bit
 * words.  The first gives the time information.  This has 15 bits of time
 * information and one sign bit.  We take the natural log of the time, and then
 * encode this as a fraction of the range from 0.001ns to 1250ns.
 * This is then followed by the quality word.
 *
 * For tile cells, information for each of the two PMTs is stored separately.
 * First is a 16-bit word containing 13 bits of energy information,
 * one sign bit, one gain bit, and one quality bit.  The quality
 * bit is 1 for good quality and 0 for bad.  The gain bit is 0 for
 * low gain and 1 for high gain.  The energy is again stored as
 * a scaled cube root, where the upper limit is 50 GeV for high gain
 * and 3.2 TeV for low gain.   If the quality is good, this is followed
 * by 16 bits of time information, encoded as before.  Following both
 * cells is a 16-bit quality word; the lower 8 bits are for the
 * first PMT, and the upper eight bits are for the second PMT.
 * If the quality flags for both PMTs are bad, then this word
 * is omitted.
 *
 * Following the cell information is the provenance information.
 * Each provenance entry consists of a cell hash code (19 bits)
 * plus a provenance word (13 bits) packed into two 16-bit words.
 * The high 13 bits of the first word are the provenance; the low
 * three bits of the first word plus the second word give the
 * hash code.  Each provenance entry says that cells starting
 * at the given hash code have the given provenance value (until
 * the hash code of the next provenance entry).  Cells with a hash
 * code less than that of the first provenance entry have a provenance
 * word of 0.
 *
 * Version 501 adds a status bitmask to the header, with the flag
 * STATUS_UNORDERED.  This is set if the packer finds that the cells
 * are not in subcalo order.  (This is the case for cell collections
 * produced by the HLT, which are concatenations of per-ROI cell collections.)
 *
 * Version 502 adds quality words and provenance for tile.
 *
 * Version 503 reduces slightly the range for negative energies,
 * to avoid confusion with error flags.
 *
 * Version 504 introduces a flag for SuperCell 
 */
class CaloCellPacker_400_500
{
public:
  /**
   * @brief Pack cells.
   * @param cells The input cell container.
   * @param packed The output packed cell container.
   * @param version The version of the header to initialize.
   */
  void pack (const CaloCellContainer& cells,
             CaloCompactCellContainer& packed,
             int version) const;


  /**
   * @brief Unpack cells.
   * @param packed The input packed cell container.
   * @param vheader The header part of the packed data.
   * @param cells The output cell container.
   * @param larpool Pool for allocating LAr cells.
   * @param tilepool Pool for allocating Tile cells.
   *
   * Note that allocations will be done from the provided pools,
   * and the pools retain ownership of the cells.
   * The @a cells container will be changed to a view container.
   */
  void unpack (const CaloCompactCellContainer& packed,
               const std::vector<CaloCompactCellContainer::value_type>&vheader,
               CaloCellContainer& cells,
               DataPool<LArCell>& larpool,
               DataPool<TileCell>& tilepool) const;

private:

  //==========================================================================
  /** @name Header and parameter definitions. */
  //@{

  /**
   * @brief Packing parameters header (v400).
   *
   * This structure is written at the beginning of the packed data.
   * It contains both the basic packing constants as well as counters
   * for the number of cells and sequences.
   *
   * These values are initialized in @c init_header.
   *
   * Since this is written directly to the persistent data,
   * don't rearrange fields!
   */
  struct header400 {
    int m_length;                      // Header length, in units of int.

    int m_version;                     // Version code (ICaloCompactCellTool).

    /// Counters of number of cells in each subcalo.
    int m_ncells_larem;
    int m_ncells_larhec;
    int m_ncells_larfcal;
    int m_ncells_tile;

    /// Masks defining the bit positions used for various quantities.
    /// These should all contain a single consecutive string of 1'.s
    unsigned int m_qualy_mask;        // Quality.
    unsigned int m_egain_mask;        // LAr gain.
    unsigned int m_esign_mask;        // LAr energy sign bit.
    unsigned int m_crtae_mask;        // LAr cbrt(energy).
    unsigned int m_egain_tile_mask;   // Tile gain.
    unsigned int m_esign_tile_mask;   // Tile energy sign bit.
    unsigned int m_crtae_tile_mask;   // Tile cbrt(energy).
    unsigned int m_tsign_mask;        // Time sign bit.
    unsigned int m_logat_mask;        // log(time).

    /// Various enumeration constants.
    int m_qabad;                      // Bad quality flag.
    int m_enlow;                      // LAr low gain.
    int m_enmed;                      // LAr medium gain.
    int m_enhig;                      // LAr high gain.
    int m_ehhig;                      // LAr high gain with 50 GeV range.
    int m_glow;                       // Tile low range.
    int m_ghigh;                      // Tile high range.

    /// Counts of number of sequences in each subcalo.
    /// Note: in early versions of the v400 packer, these words would
    /// be left uninitialized if there were no cells.
    int m_seq_larem;
    int m_seq_larhec;
    int m_seq_larfcal;
    int m_seq_tile;

    /// Packing ranges for floats.
    float m_e1_norm_res;              // Normal LAr energy range.
    float m_e1_high_res;              // High gain LAr energy range.
    float m_high_tile;                // High gain tile energy range.
    float m_low_tile;                 // Low gain tile energy range.
    float m_t0;                       // Lower time range.
    float m_t1;                       // Upper time range.
  };


  /**
   * @brief Packing parameters header (v500).
   *
   * This adds a count of the number of provenance entries.
   */
  struct header500
    : public header400
  {
    // The number of provenance entries (in 16-bit units).
    // In order to know from where to start reading provenance.
    unsigned int m_lengthProvenance;
  };


  /**
   * @brief Packing parameters header (v501).
   *
   * Also for 502.
   *
   * This adds a status flag.
   */
  struct header501
    : public header500
  {
    // A bitmask for additional status information.
    // Added for version 501.
    unsigned int m_status;

    enum {
      // Set if the cells are not in subcalo order.
      STATUS_UNORDERED = (1 << 0),
      // Set if the cells are SuperCell
      STATUS_IS_SUPERCELL = (1 << 1)
    };
  };


  /// The most recent header version.
  typedef header501 header;


  /**
   * @brief Derived packing parmeters.
   *
   * This structure contains parameters derived from those in @c header.
   * It is filled in by @c init_derived.
   *
   * It derives from @c header, so that we just need to pass one of these
   * around to access all parameters.
   */
  struct pars500
    : public header
  {
    /// Good quality flag.
    int m_qgood;

    /// Maximum number of cells in a sequence.
    unsigned int m_nseq_max;
    unsigned int m_prov_max;
    unsigned int m_prov_max_tile;

    /// Transformed packing ranges for floats.
    double m_cbrt_e1_norm_res;       // cbrt(e1_norm_res)
    double m_cbrt_e1_high_res;       // cbrt(e1_high_res)
    double m_cbrt_low_tile;          // cbrt(low_tile)
    double m_cbrt_high_tile;         // cbrt(high_tile)
    double m_log_t0;                 // log(t0)
    double m_log_t1;                 // log(t1)

    /// Bitfields for various quantities.
    CaloCellPackerUtils::Bitfield m_hash_field;
    CaloCellPackerUtils::Bitfield m_nseq_field;
    CaloCellPackerUtils::Bitfield m_prov_field;

    CaloCellPackerUtils::Bitfield m_egain_field;
    CaloCellPackerUtils::Bitfield m_qualy_field;
    CaloCellPackerUtils::Floatfield m_logat_field;
    CaloCellPackerUtils::Floatfield2 m_crtae_norm_field;
    CaloCellPackerUtils::Floatfield2 m_crtae_high_field;
    CaloCellPackerUtils::Floatfield2 m_crtae_tile_low_field;
    CaloCellPackerUtils::Floatfield2 m_crtae_tile_high_field;
    CaloCellPackerUtils::Bitfield m_egain_tile_field;

    CaloCellPackerUtils::Bitfield m_tile_qual1_field;
    CaloCellPackerUtils::Bitfield m_tile_qual2_field;

    // Some errors are flagged by setting all bits on; the resulting packed
    // values are stored in @c m_lar_dummy and @c m_tile_dummy.
    // These are, however, still valid encodings.  If we would legitimately
    // pack a value that gives the dummy word, we replace it with @c subst,
    // which has the low bit of the energy cleared.  This effectively reduces
    // the range on the low side slightly.
    CaloCompactCell::value_type m_lar_dummy;
    CaloCompactCell::value_type m_lar_dummy_subst;
    CaloCompactCell::value_type m_tile_dummy;
    CaloCompactCell::value_type m_tile_dummy_subst;
  };


  /**
   * @brief Initialize header with the current version of the packing
   *        parameters.  (Almost) all the constants are defined here.
   * @param header The header to initialize.
   * @param version The version of the header to initialize.
   */
  void init_header (header& header, int version) const;


  /**
   * @brief Clear the counters in the event header.
   * @param header The header to clear.
   */
  void clear_header (header& header) const;


  /**
   * @brief Initialize the derived packing parameters from the constants
   *        in the header.
   * @param pars The packing parameters.
   */
  void init_derived (pars500& pars) const;


  //@}
  //==========================================================================
  /** @name Packing. */
  //@{


  /**
   * @brief Pack a time value.
   * @param time The time to pack.
   * @param it The iterator into which to pack.
   * @param pars The packing parameters.
   */
  void pack_time (float time,
                  CaloCompactCellContainer::compact_output_iterator& it,
                  const pars500& pars) const;


  /**
   * @brief Pack one LAr cell.
   * @param cell The cell to pack.
   * @param subcalo The cell's subcalorimeter code.
   * @param it The iterator into which to pack.
   * @param pars The packing parameters.
   */
  void pack_lar (const CaloCell* cell,
                 CaloCell_ID::SUBCALO subcalo,
                 CaloCompactCellContainer::compact_output_iterator& it,
                 const pars500& pars) const;


  /**
   * @brief Pack one tile cell.
   * @param cell The cell to pack.
   * @param it The iterator into which to pack.
   * @param pars The packing parameters.
   */
  void pack_tile (const TileCell* cell,
                  CaloCompactCellContainer::compact_output_iterator& it,
                  const pars500& pars) const;


  /**
   * @brief Finish up one cell sequence.
   * @param hash The hash of the first cell in the sequence.
   * @param nseq The number of cells in the sequence.
   * @param it Iterator pointing at the beginning of the sequence.
   * @param subcalo Subcalorimeter code for the sequence.
   * @param pars The packing parameters.
   */
  void finish_seq (unsigned int hash,
                   unsigned int nseq,
                   CaloCompactCellContainer::compact_output_iterator& it,
                   CaloCell_ID::SUBCALO subcalo,
                   pars500& pars) const;


  /**
   * @brief Write the header to the output container.
   * @param header The header to write.
   * @param packed The container to which to write.
   */
  void write_header (const header& header,
                     CaloCompactCellContainer& packed) const;


  //@}
  //==========================================================================
  /** @name Unpacking. */
  //@{


  /**
   * @brief Unpack the time word.
   * @param it Input iterator.
   * @param pars Unpacking parameters.
   * @return The unpacked time.
   */
  double unpack_time (CaloCompactCellContainer::compact_input_iterator& it,
                      const pars500& pars) const;


  /**
   * @brief Unpack a LAr cell.
   * @param it Input iterator.
   * @param subcalo Subcalorimeter code for the cell.
   * @param cell Pointer to the cell in which to write.
   * @param pars Unpacking parameters.
   * @param provenance The provenance word for this cell.
   * @return @a cell, as a @c CaloCell*.
   *
   * The DDE and ID will be set in the cell separately; here, we need only
   * fill in the cell data.
   */
  CaloCell* unpack_lar (CaloCompactCellContainer::compact_input_iterator& it,
                        CaloCell_ID::SUBCALO subcalo,
                        LArCell* cell,
                        const pars500& pars,
                        uint16_t provenance) const;


  /**
   * @brief Unpack a tile cell.
   * @param it Input iterator.
   * @param dde Descriptor element for the cell.
   * @param pars Unpacking parameters.
   * @param provenance The provenance word for this cell.
   * @return The new cell.
   */
  TileCell unpack_tile (CaloCompactCellContainer::compact_input_iterator& it,
                        const CaloDetDescrElement* dde,
                        const pars500& pars,
                        uint16_t provenance) const;
  //@}

  friend class CaloCellPacker_400_500_test;
};


#endif // not CALOCELLPACKER_400_500_H
