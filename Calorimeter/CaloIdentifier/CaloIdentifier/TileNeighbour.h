/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>   $Id: TileNeighbour.h,v 1.6 2007-07-08 19:22:30 solodkov Exp $
//<version>     $Name: not supported by cvs2svn $

// TileNeighbour.h
//
// Helper for neighbour search in TileCalorimeter
//
// Aug-2003 by AS
//

#ifndef CALOIDENTIFIER_TILENEIGHBOUR_H
#define CALOIDENTIFIER_TILENEIGHBOUR_H

#include "CaloIdentifier/LArNeighbours.h"

#include <string>
#include <vector>
#include <set>

/** 
* @class TileNeighbour
* @brief This class return all neighbours for given hash ID of the cell
* @author Alexander Solodkov
*/

class Tile_Base_ID;
class Identifier;
class IdentifierHash;
class IdContext;
class MsgStream;

class TileNeighbour {

public:

  // default constructor
  TileNeighbour(void);

  // default destructor
  virtual ~TileNeighbour(void);

  int initialize(const Tile_Base_ID* tileID, const std::string& filename = "TileNeighbour.txt" );
  int prev_phi  (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int next_phi  (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int prev_eta  (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int next_eta  (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int prev_samp (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int next_samp (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int prev_samp_wide (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;
  int next_samp_wide (const IdentifierHash & id, std::vector<IdentifierHash> & neighbourList)    const;

private:

  unsigned int m_debug;
  unsigned int m_length;
  unsigned int m_maxHash;
  
  std::vector<std::vector<short int> > m_prev_eta;
  std::vector<std::vector<short int> > m_next_eta;
  std::vector<std::vector<short int> > m_prev_samp;
  std::vector<std::vector<short int> > m_next_samp;
  std::vector<std::vector<short int> > m_prev_samp_wide;
  std::vector<std::vector<short int> > m_next_samp_wide;

  std::vector<short int>      m_phid_index;
  std::vector<short int>      m_cell_index;
  std::vector<IdentifierHash> m_hashid_vec;

  int initialize_prev_eta (unsigned int ind, const std::vector<short int> & all_cells);
  int initialize_next_eta (unsigned int ind, const std::vector<short int> & all_cells);
  int initialize_prev_samp(unsigned int ind, const std::vector<short int> & all_cells);
  int initialize_next_samp(unsigned int ind, const std::vector<short int> & all_cells);
  int initialize_prev_samp_wide(unsigned int ind, const std::vector<short int> & all_cells);
  int initialize_next_samp_wide(unsigned int ind, const std::vector<short int> & all_cells);

  int         fill_phi_vec  (std::set<std::pair<IdentifierHash,int> > & ids, 
                             unsigned int hash_max, unsigned int max_phi, MsgStream & log);

  void          get_id      (std::string & strName, Identifier & id, const Tile_Base_ID* tileID);

  void          get_name    (Identifier & id, std::string & section, int & module,
                             std::string & cell, const Tile_Base_ID* tileID, MsgStream & log,
                             const char * end);

  void          print_list  (std::vector<IdentifierHash> & nb_list, 
                             const Tile_Base_ID* tileID, const IdContext* context,
                             MsgStream & log, const char * pref, const char * suff);
};

#endif // CALOIDENTIFIER_TILENEIGHBOUR_H
