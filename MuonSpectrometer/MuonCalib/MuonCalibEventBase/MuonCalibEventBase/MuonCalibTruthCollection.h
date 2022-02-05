/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibTruth classes in                 //
//*********************************************************//

#ifndef MuonCalibTruthCollection_h
#define MuonCalibTruthCollection_h

#include <iostream>
#include <memory>
#include <vector>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "MuonCalibEventBase/MuonCalibCscTruthHit.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"
#include "MuonCalibEventBase/MuonCalibRpcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTgcTruthHit.h"
#include "MuonCalibEventBase/MuonCalibTruth.h"

namespace MuonCalib {

    /**
       @class MuonCalibTruthCollection

       MuonCalibTruthCollection is a Calibration EDM container class containing the following truth classes:
       - a set of MuonCalibTruth s (C-EDM version of Truth Track)
       - a set of MuonCalibMdtTruthHit s (C-EDM version of MdtSDO)
       - a set of MuonCalibRpcTruthHit s (C-EDM version of RpcSDO)
       - a set of MuonCalibTgcTruthHit s (C-EDM version of TgcSDO)
       - a set of MuonCalibCscTruthHit s (C-EDM version of CscSDO)

     */

    class MuonCalibTruthCollection {
    public:
        using TruthPtr = std::shared_ptr<const MuonCalibTruth>;
        //!< typedef of a set of MuonCalibTruth* s
        using TruthVec = std::vector<TruthPtr>;
        //!< typedef of the iterator to the set of MuonCalibTruth s

        using TruthMdtPtr = std::shared_ptr<const MuonCalibMdtTruthHit>;
        //!< typedef of set of MuonCalibMdtTruthHit* s
        using MdtTruthVec = std::vector<TruthMdtPtr>;
        //!< typedef of the iterator to the set of MuonCalibMdtTruthHit* s

        using TruthRpcPtr = std::shared_ptr<const MuonCalibRpcTruthHit>;
        //!< typedef of set of MuonCalibRpcTruthHit* s
        using RpcTruthVec = std::vector<TruthRpcPtr>;
        //!< typedef of the iterator to the set of MuonCalibRpcTruthHit* s
        using TruthTgcPtr = std::shared_ptr<const MuonCalibTgcTruthHit>;
        //!< typedef of set of MuonCalibTgcTruthHit* s
        using TgcTruthVec = std::vector<TruthTgcPtr>;

        using TruthCscPtr = std::shared_ptr<const MuonCalibCscTruthHit>;
        //!< typedef of set of MuonCalibCscTruthHit* s
        using CscTruthVec = std::vector<TruthCscPtr>;

        MuonCalibTruthCollection() = default;  //!< Default constructor
        MuonCalibTruthCollection(TruthVec truthVec) :
            m_truthVec(std::move(truthVec)){};  //!< constructor initializing the set of MuonCalibTruth s
        ~MuonCalibTruthCollection() = default;  //!< destructor
        MuonCalibTruthCollection(const MuonCalibTruthCollection& truthCollection);  //!< copyconstructor

        MuonCalibTruthCollection& operator=(const MuonCalibTruthCollection& rhs);  //!< assignment operator

        int numberOfTruth() const;  //!< retrieve the number of MuonCalibTruth s in the collection

        //!< add a MuonCalibTruth to the collection
        void addTruth(MuonCalibTruth* truth);
        //!< add a MuonCalibMdtTruthHit to the collection
        void addTruth(MuonCalibMdtTruthHit* mdtTruth);
        //!< add a MuonCalibRpcTruthHit to the collection
        void addTruth(MuonCalibRpcTruthHit* rpcTruth);
        //!< add a MuonCalibTgcTruthHit to the collection
        void addTruth(MuonCalibTgcTruthHit* tgcTruth);
        //!< add a MuonCalibCscTruthHit to the collection
        void addTruth(MuonCalibCscTruthHit* cscTruth);

        /// -> Smart pointer version
        //!< add a MuonCalibTruth to the collection
        void addTruth(const TruthPtr& truth);
        //!< add a MuonCalibMdtTruthHit to the collection
        void addTruth(const TruthMdtPtr& mdtTruth);
        //!< add a MuonCalibRpcTruthHit to the collection
        void addTruth(const TruthRpcPtr& rpcTruth);
        //!< add a MuonCalibTgcTruthHit to the collection
        void addTruth(const TruthTgcPtr& tgcTruth);
        //!< add a MuonCalibCscTruthHit to the collection
        void addTruth(const TruthCscPtr& cscTruth);

        /// Retrieve a reference to the full truth container
        TruthVec& TruthContainer();
        const TruthVec& TruthContainer() const;

        /// Retrieve a reference to the full Mdt container
        MdtTruthVec& MdtContainer();
        const MdtTruthVec& MdtContainer() const;

        /// Retrieve a refernece to the full Rpc container
        RpcTruthVec& RpcContainer();
        const RpcTruthVec& RpcContainer() const;

        /// Retrieve a reference to the full Csc container
        CscTruthVec& CscContainer();
        const CscTruthVec& CscContainer() const;

        /// Retrieve a referece to the full Tgc container
        TgcTruthVec& TgcContainer();
        const TgcTruthVec& TgcContainer() const;

    private:
        TruthVec m_truthVec;        //!< set of MuonCalibTruth s
        MdtTruthVec m_mdtTruthVec;  //!< set of MuonCalibMdtTruthHit s
        RpcTruthVec m_rpcTruthVec;  //!< set of MuonCalibRpcTruthHit s
        TgcTruthVec m_tgcTruthVec;  //!< set of MuonCalibTgcTruthHit s
        CscTruthVec m_cscTruthVec;  //!< set of MuonCalibCscTruthHit s
        void copy(const MuonCalibTruthCollection& other);
    };

}  // end namespace MuonCalib

#endif
