/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTcablingSvc_H
#define MDTcablingSvc_H


#ifndef LVL1_STANDALONE

#include <string>

#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "GaudiKernel/HistoProperty.h"
#include "StoreGate/StoreGateSvc.h"


#include "AmdcAth/AmdcsimrecAthenaSvc.h"


#include "MDTcabling/IMDTcablingSvc.h"


class MuonMDT_CablingMap;
class IMDTCablingDbTool;

class TH1F;


class MDTcablingSvc : public Service,
                      virtual public IMDTcablingSvc
{   
    private:    
    const MdtIdHelper*    m_pMdtIdHelper;
    
    public:
    MDTcablingSvc(const std::string& name,ISvcLocator* sl);
    virtual ~MDTcablingSvc() {}

    //static const InterfaceID& interfaceID() { return IID_IMDTcablingSvc; }

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    private:
    BooleanProperty m_map_flag;    // flag for printing out the geometry map
    BooleanProperty m_geo_flag;    // flag for printing out the MDT geometry
    BooleanProperty m_che_flag;    // flag for printing out the geometry checks
    BooleanProperty m_rem_flag;    // flag for printing out the TDC remapping
    BooleanProperty m_geo_out;     // flag for writing into a file the geometry
    BooleanProperty m_rgs_out;     // flag for writing the region selector map
    BooleanProperty m_read_flag;   // flag for reading the geometry from a file
    BooleanProperty m_shift_LinkId;
    BooleanProperty m_useOldGeoAccess;
    
    BooleanProperty m_unit_in_cm;  // flag to set the primary numbers in cm
    
    StringProperty m_rgs_filename; // file containing the region selector data
    StringProperty m_geo_filename; // file containing the geometry data
    StringProperty m_rod_filename; // file containing the ROD map data
    StringProperty m_rem_filename; // file containing the remapping of cabling
    StringProperty m_mez_filename; // file containing the cabling of mezanine

    BooleanProperty m_hasTagInfoMatched;
    BooleanProperty m_do_not_loadFromDB;
    bool            m_cabling_updated;
    BooleanProperty m_doNotUseAlignment;
    bool            m_alignment_updated;
    
    Histo1DProperty m_histProp_barrel_alignCorrOnRadius;
    Histo1DProperty m_histProp_barrel_alignCorrOnZeta;
    
    Histo1DProperty m_histProp_endcap_alignCorrOnRadius;
    Histo1DProperty m_histProp_endcap_alignCorrOnZeta;
    
    ServiceHandle<StoreGateSvc>        m_pDetStore;
    ServiceHandle<AmdcsimrecAthenaSvc> m_AmdcsimrecAthenaSvc;


    TH1F* m_hist_barrel_alignCorrOnRadius;
    TH1F* m_hist_barrel_alignCorrOnZeta;
    TH1F* m_hist_endcap_alignCorrOnRadius;
    TH1F* m_hist_endcap_alignCorrOnZeta;


    StatusCode buildMdtGeometry(void);
    public:

    virtual bool getOnlineIDfromOfflineID(int name, int StationEta,
                                  int StationPhi,int MultiLayer, int TubeLayer,
				  int Tube, 
                                  unsigned short int& SubsystemId,
				  unsigned short int& MrodId,
				  unsigned short int& LinkId,
				  unsigned short int& TdcId,
				  unsigned short int& ChannelId) const;

    virtual bool getOfflineIDfromOnlineID(unsigned short int SubsystemId,
				  unsigned short int MrodId,
				  unsigned short int LinkId,
				  unsigned short int TdcId,
				  unsigned short int ChannelId,
                                  int& name, int& StationEta,
                                  int& StationPhi,int& MultiLayer, 
                                  int& TubeLayer,int& Tube) const;
    
    virtual const MdtIdHelper* idHelper(void) const;
    

    virtual const MDTGeometry* mdtGeometry(void) const;
     
    virtual const MDTGeometry::ROBid  getROBid(float phiMin,float PhiMax,
                                               float etaMin,float etaMax) const;
    virtual const MDTGeometry::HASHid getHASHid(float phiMin,float PhiMax,
                                               float etaMin,float etaMax) const;
					            
     virtual bool isTestBeamSetup(void) const;
     
     // call back functions
     StatusCode matchTagInfo(IOVSVC_CALLBACK_ARGS);
     StatusCode updateAMDC(IOVSVC_CALLBACK_ARGS);
     StatusCode loadCablingMap(IOVSVC_CALLBACK_ARGS_P(I,keys));
     
     
     private:
     /** pointer to the class holding the cabling map */
     //  MuonMDT_CablingMap* m_cablingMap;
     DataHandle<MuonMDT_CablingMap> m_cablingMap;

     /** pointer to the db access tool */
     ToolHandle<IMDTCablingDbTool> m_dbTool;
};

#endif



#endif 
