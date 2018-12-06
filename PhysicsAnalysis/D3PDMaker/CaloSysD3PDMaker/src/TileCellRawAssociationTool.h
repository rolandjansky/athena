/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellRawAssociationTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on October 26, 2011, 11:08 AM
 */

#ifndef TILECELLRAWASSOCIATIONTOOL_H
#define	TILECELLRAWASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "Identifier/HWIdentifier.h"
#include "CaloEvent/CaloCell.h"

#include "SGTileRawChannelGetterTool.h"

class TileHWID;
class TileRawChannel;
//class StoreGateSvc;

namespace D3PD{


class TileCellRawAssociationTool 
    : public MultiAssociationTool<CaloCell,TileRawChannel> {
public:
    typedef MultiAssociationTool<CaloCell,TileRawChannel> Base;
    typedef SGTileRawChannelGetterTool Getter;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileCellRawAssociationTool(const std::string& type,
            const std::string& name, const IInterface* parent);
    
    virtual ~TileCellRawAssociationTool();
    
    StatusCode initialize();
    
    /**
     * @brief Return the target object.
     * @param p The source object for the association.
     * Return the target of the association, or 0.
     */
    virtual StatusCode reset (const CaloCell& p);
    
    /**
     * @brief Return a pointer to the next element in the association.
     * Return 0 when the association has been exhausted.
     */
    virtual const  TileRawChannel* next();
  
private:
    Getter m_rawGetter;
    std::string m_tileRawKey;
    const TileHWID* m_tilehwid;
    HWIdentifier m_adcId1, m_adcId2;
    int m_nRawReturned;

};

}

#endif	/* TILECELLRAWASSOCIATIONTOOL_H */

