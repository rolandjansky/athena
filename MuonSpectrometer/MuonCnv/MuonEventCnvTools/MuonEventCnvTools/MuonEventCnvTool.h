/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONEVENTCNVTOOL_H
#define MUONEVENTCNVTOOL_H

#include "TrkEventCnvTools/ITrkEventCnvTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include <string>

class Identifier;
class IdentifierHash;
class StoreGateSvc;

namespace MuonGM {
    class MuonDetectorManager;
}

namespace Trk {
    class RIO_OnTrack;
    class PrepRawData;
    class TrkDetElementBase;
}

namespace Muon {

    /**Tool used in the persisency convertors to help rebuild EDM objects*/
    class MuonEventCnvTool :  virtual public Trk::ITrkEventCnvTool, public AthAlgTool   {
    public:
    
        enum MuonConcreteType { MDT, RPC, CSC, TGC, MM, STGC, TypeUnknown };

        MuonEventCnvTool(const std::string&,const std::string&,const IInterface*);

        virtual ~MuonEventCnvTool ();

        virtual StatusCode initialize();

        /** check that the RoT is correctly filled*/
        virtual void checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const;

        virtual std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
            getLinks( const Trk::RIO_OnTrack& rioOnTrack ) const;
        
        /** @copydoc Trk::ITrkEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack* rot)*/    
        virtual void prepareRIO_OnTrack( Trk::RIO_OnTrack* rot) const;
        
        /** Take the passed RoT and recreate it (i.e. fill missing pointers etc)*/
        virtual void recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const;
        
        /** Return the detectorElement associated with this Identifier*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const;
        
        /** Return the detectorElement associated with this Identifier*/
        virtual const Trk::TrkDetElementBase* getDetectorElement(const Identifier& id) const;
        
        const Trk::PrepRawData* rpcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const;
        const Trk::PrepRawData* cscClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const;
        const Trk::PrepRawData* tgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const;
        const Trk::PrepRawData* mdtDriftCircleLink( const Identifier& id,  const IdentifierHash& idHash  ) const;
        const Trk::PrepRawData* mmClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const;
        const Trk::PrepRawData* stgcClusterLink( const Identifier& id,  const IdentifierHash& idHash  ) const;

    private:

        const MuonGM::MuonDetectorManager* m_muonMgr;                 //!<Muon detector manager

        std::string m_rpcClusContName;
        std::string m_cscClusContName;
        std::string m_tgcClusContName;
        std::string m_mdtClusContName;
        std::string m_mmClusContName;
        std::string m_stgcClusContName;
        
        /** If true, search for PRDs manually - i.e. do not rely on ElementLink working
        @warning This functionality is left in primarily for debugging - will probably be removed in near future.*/
        bool m_manuallyFindPRDs;

    };

}
#endif // MOORETOTRACKTOOL_H

