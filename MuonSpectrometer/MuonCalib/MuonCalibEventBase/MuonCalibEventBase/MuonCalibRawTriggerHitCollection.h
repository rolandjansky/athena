/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//*********************************************************//
// Class to store MuonCalibRawTriggerHit classes           //
//*********************************************************//

#ifndef MuonCalibRawTriggerHitCollection_h
#define MuonCalibRawTriggerHitCollection_h

#include <iostream>
#include <memory>
#include <vector>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

namespace MuonCalib {

    class MuonCalibRawRpcTriggerHit;

    /**
       @class MuonCalibRawTriggerHitCollection
    */

    class MuonCalibRawTriggerHitCollection {
    public:
        using CalibRawRpcTriggerHitPtr = std::shared_ptr<const MuonCalibRawRpcTriggerHit>;
        //!< typedef of a set of MuonCalibRawRpcTriggerHit s
        using MuonCalibRawRpcTriggerHitVec = std::vector<CalibRawRpcTriggerHitPtr>;
        //!< typedef of an iterator of the set muonCalibRawRpcTriggerHit s

        MuonCalibRawTriggerHitCollection() = default;  //<! default constructor
        MuonCalibRawTriggerHitCollection(MuonCalibRawRpcTriggerHitVec rawRpcTriggerHitVec);
        //!< constructor initializing all raw hit collections
        ~MuonCalibRawTriggerHitCollection() = default;                                                               //!< destructor
        MuonCalibRawTriggerHitCollection(const MuonCalibRawTriggerHitCollection& MuonCalibRawTriggerHitCollection);  //!< copyconstructor

        MuonCalibRawTriggerHitCollection& operator=(const MuonCalibRawTriggerHitCollection& rhs);  //!< assingment operator

        //!< retrieve (const)_iterator to the first MuonCalibRawRpcHit of the collection
        const MuonCalibRawRpcTriggerHitVec& get() const;
        MuonCalibRawRpcTriggerHitVec& get();

        int numberOfMuonCalibRawRpcTriggerHits() const;  //!< retrieve the number of MuonCalibRawRpcHits in the collection
        void addMuonCalibRawTriggerHit(const CalibRawRpcTriggerHitPtr& rawTriggerHit);  //!< add a MuonCalibRawRpcHit to the collection

    private:
        MuonCalibRawRpcTriggerHitVec m_rawRpcTriggerHitVec;  //!< set of MuonCalibRawRpcHit s

        void copy(const MuonCalibRawTriggerHitCollection& other);
    };

}  // namespace MuonCalib

#endif
