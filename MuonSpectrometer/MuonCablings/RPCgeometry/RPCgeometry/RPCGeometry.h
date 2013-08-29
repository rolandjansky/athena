// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef PHYSICS_RPC_MUGEOMETRY_H
#define PHYSICS_RPC_MUGEOMETRY_H

#include <fstream>
#include "RPCgeo.h"

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "AmdcCore/Amdcsimrec.h"
#define AMDC  Amdcsimrec*  amdc = s_amdc

#else

#include "AmdcCore/Amdcsimrec.h"
#include "AmdcCore/AmdcsimrecAccess.h"
#define AMDC  Amdcsimrec*  amdc = AmdcsimrecAccess::GetAmdcsimrec()

#endif


#include "MuonCablingTools/OutputDef.h"




enum station {First,Second,Third,Extended};

class RPCgeometrySvc;
class TH1F;

class RPCGeometry {

public:
    ~RPCGeometry(void);

public:  // public static access

//#ifdef LVL1_STANDALONE
    static const RPCGeometry* instance(void);
//#endif

private:
    RPCGeometry(void);

private:  //members for printout facilities 
    
    void myprin(char,int) const;
    void cham_info(MultiChamber *,int);
    void pack_info(RPC_pack *);
    void list_of_packs(MultiChamber *);
    void first_packs_info(RPC_pack*,RPC_pack*,int);
    void check_error(MultiChamber *,std::string);
    void axis(__osstream **,std::string,std::string);
    int  find_front(__osstream **);
    void align_disp(__osstream **,int,int,int);

private:  //members for dumping the chamber structure
    
    void dump_chamber(MultiChamber *);
    void dump_pack_check(__osstream **,RPC_pack*,int);
    void dump_pack_roof(__osstream *,int,int);
    void dump_pack_bound(__osstream *,int,int,int,int);
    void dump_logic_sectors(void);
    void dump_pack_id(__osstream *,int,int,int,int);
    int  give_radial_offset(float,float,int);
    
private:  //members for managing the lists of geometry structures
    
    void rreset(void);
    template <class entry> entry* remove(entry* first, entry* item);
    template <class entry> entry* give_last(entry*);
    void set_first_packs(MultiChamber* cham);
    RPC_pack* find_next_pack(MultiChamber*,RPC_pack*,int);
    void connect (RPC_pack*, RPC_pack*);
    MultiChamber* find_first_chamber(MultiChamber*,int,int);
    RPC_pack* give_last_pack(MultiChamber*,int,int);
    float give_radius(RPC_pack*);
    int same_cham(RPC_pack*,RPC_pack*);

private:  //members for setting up the geometry and the logic maps
    
    void setupmap(void);
    void build_InterChamS_links(MultiChamber*, RPC_pack*);
    void check_map(MultiChamber*);
    void setuplogic(void);
    void build_TransChamS_links(MultiChamber **,int,int,int);
    void build_RPC_coding(int,int,int);
    void check_logic(void);
    void shift_logic(void);

private:  //members for dumping the geometry and the logic maps

    bool RPC_packs_not_equal (RPC_pack*,RPC_pack*) const;
    bool Logic_sectors_are_equal (int, int) const;

private:  //facilities for the data decoding   
    
    const encodeProj* find_proj(int) const;
    void add_proj(encodeProj* );
    //void check_projMap(void);
    void dump_projMap(void);

public:   //members interfacing the geometry to the LVL1 world
    
    void write_geodata(std::ofstream&) const;
#ifndef LVL1_STANDALONE
    bool give_strip_index(unsigned long int code,int& Jtype, 
                          int& StationEta, int& StationPhi, int& DoubletR, 
                          int& DoubletZ,   int& DoubletP,   int& GasGap, 
                          int& MeasuresPhi,int& Strip) const;

    unsigned long int give_strip_code( std::string, int, int, int, 
                                       int, int, int, int, int) const;
#endif
    unsigned long int give_strip_code(int,int,int,int,int,int,int,int) const;
    bool give_strip_coordinates(unsigned long int, float*) const;
    bool give_strip_radius(unsigned long int,float&) const;
    bool give_phi_strip_size(unsigned int code,float& size) const;
    bool give_eta_strip_size(unsigned int code,float& size) const;
    bool existCode(unsigned int code) const;
    
    bool give_station_radius(unsigned int code,float& radius) const;
    bool give_station_phi(unsigned int code,float& phi) const;
    
    bool give_rpc_boundaries(int,int,int,float&,float&) const;
    
    bool give_rpc_XYdim(int,int,int,float&) const;
    
    bool isFull(void) const;
    
private:  //access to the geometry data (depends on software evironment)    
//#ifndef LVL1_STANDALONE
    void buildgeo(void);    
//#else
    void readgeo(void) const;
//#endif

    void rotate(float, float, float, float, float, float, float c[3]) const;

    double  nor(double) const;

    void get_xyz(std::string, int, int, int, int, int, int, int, 
                 double*, double*) const;



private:  // Static access + service variables
    static RPCGeometry* s_instance;
    static bool DumpMap;
    static bool DumpLog;
    static bool DumpCon;
    static bool DebuGeo;
    static std::string FileName;
    
    static double s_ScaleFactor;
    static float s_Zsign;
    
    static bool s_testbeam;
    static bool s_useOldGeoAccess;
    
    static int s_JobMinForEncodeProj;

#ifndef LVL1_STANDALONE
    static Amdcsimrec* s_amdc;
    static TH1F* s_hist_alignCorrOnX;
    static TH1F* s_hist_alignCorrOnY;
    static TH1F* s_hist_alignCorrOnZ;
    
    static TH1F* s_hist_alignCorrOnMeanX;
    static TH1F* s_hist_alignCorrOnMeanY;
    static TH1F* s_hist_alignCorrOnMeanZ;
#endif
    static bool s_status;


private:  //definition of variables for the displaying the geometry data

#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    // nothing to be defined
#else
    // put your gcc 2.95 specific code here
    char display[23][80];
#endif


private:
    int Mff;
    int Mzz;
    int Mtype;

    int Removed;
    
    static double s_pi;
            
    RPC_pack* LogicSector[64][4][2];
    RPC_pack* allpacks;
    
    encodeProj* projMap;

    friend class RPCgeometrySvc;

};


inline double
RPCGeometry::nor(double val) const
{
    return val/s_ScaleFactor;
}

#endif // PHYSICS_MU_MUGEOMETRY_H
