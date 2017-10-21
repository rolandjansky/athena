/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloCellPacker_400_500.cxx,v 1.5 2009-03-31 19:04:04 ssnyder Exp $
/**
 * @file CaloTPCnv/src/CaloCellPacker_400_500.cxx
 * @author scott snyder, from earlier code by Ilija Vukotic and Sven Menke
 * @date Jan 2009
 * @brief Calo cell packer/unpacker v400/500.
 */


#include "CaloCellPacker_400_500.h"
#include "CaloCompactCellTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloEvent/CaloCompactCellContainer.h"
#include "CaloEvent/CaloCell.h"
#include "TileEvent/TileCell.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/SystemOfUnits.h"


//============================================================================
// Common methods.
//


/**
 * @brief Initialize header with the current version of the packing
 *        parameters.  (Almost) all the constants are defined here.
 * @param header The header to initialize.
 * @param version The version of the header to initialize.
 */
void CaloCellPacker_400_500::init_header (header& header,
                                          int version) const
{
  header.m_version = version;
  switch (version) {
  case CaloCompactCellTool::VERSION_501:
  case CaloCompactCellTool::VERSION_502:
  case CaloCompactCellTool::VERSION_503:
  case CaloCompactCellTool::VERSION_504:
    header.m_length = sizeof(header501)/sizeof(int);
    break;

  case CaloCompactCellTool::VERSION_500:
    header.m_length = sizeof(header500)/sizeof(int);
    break;
  case CaloCompactCellTool::VERSION_400:
    header.m_length = sizeof(header400)/sizeof(int);
    break;
  default:
    std::abort();
  }

  header.m_qualy_mask = 0x8000;
  header.m_egain_mask = 0x6000;
  header.m_esign_mask = 0x1000;
  // 12 bits for cbrt(|E|) and 2 ranges, one normal
  // resolution range with upper limit 3.2 TeV and precision loss
  // sigma_E'/E < 1.4%/sqrt(E/GeV) and one high-resolution range used
  // whenever E < 50 GeV and the hardware gain is HIGH (HEC: MEDIUM)
  // resulting in a precision loss of sigma_E'/E < 0.15%/sqrt(E/GeV)
  header.m_crtae_mask = 0x0fff;
  header.m_egain_tile_mask = 0x4000;
  header.m_esign_tile_mask = 0x2000;
  // 13 bits cbrt(|E|) and 2 ranges
  // upper limit for low gain is 3.2 TeV,
  // upper limit for high gain is 50 GeV (safety factor of ~3)
  header.m_crtae_tile_mask = 0x1fff;
  header.m_tsign_mask = 0x8000;
  header.m_logat_mask = 0x7fff;

  header.m_qabad = 0;
  header.m_enlow = 0; // normal resolution LOW gain
  header.m_enmed = 1; // normal resolution MEDIUM gain
  header.m_enhig = 2; // normal resolution HIGH gain
  header.m_ehhig = 3; // high resolution HIGH (HEC: MEDIUM) gain
  header.m_glow  =  TileID::LOWGAIN;
  header.m_ghigh = TileID::HIGHGAIN;

  header.m_e1_norm_res  = 3.2*Gaudi::Units::TeV;
  header.m_e1_high_res  = 50*Gaudi::Units::GeV;
  header.m_high_tile = 50*Gaudi::Units::GeV;
  header.m_low_tile  = 3.2*Gaudi::Units::TeV;
  header.m_t0    = 0.001*Gaudi::Units::ns;
  header.m_t1    = 1250.0*Gaudi::Units::ns;

  clear_header(header);
}


/**
 * @brief Clear the counters in the event header.
 * @param header The header to clear.
 */
void CaloCellPacker_400_500::clear_header (header& header) const
{
  header.m_ncells_larem = 0;
  header.m_ncells_larhec = 0;
  header.m_ncells_larfcal = 0;
  header.m_ncells_tile  = 0;
  header.m_seq_larem = 0;
  header.m_seq_larhec = 0;
  header.m_seq_larfcal = 0;
  header.m_seq_tile = 0;
  header.m_lengthProvenance = 0;
  header.m_status = 0;
}


/**
 * @brief Initialize the derived packing parameters from the constants
 *        in the header.
 * @param pars The packing parameters.
 */
void CaloCellPacker_400_500::init_derived (pars500& pars) const
{
  // Good quality flag.
  pars.m_qgood = 1;

  // Set up parameters for packing/unpacking the sequence header.
  pars.m_hash_field = CaloCellPackerUtils::Bitfield (0x0003ffff);
  pars.m_nseq_field = CaloCellPackerUtils::Bitfield (0xfffc0000);
  pars.m_nseq_max = 0x3fff;

  // Set up parameters for packing/unpacking the provenance list.
  pars.m_prov_field     = CaloCellPackerUtils::Bitfield (0xfff80000);
  pars.m_prov_max = 0x1fff;
  pars.m_prov_max_tile = 0x1f1f;

  // Cube roots of energy ranges.
  pars.m_cbrt_e1_norm_res = cbrt(static_cast<double>(pars.m_e1_norm_res));
  pars.m_cbrt_e1_high_res = cbrt(static_cast<double>(pars.m_e1_high_res));
  pars.m_cbrt_low_tile = cbrt(static_cast<double>(pars.m_low_tile));
  pars.m_cbrt_high_tile = cbrt(static_cast<double>(pars.m_high_tile));

  // Logarithms of time ranges.
  pars.m_log_t0 = log(static_cast<double>(pars.m_t0));
  pars.m_log_t1 = log(static_cast<double>(pars.m_t1));

  // Fill in remaining bit fields.
  pars.m_qualy_field = CaloCellPackerUtils::Bitfield (pars.m_qualy_mask);
  pars.m_egain_field = CaloCellPackerUtils::Bitfield (pars.m_egain_mask);
  pars.m_crtae_norm_field =
    CaloCellPackerUtils::Floatfield2 (pars.m_crtae_mask,
                                      pars.m_cbrt_e1_norm_res);
  pars.m_crtae_high_field =
    CaloCellPackerUtils::Floatfield2 (pars.m_crtae_mask,
                                      pars.m_cbrt_e1_high_res);
  pars.m_logat_field =
    CaloCellPackerUtils::Floatfield (pars.m_logat_mask,
                                     pars.m_log_t0, pars.m_log_t1);
  pars.m_egain_tile_field =
    CaloCellPackerUtils::Bitfield (pars.m_egain_tile_mask);
  pars.m_crtae_tile_high_field =
    CaloCellPackerUtils::Floatfield2 (pars.m_crtae_tile_mask,
                                      pars.m_cbrt_high_tile);
  pars.m_crtae_tile_low_field =
    CaloCellPackerUtils::Floatfield2 (pars.m_crtae_tile_mask,
                                      pars.m_cbrt_low_tile);

  pars.m_tile_qual1_field   = CaloCellPackerUtils::Bitfield (0x00ff);
  pars.m_tile_qual2_field   = CaloCellPackerUtils::Bitfield (0xff00);

  // Dummy flags.
  pars.m_lar_dummy =
    pars.m_egain_field.in (pars.m_enhig) |
    pars.m_qualy_field.in (pars.m_qabad) |
    pars.m_crtae_norm_field.in (cbrt (pars.m_e1_norm_res)) |
    pars.m_esign_mask;

  pars.m_tile_dummy =
        pars.m_egain_tile_field.in (pars.m_glow) |
        pars.m_qualy_field.in (pars.m_qabad) |
        pars.m_crtae_tile_high_field.in (pars.m_cbrt_high_tile) |
        pars.m_esign_tile_mask;

  
  {
    CaloCellPackerUtils::Bitfield& elar = pars.m_crtae_norm_field;
    pars.m_lar_dummy_subst = (pars.m_lar_dummy & ~pars.m_crtae_mask) |
      elar.in (elar.out (pars.m_lar_dummy)-1);

    CaloCellPackerUtils::Bitfield& etile = pars.m_crtae_tile_high_field;
    pars.m_tile_dummy_subst = (pars.m_tile_dummy & ~pars.m_crtae_tile_mask) |
      etile.in (etile.out (pars.m_tile_dummy)-1);
  }    
}


//============================================================================
// Packing.
// To ensure full inlining, don't reorder these.
//


/**
 * @brief Pack a time value.
 * @param time The time to pack.
 * @param it The iterator into which to pack.
 * @param pars The packing parameters.
 */
inline
void CaloCellPacker_400_500::pack_time
   (float time,
    CaloCompactCellContainer::compact_output_iterator& it,
    const pars500& pars) const
{
  // We want to pack log(abs(time)).
  float ltime = pars.m_log_t0;
  if (time != 0)
    ltime = log(fabs(time));

  // Pack it into the bitfield.
  CaloCompactCell::value_type data = pars.m_logat_field.in (ltime);

  // Set the sign bit.
  if ( time < 0 )
    data |= pars.m_tsign_mask;

  // Fill output.
  it.set (data);
}


/**
 * @brief Pack one LAr cell.
 * @param cell The cell to pack.
 * @param subcalo The cell's subcalorimeter code.
 * @param it The iterator into which to pack.
 * @param pars The packing parameters.
 */
inline
void CaloCellPacker_400_500::pack_lar
   (const CaloCell* cell,
    CaloCell_ID::SUBCALO subcalo,
    CaloCompactCellContainer::compact_output_iterator& it,
    const pars500& pars) const
{
  // Get values from the cell.
  double energy  = cell->energy();
  double time    = cell->time();
  int qualflag;
  if( (cell->provenance() & 0x2000) == 0x2000 )
    qualflag=pars.m_qgood;
  else
    qualflag=pars.m_qabad;

  int gain       = cell->gain();

  // Figure out which gain to use.
  // cbrt_flag is set to 1 for the high gain range (50 GeV) and 0 for
  // the low gain range (3.2 TeV).
  int gainflag = -999;
  int cbrt_flag = 0;
  switch ( gain ) {
  case CaloGain::LARLOWGAIN:
    gainflag = pars.m_enlow;
    break;
  case CaloGain::LARMEDIUMGAIN:
    if ( subcalo == CaloCell_ID::LARHEC &&
         fabs(energy) < pars.m_e1_high_res )
    {
      gainflag = pars.m_ehhig;
      cbrt_flag = 1;
    }
    else
      gainflag = pars.m_enmed;
    break;
  case CaloGain::LARHIGHGAIN:
    if ( fabs(energy) < pars.m_e1_high_res &&
         subcalo != CaloCell_ID::LARHEC )
    {
      gainflag = pars.m_ehhig;
      cbrt_flag = 1;
    }
    else
      gainflag = pars.m_enhig;
    break;

  default:
    // Some invalid gain.  Mark as an error.
    gainflag = -999;
  }

  if (gainflag == -999) {
    it.set (pars.m_lar_dummy);
   }
  else {
    // Pack the energy, gain, and quality into the output word.
    double crtae = cbrt(fabs(energy));
    CaloCompactCell::value_type data =
      pars.m_egain_field.in (gainflag) |
      pars.m_qualy_field.in (qualflag) |
      (cbrt_flag ? pars.m_crtae_high_field.in (crtae)
       : pars.m_crtae_norm_field.in (crtae));

    // Set the sign bit.
    if (energy < 0)
      data |= pars.m_esign_mask;

    if (data == pars.m_lar_dummy)
      data = pars.m_lar_dummy_subst;

    // Fill the output container.
    it.set (data);

    // If quality is not bad there are time and quality (chi^2) measurements.
    if ( qualflag != pars.m_qabad){
      pack_time (time, it, pars);
      if (pars.m_version >= 500)
        it.set(cell->quality());
    }
  }
}


/**
 * @brief Pack one tile cell.
 * @param cell The cell to pack.
 * @param it The iterator into which to pack.
 * @param pars The packing parameters.
 */
inline
void CaloCellPacker_400_500::pack_tile
   (const TileCell* cell,
    CaloCompactCellContainer::compact_output_iterator& it,
    const pars500& pars) const
{
  // Tile cells have two separate measurements to pack.
  // Retrieve them both:
  double ene[2] = {cell->ene1(), cell->ene2()};
  double time[2] = {cell->time1(), cell->time2()};
  int qbit[2] = {cell->qbit1(), cell->qbit2()};
  int gain[2] = {cell->gain1(), cell->gain2()};

  // Loop over the two measurements (PMTs).
  // If no cell both gains are bad;
  // if only one pmt per cell, second gain is bad.
  bool write_qual = false;
  for (int ipmt=0; ipmt<2; ++ipmt) {
    // We'll set this to true if we want to write time information.
    bool write_time = false;

    // The data word we're building.
    CaloCompactCell::value_type data;

    // Does this measurement exist?
    if (gain[ipmt] == CaloGain::INVALIDGAIN) {
      // No --- make a dummy.
      data = pars.m_tile_dummy;
    }
    else {
      // Yes --- we have a measurement.  See if the quality's good.
      // If so, then we'll want to write the time too.
      int qualflag = pars.m_qabad;
      if (qbit[ipmt] >= TileCell::KEEP_TIME) {
        qualflag = pars.m_qgood;
        write_time = true;
        write_qual = true;
      }

      // Pack the energy, with the proper range, depending on the gain.
      double crtae = cbrt(fabs(ene[ipmt]));
      if (gain[ipmt] != pars.m_glow)
        data = pars.m_crtae_tile_high_field.in (crtae);
      else
        data = pars.m_crtae_tile_low_field.in (crtae);

      // Add in the gain and quality.
      data |=
        pars.m_egain_tile_field.in (gain[ipmt]) |
        pars.m_qualy_field.in (qualflag);

      // Add the sign bit.
      if (ene[ipmt] < 0)
        data |= pars.m_esign_tile_mask;

      if (data == pars.m_tile_dummy)
        data = pars.m_tile_dummy_subst;
    }

    // Fill to the output container.
    it.set (data);

    // Fill the time and quality, if needed.
    if (write_time)
      pack_time (time[ipmt], it, pars);
  }

  if (pars.m_version >= 502 && write_qual) {
    it.set (cell->quality());
  }
}


/**
 * @brief Finish up one cell sequence.
 * @param hash The hash of the first cell in the sequence.
 * @param nseq The number of cells in the sequence.
 * @param it Iterator pointing at the beginning of the sequence.
 * @param subcalo Subcalorimeter code for the sequence.
 * @param pars The packing parameters.
 */
inline
void CaloCellPacker_400_500::finish_seq
   (unsigned int hash,
    unsigned int nseq,
    CaloCompactCellContainer::compact_output_iterator& it,
    CaloCell_ID::SUBCALO subcalo,
    pars500& pars) const
{
  // Pack the hash code and number of cells into a bitfield.
  CaloCompactCellContainer::value_type data =
    pars.m_hash_field.in(hash) | pars.m_nseq_field.in(nseq);

  // Write it into the output container.
  it.set ((data>>16) & 0xffff);
  it.set (data & 0xffff);

  // Update counters.
  switch (subcalo) {
  case CaloCell_ID::LAREM:
    pars.m_ncells_larem += nseq;
    ++pars.m_seq_larem;
    break;
  case CaloCell_ID::LARHEC:
    pars.m_ncells_larhec += nseq;
    ++pars.m_seq_larhec;
    break;
  case CaloCell_ID::LARFCAL:
    pars.m_ncells_larfcal += nseq;
    ++pars.m_seq_larfcal;
    break;
  case CaloCell_ID::TILE:
    pars.m_ncells_tile += nseq;
    ++pars.m_seq_tile;
    break;
  default:
    std::abort();
  }
}


/**
 * @brief Pack cells.
 * @param cells The input cell container.
 * @param packed The output packed cell container.
 * @param version The version of the header to initialize.
 */
void CaloCellPacker_400_500::pack (const CaloCellContainer& cells,
                                   CaloCompactCellContainer& packed,
                                   int version) const
{
  // Set up the header and derived parameters.
  pars500 pars;
  init_header (pars, version);
  init_derived (pars);

  std::vector<short unsigned int> vProvenance;

  // Figure out an upper limit for the container size.
  // Header, plus one word/cell for sequence, two words/cell for data,
  // and another word/cell for provenance.
  // Tile cells have two additional words/cell - energy and time for second PMT.
  // Add one more to account for possible padding before the provenance info.
  // We'll resize this down correctly when we're done.
  unsigned int maxsize =
    pars.m_length + 4 * cells.size() + 2 * cells.nCellsCalo (CaloCell_ID::TILE)+1;
  packed.resize (maxsize);

  // Set up for loop over cells.

  // This is the output iterator, to which we write.
  CaloCompactCellContainer::compact_output_iterator outit
    (packed.compact_begin_output (pars.m_length));

  // Here we save the output iterator at the beginning of each sequence.
  // We'll use that to go back and plug in the count when we're done.
  CaloCompactCellContainer::compact_output_iterator seqit
    (packed.compact_begin_output (pars.m_length));

  // The hash of the first cell in the current sequence.
  unsigned int seqhash = static_cast<unsigned int> (-1);

  // Number of cells so far in the current sequence.
  // 0 if not in a sequence.
  unsigned int nseq = 0;

  // The subcalorimeter code of the previous cell we looked at.
  CaloCell_ID::SUBCALO prevcalo = CaloCell_ID::NOT_VALID;

  // If all the cells have provenance 0, there is nothing in the vector.
  short unsigned int prevCellProvenance=0;

  // check if it is a SuperCell
  bool is_SC=false; 
  if (cells.size()>0){
    // assuming no mixed SC and Cells. 
    const CaloDetDescrElement* dde = cells[0]->caloDDE();
    const CaloCell_Base_ID* idhelper =
      dde->descriptor()->get_calo_helper();
    is_SC = idhelper->is_supercell(cells[0]->ID());
  }

  if (is_SC)
    pars.m_status |= header::STATUS_IS_SUPERCELL;

  // Loop over input cells.
  for (const CaloCell* cell : cells)
  {
    // Pick up values from the cell.
    const CaloDetDescrElement* dde = cell->caloDDE();
    unsigned int hash = dde->calo_hash();
    CaloCell_ID::SUBCALO subcalo = dde->getSubCalo();

    // Test to see if we need to start a new sequence.
    if (// Not in a sequence now?
        nseq == 0 ||
        // We're at the maximum sequence length?
        nseq >= pars.m_nseq_max ||
        // A skip in the cell hash codes?
        seqhash + nseq != hash ||
        // Moving to a new subcalorimeter?
        subcalo != prevcalo)
    {
      // Mark if cells aren't ordered.
      if (prevcalo != CaloCell_ID::NOT_VALID &&
          subcalo < prevcalo)
        pars.m_status |= header::STATUS_UNORDERED;

      // If we're already in a sequence, need to finish it.
      if (nseq > 0)
        finish_seq (seqhash, nseq, seqit, prevcalo, pars);

      // Start a new sequence.
      nseq = 0;
      seqhash = hash;
      prevcalo = subcalo;
      // Leave space to fill in the hash/count later.
      seqit = outit;
      ++outit;
      ++outit;
    }

    // Add the current cell on to sequence.
    if (!is_SC && subcalo == CaloCell_ID::TILE) {
      pack_tile (static_cast<const TileCell*>(cell), outit, pars);

      // Check to see if the provenance changed.
      if (version >= 502 &&
          (cell->provenance() & pars.m_prov_max_tile) != prevCellProvenance)
      {
        prevCellProvenance = cell->provenance() & pars.m_prov_max_tile;
        CaloCompactCellContainer::value_type data =
          pars.m_hash_field.in(hash) |
          pars.m_prov_field.in(prevCellProvenance);
        vProvenance.push_back ((data>>16) & 0xffff);
        vProvenance.push_back (data & 0xffff);
      }
    }
    else {
      pack_lar (cell, subcalo, outit, pars);

      // Check to see if the provenance changed.
      if (version >= 500 &&
          (cell->provenance() & pars.m_prov_max) != prevCellProvenance)
      {
        prevCellProvenance = cell->provenance() & pars.m_prov_max;
        CaloCompactCellContainer::value_type data =
          pars.m_hash_field.in(hash) |
          pars.m_prov_field.in(prevCellProvenance);
        vProvenance.push_back ((data>>16) & 0xffff);
        vProvenance.push_back (data & 0xffff);
      }
    }
    ++nseq;
  }

  // Finish the last sequence.
  if (nseq != 0)
    finish_seq (seqhash, nseq, seqit, prevcalo, pars);

  pars.m_lengthProvenance = vProvenance.size();

  assert (outit.used() + pars.m_length + vProvenance.size()/2 + 1 <= maxsize);

  if (pars.m_lengthProvenance > 0) {
    // this part is making existing vector to have full number of 32 bit words.
    int x=outit.used();
    // dummy value of 0 is inserted
    outit.set(0);
    int y=outit.used();
    // if size changed it means that vector was flat at the end and that we spoiled it.
    // so we have to fix it.
    if (x!=y) outit.set(0);
    // now its surely flat.

    // add provenance stuff at the end.
    for (unsigned short & iter : vProvenance)
    {
      outit.set (iter);
    }
  }

  // Now resize the container to the final size.
  packed.resize (outit.used() + pars.m_length);

  // And copy the header to the front.
  write_header (pars, packed);
}


/**
 * @brief Write the header to the output container.
 * @param header The header to write.
 * @param packed The container to which to write.
 */
void
CaloCellPacker_400_500::write_header (const header& header,
                                      CaloCompactCellContainer& packed) const
{
  const int * phead = &(header.m_length);
  std::vector<CaloCompactCellContainer::value_type> 
    vhead (phead, phead + header.m_length);
  packed.setHeader(vhead);
}



//============================================================================
// Unpacking.
// To ensure full inlining, don't reorder these.
//


/**
 * @brief Unpack the time word.
 * @param it Input iterator.
 * @param pars Unpacking parameters.
 * @return The unpacked time.
 */
inline
double CaloCellPacker_400_500::unpack_time
   (CaloCompactCellContainer::compact_input_iterator& it,
    const pars500& pars) const
{
  // Get the word from the input container.
  CaloCompactCell::value_type data = it.next();

  // Unpack to a float.
  int underflow;
  double time = pars.m_logat_field.out (data, underflow);
  if (UNLIKELY(underflow))
    return 0;

  // Exponentiate, and restore the sign.
  // (nb. introducing `ee' helps the gcc optimizer avoid a redundant
  // test on the underflow flag.)
  double ee = std::exp (time);
  if (data & pars.m_tsign_mask)
    return -ee;
  return ee;
}


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
inline
CaloCell*
CaloCellPacker_400_500::unpack_lar
   (CaloCompactCellContainer::compact_input_iterator& it,
    CaloCell_ID::SUBCALO subcalo,
    LArCell* cell,
    const pars500& pars,
    uint16_t provenance) const
{
  // Get the data word from the input.
  CaloCompactCell::value_type data = it.next();

  // Recognize cells filled with dummy values.
  if (data == pars.m_lar_dummy)
  {
    cell->set (0, 0, 0, provenance, CaloGain::INVALIDGAIN);
    return cell;
  }

  // Unpack the gain flag.
  int gainflag = pars.m_egain_field.out (data);
  int qualflag = pars.m_qualy_field.out (data);

  CaloGain::CaloGain gain = CaloGain::UNKNOWNGAIN;

  // Convert to the CaloCell gain, and convert the energy back to a float
  // using the proper range.
  double energy;
  if ( gainflag == pars.m_ehhig ) {
    if ( subcalo == CaloCell_ID::LARHEC )
      gain = CaloGain::LARMEDIUMGAIN;
    else
      gain = CaloGain::LARHIGHGAIN;
    energy = pars.m_crtae_high_field.out (data);
  }
  else {
    if ( gainflag == pars.m_enhig ) {
      gain = CaloGain::LARHIGHGAIN;
    }
    else if ( gainflag == pars.m_enmed ) {
      gain = CaloGain::LARMEDIUMGAIN;
    }
    else if ( gainflag == pars.m_enlow) {
      gain = CaloGain::LARLOWGAIN;
    }
    energy = pars.m_crtae_norm_field.out (data);
  }

  // Now undo the cube root and apply the sign bit.
  energy = energy*energy*energy;
  if (data & pars.m_esign_mask)
    energy = -energy;

  // If the quality's good, then we need to unpack the time/chi2 too.
  double time = 0;
  uint16_t quality=0;
  if ( qualflag != pars.m_qabad ) {
    time = unpack_time (it, pars);
    if (pars.m_version >= 500)
      quality=it.next();
    provenance = provenance | 0x2000;
  }

  // Fill the data into the cell.
  cell->set (energy, time, quality, provenance, gain);

  // Return it.
  return cell;
}


/**
 * @brief Unpack a tile cell.
 * @param it Input iterator.
 * @param dde Descriptor element for the cell.
 * @param pars Unpacking parameters.
 * @return The new cell.
 */
inline
TileCell CaloCellPacker_400_500::unpack_tile
   (CaloCompactCellContainer::compact_input_iterator& it,
    const CaloDetDescrElement* dde,
    const pars500& pars,
    uint16_t provenance) const
{
  // Loop over the two elements for the cell.
  double ene[2];
  double time[2];
  int gain[2];
  int qbit[2];

  bool read_qual = false;

  for (int ipmt = 0; ipmt < 2; ++ipmt) {
    // Unpack the data word.
    CaloCompactCell::value_type data = it.next();

    // Is this measurement a dummy?
    if (data == pars.m_tile_dummy)
    {
      // Yeah --- fill in dummy values and skip the rest.
      ene[ipmt] = 0;
      gain[ipmt] = CaloGain::INVALIDGAIN;
      qbit[ipmt] = 0;
      if (ipmt == 1)
        time[ipmt] = time[0];
      else
        time[ipmt] = 0;
      continue;
    }

    // Get the quality and gain.
    int qualflag = pars.m_qualy_field.out (data);
    int gainflag = pars.m_egain_tile_field.out (data);

    gain[ipmt] = gainflag;

    // Unpack the energy, using the range appropriate to the gain.
    double e;
    if (gainflag != pars.m_glow)
      e = pars.m_crtae_tile_high_field.out (data);
    else
      e = pars.m_crtae_tile_low_field.out (data);
    ene[ipmt] = e*e*e;
    if (data & pars.m_esign_tile_mask)
      ene[ipmt] = -ene[ipmt];

    // If the quality is good, we need to unpack the time too.
    if (qualflag != pars.m_qabad) {
      read_qual = true;
      time[ipmt] = unpack_time (it, pars);
      qbit[ipmt] = TileCell::MASK_CMPC | TileCell::MASK_TIME;
    }
    else {
      time[ipmt] = 0;
      qbit[ipmt] = TileCell::MASK_CMPC;
    }
  }

  uint8_t qual[2];
  if (pars.m_version >= 502) {
    qbit[0] |= pars.m_tile_qual1_field.out (provenance);
    qbit[1] |= pars.m_tile_qual2_field.out (provenance);
    if (read_qual) {
      uint16_t qualp = it.next();
      qual[0] = pars.m_tile_qual1_field.out (qualp);
      qual[1] = pars.m_tile_qual2_field.out (qualp);
    } else {
      qual[0] = ((qbit[0] & TileCell::MASK_BADCH) != 0) ? 255 : 0;
      qual[1] = ((qbit[1] & TileCell::MASK_BADCH) != 0) ? 255 : 0;
    }
  }
  else {
    qual[0] = qual[1] = 0;
  }

  // Make the cell.
  return TileCell (dde, ene[0], ene[1], time[0], time[1],
                   qual[0], qual[1], qbit[0], qbit[1], gain[0], gain[1]);
}


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
void CaloCellPacker_400_500::unpack
   (const CaloCompactCellContainer& packed,
    const std::vector<int>& vheader,
    CaloCellContainer& cells,
    DataPool<LArCell>& larpool,
    DataPool<TileCell>& tilepool) const
{
  // Convert the header.
  pars500 pars;
  pars.m_status = 0;
  pars.m_seq_tile = 0;
  pars.m_seq_larem = 0;
  pars.m_seq_larhec = 0;
  pars.m_seq_larfcal = 0;
  pars.m_lengthProvenance = 0;
  pars.m_ncells_tile = 0;
  pars.m_ncells_larhec = 0;
  pars.m_ncells_larfcal = 0;
  pars.m_ncells_larem = 0;

  {
    const int* headerbeg = &*vheader.begin();
    const int* headerend = headerbeg + vheader.size();
    size_t nheader = headerend - headerbeg;
    size_t parsize = sizeof(header) / sizeof(int);
    size_t ncopy = std::min (nheader, parsize);
    int* parsbeg = &pars.m_length;
    std::copy (headerbeg, headerbeg+ncopy, parsbeg);
    if (nheader > parsize) {
      // Header was longer than we expected --- there's something
      // wrong with the data.  Issue a warning.
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloCellPacker_400_500 ")
        << "Corrupted data: Compact cell header is "
        << nheader << " words long, longer than the largest expected value of "
        << parsize << ".";
    }
    else if (ncopy < parsize) {
      // Header was shorter than we expected.
      // It may be a previous version --- clear out the remainder of pars.
      // TODO: Cross-check the size we got with what we expect based
      // on the version number in the header.
      std::fill (parsbeg + ncopy, parsbeg + parsize, 0);
    }
  }

  // Initialize derived parameters from the header.
  init_derived (pars);

  // Bounds check on size of provenance.
  unsigned nprov = pars.m_lengthProvenance/2;
  if (nprov + vheader.size() > packed.getData().size()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloCellPacker_400_500 ")
      << "Corrupted data: Provenance count too large "
      << pars.m_lengthProvenance << ".";
    pars.m_lengthProvenance = 0;
    nprov = 0;
  }

  // need to make a new iterator and use it for provenance.
  CaloCompactCellContainer::compact_input_iterator provIt =
    packed.compact_begin_input_from(pars.m_lengthProvenance/2);
  unsigned iprov=0;

  // getting starting values
  short unsigned int currProvValue=0;
  int nextProvHash=-1;
  short unsigned int nextProvValue=0;
  if (pars.m_lengthProvenance) {
    unsigned int provhash = provIt.next();
    provhash = (provhash<<16) | provIt.next();
    nextProvValue = pars.m_prov_field.out (provhash);
    nextProvHash  = pars.m_hash_field.out (provhash);
    iprov++;
  }

  bool is_SC = (pars.m_status & header::STATUS_IS_SUPERCELL);

  // We need the detector description.
  const CaloDetDescrManager_Base *ddmgr = nullptr;
  if (is_SC){
    ddmgr = CaloSuperCellDetDescrManager::instance();
  }else
  {
    ddmgr = CaloDetDescrManager::instance();
  }
  const CaloCell_Base_ID *calo_id = ddmgr->getCaloCell_ID();

  // Clear the output container and reserve the right number of elements.
  cells.clear(SG::VIEW_ELEMENTS);
  CaloCell_ID::size_type totcells =
    pars.m_ncells_larem   + pars.m_ncells_larhec + 
    pars.m_ncells_larfcal + pars.m_ncells_tile;
  if (totcells > calo_id->calo_cell_hash_max()) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloCellPacker_400_500 ")
      << "Corrupted data: Too many cells " << totcells << ".";
    totcells = calo_id->calo_cell_hash_max();
  }
  cells.reserve (totcells);

  // To speed things up, we'll use the underlying vector from the cell
  // container.
  std::vector<CaloCell*>& cellsv =
    const_cast<std::vector<CaloCell*>&>(cells.stdcont());

  // Iterator for scanning the input.
  CaloCompactCellContainer::compact_input_iterator it =
    packed.compact_begin_input();

  // To test for falling off the end.
  std::vector<CaloCompactCellContainer::value_type>::const_iterator pend =
    packed.getData().end() - (pars.m_lengthProvenance+1)/2;

  // Sum up the total number of cells/sequences over all subcalos.
  unsigned int ncells =
    pars.m_ncells_larem +
    pars.m_ncells_larhec +
    pars.m_ncells_larfcal +
    pars.m_ncells_tile;
  unsigned int nseqs = 
    pars.m_seq_larem +
    pars.m_seq_larhec +
    pars.m_seq_larfcal +
    pars.m_seq_tile;

  // Note: In the first version of the v400 packer, the sequence counts
  // would be left uninitialized if ncells==0.
  if (ncells == 0)
    nseqs = 0;

  // Warn if there are too many cells/sequences.
  IdentifierHash hashmax = calo_id->calo_cell_hash_max();
  if (ncells > hashmax || nseqs > hashmax || nseqs > ncells) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloCellPacker_400_500 ")
      << "Corrupted data: Bad counts"
      << ": ncells " << ncells << " nseqs " << nseqs << " hashmax " << hashmax;
  }

  CaloCell_ID::SUBCALO prevcalo = CaloCell_ID::NOT_VALID;

  // Loop over sequences.
  while (nseqs--) {
    
    // Check for overrun.
    if (it.base() >= pend) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloCellPacker_400_500 ")
        << "Corrupted data: cell vector overrun.";
      break;
    }

    // Get the starting hash code and count.
    CaloCompactCell::value_type data = it.next();
    unsigned int hashlength = data << 16;
    hashlength |= it.next();

    unsigned int hash = pars.m_hash_field.out (hashlength);
    unsigned int nseq = pars.m_nseq_field.out (hashlength);

    if (nseq > ncells || hash+nseq > hashmax)
    {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                  "CaloCellPacker_400_500 ")
        << "Corrupted data: bad sequence.  "
        << "nseq " << nseq << " hash " << hash << " ncells " << ncells
        << " hashmax " << hashmax;
      break;
    }

    // Subcalo code for this cell.
    CaloCell_ID::SUBCALO subcalo = 
      static_cast<CaloCell_ID::SUBCALO> (calo_id->sub_calo (hash));

    // Maintain the cell container's pointers --- only if cells were ordered!
    if (subcalo != prevcalo) {
      if ((pars.m_status & header::STATUS_UNORDERED) == 0) {
        if (prevcalo != CaloCell_ID::NOT_VALID) {
          cells.updateCaloEndIterators (prevcalo, cells.size());
          if (subcalo < prevcalo) {
            REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                        "CaloCellPacker_400_500 ")
              << "Cells not in subcalo order; iterators will be wrong.";
          }
        }
        cells.updateCaloBeginIterators (subcalo, cells.size());
      }
      cells.setHasCalo (subcalo);
      prevcalo = subcalo;
    }

    // Loop over cells in the sequence.
    while (nseq--) {
      // Find the descriptor element for this cell.
      const CaloDetDescrElement *dde = ddmgr->get_element(hash);

      if (dde == nullptr) {
        REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                    "CaloCellPacker_400_500 ")
          << "Corrupted data: can't find DDE for cell with hash " << hash;
      }

      // Unpack the cell.
      //
      // One point here that needs explaining.
      // The pack_tile method returns a TileCell by value.
      // Normally, this would involve a copy; however, we're careful
      // to allow the compiler to use the `return value optimization'
      // to avoid this copy.  That way, we can use the TileCell constructor
      // directly, without having to add extra set methods (or violate
      // encapsulation, as the previous version of this code did).
      // We get a pointer to a TileCell from the data pool and
      // use this in a placement new to initialize a TileCell
      // using the value returned from unpack_tile.  Due to the RVO,
      // the copy is avoided, and the constructor in unpack_tile
      // will run its constructor directly on the pointer gotten
      // from the pool.  For this to work correctly, we rely
      // on the fact that TileCell has only a trivial destructor ---
      // thus it's safe to rerun the constructor on an object that
      // has already been constructed.
      //
      // You'll notice, however, that CaloCell is done differently.
      // CaloCell is not a simple class; it has a complicated inheritance
      // structure.  It was found that setting up all the vtable pointers
      // in CaloCell was taking a considerable amount of time
      // (comparable to filling in the cell data).  So, for CaloCell,
      // we instead add a couple new (inlined) set methods to fill
      // in the data directly, without having to redo the vtable pointers.
      // We split it in two, one for dde/id and the other for the cell data,
      // to reduce the amount of data we need to pass to unpack_lar.
      //
      // Why wasn't the same thing done for TileCell, then?
      //   - I don't have TC rights for TileEvent, so changing that
      //     is more of a hassle.
      //   - There are many fewer tile cells than LAr cells.
      //   - The TileCell constructor we use is significantly
      //     more complicated than LArCell.  We'd have to duplicate
      //     this code.  Further, this, together with the previous
      //     point, implies that the performance implications
      //     of rebuilding the vtable pointers is much less
      //     for tile cells than for LAr cells.

      // New provenance?
      if (hash==static_cast<unsigned int>(nextProvHash)){
        currProvValue = nextProvValue;
        if (iprov<nprov) {
          unsigned int provhash = provIt.next();
          provhash = (provhash<<16) | provIt.next();
          nextProvValue = pars.m_prov_field.out (provhash);
          nextProvHash  = pars.m_hash_field.out (provhash);
          iprov++;
        } else {
          nextProvHash = -1;
        }
      }

      CaloCell* cell;
      if (!is_SC && subcalo == CaloCell_ID::TILE)
        cell = new (tilepool.nextElementPtr())
          TileCell (unpack_tile (it,
                                 dde,
                                 pars, currProvValue));
      else {
        cell = unpack_lar (it,
                           subcalo,
                           larpool.nextElementPtr(),
                           pars,
                           currProvValue);
        cell->set (dde, calo_id->cell_id(hash));
      }

      // Add the cell to the container.
      if (dde)
        cellsv.push_back (cell);

      // Move to next cell.
      ++hash;
    }
  }

  // Finish off the last iterator.
  if (prevcalo != CaloCell_ID::NOT_VALID)
    cells.updateCaloEndIterators (prevcalo, cells.size());

  // Check that we've consumed all the data.
  // Note that there may be a padding word before the provenance word.
  if (it.base() < pend-2) {
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING, 
                                "CaloCellPacker_400_500 ")
      << "Corrupted data: didn't consume all packed data.";
  }
}


//============================================================================
