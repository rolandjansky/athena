/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelClusterParts.h
//   Header file for class PixelClusterParts
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_PIXELCLUSTERPARTS_H
#define TRKPREPRAWDATA_PIXELCLUSTERPARTS_H

#include "Identifier/Identifier.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace InDet {
    /** @class PixelClusterParts 
   
   groups Idetifier, tot vector and lvl1 vector together 
   facilitates interface of IClusterSplitter
   
   @author Andreas.Salzburger@cern.ch  
   */
  class PixelClusterParts {

    public :
        /** constructor */
    PixelClusterParts(const std::vector<Identifier>& group,
        const std::vector<int>& totgroup,
        const std::vector<int>& lvl1group) :
        m_idgroup(group),
        m_totgroup(totgroup),
        m_lvl1group(lvl1group),
        m_localPosition(0),
        m_errorMatrix(0)
        {}    

        /** constructor */
    PixelClusterParts(const std::vector<Identifier>& group,
        const std::vector<int>& totgroup,
        const std::vector<int>& lvl1group,
        const Amg::Vector2D& position) :
        m_idgroup(group),
        m_totgroup(totgroup),
        m_lvl1group(lvl1group),
        m_localPosition(new Amg::Vector2D(position)),
        m_errorMatrix(0)
        {}

        /** constructor */
    PixelClusterParts(const std::vector<Identifier>& group,
        const std::vector<int>& totgroup,
        const std::vector<int>& lvl1group,
        const Amg::Vector2D& position,
        const Amg::MatrixX& error) :
        m_idgroup(group),
        m_totgroup(totgroup),
        m_lvl1group(lvl1group),
        m_localPosition(new Amg::Vector2D(position)),
        m_errorMatrix(new Amg::MatrixX(error))
        {}


        /** copy constructor */
    PixelClusterParts(const PixelClusterParts& pcp) : 
        m_idgroup(pcp.m_idgroup),
        m_totgroup(pcp.m_totgroup),
        m_lvl1group(pcp.m_lvl1group),
        m_localPosition( pcp.m_localPosition ? new Amg::Vector2D(*pcp.m_localPosition) : 0),
        m_errorMatrix(pcp.m_errorMatrix ? new Amg::MatrixX(*pcp.m_errorMatrix) : 0)
        {}    

        /** move constructor */
    PixelClusterParts(PixelClusterParts&& pcp) noexcept :
        m_idgroup(std::move(pcp.m_idgroup)),
        m_totgroup(std::move(pcp.m_totgroup)),
        m_lvl1group(std::move(pcp.m_lvl1group)),
        m_localPosition( pcp.m_localPosition ),
        m_errorMatrix(pcp.m_errorMatrix)
        {
            pcp.m_localPosition= nullptr;
            pcp.m_errorMatrix  = nullptr;
        }


        /** assignment operator*/
    PixelClusterParts& operator=(const PixelClusterParts& pcp){
        if (this != &pcp){
            m_idgroup = pcp.m_idgroup;
            m_totgroup = pcp.m_totgroup;
            m_lvl1group = pcp.m_lvl1group;
            m_localPosition =  pcp.m_localPosition ? new Amg::Vector2D(*pcp.m_localPosition) : 0 ;
            m_errorMatrix = pcp.m_errorMatrix ? new Amg::MatrixX(*pcp.m_errorMatrix) : 0 ;
       
        }
        return (*this);
    }

        /** destructor */
    ~PixelClusterParts(){
        delete m_localPosition;
        delete m_errorMatrix;
    }

        /** return method identifiers */
    const std::vector<Identifier>& identifierGroup() const;

        /** return method tot */
    const std::vector<int>&        totGroup() const;

        /** return method lvl1 Group */
    const std::vector<int>&        lvl1Group() const;

        /** return lcoal positions */
    const Amg::Vector2D* localPosition() const;

        /** return error description */
    const Amg::MatrixX* errorMatrix() const;

    private :    

    std::vector<Identifier>             m_idgroup;
    std::vector<int>                    m_totgroup;
    std::vector<int>                    m_lvl1group; 
    Amg::Vector2D*                 m_localPosition;
    Amg::MatrixX*                   m_errorMatrix;

};    

// inline implementation of return methods
inline const std::vector<Identifier>& PixelClusterParts::identifierGroup() const { return m_idgroup; }
inline const std::vector<int>& PixelClusterParts::totGroup() const { return m_totgroup; }
inline const std::vector<int>& PixelClusterParts::lvl1Group() const { return m_lvl1group; }
inline const Amg::Vector2D* PixelClusterParts::localPosition() const { return (m_localPosition); }
inline const Amg::MatrixX* PixelClusterParts::errorMatrix() const { return (m_errorMatrix); }

}

#endif
