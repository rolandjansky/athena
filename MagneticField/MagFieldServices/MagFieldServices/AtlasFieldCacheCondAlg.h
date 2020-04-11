/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// AtlasFieldCacheCondAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_ATLASFIELDCACHECONDALG_H
#define MAGFIELDSERVICES_ATLASFIELDCACHECONDALG_H 

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"

// #include "MagFieldInterfaces/IMTMagFieldSvc.h"

#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldConditions/AtlasFieldMapCondObj.h"

namespace MagField {

    class AtlasFieldCacheCondAlg : public AthReentrantAlgorithm 
    {
    public:
        AtlasFieldCacheCondAlg(const std::string& name,ISvcLocator* pSvcLocator);
        virtual ~AtlasFieldCacheCondAlg();

        StatusCode initialize() override final;
        StatusCode execute(const EventContext& ctx) const override final;
        StatusCode finalize() override final;  

    private:

        /*
         *  Cache of the variables to be updated before we write the conditions object
         */
        struct Cache{
            float  m_solenoidCurrent{0};   // solenoid current in ampere
            float  m_toroidCurrent{0};     // toroid current in ampere
            double m_solScaleFactor{1};    // solenoid current scale factor
            double m_torScaleFactor{1};    // toroid current scale factor
            
            //"infinite in case we do not update from DCS" - full run/event range
            EventIDRange m_condObjOutputRange {EventIDRange(EventIDBase(0,0), EventIDBase(EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFEVT-1))}; 
        }; 
        StatusCode updateCurrentFromConditions(const EventContext& ctx, Cache& cache) const;
        StatusCode updateCurrentFromParameters(Cache& cache) const;
        void       scaleField(Cache& cache, const MagField::AtlasFieldMap* fieldMap) const;

        /// Temporary flag for switching between 'old' and 'new' magField usage
        Gaudi::Property<bool> m_useNewBfieldCache {this, 
                                                   "UseNewBfieldCache", true, "Temporary flag for switching between 'old' and 'new' magField usage. Default = true"};

        // threshold below which currents are considered zero
        Gaudi::Property<double> m_soleMinCurrent {this, 
                                                  "SoleMinCurrent", 1.0, "Minimum solenoid current (A) for which solenoid is considered ON"};
        Gaudi::Property<double> m_toroMinCurrent {this, 
                                                  "ToroMinCurrent", 1.0, "Minimum toroid current (A) for which toroid is considered ON"};

        // flag to use magnet current from DCS in COOL
        Gaudi::Property<bool> m_useDCS {this, 
                                        "UseDCS", false, "Get magnet currents from DCS through ConditionsSvc"};

        // COOL folder name containing current information
        // current input key
        SG::ReadCondHandleKey<CondAttrListCollection> m_currInputKey
        {this, 
         "COOLCurrentsFolderName", "/EXT/DCS/MAGNETS/SENSORDATA", "Name of the COOL folder containing magnet currents"};

        // AtlasFieldMapCondObj - read handle to access magnet field conditions object containing the map file names
        SG::ReadCondHandleKey<AtlasFieldMapCondObj> m_mapCondObjInputKey
        {this, 
         "AtlasFieldMapCondObj", "fieldMapCondObj", "Name of key for the Magnetic Field conditions object with the map file names"};

        // AtlasFieldCacheCondObj - magnet field conditions object containing the current scale factors
        SG::WriteCondHandleKey<AtlasFieldCacheCondObj> m_condObjOutputKey
        {this, 
         "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the key for the Magnetic Field conditions object with currents for scaling"};

        // actual current if DCS is not in use
        Gaudi::Property<double> m_useSoleCurrent {this, 
                                                  "UseSoleCurrent", 7730.,  "Set actual solenoid current (A)"};
        Gaudi::Property<double> m_useToroCurrent {this, 
                                                  "UseToroCurrent", 20400., "Set actual toroid current (A)"};
        ServiceHandle<ICondSvc> m_condSvc { this, 
                                            "CondSvc", "CondSvc", "conditions service" };

    };
}

#endif //> !MAGFIELDSERVICES_ATLASFIELDCACHECONDALG_H
