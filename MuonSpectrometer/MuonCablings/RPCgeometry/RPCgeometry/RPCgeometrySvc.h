/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCgeometrySvc_H
#define RPCgeometrySvc_H


//#ifndef LVL1_STANDALONE

#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/HistoProperty.h"

// added to use cabling
#include "RPCcablingInterface/IRPCcablingServerSvc.h"
#include "RPCcablingInterface/IRPCcablingSvc.h"

#include "AmdcAth/AmdcsimrecAthenaSvc.h"

#include "RPCgeometry/IRPCgeometrySvc.h"


// from RPCcabling
struct PADdim{
    double etaMin;
    double etaMax;
    double phiMin;
    double phiMax;    
    
    PADdim()
    {
        etaMin = +99999.;
	etaMax = -99999.;
	phiMin = +4*M_PI;
	phiMax = -4*M_PI;	
    }

};



class TH1F;


class RPCgeometrySvc : public Service,
                       virtual public IRPCgeometrySvc
{
   public:
   
    RPCgeometrySvc(const std::string& name,ISvcLocator* sl);
    virtual ~RPCgeometrySvc() {}

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);
    
   
    // data access methods 
    virtual unsigned long int
        give_strip_code( std::string,int,int,int,int,int,int,int,int) const;
    virtual unsigned long int  
        give_strip_code(int,int,int,int,int,int,int,int) const;

    virtual bool give_strip_index(unsigned long int code,int& Jtype, 
                          int& StationEta, int& StationPhi, int& DoubletR, 
                          int& DoubletZ,   int& DoubletP,   int& GasGap, 
                          int& MeasuresPhi,int& Strip) const;

    virtual bool give_strip_coordinates(unsigned long int, float*) const;
    virtual bool give_strip_radius(unsigned long int,float&) const;
    virtual bool give_phi_strip_size(unsigned int code,float& size) const;
    virtual bool give_eta_strip_size(unsigned int code,float& size) const;
    virtual bool existCode(unsigned int) const;

    virtual const RPCGeometry* rpcGeometry() const;
    
    
    //copied from cabling -- map configuration

    std::vector<uint32_t> getRobId (float etaMin, float etaMax,
                                    float phiMin,float phiMax) const;
    std::vector<uint32_t> getPadHash (float etaMin, float etaMax,
                                      float phiMin,float phiMax) const;
    
    
    // call back functions
    virtual StatusCode initFromCablingSvc(IOVSVC_CALLBACK_ARGS);
    virtual StatusCode updateAMDC(IOVSVC_CALLBACK_ARGS);
    

   private:
    BooleanProperty m_dump_RPC_geometry;
    BooleanProperty m_dump_RPC_stations;
    BooleanProperty m_debug;
    BooleanProperty m_dump_lvl1conf;
    StringProperty  m_dump_filename;
    
    BooleanProperty m_H8;
    
    BooleanProperty m_unit_in_cm;  // flag to set the primary numbers in cm
    
    BooleanProperty m_useOldGeoAccess;

    BooleanProperty m_doNotUseAlignment;
    bool            m_alignment_updated;
    BooleanProperty m_doNotInitFromDB;
    bool            m_cablingInitFromDB;
    
    
    BooleanProperty m_doMonitorAlignment;
    Histo1DProperty m_histProp_alignCorrOnX;
    Histo1DProperty m_histProp_alignCorrOnY;
    Histo1DProperty m_histProp_alignCorrOnZ;
    
    Histo1DProperty m_histProp_alignCorrOnMeanX;
    Histo1DProperty m_histProp_alignCorrOnMeanY;
    Histo1DProperty m_histProp_alignCorrOnMeanZ;
    
    ServiceHandle<AmdcsimrecAthenaSvc> m_AmdcsimrecAthenaSvc;        

    TH1F* m_hist_alignCorrOnX;
    TH1F* m_hist_alignCorrOnY;
    TH1F* m_hist_alignCorrOnZ;
    
    TH1F* m_hist_alignCorrOnMeanX;
    TH1F* m_hist_alignCorrOnMeanY;
    TH1F* m_hist_alignCorrOnMeanZ;
    
    


    typedef std::vector<PADdim> DimensionMap;
    DimensionMap       m_regions;
    
    // from RPC cabling Svc
    const IRPCcablingSvc *m_rpcCabling;
    std::vector<const RDOindex*> m_hashVec;
    
    unsigned int checkLowCode(unsigned int) const;

    unsigned int checkHighCode(unsigned int) const;

      
    bool etaPhiDim(unsigned short int SubsystemId,
                   unsigned short int SectorId,
                   unsigned short int PADId,
		   PADdim& dim) const;
    
    
    bool giveEtaPhi(unsigned long int EtaLowBorder,
                    unsigned long int EtaHighBorder,
                    unsigned long int PhiLowBorder,
		    unsigned long int PhiHighBorder,
                    PADdim& etaphi) const;

    void computeEtaPhi(float etaLow[3], float etaHigh[3], float etaRad,
                       float phiLow[3], float phiHigh[3],
		       float& etaMin, float& etaMax, 
                       float& phiMin, float& phiMax) const;


    std::vector<uint32_t> getHash(float etaMin, float etaMax,
                                  float phiMin,float phiMax) const;
				  
    StatusCode buildRPCGeometry(void);
    
    StatusCode initTowerMap(void);
};


//#endif

#endif 
