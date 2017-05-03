/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDPROXYFILE_H
#define TILECONDITIONS_TILECONDPROXYFILE_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"

// Tile includes
#include "TileConditions/ITileCondProxy.h"

#include "CoralBase/Blob.h"
#include <string>
#include <vector>

template<typename T>
class TileCondProxyFile: public AthAlgTool, virtual public ITileCondProxy<T> {
  public:

    TileCondProxyFile(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondProxyFile() {};

    StatusCode initialize();
    StatusCode finalize();

    const T* getCalibDrawer(unsigned int drawerIdx) const;

  private:

    /** @brief Returns cache index used for online calibration constants.
        @details Returns cache index used for the calibration constant applied in the DSP
        @param drawerIdx Drawer index                                                                                                                                         @param channel Tile channel                                                                                                                                           @param adc Gain 
    */
    inline unsigned int cacheIndex(unsigned int drawerIdx, unsigned int channel, unsigned int adc) const {
      return m_drawerCacheSize * drawerIdx + m_maxChannels * adc + channel;
    };

    typedef std::map<unsigned int, std::vector<float> > DataMap;

    /** Creates a calibDrawer of type T */
    const T* createCalibDrawer(unsigned int drawerIdx, unsigned int nChannels
                               , unsigned int objVers, const DataMap& dataMap);

    /** Source = COOL folder */
    std::string m_source;

    /** CalibDrawer vector to keep updated */
    std::vector<const T*> m_drawerVec;

    /** Keep track of coral::Blobs for later deletion */
    std::vector<coral::Blob*> m_blobStore;

    unsigned int m_maxChannels;
    unsigned int m_maxGains;
    unsigned int m_drawerCacheSize;

};

#endif
