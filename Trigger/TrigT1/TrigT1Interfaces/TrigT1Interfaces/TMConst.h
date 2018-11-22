/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/****************************************************
 *
 * TMConst contains constants relevant to all the L1 trigger. There
 * clearly is some overlap with things like Trigt1Defs etc, and the 
 * thing needs cleaning up. Considering the little content, perhaps
 * I should event get rid of the file. 
 * Author: Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
 *
 ****************************************************/
#ifndef TRIGT1INTERFACES_TMCONST_H
#define TRIGT1INTERFACES_TMCONST_H

/* The maximum number of PITS altogether (summed over all LUTS) */
static const unsigned int MAXNUMPIT = 160;

/* The maximal number of items, summed over all CMBs */
static const unsigned int MAXITEMNUM = 96;

#endif // TRIGT1INTERFACES_TMCONST_H
