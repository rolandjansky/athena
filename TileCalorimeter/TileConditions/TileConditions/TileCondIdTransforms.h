/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILECONDIDTRANSFORMS_H
#define TILECONDITIONS_TILECONDIDTRANSFORMS_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/HWIdentifier.h"
#include "Identifier/IdentifierHash.h"

class TileID;
class TileHWID;

class TileCondIdTransforms: public AthAlgTool {

  public:

    static const InterfaceID& interfaceID();
    TileCondIdTransforms(const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~TileCondIdTransforms() {}

    StatusCode initialize();
    StatusCode finalize();

    const TileHWID* getTileHWID() const { return m_tileHWID; }

    void getIndices(const HWIdentifier &hwid
                    , unsigned int& ros
                    , unsigned int& drawer
                    , unsigned int& channel
                    , unsigned int& adc) const;

    void getIndices(const HWIdentifier &hwid
                    , unsigned int& drawerIdx
                    , unsigned int& channel
                    , unsigned int& adc) const;

    void getIndices(const HWIdentifier &hwid
                     , unsigned int& drawerIdx
                     , unsigned int& channel) const;

    void getIndices(IdentifierHash hash_id
                    , unsigned int& drawerIdx
                    , unsigned int& channel) const;

  private:

    const TileHWID* m_tileHWID;
    const TileID* m_tileID;
};

#endif
