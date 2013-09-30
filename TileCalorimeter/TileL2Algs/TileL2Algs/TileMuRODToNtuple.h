/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TileMuRODToNtuple.h
//  Author   : Aranzazu Ruiz
//  Created  : March 2006
//
//  DESCRIPTION:
//     Implementation comments only.  Class level comments go in .h file.
//
//  HISTORY:
//
//  BUGS:
//    
//    
//*****************************************************************************

#ifndef TILEMUID_TILEMURODTONTUPLE_H
#define TILEMUID_TILEMURODTONTUPLE_H

// Gaudi includes
#include "GaudiKernel/NTuple.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"

/**
 * @class TileMuRODToNtuple
 * @brief This class creates a directory, called TileMuRODTag, in the ntuple with the Tile ROD DSP muon tagging information
 * @author Aranzazu Ruiz
 *
 * This class retrieves the Tile ROD DSP muon tagging information from TileL2Container and fills the ntuple.
 * The variables stored are NMuons, EtaMuons and PhiMuons.
 */

class TileMuRODToNtuple: public AthAlgorithm {
  public:

    /** Constructor */
    TileMuRODToNtuple(std::string name, ISvcLocator* pSvcLocator);

    /** Destructor */
    virtual ~TileMuRODToNtuple();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:

    NTuple::Tuple* m_ntuplePtr;
    int m_ntupleID;
    int m_maxNMuons;
    int m_close;
    std::string m_ntupleLoc;

    NTuple::Item<int> m_ntag;
    NTuple::Array<int> m_ID;
    NTuple::Array<int> m_val;
    NTuple::Item<int> m_NMuons;
    NTuple::Array<float> m_eta;
    NTuple::Array<float> m_phi;

    /** Name of TileL2Container in detector store */
    std::string m_tileL2Container;

};

#endif  //TILEMUID_TILEMURODTONTUPLE_H
