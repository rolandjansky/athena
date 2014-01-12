/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGS_TGCGEOMETRYSVC_H
#define MUONCABLINGS_TGCGEOMETRYSVC_H


#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "Identifier/Identifier.h"
#include "CxxUtils/clock.h"

#include "GaudiKernel/ServiceHandle.h" 
 
#include "AmdcAth/AmdcsimrecAthenaSvc.h"

namespace MuonGM 
{
  class MuonDetectorManger;
}

class ITGCcablingSvc;
class TgcIdHelper;
class MsgStream;
class StoreGateSvc;

static const InterfaceID IID_ITGCgeometrySvc(1497, 0, 0);

class TGCgeometrySvc : public Service, 
                       virtual public IInterface
{
public:
    struct Sizes
    {
        short minRodId;
        short maxRodId;
        short numRods;
        short numSsws;
        short numSlbs;
        short minChannelId;
        short maxChannelId;
        short numChannels;
        std::string toString() const;
        int total() const;
    };

    struct Entry
    {
        Identifier offlineID;
        float eta;
        float phi;
        float r;
        float z;
        float width;
        int sta;
        bool isStrip;
    };

    static const InterfaceID& interfaceID() { return IID_ITGCgeometrySvc; }
    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);


    TGCgeometrySvc(const std::string& name, ISvcLocator* sl);
    StatusCode initialize();
    StatusCode finalize();
    StatusCode buildMap();
    StatusCode createLUT();
    

    //Interface methods implementation
    const Entry& getEntry(int side, int rod, int ssw, int slb, int channel) const;
    int robNumber(double eta, double phi) const;
    int rdoID(double eta, double phi) const;

    bool isConfigured(void) const;
   
    StatusCode matchTagInfo(IOVSVC_CALLBACK_ARGS);
    StatusCode updateAMDC(IOVSVC_CALLBACK_ARGS);

    
private:
    StatusCode getCabling();

    Entry* m_LUT;
    static Entry m_nullEntry;

    StringProperty  m_fileName;
    BooleanProperty m_createLUT;
    BooleanProperty m_readLUT;
    BooleanProperty m_hasTagInfoMatched;
    
    bool m_configured;
    
    Sizes m_sizes;
    
    ServiceHandle<StoreGateSvc>        m_pDetStore;
    ServiceHandle<AmdcsimrecAthenaSvc> m_AmdcsimrecAthenaSvc; 

    Amdcsimrec* m_amdc;
    const MuonGM::MuonDetectorManager* m_MuonMgr;
    
    const ITGCcablingSvc* m_pTgcCab;
    const TgcIdHelper*    m_pTgcIdHelper;
};

#endif //MUONCABLINGS_TGCGEOMETRYSVC_H

