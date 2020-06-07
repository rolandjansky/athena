/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTCNVTOOLS_ITRKEVENTCNVTOOL
#define TRKEVENTCNVTOOLS_ITRKEVENTCNVTOOL


#include "GaudiKernel/IAlgTool.h"

#include "AthLinks/ElementLink.h"
#include "StoreGate/StoreGate.h"
#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

class Identifier;
class IdentifierHash;

namespace Trk
{

class TrkDetElementBase;
class PrepRawData;
class RIO_OnTrack;


class ITrkEventCnvTool : virtual public IAlgTool
    {
    public:
        /// Key and index types used for an EL to IdentifiableContainer.
        typedef std::string ELKey_t;
        typedef unsigned int ELIndex_t;

        DeclareInterfaceID( ITrkEventCnvTool, 1, 0 );

        
        ITrkEventCnvTool():
            m_storeGate      ( "StoreGateSvc", "ITrkEventCnvTool" ),
            m_msgSvc         ( "MessageSvc",   "ITrkEventCnvTool" )
	    {
//	     m_log(&(*m_msgSvc), name() ); 
	    }
        
        /** check the passed RIO_OnTrack to make sure it is correctly filled*/
        virtual void checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const =0;
        
        /**This method must use the passed RIO_OnTrack to recover pointers
        The PrepRawData pointer will only be filled if the concrete tools are so set
        @return this is a pair, where the (possibly null) pointers contained represent the missing links from rioOnTrack*
        @param[in] rioOnTrack the RIO_OnTrack which needs recreating. MUST point to a valid object.*/
        virtual std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>
                getLinks( Trk::RIO_OnTrack& rioOnTrack  ) const = 0;
        
        /**This templated method will set the EL for the passed RIO_OnTrack.*/
        template <class CONT, class ROT> void prepareRIO_OnTrackElementLink(ROT* rot) const;

        /**This templated method will find EL components for the passed RIO_OnTrack.*/
        template <class CONT, class ROT>
        void prepareRIO_OnTrackElementLink(const ROT* rot,
                                           ELKey_t& key,
                                           ELIndex_t& index) const;

        /**This templated method will return the hashAndIndex of the passed RIO_OnTrack.*/
        template <class CONT, class ROT> bool getHashAndIndex(const ROT* rot,
                                                              const SG::ReadHandleKey<CONT>& contName,
                                                              typename ElementLink<CONT>::index_type& hashAndIndex) const;
        
        /** Take the passed RoT and prepare the PRD ElementLink for writing to disk.
        The procedure should be:
          - Get DataHandles to all IDC containers in the event
          - See if collection storing the linked PRD is part of the container
          - If so, check that the pointer of the PRD matches that in the container
          - Then set strings and index.
          - If any of the above failed, try the next container.*/
        virtual void prepareRIO_OnTrack( Trk::RIO_OnTrack* rot) const =0;

      /** Similar, but just return the EL components rather then
          changing ROT. */
        virtual void prepareRIO_OnTrackLink( const Trk::RIO_OnTrack* rot,
                                             ELKey_t& key,
                                             ELIndex_t& index ) const =0;
        
        /** Take the passed RoT and recreate it (i.e. fill missing pointers etc)*/
        virtual void recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const =0;
        
        /** Returns the detectorElement associated with this Identifier & Hash*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const =0;

        /** Returns the detectorElement associated with this Identifier*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id) const =0;
        
        virtual void setRoT_Values(std::pair<const Trk::TrkDetElementBase *, const Trk::PrepRawData *>& pair, Trk::RIO_OnTrack *RoT ) const;
            
    private:
        ServiceHandle<StoreGateSvc>     m_storeGate;
        ServiceHandle<IMessageSvc>      m_msgSvc;	 
};
}
  
#include "TrkEventCnvTools/ITrkEventCnvTool.icc"
    
#endif

