//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECALIBDDATA_H
#define TILECONDITIONS_TILECALIBDDATA_H



#include <vector>

/**
 * @class TileCalibData
 * @brief Templated condition object to keep TileCalibDrawers
 *
 *
 * Can keep TileCalibDrawers of the following types:
 * - TileCalibDrawerFlt
 * - TileCalibDrawerBch
 * - TileCalibDrawerOfc
 */

template<class T>
class TileCalibData {

  public:

    TileCalibData();
    virtual ~TileCalibData();

    /**
     * @brief Return TileCalibDrawer of type T
     * @returns TileCalibDrawer of type T
     * @param drawerIdx Tile drawer index in range [0..275]
     */
    const T* getCalibDrawer(unsigned int drawerIdx) const;

   /**
    * @brief Store TileCalibDrawer of type T
    *        The TileCalibData takes ownership of TileCalibDrawer
    * @param drawerIdx Tile drawer index in range [0..275]
    * @param calibDrawer TileCalibDrawer of type T to store
    */
    void setCalibDrawer(unsigned int drawerIdx, const T* calibDrawer);

    TileCalibData (const TileCalibData&) = delete;
    TileCalibData& operator= (const TileCalibData&) = delete;

  private:

    std::vector<const T*> m_calibDrawers;
};


#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"
typedef TileCalibData<TileCalibDrawerFlt> TileCalibDataFlt;
CLASS_DEF(TileCalibData<TileCalibDrawerFlt>, 111225290, 0)
CONDCONT_DEF(TileCalibData<TileCalibDrawerFlt>, 149264696);

#include "TileCalibBlobObjs/TileCalibDrawerOfc.h"
typedef TileCalibData<TileCalibDrawerOfc> TileCalibDataOfc;
CLASS_DEF(TileCalibData<TileCalibDrawerOfc>, 141822674, 0)
CONDCONT_DEF(TileCalibData<TileCalibDrawerOfc>, 262948840);

#include "TileCalibBlobObjs/TileCalibDrawerBch.h"
typedef TileCalibData<TileCalibDrawerBch> TileCalibDataBch;
CLASS_DEF(TileCalibData<TileCalibDrawerBch>, 88425733, 0)
CONDCONT_DEF(TileCalibData<TileCalibDrawerBch>, 218165589);

#endif // TILECONDITIONS_TILECALIBDDATA_H
