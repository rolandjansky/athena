// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef RPCGEO_H
#define RPCGEO_H

#define MaxJtype 12
#define MaxJff   8
#define MaxJzz   15

// enum stations {LowPt,Pivot,HiPt};

#include <string>

class MultiChamber;

struct RPC_pack
{
    int lvl1_z_index;
    int Jobj;
    int Readout_f;
    int station;
    int nstrip_z;
    int nstrip_f;
    float stripsize_z;
    float stripsize_f;
    float rotation_angle;
    float half_dim[3];
    float pack_coo[3];
    float pack_ref_coo[3][2][2];
    float pack_ref_deltas[2][2];
    float gas_layer_radius[2];
    float gas_layer_XY[2][2][2];
    float* z_proj;
    float* f_proj[2];
    RPC_pack* next;
    RPC_pack* next_in_z[2];
    RPC_pack* prev_in_z[2];
    MultiChamber* cham;
    RPC_pack* nextall;
};

struct MultiChamber
{
    std::string name;
    int type;   
    int physics_sector;
    int amdb_z_index;
    
    float Rmid;
    int nRPC_packs;
    RPC_pack* firstRPC_z[2][4];
    RPC_pack* RPC_packs;
    MultiChamber* next;
};

struct encodeProj
{
    int Jobj;
    int proj;
    encodeProj* next;
};
#endif
