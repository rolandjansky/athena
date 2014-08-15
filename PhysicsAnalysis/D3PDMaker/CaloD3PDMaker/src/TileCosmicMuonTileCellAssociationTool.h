/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCosmicMuonTileCellAssociationTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on November 3, 2011, 3:48 PM
 */

#ifndef TILECOSMICMUONTILECELLASSOCIATIONTOOL_H
#define	TILECOSMICMUONTILECELLASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "TileEvent/TileCosmicMuon.h"
#include "CaloEvent/CaloCellContainer.h"

class TileID;

namespace D3PD{

class TileCosmicMuonTileCellAssociationTool
    : public MultiAssociationTool<TileCosmicMuon,CaloCell> {
public:
    typedef MultiAssociationTool<TileCosmicMuon,CaloCell> Base;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileCosmicMuonTileCellAssociationTool(const std::string& type,
            const std::string& name, const IInterface* parent);
    
    virtual ~TileCosmicMuonTileCellAssociationTool();
    
    /**
     * @brief Return the target object.
     * @param p The source object for the association.
     * Return the target of the association, or 0.
     */
    virtual StatusCode reset (const TileCosmicMuon& p);
    
    /**
     * @brief Return a pointer to the next element in the association.
     * Return 0 when the association has been exhausted.
     */
    virtual const  CaloCell* next();
    
private:
    const TileID* m_tileid;
    CaloCellContainer::const_iterator m_cEnd,m_itr;
    std::vector<IdentifierHash> m_tileHash;
    std::vector<IdentifierHash>::iterator m_tileItr;
};

}

#endif	/* TILECOSMICMUONTILECELLASSOCIATIONTOOL_H */

