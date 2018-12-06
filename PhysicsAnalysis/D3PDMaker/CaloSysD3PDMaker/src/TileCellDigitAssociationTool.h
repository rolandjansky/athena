/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileCellDigitAssociationTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on October 31, 2011, 11:23 AM
 */

#ifndef TILECELLDIGITASSOCIATIONTOOL_H
#define	TILECELLDIGITASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "Identifier/HWIdentifier.h"
#include "CaloEvent/CaloCell.h"

#include "SGTileDigitsGetterTool.h"

class TileHWID;
class TileDigits;

namespace D3PD{

class TileCellDigitAssociationTool 
    : public MultiAssociationTool<CaloCell,TileDigits> {
public:
    typedef MultiAssociationTool<CaloCell,TileDigits> Base;
    typedef SGTileDigitsGetterTool Getter;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    TileCellDigitAssociationTool(const std::string& type,
            const std::string& name, const IInterface* parent);
    
    virtual ~TileCellDigitAssociationTool();
    
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
    virtual const  TileDigits* next();
    
private:
    Getter m_digitGetter;
    std::string m_tileDigitKey;
    const TileHWID* m_tilehwid;
    HWIdentifier m_adcId1, m_adcId2;
    int m_nDigitReturned;
    
};

}

#endif	/* TILECELLDIGITASSOCIATIONTOOL_H */

