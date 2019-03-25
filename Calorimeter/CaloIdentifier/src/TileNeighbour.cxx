/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Access to Tile Calorimeter raw data
 -----------------------------------------
 ***************************************************************************/

//<doc><file>   $Id: TileNeighbour.cxx,v 1.13 2008-12-13 04:57:11 ssnyder Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "CaloIdentifier/TileNeighbour.h"
#include "CaloIdentifier/TileID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "PathResolver/PathResolver.h"
#include "GaudiKernel/MsgStream.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <string.h>

using namespace LArNeighbours;

//<<<<<< PRIVATE DEFINES                                                >>>>>>

#define MAX_TOKEN_SIZE 256

//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>

typedef struct
{
  Identifier id;
  IdentifierHash hash_id;
  short int index = 0;

  std::string name;
  std::vector<std::string> neighbours[4];
  std::vector<int> neighbours_ind[4];

} Cell;

//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

TileNeighbour::TileNeighbour(void) 
  : m_debug(0)
  , m_length(0)
  , m_maxHash(0)
{
}

TileNeighbour::~TileNeighbour(void) 
{
}

int TileNeighbour::initialize(const Tile_Base_ID* tileID, std::string filename)
{
  MsgStream log(tileID->m_msgSvc, "TileNeighbour" );
  MSG::Level logLevel = log.level();
  if (logLevel==MSG::VERBOSE) m_debug = 1;
  if (logLevel< MSG::VERBOSE) m_debug = 2;

  // Find the full path to filename:
  std::string file = PathResolver::find_file (filename, "DATAPATH");
  log << MSG::INFO << "Reading file  " << file << endmsg;
  std::ifstream fin;
  if (file != "") {
    fin.open(file.c_str());
  }
  else {
    log << MSG::ERROR << "Could not find input file " << filename <<  endmsg;
    return 1;
  }
  if (fin.bad()) {
    log << MSG::ERROR << "Could not open file " << file << endmsg;
    return 1;
  }

  //
  // Parse the input file
  //
  unsigned int line=0, record=0;              // file line number, record number
  char token[MAX_TOKEN_SIZE];                 // input token

  log << MSG::VERBOSE << "Parsing input file:" << endmsg;
    
  std::vector<Cell> allCells;

  while ( fin >> token ) {
    line++;                                 // count input lines
    if ( token[0] == '#' || token[0] == '!' || ( token[0] == '/' && token[1] == '/' ) ) {
      fin.ignore(MAX_TOKEN_SIZE, '\n'); 
      continue;                           // skip comments
    }

    unsigned int toklen = strlen(token) - 1 ;
    token[toklen] = 0; // remove ":" at the end
    log << MSG::VERBOSE << token << "  ";

    Cell newCell;
    newCell.name = token;
    for (unsigned int i=0; i<4; ++i) newCell.neighbours[i].clear();
    
    unsigned int column=0;
    while ( fin >> token) {
      toklen = strlen(token) - 1;
      bool lastelem = ( token[toklen] == ';' );
      token[toklen] = 0; // remove "," or ";"
      log << token << "  ";
      if (strcmp(token,"none"))
        newCell.neighbours[column].push_back(token);
      if (lastelem) {
        ++column;
        if (4 == column) break;
      }
    }

    fin.ignore(MAX_TOKEN_SIZE, '\n'); // skip to eol

    log << endmsg;
    allCells.push_back(newCell);
    record++;				     // count input records
    
  } // end while fin
 
  fin.close();

  log << MSG::DEBUG << "Processed " << line << " lines, " << record << " records." << endmsg;

  unsigned int curSize = allCells.size();
  for (unsigned int i=0; i<curSize; ++i) {

    std::string::size_type pos = allCells[i].name.find( "-", 0 );
    if ( std::string::npos != pos ) { // it is not D0 cell

      Cell newCell;
      std::string tmpName;
      tmpName = allCells[i].name;
      tmpName.replace(pos,1,"+");
      newCell.name = tmpName;

      for (unsigned int j=0; j<4; ++j) {
        unsigned int j1 = (j<2) ? 1-j : j; // swap prev and next eta
        unsigned int nb_size=allCells[i].neighbours[j].size();
        for (unsigned int k=0; k<nb_size; ++k) {
          tmpName = allCells[i].neighbours[j][k];
          pos = tmpName.find( "-", 0 );
          if ( std::string::npos != pos ) {
            tmpName.replace(pos,1,"+");
          } else {
            pos = tmpName.find( "+", 0 );
            if ( std::string::npos != pos ) {
              tmpName.replace(pos,1,"-");
            }
          }
          newCell.neighbours[j1].push_back(tmpName);
        }
      }
      allCells.push_back(newCell);
    }
  }

  unsigned int nids = 0;
  unsigned int max_phi = 0, phi=0;
  std::set<std::pair<IdentifierHash,int> > ids;
  ids.clear();
  for (unsigned int i = 0; i < tileID->m_full_cell_range.size(); ++i) {
    const Range& range = tileID->m_full_cell_range[i];
    Range::const_identifier_factory first = range.factory_begin();
    Range::const_identifier_factory last  = range.factory_end();
    for (; first != last; ++first) {
      const ExpandedIdentifier& exp_id = (*first);
      phi = exp_id[tileID->m_MODULE_INDEX];
      if (phi > max_phi) max_phi = phi;
      Identifier id = tileID->cell_id (  exp_id[tileID->m_SECTION_INDEX],
                                         exp_id[tileID->m_SIDE_INDEX], 
                                         exp_id[tileID->m_MODULE_INDEX], 
                                         exp_id[tileID->m_TOWER_INDEX],
                                         exp_id[tileID->m_SAMPLE_INDEX]);
      IdentifierHash hashid = tileID->channels().hash (id);
      if(!(ids.insert(std::make_pair(hashid,phi))).second){
        log << MSG::ERROR << "init_hashes "
            << " Error: duplicated id for cell id. nids= " << nids
            << " compact Id  " << tileID->show_to_string(id)
            << endmsg;
      }
      nids++;
    }
  }
  ++max_phi;
  m_maxHash = tileID->m_full_cell_range.cardinality();
  m_length = m_maxHash / max_phi;
  if ( fill_phi_vec(ids, m_maxHash, max_phi, log) ) {
    m_maxHash = m_length = 0;
    return (1);
  }
  
  curSize = allCells.size();

  std::string nb_name[4] = {"Prev_eta_", "Next_eta_", "Prev_smp_", "Next_smp_"};
  
  for (unsigned int i=0; i<curSize; ++i) {
    get_id(allCells[i].name,allCells[i].id,tileID);
    allCells[i].hash_id = tileID->channels().hash (allCells[i].id);
    for (unsigned int ii=0; ii<m_length; ++ii) {
      if ( m_hashid_vec[ii] == allCells[i].hash_id) {
        allCells[i].index = ii;
        break;        
      }
    }

    for (unsigned int j=0; j<4; ++j) {
      unsigned int nb_size=allCells[i].neighbours[j].size();
      allCells[i].neighbours_ind[j].resize(nb_size);
      for (unsigned int k=0; k<nb_size; ++k) {
        for (unsigned int ii=0; ii<curSize; ++ii) {
          if ( allCells[ii].name == allCells[i].neighbours[j][k]) {
            allCells[i].neighbours_ind[j][k] = ii;
            if (m_debug) {
              log << MSG::VERBOSE << allCells[i].name << " "
                  << tileID->to_string(allCells[i].id,-2) << " "
                  << allCells[i].hash_id << " "
                  << allCells[i].index << " "
                  << nb_name[j] << k << " "
                  << allCells[i].neighbours[j][k] << " "
                  << allCells[i].neighbours_ind[j][k] << " "
                  << endmsg;
            }
            break;
          }
        }
      }
    }
  }
  
  m_prev_eta.resize(m_length);
  m_next_eta.resize(m_length);
  m_prev_samp.resize(m_length);
  m_next_samp.resize(m_length);
  m_prev_samp_wide.resize(m_length);
  m_next_samp_wide.resize(m_length);

  std::vector<short int> neighbours[4];
  for (unsigned int i=0; i<curSize; ++i) {
    unsigned int ind = allCells[i].index;

    for (unsigned int j=0; j<4; ++j) {
      unsigned int nb_size=allCells[i].neighbours[j].size();
      neighbours[j].resize(nb_size);
      for (unsigned int k=0; k<nb_size; ++k) {
        neighbours[j][k] = allCells[allCells[i].neighbours_ind[j][k]].index;
      }
    }

    initialize_prev_eta (ind,neighbours[0]);
    initialize_next_eta (ind,neighbours[1]);
    initialize_prev_samp(ind,neighbours[2]);
    initialize_next_samp(ind,neighbours[3]);

    // build wide prev/next sampling for 3D neighbours with corners

    // tower and side of current cell
    int sd = tileID->side(allCells[i].id);
    int tw = tileID->tower(allCells[i].id);
    int sm = tileID->sample(allCells[i].id);
    int se = tileID->section(allCells[i].id);
    
    for (unsigned int j=2; j<4; ++j) {
      // find eta neighbours for all cells in prev/next sampling
      // and keep only those which are not too far away
      unsigned int nb_size=allCells[i].neighbours[j].size(); // how many cells in prev/next sampling
      for (unsigned int k=0; k<nb_size; ++k) { // loop over them
        unsigned int new_i = allCells[i].neighbours_ind[j][k]; // this is ind of cell in other sampling
        for (unsigned int new_j=0; new_j<2; ++new_j) { // loop over prev/next eta for new cell
          unsigned int new_nb_size=allCells[new_i].neighbours[new_j].size();
          for (unsigned int new_k=0; new_k<new_nb_size; ++new_k) { // all new neighbours
            unsigned int new_nb_i = allCells[new_i].neighbours_ind[new_j][new_k];
            short new_nb_index = allCells[new_nb_i].index; // hash index of new neighbour
            int n=neighbours[j].size()-1;
            for ( ; n>-1; --n) { // check for duplicates
              if (neighbours[j][n] == new_nb_index)
                break;
            }
            if (n<0) { // new cell, check how far is it
              int new_sd = tileID->side(allCells[new_nb_i].id);
              int new_tw = tileID->tower(allCells[new_nb_i].id);
              int new_sm = tileID->sample(allCells[new_nb_i].id);
              int new_se = tileID->section(allCells[new_nb_i].id);
              if ( ( new_sd != sd && new_tw == tw ) || // only tower=0 from other side
                   ( new_sd == sd && (                            // the same eta side
                     ( new_se == se && abs(new_tw - tw) < 3 ) ||  // withit one detector
                     ( new_se != se && (                          // when we jump over crack
                       ( new_sm == sm+1 && abs(new_tw - tw) < 3 ) || // next sampling
                       ( new_sm != sm+1 && abs(new_tw - tw) < 4 ) ))))) { // previous sampling
                neighbours[j].push_back(new_nb_index);  // not too far away, take it
              }
            }
          }
        }
      }
    }
    
    initialize_prev_samp_wide(ind,neighbours[2]);
    initialize_next_samp_wide(ind,neighbours[3]);
  }
  
  if (m_debug) {

    IdContext context = tileID->cell_context();
    std::vector<IdentifierHash> nb_list;

    unsigned int hash_max = tileID->cell_hash_max();
    for (unsigned int i=0; i<hash_max; ++i) {
      
      Identifier id;
      tileID->get_id (i, id, &context);
      if ( tileID->module(id) != 0 ) continue;

      IdentifierHash hash_id=i;
      nb_list.clear(); nb_list.push_back(hash_id);
      print_list(nb_list, tileID, &context, log, "\nCell     "," : ");

      tileID->get_neighbours(hash_id, prevInPhi, nb_list);
      print_list(nb_list, tileID, &context, log, "Prev phi "," ;  ");
 
      tileID->get_neighbours(hash_id, nextInPhi,  nb_list);
      print_list(nb_list, tileID, &context, log, "Next phi "," ;  ");

      tileID->get_neighbours(hash_id, prevInEta,  nb_list);
      print_list(nb_list, tileID, &context, log, "Prev eta "," ;  ");

      tileID->get_neighbours(hash_id, nextInEta,  nb_list);
      print_list(nb_list, tileID, &context, log, "Next eta "," ;  ");

      tileID->get_neighbours(hash_id, prevInSamp, nb_list);
      print_list(nb_list, tileID, &context, log, "Prev smp "," ;  ");

      tileID->get_neighbours(hash_id, nextInSamp, nb_list);
      print_list(nb_list, tileID, &context, log, "Next smp "," ;  ");

      tileID->get_neighbours(hash_id, faces2D, nb_list);
      print_list(nb_list, tileID, &context, log, "Face 2D  "," ;  ");

      tileID->get_neighbours(hash_id, corners2D, nb_list);
      print_list(nb_list, tileID, &context, log, "Corn 2D  "," ;  ");

      tileID->get_neighbours(hash_id, all2D, nb_list);
      print_list(nb_list, tileID, &context, log, "All  2D  "," ;  ");

      tileID->get_neighbours(hash_id, all3D, nb_list);
      print_list(nb_list, tileID, &context, log, "All  3D  "," ;  ");

      tileID->get_neighbours(hash_id, all3DwithCorners, nb_list);
      print_list(nb_list, tileID, &context, log, "All  3DC "," ;  ");
    
      tileID->get_neighbours(hash_id, super3D, nb_list);
      print_list(nb_list, tileID, &context, log, "Super 3D "," ;  ");
    
    }
  }
  
  return (0);
}

int TileNeighbour::fill_phi_vec  (std::set<std::pair<IdentifierHash,int> > & ids, 
                                  unsigned int hash_max, unsigned int max_phi, MsgStream & log)
{

  if(ids.size() != hash_max) {
    log << MSG::ERROR << "fill_phi_vec "
        << " Error: set size NOT EQUAL to hash max. size " << ids.size()
        << " hash max " << hash_max
        << endmsg;
    return (1);
  }

  m_phid_index.resize(hash_max);
  m_cell_index.resize(hash_max);
  m_hashid_vec.resize(hash_max);

  int dphi = hash_max / max_phi; // number of Ids in one phi-slice

  int ind_min=0, ind_max=-1, phi, phi_prev=-1, index=0;

  std::set<std::pair<IdentifierHash,int> >::const_iterator first = ids.begin();
  std::set<std::pair<IdentifierHash,int> >::const_iterator last  = ids.end();

  for (;first != last; ++first) {
    phi = (*first).second;
    if (phi != phi_prev) {
      if (0 == phi) {
        ++ind_max;
        ind_min = ind_max;
      }
      index = ind_min + dphi * phi;
      phi_prev = phi;
    } else {
      if (0 == phi) {
        ++ind_max;
      }
      ++index;
    }
    IdentifierHash hash_id = (*first).first;
    m_hashid_vec[index]    = hash_id ;
    m_phid_index[hash_id]  = dphi*phi;
    m_cell_index[hash_id]  = index - dphi*phi;
  }

  return (0);
}

int TileNeighbour::initialize_prev_eta(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_prev_eta [ind] = all_cells;
  return (0);
}

int TileNeighbour::initialize_next_eta(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_next_eta [ind] = all_cells;
  return (0);
}

int TileNeighbour::initialize_prev_samp(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_prev_samp [ind] = all_cells;
  return (0);
}

int TileNeighbour::initialize_next_samp(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_next_samp [ind] = all_cells;
  return (0);
}

int TileNeighbour::initialize_prev_samp_wide(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_prev_samp_wide [ind] = all_cells;
  std::sort(m_prev_samp_wide[ind].begin(),m_prev_samp_wide[ind].end());
  return (0);
}

int TileNeighbour::initialize_next_samp_wide(unsigned int ind,const std::vector<short int> & all_cells)
{
  m_next_samp_wide [ind] = all_cells;
  std::sort(m_next_samp_wide[ind].begin(),m_next_samp_wide[ind].end());
  return (0);
}

int TileNeighbour::prev_phi  (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];
  if ( phid == 0 ) phid = m_maxHash;
  phid -= m_length;
  
  neighbourList.push_back(m_hashid_vec[phid+cell]);
  
  return 1;
}

int TileNeighbour::next_phi  (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];
  phid += m_length;
  if ( phid == m_maxHash ) phid = 0;
  
  neighbourList.push_back(m_hashid_vec[phid+cell]);
  
  return 1;
}

int TileNeighbour::prev_eta  (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_prev_eta[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_prev_eta[cell][i]]);
  }

  return size;
}

int TileNeighbour::next_eta  (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_next_eta[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_next_eta[cell][i]]);
  }

  return size;
}

int TileNeighbour::prev_samp (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_prev_samp[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_prev_samp[cell][i]]);
  }

  return size;
}

int TileNeighbour::next_samp (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_next_samp[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_next_samp[cell][i]]);
  }

  return size;
}

int TileNeighbour::prev_samp_wide (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_prev_samp_wide[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_prev_samp_wide[cell][i]]);
  }

  return size;
}

int TileNeighbour::next_samp_wide (const IdentifierHash & id,
                              std::vector<IdentifierHash> & neighbourList)    const
{
  unsigned int phid = m_phid_index[id];
  unsigned int cell = m_cell_index[id];

  unsigned int size = m_next_samp_wide[cell].size();
  for (unsigned int i=0; i<size; ++i) {
    neighbourList.push_back(m_hashid_vec[phid+m_next_samp_wide[cell][i]]);
  }

  return size;
}

void TileNeighbour::get_id(std::string & strName, Identifier & id, const Tile_Base_ID* tileID)
{
    char * name = new char[strName.size()+1];
    strcpy(name,strName.data());
    int se;
    int sd;
    int sm;
    int tw;

    std::string::size_type pos = strName.find( "-", 0 );
    if ( std::string::npos != pos ) sd = -1; else sd = 1;

    sscanf(name+2,"%80d",&tw);
    if (tw<0) tw *= -1;

    switch ( name[0] ) {
        case 'a': case 'A': sm = 0; tw -= 1; 
            if (tw<10) se = 1; else se = 2;
            break;
        case 'b': case 'B': sm = 1; tw -= 1;
            if (tw<10) se = 1; else se = 2;
            break;
        case 'c': case 'C': sm = 1; tw -= 1;
            se = 3;
            break;
        case 'd': case 'D': sm = 2; tw *= 2;
            if (tw<10) se = 1; else se = 2;
            if (tw==8)  se = 3;
            break;
        case 'e': case 'E': sm = 3; 
            switch ( tw ) {
                case 1: tw = 10; break;
                case 2: tw = 11; break;
                case 3: tw = 13; break;
                case 4: tw = 15; break;
            }
            se = 3;
            break;
        case 's': case 'S': sm = 0; tw -= 1; 
            if (tw<9) se = 1; else se = 2;
            break;
        case 'v': case 'V': sm = 2; tw *= 2;
            if (tw<10) se = 1; else se = 2;
            break;
        default: se = 0; sm = -1; tw = -1; break;
    }

    id = tileID->cell_id(se,sd,0,tw,sm);

    delete[] name;
}

void TileNeighbour::get_name(Identifier & id, std::string & strSection,
                             int & module, std::string & strCell, 
                             const Tile_Base_ID* tileID, MsgStream & log, const char * end)
{
    int section  = tileID->section  (id);
    int side     = tileID->side     (id);
        module   = tileID->module   (id);
    int tower    = tileID->tower    (id);
    int sample   = tileID->sample   (id);
    bool supercell = tileID->is_supercell(id);

    switch (section) {
    case  0: strSection = "Online"; break;
    case  1: strSection = "Barrel"; break;
    case  2: strSection = "Ext.Barrel"; break;
    case  3: strSection = (sample < 3) ? "ITC": "Gap.Scin"; break;
    default: strSection = "Unknown";
    }

    int  tw=0;
    char sd = (side<0) ? '-' : '+';
    char sm=0,s1=0;

    switch (sample) {
    case 0: sm=(supercell?'S':'A');
            tw=tower+1; 
            break;
    case 1: sm='B';
            tw=tower+1;
            if (tw<9) s1='C';
            if (tw==10) sm='C';
            break;
    case 2: sm=(supercell?'V':'D');
            tw=tower/2;
            if (tw==0) {
              if (sd=='+') sd='*';
              else sd='0';
            }
            break;
    case 3: sm='E'; 
            tw=(tower-7)/2;
            break;
    }
      
    char name[15];
    if (s1>0) snprintf(name,sizeof(name),"%c%c%c%d",sm,s1,sd,tw);
    else      snprintf(name,sizeof(name),"%c%c%d",  sm,sd,tw);
    strCell = name;

    if (end) 
      log << tileID->to_string(id,-2) << " " 
          << strSection << " " << module << " " << strCell << end;
}

void TileNeighbour::print_list(std::vector<IdentifierHash> & nb_list, 
                               const Tile_Base_ID* tileID, 
                               const IdContext* context,
                               MsgStream & log,
                               const char * pref,
                               const char * suff)
{
  Identifier id;
  int module;
  std::string strCell;
  std::string strSection;

  std::sort(nb_list.begin(), nb_list.end());
  unsigned int size=nb_list.size();
  log << MSG::VERBOSE << pref << "(" << size << ")  ";
//  size = strlen(pref) + 4 + ((size)<10 ? 1 : 2);
//  char *space = new char[size+1];
//  memset(space,32,size);
//  space[size]=0;
  for (unsigned int j=0; j<size; ++j) {
    log << MSG::VERBOSE << endmsg;
    log << MSG::VERBOSE << "\t";
    tileID->get_id (nb_list[j], id, context);
    get_name(id,strSection,module,strCell,tileID,log,suff);
  }
  log << endmsg;
//  delete [] space;
}
