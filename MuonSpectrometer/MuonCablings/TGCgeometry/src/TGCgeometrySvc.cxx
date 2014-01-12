/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCgeometry/TGCgeometrySvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "GaudiKernel/SvcFactory.h"
#include "PathResolver/PathResolver.h"

#include "EventInfo/TagInfo.h"
#include "EventInfoMgt/ITagInfoMgr.h"



#include <unistd.h>

TGCgeometrySvc::Entry TGCgeometrySvc::m_nullEntry;

std::string TGCgeometrySvc::Sizes::toString() const
{
    std::ostringstream oss;
    oss
    << "  minRodId=" << minRodId
    << ", maxRodId=" << maxRodId
    << ", numRods=" << numRods
    << ", numSsws=" << numSsws
    << ", numSlbs=" << numSlbs
    << ", minChannelId=" << minChannelId
    << ", maxChannelId=" << maxChannelId
    << ", numChannels=" << numChannels;
    return oss.str();
}

bool TGCgeometrySvc::isConfigured() const
{
    return m_configured;
}

int TGCgeometrySvc::Sizes::total() const
{
    return (2 * numRods * numSsws * numSlbs * numChannels);
}

struct FileEntry
{
    unsigned char subDet, rodID, sswID, slbID, channel;
    TGCgeometrySvc::Entry e;
};



TGCgeometrySvc::TGCgeometrySvc(const std::string& name, ISvcLocator* sl) :
        Service(name, sl),
        m_LUT(NULL),
        m_pDetStore("DetectorStore",name),
	m_AmdcsimrecAthenaSvc ("AmdcsimrecAthenaSvc",name), 
	m_MuonMgr(NULL),
        m_pTgcCab(NULL),
        m_pTgcIdHelper(NULL)
{
    declareProperty("DetStore",m_pDetStore,"Detector Store");
    declareProperty("AMDC", m_AmdcsimrecAthenaSvc, "AMDC geometry service");
    declareProperty("FileName", m_fileName = "TGCgeometry.lut","Name of the LUT file");
    declareProperty("CreateLUT", m_createLUT = false,"Build the LUT file once created in memory");
    declareProperty("ReadLUT", m_readLUT = false,"Force the reading of the geometry LUT from file");
    declareProperty("DisableTagInfoMatch", m_hasTagInfoMatched = false,"Disable the callback for TagInfo");
}


// queryInterface
StatusCode TGCgeometrySvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
    if (IID_ITGCgeometrySvc.versionMatch(riid) )
    {
        *ppvIF = dynamic_cast<TGCgeometrySvc*>(this);
    } 
    else
    {
        return Service::queryInterface(riid, ppvIF);
    }

    addRef();
    return StatusCode::SUCCESS;
}


inline std::string pt2str(double pt[3])
{
    char buf[100];
    sprintf(buf, "(%f,%f,%f)", pt[0], pt[1], pt[2]);
    return buf;
}

#include <time.h>
inline timespec operator+(const timespec& ts1, const timespec& ts2)
{
    timespec out;
    out.tv_sec = ts1.tv_sec + ts2.tv_sec;
    out.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
    if (out.tv_nsec > 1000000000)
    {
        out.tv_sec += 1;
        out.tv_nsec -= 1000000000;
    }
    return out;
}

inline timespec operator-(const timespec& ts1, const timespec& ts2)
{
    timespec out;
    out.tv_sec = ts1.tv_sec - ts2.tv_sec;
    out.tv_nsec = ts1.tv_nsec - ts2.tv_nsec;
    if (out.tv_nsec < 0)
    {
        out.tv_sec -=1;
        out.tv_nsec += 1000000000;
    }
    return out;
}

inline std::string timespec2str(const timespec& ts)
{
    char buf[100];
    sprintf(buf, "%.4f", (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.);
    return buf;
}


StatusCode TGCgeometrySvc::createLUT()
{
    MsgStream log(msgSvc(), name());
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "inside createLUT procedure" << endreq;

    if(m_amdc==0) {
        log << MSG::INFO << "AMDC not initialized yet!" << endreq;
	return StatusCode::RECOVERABLE;
    }

    if(!m_pTgcCab) // If the TGC cabling service is not yet configured, the configuration is tried here.    
    {
        // ==================== GET THE TGC CABLING SERVICE  =====================
        StatusCode sc = getCabling();
	if(!sc.isSuccess()) 
	{
            log << (sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "getCabling failed in createLUT" << endreq; 
	    return sc;
	}
    }
    
    
    int maxRodId, maxSswId, maxSlbId, minChannelId, maxChannelId;
    m_pTgcCab->getReadoutIDRanges(maxRodId, maxSswId, maxSlbId, minChannelId, maxChannelId);
    m_sizes.minRodId = (maxRodId == 12 ? 1 : 0);
    m_sizes.maxRodId = (short)maxRodId;
    m_sizes.numRods = m_sizes.maxRodId - m_sizes.minRodId + 1;
    m_sizes.numSsws = (short)maxSswId + 1;
    m_sizes.numSlbs = (short)maxSlbId + 1;
    m_sizes.minChannelId = (short)minChannelId;
    m_sizes.maxChannelId = (short)maxChannelId;
    m_sizes.numChannels = m_sizes.maxChannelId - m_sizes.minChannelId + 1;
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "Sizes: sides=2, " << m_sizes.toString() << endreq;

    int lutSize = m_sizes.total();
    m_LUT = new Entry[lutSize];
    memset(m_LUT, 0, lutSize * sizeof(Entry));

    int subDet, rodID, sswID, slbID, channel, stationEta, stationPhi, gasGap, chn;
    std::string stationName;
    Amg::Vector3D pos;
    double width = 0.0;
    int isStrip;
    Identifier offlineID;
    timespec ts_start, ts_end, ts1 = {0, 0}, ts2 = {0, 0}, ts3 = {0, 0}, ts4 = {0, 0};

    log << MSG::INFO << "Building LUT..." << endreq;

    for (subDet = 0x67; subDet <= 0x68; subDet++)
    {
        for (rodID = m_sizes.minRodId; rodID <= m_sizes.maxRodId; rodID++)
        {
            for (sswID = 0; sswID < m_sizes.numSsws; sswID++)
            {
                for (slbID = 0; slbID < m_sizes.numSlbs; slbID++)
                {
                    int phi, moduleType, id;
                    bool isAside, isEndcap;
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
                    bool ok = m_pTgcCab->getSLBIDfromReadoutID(phi, isAside, isEndcap, moduleType, id,
                                                             subDet, rodID, sswID, slbID);
                    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end); ts4 = ts4 + ts_end - ts_start;
                    if (!ok)
                        continue;
                    for (channel = m_sizes.minChannelId; channel <= m_sizes.maxChannelId; channel++)
                    {
                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
                        bool ok = m_pTgcCab->getOfflineIDfromReadoutID(offlineID, subDet, rodID, sswID, slbID, channel);
                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end); ts1 = ts1 + ts_end - ts_start;
                        if (!ok)
                            continue;

			const MuonGM::TgcReadoutElement* pROElement = m_MuonMgr->getTgcReadoutElement(offlineID);
			if(!pROElement) continue;
			if(!pROElement->containsId(offlineID)) continue;

                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
                        stationName = m_pTgcIdHelper->stationNameString(m_pTgcIdHelper->stationName(offlineID));
                        stationEta  = m_pTgcIdHelper->stationEta(offlineID);
                        stationPhi  = m_pTgcIdHelper->stationPhi(offlineID);
                        gasGap      = m_pTgcIdHelper->gasGap(offlineID);
                        isStrip     = m_pTgcIdHelper->isStrip(offlineID);
                        chn         = m_pTgcIdHelper->channel(offlineID);
                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end); ts2 = ts2 + ts_end - ts_start;
                        if (log.level() <= MSG::VERBOSE)
                        {
                            log << MSG::VERBOSE
                            << "rodID=" << rodID
                            << ", sswID=" << sswID
                            << ", slbID=" << slbID
                            << ", channel=" << channel << endreq;
                            log << MSG::VERBOSE
                            << "offlineID=" << std::ios::hex << offlineID << std::ios::dec
                            << ", stationName=" << stationName
                            << ", stationEta=" << stationEta
                            << ", stationPhi=" << stationPhi
                            << ", gasGap=" << gasGap
                            << ", isStrip=" << isStrip
                            << ", chn=" << chn << endreq;
                        }

                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_start);
			pos = pROElement->channelPos(offlineID); 
			if(isStrip) {
			  double locZ = (pROElement->transform(offlineID).inverse() * pROElement->stripPos(offlineID)).z();
			  width = pROElement->stripMaxX(gasGap, chn, locZ)-pROElement->stripMinX(gasGap, chn, locZ);
			} 
			else {
			  width = pROElement->gangMaxZ(gasGap, chn) - pROElement->gangMinZ(gasGap, chn);
			}
                        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end); ts3 = ts3 + ts_end - ts_start;

                        int index = ((((subDet - 0x67)*m_sizes.numRods +
                                       (rodID - m_sizes.minRodId))*m_sizes.numSsws +
                                      sswID)*m_sizes.numSlbs +
                                     slbID)*m_sizes.numChannels +
                                    (channel - m_sizes.minChannelId);
                        Entry& e = m_LUT[index];
                        e.offlineID = offlineID;
                        e.eta     = pos.eta();
                        e.phi     = pos.phi();
                        e.r       = pos.perp();
                        e.z       = pos.z();
                        e.width   = width;
                        e.sta     = stationName[1] - '1';
                        e.isStrip = isStrip;
                    }
                }
            }
        }
    }
    log << MSG::INFO << "Timing:"
    << " getOfflineIDfromReadoutID " << timespec2str(ts1) << " secs"
    << ", TgcIdHelper " << timespec2str(ts2) << " secs"
    << ", pos and width " << timespec2str(ts3) << " secs"
    << ", getSLBIDfromReadoutID " << timespec2str(ts4) << " secs"
    << endreq;

    // output the file only if requested
    if( m_createLUT )
    {
        FileEntry oe;
        std::string dirName = PathResolver::find_directory("", "DATAPATH");
        // if write permission is granted to the user then write the file
        if( !access(dirName.c_str(), W_OK) )
        {
            std::string fileName = dirName + "/" + (const std::string&)m_fileName;
            FILE* f = fopen(fileName.c_str(), "wb");
            log << MSG::INFO << "Writing LUT to " << fileName << endreq;
            fwrite(&m_sizes, 1, sizeof(m_sizes), f);
            for (subDet = 0x67; subDet <= 0x68; subDet++)
            {
                for (rodID = m_sizes.minRodId; rodID <= m_sizes.maxRodId; rodID++)
                {
                    for (sswID = 0; sswID < m_sizes.numSsws; sswID++)
                    {
                        for (slbID = 0; slbID < m_sizes.numSlbs; slbID++)
                        {
                            for (channel = m_sizes.minChannelId; channel <= m_sizes.maxChannelId; channel++)
                            {
                                int index = ((((subDet - 0x67)*m_sizes.numRods +
                                               (rodID - m_sizes.minRodId))*m_sizes.numSsws +
                                              sswID)*m_sizes.numSlbs +
                                             slbID)*m_sizes.numChannels +
                                            (channel - m_sizes.minChannelId);
                                Entry& e = m_LUT[index];
                                if (e.r > 0.0)
                                {
                                    oe.subDet  = subDet;
                                    oe.rodID   = rodID;
                                    oe.sswID   = sswID;
                                    oe.slbID   = slbID;
                                    oe.channel = channel;
                                    memcpy(&oe.e, &e, sizeof(Entry));
                                    fwrite(&oe, sizeof(FileEntry), 1, f);
                                }
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
        else
        {
            log << MSG::ERROR << "Cannot access "
            << dirName << ", cannot output LUT file!" << endreq;
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode TGCgeometrySvc::initialize()
{
    StatusCode sc;

    sc = Service::initialize();
    if (sc.isFailure())
        return sc;

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initializing " << name() << " - package version "
    << PACKAGE_VERSION << endreq;


    m_configured = false;


    // get DetectorStore
    //m_pDetStore = NULL;
    //sc = service("DetectorStore", m_pDetStore);
    sc = m_pDetStore.retrieve();
    if ( sc.isFailure() )
    {
        log << MSG::FATAL << "DetectorStore service not found!" << endreq;
        return sc;
    }


    // **** **** **** TagInfo **** **** ****
    const DataHandle<TagInfo> tagInfoH;
    std::string tagInfoKey = "";

    ITagInfoMgr*  pTagInfoMgr = NULL;
    sc = service("TagInfoMgr", pTagInfoMgr);
    if(sc.isFailure() || pTagInfoMgr==0)
    {
        log << MSG::WARNING << " Unable to locate TagInfoMgr service" << endreq;
    }
    else
    {
        tagInfoKey = pTagInfoMgr->tagInfoKey();
    }

    if (!m_hasTagInfoMatched)
    {
        //register the call-back to matchTagInfo
        sc = m_pDetStore->regFcn(&TGCgeometrySvc::matchTagInfo,
                                 this,tagInfoH,tagInfoKey);

        if( sc.isFailure() )
        {
            log << MSG::WARNING
            << "Cannot register matchTagInfo function for key "
            << tagInfoKey << endreq;
        }
        else
        {
            log << MSG::DEBUG
            << "Registered matchTagInfo callback for key: "
            << tagInfoKey << endreq;
        }
    }


    if(m_pDetStore->retrieve(m_MuonMgr).isFailure() || m_MuonMgr == NULL) {
      log << MSG::FATAL << "Could not find MuonDetDescrMgr!" << endreq;
      return StatusCode::FAILURE;
    }

    //retrieve the AMDC geometry
    if (m_AmdcsimrecAthenaSvc.retrieve().isFailure()) {
        log << MSG::FATAL << "Failed to retrieve service " 
	                  << m_AmdcsimrecAthenaSvc << endreq;
        return StatusCode::FAILURE;
    } 

    if (m_AmdcsimrecAthenaSvc->InitializedSvc()) {
        m_amdc = m_AmdcsimrecAthenaSvc->GetAmdcsimrec();
	log << MSG::INFO << "AMDC already initialized; "
	                 << "no need of callback" << endreq;
    } else  {
        m_amdc = 0;
	sc = m_pDetStore->regFcn(
                  &AmdcsimrecAthenaSvc::UpdatedSvc,(&*m_AmdcsimrecAthenaSvc),
                  &TGCgeometrySvc::updateAMDC,this,true);
        if (sc.isFailure()) {
            log << MSG::FATAL << "Unable to register callback on buildLUT "
	                      << "from AmdcsimrecAthenaSvc::UpdatedSvc " 
			      << endreq;
            return StatusCode::FAILURE;
        }
        log << MSG::INFO << "Registered callback on buildLUT "
	                 << "from AmdcsimrecAthenaSvc::UpdatedSvc " << endreq;
    }


         
    // ======================= GET THE TGC ID HELPER  ========================
    sc = m_pDetStore->retrieve(m_pTgcIdHelper, "TGCIDHELPER");
    if (sc.isFailure() || m_pTgcIdHelper == NULL)
    {
        MsgStream log(messageService(), name());
        log << MSG::FATAL << "Could not get TgcIdHelper!" << endreq;
        return sc;
    }
    

    // Try to build the map now    
    sc = buildMap();
    if( sc.isFailure() )
    {
        if( sc.isRecoverable() ){
	    log << MSG::INFO << "Postpone LUT creation " << endreq;
	    return StatusCode::SUCCESS;
	}
        return sc;
    }
    else return StatusCode::SUCCESS;
    
    return StatusCode::SUCCESS;
}

StatusCode TGCgeometrySvc::updateAMDC (IOVSVC_CALLBACK_ARGS)
{
    m_amdc = m_AmdcsimrecAthenaSvc->GetAmdcsimrec();
    
    StatusCode sc = buildMap();
    if ( sc.isFailure() )
    {
        if( sc.isRecoverable() ){
	    MsgStream log(messageService(), name());
	    log << MSG::INFO << "Postpone LUT creation " << endreq;
	    return StatusCode::SUCCESS;
	}
        return sc;
    }
    else return StatusCode::SUCCESS;
}

StatusCode TGCgeometrySvc::matchTagInfo (IOVSVC_CALLBACK_ARGS)
{

    MsgStream log(msgSvc(), name());
    //StatusCode sc;
    bool tagMatch = true;

    log << MSG::DEBUG << "matchTagInfo() callback triggered" << endreq;

    // configuration tag
    std::string mapname = m_fileName;
    unsigned int pos = (mapname.rfind(".8.lut")!= std::string::npos)?
                       mapname.rfind(".8.lut") : mapname.rfind(".12.lut");
    
    std::string confTag;
    
    try
    {
        confTag = mapname.substr(pos, mapname.length());
    }
    catch (...) {
        log << MSG::FATAL << "TGC LUT file " << m_fileName 
	                  << " doesn't have expected end (.8.lut/.12.lut)"
			  << endreq;
         return StatusCode::FAILURE;
    }
    
    log << MSG::DEBUG << "Configuration tag: " << confTag << endreq;

    // Get TagInfo and retrieve tags
    const TagInfo* tagInfo = 0;
    StatusCode sc = m_pDetStore->retrieve(tagInfo);

    std::string cablingType;

    if (sc.isFailure() || tagInfo==0)
    {
        log << MSG::DEBUG
        << "No TagInfo in DetectorStore while attempting to compare tags"
        << endreq;
    }
    else
    {
        tagInfo->findInputTag("TGC_CablingType", cablingType);

        log << MSG::DEBUG
        << "TGC_CablingType from TagInfo: " << cablingType << endreq;

        if(cablingType=="")
        {
            // assume it is 8-fold in case the call-back is active
            cablingType="TGCcabling8Svc";
        }
        if (confTag==".12.lut")
        {
            tagMatch = (cablingType != "TGCcabling8Svc");
        }
        else
        {
            tagMatch = (cablingType != "TGCcabling12Svc");
        }
    }

    if (!tagMatch)
    {
        if (confTag==".12.lut")
        {
            std::string newname = mapname.substr(0,pos) + ".8.lut";
            log << MSG::INFO
            << "TagInfo configured for 8-fold, but TGCgeometry configured as 12-fold"
            << endreq;
            log << MSG::INFO << "LUT name " << m_fileName
            << " changed into " << newname << endreq;
            m_fileName = newname;
        }
        else
        {
            std::string newname = mapname.substr(0,pos) + ".12.lut";
            log << MSG::INFO
            << "TagInfo configured for 12-fold, but TGCgeometry configured as 8-fold"
            << endreq;
            log << MSG::INFO << "LUT name " << m_fileName
            << " changed into " << newname << endreq;
            m_fileName = newname;
        }
    }
    
    m_hasTagInfoMatched = true;

    sc = buildMap();
    if ( sc.isFailure() )
    {
        if( sc.isRecoverable() ){
	    log << MSG::INFO << "Postpone LUT creation " << endreq;
	    return StatusCode::SUCCESS;
	}
        return sc;
    }
    else return StatusCode::SUCCESS;

}


StatusCode TGCgeometrySvc::buildMap()
{
    MsgStream log(msgSvc(), name());

    if(!m_hasTagInfoMatched) {
        log << MSG::INFO << "Cannot build " << name() 
                         << " now, becasue a TagInfo match is required" 
	                 << endreq;
	return StatusCode::RECOVERABLE;
    }
    
    if (log.level() <= MSG::INFO)
    {
        if(m_readLUT)
        {
	    log << MSG::INFO << "Build TGC cable map LUT reading it from file "
            << m_fileName << endreq;
        }
        else
        {
            log << MSG::INFO << "Build TGC cable map LUT <"
            << m_fileName << "> creating it on the fly "
            << endreq;
        }
    }
    
    m_configured  = false;
    
    if(m_readLUT)
    {
        std::string fileName = PathResolver::find_file(m_fileName, "DATAPATH");
        if (fileName.empty())
        {
            log << MSG::INFO << "TGC cable map LUT <"
            << fileName << "> not found, compute LUT on the fly!"
            << endreq;

            //recovering creating the LUT on the fly
            StatusCode sc = createLUT();
            if (sc.isFailure()) // and exiting on failure
            {
	        if(!sc.isRecoverable()) {
                    log << MSG::ERROR << "createLUT procedure failed!" << endreq;
                }
		return sc;
            } else
            {
                if (log.level() <= MSG::DEBUG)
                    log << MSG::DEBUG << "initialize done" << endreq;
                m_configured = true;
                return StatusCode::SUCCESS;
            }
        }
        FILE* f = fopen(fileName.c_str(), "rb");
        if (f == NULL)
        {
            log << MSG::ERROR << "Cannot open file " << fileName << endreq;
            return StatusCode::FAILURE;
        }
        else
        {
            log << MSG::INFO << "Reading LUT from " << fileName << endreq;
            fread(&m_sizes, sizeof(m_sizes), 1, f);
            if (log.level() <= MSG::DEBUG)
                log << MSG::DEBUG << "Sizes: sides=2, " << m_sizes.toString() << endreq;

            int lutSize = m_sizes.total();
            m_LUT = new Entry[lutSize];
            FileEntry oe;
            while (fread(&oe, sizeof(FileEntry), 1, f) == 1)
            {
                int index = ((((oe.subDet - 0x67)*m_sizes.numRods + (oe.rodID - m_sizes.minRodId))*m_sizes.numSsws + oe.sswID)*m_sizes.numSlbs + oe.slbID)*m_sizes.numChannels + (oe.channel - m_sizes.minChannelId);
                memcpy(&m_LUT[index], &oe.e, sizeof(Entry));
            }
            fclose(f);
        }
    }
    else
    {
	
	if(m_amdc == 0) {
            log << MSG::INFO << "Cannot build " << name() 
                             << " now, becasue AMDC is not updated yet" 
	                     << endreq;
	    return StatusCode::RECOVERABLE;
        }

        StatusCode sc = createLUT();
        if (sc.isFailure())
        {
            log << MSG::ERROR << "createLUT procedure failed!" << endreq;
            return sc;
        }
	else if (sc.isRecoverable())
	{
	    log << MSG::INFO << "could not create the LUT now because "
	        << "geometry or cabling is not fully configured" << endreq;
	    return StatusCode::SUCCESS;
	}
    }

    
    
    if (log.level() <= MSG::INFO)
        log << MSG::INFO << "Building TGC cable map LUT done" << endreq;

    m_configured = true;

    return StatusCode::SUCCESS;
}

StatusCode TGCgeometrySvc::finalize()
{
    delete [] m_LUT;
    return StatusCode::SUCCESS;
}

const TGCgeometrySvc::Entry& TGCgeometrySvc::getEntry(int subDet, int rodID, int sswID, int slbID, int channel) const
{
    MsgStream log(msgSvc(), name());
    if ((subDet != 0x67 && subDet != 0x68) ||
            rodID < m_sizes.minRodId || rodID > m_sizes.maxRodId ||
            sswID < 0 || sswID >= m_sizes.numSsws ||
            slbID < 0 || slbID >= m_sizes.numSlbs ||
            channel < m_sizes.minChannelId || channel > m_sizes.maxChannelId)
    {
        log << MSG::ERROR << "Bad TGCgeometry access:"
        << " subDet=" << subDet
        << " rodID=" << rodID
        << " sswID=" << sswID
        << " slbID=" << slbID
        << " channel=" << channel
        << endreq;
        return m_nullEntry;
    }
    int index = ((((subDet - 0x67)*m_sizes.numRods + (rodID - m_sizes.minRodId))*m_sizes.numSsws + sswID)*m_sizes.numSlbs + slbID)*m_sizes.numChannels + (channel - m_sizes.minChannelId);
    if (log.level() <= MSG::DEBUG)
        log << MSG::DEBUG << "getEntry "
        << " subDet=" << subDet
        << " rodID=" << rodID
        << " sswID=" << sswID
        << " slbID=" << slbID
        << " channel=" << channel
        << " eta=" << m_LUT[index].eta
        << " phi=" << m_LUT[index].phi
        << endreq;

    return m_LUT[index];
}

const double FIFTEEN_DEG = 15.0 * M_PI / 180.0;

int TGCgeometrySvc::robNumber(double eta, double phi) const
{
    double Phi = phi + FIFTEEN_DEG;
    if(Phi > (2.0 * M_PI))
        Phi -= 2.0*M_PI;

    int nSectors = m_sizes.numRods;
    if (m_sizes.numRods == 13)
        nSectors = 12;
    int iRob = (int)(nSectors * (Phi) / (2.0 * M_PI));
    if (nSectors == 12)
        iRob+=1;
    int rob_number = ((eta >= 0. ? 0x67 : 0x68) << 16) | iRob;
    return (rob_number);
}

int TGCgeometrySvc::rdoID(double eta, double phi) const
{
    double Phi = phi + FIFTEEN_DEG;
    if (Phi > (2.0 * M_PI) )
        Phi -= 2.0*M_PI;

    int nSectors = m_sizes.numRods;
    if (m_sizes.numRods == 13)
        nSectors = 12;
    int rdo_id = (int)(nSectors * (Phi) / (2.0 * M_PI));
    if (eta < 0.)
        rdo_id += nSectors;
    return (rdo_id);
}


StatusCode TGCgeometrySvc::getCabling() 
{
    std::string mapname = m_fileName;
    unsigned int POS = (mapname.rfind(".8.lut")!= std::string::npos)?
    mapname.rfind(".8.lut") : mapname.rfind(".12.lut");
    
    std::string confTag;
    try
    {
        confTag = mapname.substr(POS, mapname.length());
    }
    catch (...) {
        MsgStream log(messageService(), name());
        log << MSG::FATAL << "TGC LUT file " << m_fileName 
	                  << " doesn't have expected end (.8.lut/.12.lut)"
			  << endreq;
         return StatusCode::FAILURE;
    }

    if(confTag==".12.lut")
    {
        StatusCode sc = service("TGCcabling12Svc", m_pTgcCab, true);
        if (sc != StatusCode::SUCCESS)
        {
            MsgStream log(messageService(), name());
            log << MSG::FATAL << "Cannot retrieve the instance of TGCcabling12"
            << endreq;
            return sc;
        }
    }
    else
    {
        StatusCode sc = service("TGCcablingSvc", m_pTgcCab, true);
        if (sc != StatusCode::SUCCESS)
        {
            MsgStream log(messageService(), name());
            log << MSG::FATAL << "Cannot retrieve the instance of TGCcabling"
            << endreq;
            return sc;
        }
    }

    return StatusCode::SUCCESS; 
}
