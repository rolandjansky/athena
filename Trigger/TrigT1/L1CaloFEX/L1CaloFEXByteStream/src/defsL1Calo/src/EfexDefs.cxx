//==============================================================================
// EfexDefs.cxx
//==============================================================================

#include <stdlib.h>
#include <iostream>

#include "defsL1Calo/EfexDefs.h"

/*!
 *  \class EfexDefs
 *  This class provides static methods to handle input fibre mappings:
 *  between MTP input connectors and minipods on the board, also for
 *  mapping of fanned out fibres to the quads on each FPGA.
 *  \todo **FIXME** These initial mappings are surely wrong: SJH 07/07/21 tried to get it right
 */
                         

//==============================================================================
// connectorForMinipod
//==============================================================================
/*!
 *  Return MTP connector number (0-3) for a given minipod.
 *  \param minipod input ribbon (minipod) number (0-9 EM, 10-12 hadronic)
 *  \return input connector number (0-3, -1 for invalid minipod)
 */
int
EfexDefs::connectorForMinipod(int minipod)
{
  // Actual mapping has hadronic on connector 1 (of 0-3)
  // and the two minipod central region on connector 3.
  // **CHECK!!**  
  // SJH 07/07/21 Updated in line with Gareth's mapping
  // Hopefully Gareth's mapping is correct
  if      (minipod >=  0 && minipod <  4) { return  0; }  // E0-E3
  else if (minipod >=  4 && minipod <  6) { return  2; }  // E4-E5
  else if (minipod >=  6 && minipod < 10) { return  3; }  // E6-E9
  else if (minipod >= 10 && minipod < 13) { return  1; }  // H0-H2
  else                                    { return -1; }
}

//==============================================================================
// connectorFirstFibre
//==============================================================================
/*!
 *  Return first fibre in the input MTP connector for a given minipod.
 *  \param minipod input ribbon (minipod) number (0-9 EM, 10-12 hadronic)
 *  \return first fibre number (0,12,24,36)
 */
int
EfexDefs::connectorFirstFibre(int minipod)
{
  // Not all 12-fibre ribbons are used, so skip unused ones
  // to map onto minipods.
  int conn = EfexDefs::connectorForMinipod(minipod);
  // SJH 07/07/21 Updated in line with Gareth's mapping
  // which we believe is correct.  It's rather messier
  // than earlier assumptions
  // **CHECK!!**
  int numUnused = 0;
  int reverse = 1;
  if      (conn  < 0) { return -1; }
  else if (conn == 0) { numUnused = 0; reverse = 1;  }
  else if (conn == 1) { numUnused = 3; reverse = 1; } 
  else if (conn == 2) { numUnused = 2; reverse = -1; } 
  else if (conn == 3) { numUnused = 1; reverse = -1;  } 
  int rpc = EfexDefs::numRibbonsPerConnector();
  int ribbon = (5*rpc + minipod*reverse + numUnused) % rpc;
  // Deal with annoying hadronic case...
  if ( minipod == 12 )
    ribbon = 0;
  return ribbon * FexDefs::numFibresPerRibbon();
}

//==============================================================================
// globalEtaIndex
//==============================================================================
/*!
 *  Return unsigned eta bin within eFEX system (used for calibration channels).
 *  Return first fibre in the input MTP connector for a given minipod.
 *  \param efexLogicalID logical ID of eFEX within crate (0-11)
 *  \param fpgaNumber index of FPGA (0-3)
 *  \param etaWithinFpga eta bin within FPGA (0-4)
 *  \return unsigned global eta bin in range 0-49 (where 0 is -2.5)
 */
unsigned int
EfexDefs::globalEtaIndex(unsigned int efexLogicalID,
                         unsigned int fpgaNumber,
                         unsigned int etaWithinFpga)
{
  unsigned int positionID = efexLogicalID % 3;   // eFEX C,B,A role
  unsigned int etaIndex = (16 * positionID) + (fpgaNumber * 4) + etaWithinFpga;
  etaIndex += (positionID != 0 || fpgaNumber != 0) ? 1 : 0;  // Adjustment for ends
  return etaIndex;
}

//==============================================================================
// globalEtaValue
//==============================================================================
/*!
 *  Return signed eta bin within eFEX system (used for trigger menu API).
 *  \param efexLogicalID logical ID of eFEX within crate (0-11)
 *  \param fpgaNumber index of FPGA (0-3)
 *  \param etaWithinFpga eta bin within FPGA (0-4)
 *  \return signed global eta bin in range -25 to +24
 */
int
EfexDefs::globalEtaValue(unsigned int efexLogicalID,
                         unsigned int fpgaNumber,
                         unsigned int etaWithinFpga)
{
  return (int)EfexDefs::globalEtaIndex(efexLogicalID,fpgaNumber,etaWithinFpga) - 25;
}
