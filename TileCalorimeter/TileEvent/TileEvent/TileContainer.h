/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************************
// Filename : TileContainer.h
// Author   : Zhifang
// Created  : Sept, 2002
//
// DESCRIPTION:
//   Create a general container in the Athena framework. 
//    
//
// HISTORY:
//    
// BUGS:
//
// *******************************************************************

#ifndef TILEEVENT_TILECONTAINER_H
#define TILEEVENT_TILECONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
// #include "TileEvent/TileDeposit.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileTTL1.h"
#include "TileEvent/TileTTL1Cell.h"
#include "TileEvent/TileMu.h"
#include "TileEvent/TileL2.h"
#include "TileEvent/TileTrigger.h"
#include "TileEvent/TileCosmicMuon.h"
#include "TileEvent/TileMuonReceiverObj.h"
#include <string>

template <typename _TElement>
class TileContainer: public DataVector<_TElement>
{
public:

    typedef DataVector<_TElement> MyVector;
    typedef typename MyVector::const_iterator my_const_iterator ; 

    // constructor
    TileContainer(SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS) : DataVector < _TElement > (ownPolicy) { }

    // desctructor
    virtual ~TileContainer() { }

    // insert a Tile element into a collection.
    inline void add (_TElement* rc) { MyVector::push_back(rc); }

    // print all the channels
    virtual void print() const;
    virtual operator std::string () const;
    std::string whoami (void) const { _TElement elem;
                 return "TileContainer of "+elem.whoami(); }

private:

};

// typedef TileContainer<TileDeposit> TileDepositContainer;
typedef TileContainer<TileCell> TileCellContainer;
typedef TileContainer<TileTTL1> TileTTL1Container;
typedef TileContainer<TileTTL1Cell> TileTTL1CellContainer;
typedef TileContainer<TileL2>   TileL2Container;
typedef TileContainer<TileMu>   TileMuContainer;
typedef TileContainer<TileTrigger>   TileTriggerContainer;
typedef TileContainer<TileCosmicMuon>   TileCosmicMuonContainer;
typedef TileContainer<TileMuonReceiverObj> TileMuonReceiverContainer;
class TileCellVec: public std::vector<unsigned int> 
{};

// CLASS_DEF(TileDepositContainer,   2921, 0)
CLASS_DEF(TileCellContainer,      2931, 0)
CLASS_DEF(TileTTL1Container,      2933, 0)
CLASS_DEF(TileL2Container,        2934, 0)
// 2935 and 2936 are used by BeamElem
CLASS_DEF(TileMuContainer,        2937, 0)
CLASS_DEF(TileTriggerContainer,   2939, 0)
// 2940 used by TileLaser Object
CLASS_DEF(TileCosmicMuonContainer, 2951, 0)
CLASS_DEF(TileTTL1CellContainer,  2952, 0)
// 2953 used by MBTSCollisionTime Object
CLASS_DEF(TileMuonReceiverContainer, 2954, 0)

// explicitly declared inheritance information about template  class 
// TileContainer<> , information may be read in
// http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Control/SGTools/SGTools/BaseInfo.h#0047
namespace SG {
  template <class T>
  struct Bases<TileContainer<T> > {
    typedef DataVector<T> Base1;
    typedef NoBase Base2;
    typedef NoBase Base3;
  };
}

#include "TileEvent/TileContainer.icc"

#endif

