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

#ifndef TILEL2ALGS_TILEMURODTONTUPLE_H
#define TILEL2ALGS_TILEMURODTONTUPLE_H

// Tile includes
#include "TileEvent/TileContainer.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

// Gaudi includes
#include "GaudiKernel/NTuple.h"


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

        /** TileL2Container in detector store */
    SG::ReadHandleKey<TileL2Container> m_l2ContainerKey{this,"TileL2Container","TileL2Cnt",
                                                        "Input Tile L2 container key"};

};

#endif  //TILEL2ALGS_TILEMURODTONTUPLE_H
