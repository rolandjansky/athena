/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   JetTileAssociationTool.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on November 1, 2011, 9:51 PM
 */

#ifndef JETTILEASSOCIATIONTOOL_H
#define	JETTILEASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "Navigation/NavigationToken.h"
#include "JetEvent/Jet.h"

class CaloCell;

namespace D3PD{

class JetTileAssociationTool 
    : public MultiAssociationTool<Jet,CaloCell> {
public:
    typedef MultiAssociationTool<Jet,CaloCell> Base;
    
    /**
     * @brief Standard Gaudi tool constructor.
     * @param type The name of the tool type.
     * @param name The tool name.
     * @param parent The tool's Gaudi parent.
     */
    JetTileAssociationTool(const std::string& type,
            const std::string& name, const IInterface* parent);
    
    virtual ~JetTileAssociationTool();
    
    /**
     * @brief Return the target object.
     * @param p The source object for the association.
     * Return the target of the association, or 0.
     */
    virtual StatusCode reset (const Jet& p);
    
    /**
     * @brief Return a pointer to the next element in the association.
     * Return 0 when the association has been exhausted.
     */
    virtual const  CaloCell* next();
        
private:
    NavigationToken<CaloCell,double> m_cellToken;
    NavigationToken<CaloCell,double>::const_iterator m_cEnd, m_itr;
    
};

}

#endif	/* JETTILEASSOCIATIONTOOL_H */

