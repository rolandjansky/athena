/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdio.h>
#include <stdlib.h>             // for exit()
//#include <string>             // for strcmp()
#include <math.h>
#include <iomanip>
#include <boost/io/ios_state.hpp>

#include "RPCgeometry/RPCGeometry.h"
#include "MuonCablingTools/RPCdecoder.h"


static MultiChamber *T1muonGeo[MaxJtype][MaxJff*(2*MaxJzz+1)];


RPCGeometry* RPCGeometry::s_instance = 0;
bool RPCGeometry::DumpMap = false;
bool RPCGeometry::DumpLog = false;
bool RPCGeometry::DumpCon = false;
bool RPCGeometry::DebuGeo = false;

double RPCGeometry::s_ScaleFactor = 1.;
float  RPCGeometry::s_Zsign = +1.;
double RPCGeometry::s_pi = 3.14159265358979323846;

bool RPCGeometry::s_testbeam = false;
bool RPCGeometry::s_useOldGeoAccess = true;
int  RPCGeometry::s_JobMinForEncodeProj = 9999;

std::string RPCGeometry::FileName = "LVL1conf.dump";

#ifndef LVL1_STANDALONE
#include <TH1F.h>
Amdcsimrec* RPCGeometry::s_amdc = 0;
TH1F* RPCGeometry::s_hist_alignCorrOnX = 0;
TH1F* RPCGeometry::s_hist_alignCorrOnY = 0;
TH1F* RPCGeometry::s_hist_alignCorrOnZ = 0;
   
TH1F* RPCGeometry::s_hist_alignCorrOnMeanX = 0;
TH1F* RPCGeometry::s_hist_alignCorrOnMeanY = 0;
TH1F* RPCGeometry::s_hist_alignCorrOnMeanZ = 0;
#endif

bool RPCGeometry::s_status      = true;


RPCGeometry::RPCGeometry(void)
{
	allpacks = 0;
    for (int sect=0;sect<64;++sect) 
    {
        for(int stat=First;stat<=Extended;++stat)
	{
	    for(int i=0;i<2;++i)
	    {
	        LogicSector[sect][stat][i] = 0;
	    }
	}
    }
    
    if(s_testbeam) s_Zsign=-1.;
        
//#ifndef LVL1_STANDALONE

    projMap = 0;
    
    buildgeo();
    //check_projMap();

//#else

    //readgeo();    
    
//#endif

    setupmap();
    setuplogic();
    
    if(DumpCon)
    {
        std::ofstream dump;
	dump.open(RPCGeometry::FileName.c_str());
	write_geodata(dump);
    }
}

RPCGeometry::~RPCGeometry(void)
{
	
    // Delete the Chamber Map
    for (int Jtype=1;Jtype<=MaxJtype;++Jtype)
    {
        for (int Jff=1;Jff<=Mff;++Jff)
	{
	    for (int Jzz=-Mzz;Jzz<=Mzz;++Jzz)
	    {
	        MultiChamber *cham = 
		                 T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
				  		
	        if (cham)
		{
		    delete cham;
		}
            }
        }
    }
    while (allpacks) {
      RPC_pack* pack = allpacks;
      allpacks = allpacks->nextall;
      delete [] pack->f_proj[0];
      delete [] pack->f_proj[1];
      delete [] pack->z_proj;
      delete pack;
    }
    //Delete Logic Map
     encodeProj* item = projMap;
    while (item) {
      encodeProj* next = item->next;
      delete item;
      item = next;
    }
}

//#ifdef LVL1_STANDALONE
const RPCGeometry*
RPCGeometry::instance(void)
{
    if (! s_instance) {
        s_instance = new RPCGeometry();
    }

    return s_instance;
}
//#endif

//=============================================================================
//**************************** Printout facilities ****************************
//=============================================================================

void 
RPCGeometry::myprin(char c,int i) const 
{
  int j;
  
  for(j=0;j<i;j++) printf("%c",c);
}

void
RPCGeometry::cham_info(MultiChamber *cham,int stat)
{
    printf("    Chamber %s,  Phyisic sector = %d,  Z index = %d",
		      cham->name.c_str(),cham->physics_sector+1,
		      cham->amdb_z_index);
    if (stat > 3) printf("\n\n");
    else printf(",  Station = %d\n\n",stat + 1);
}

void
RPCGeometry::pack_info(RPC_pack *pack)
{
    printf("\n      Jobj = %d,  Station = %d, Z index = %d\n",
                    pack->Jobj, pack->station, pack->lvl1_z_index);
    printf("      Pack center: X=%8.2f, Y=%8.2f, Z=%8.2f\n",
                    pack->pack_coo[0],pack->pack_coo[1],pack->pack_coo[2]);
    printf("      #z strips = %d (pitch=%5.3f cm), #phi strips = %d",
                    pack->nstrip_z, pack->stripsize_z, pack->nstrip_f);
    printf(" (pitch=%5.2f cm)\n",pack->stripsize_f);
    printf("      Z coordinate of first strip: Z = %8.2f\n",pack->z_proj[2]);
}

void
RPCGeometry::list_of_packs(MultiChamber *cham)
{
    cham_info(cham,3);
    printf("    has %d RPC packs:\n\n",cham->nRPC_packs);
    RPC_pack *pack = cham->RPC_packs;
    while(pack)
    {
        pack_info(pack);
	pack = pack->next;
    }
}

void
RPCGeometry::first_packs_info(RPC_pack *pack1,RPC_pack *pack2,int stat)
{
    if (pack1)
    {
        printf("\n    The chamber\n");
        pack_info(pack1);
     printf("\n    has been chosen as first RPC(proj 0) of station %d\n",stat);
    }
    if (pack2)
    {
        printf("\n    The chamber\n");
        pack_info(pack2);
     printf("\n    has been chosen as first RPC(proj 1) of station %d\n",stat);
    }
}

void
RPCGeometry::check_error(MultiChamber *cham,std::string message)
{
    printf("RPCGeometry, check_links: ");
    printf("%s\n",message.c_str());
    cham_info(cham,10);
    exit(1);
}

void
RPCGeometry::axis(__osstream **disp,std::string X, std::string Y)
{    
    *disp[6] << Y;
    *disp[7] << "^";
    for(int i=8;i<22;++i) *disp[i] << "|";
    *disp[22] << "+-------------> " << X;
}

int
RPCGeometry::find_front(__osstream **disp)
{
    unsigned int head = 0;
    for (int i=0;i<22;++i)
    {
        if((disp[i])->COUNT > head) head = (disp[i])->COUNT;
    }
    return head;
}

void
RPCGeometry::align_disp(__osstream **disp,int start,int stop,int front)
{
    for(int i=start;i<=stop;++i)
    {
        if(i<23)
	{
            int head_line = (disp[i])->COUNT;
	    for(int j=0;j<front - head_line;++j) 
	    {
	        if(i==10) *disp[i] << "-";
	        else *disp[i] << " ";
	    }
	}
    }
}


//=============================================================================
//********************** Single chamber dump routines *************************
//=============================================================================

void
RPCGeometry::dump_chamber(MultiChamber *cham)
{
    __osstream *disp[23];
    
    for(int stat=First;stat<=Extended;++stat)      // for all the trigger stations
    {
        RPC_pack* pack0 = cham->firstRPC_z[0][stat];
        RPC_pack* pack1 = cham->firstRPC_z[1][stat];
        
	if (pack0 || pack1)
	{
#if (__GNUC__) && (__GNUC__ > 2)   // put your gcc 3.2 specific code here
            for(int i=0;i<23;++i) disp[i] = new __osstream();
#else                              // put your gcc 2.95 specific code here
            for(int i=0;i<23;++i) disp[i] = new __osstream(display[i],80);
#endif

//          ============== Start the dumping of the X-Y view <-----------------
            axis(disp,"Z","Phi");
            
	    float Phi = (pack0)? pack0->rotation_angle : pack1->rotation_angle;
            *disp[10] << " Phi=";
	    
	    align_disp(disp,0,22,(disp[10])->COUNT);
	    
	    *disp[10] << std::setw(5) << std::setprecision(3) 
	              << std::setiosflags(std::ios::fixed)
	              << Phi << "-";
	
//          =============== Loop for dumping the RPC packs in the X-Y view <---
	    while (pack0||pack1)
	    {
	        dump_pack_check(disp,pack1,1);
		dump_pack_check(disp,pack0,0);
		 
		if(pack0) pack0 = pack0->next_in_z[0];
		if(pack1) pack1 = pack1->next_in_z[1];
		
		if(pack0 || pack1) 
		{
		    align_disp(disp,0,21,find_front(disp));
		    for(int i=0;i<10;++i) *disp[i] << "  ";
		    *disp[10] << "--------";
		    for(int i=11;i<22;++i) *disp[i] << "  ";
		}
	    }
	    
	    align_disp(disp,0,22,find_front(disp));
	
//          =============== Start the dumping of the R-Z view <----------------
	    for(int i=0;i<23;++i) *disp[i] << "  ";
	    axis(disp,"Z","R");
	    align_disp(disp,0,21,(disp[10])->COUNT);
	    for(int i=0;i<21;++i) *disp[i] << "  ";
	    
	    pack0 = cham->firstRPC_z[0][stat];
            pack1 = cham->firstRPC_z[1][stat];

            int head = (disp[10])->COUNT;
	    int offs = 0;
	    
//          =============== Loop for dumping the RPC packs in the R-Z view <---
	    while (pack0||pack1)
	    {
	        int proj = (pack0)? 0 : 1;
                RPC_pack* pack = (pack0)? pack0 : pack1;
		
		RPC_pack * prev = pack->prev_in_z[proj];
		
		float radius =sqrt(powf(pack->pack_coo[0],2) +
		                   powf(pack->pack_coo[1],2));
			     
		float prev_r =(!prev)?  0 : sqrt(powf(prev->pack_coo[0],2) +
			                         powf(prev->pack_coo[1],2));
			       				
		if (prev)
		{
		    if (pack->pack_coo[2] - pack->half_dim[1] < 
		        prev->pack_coo[2] + prev->half_dim[1]) 
                    {
		        align_disp(disp,offs-2,offs+2,head-1);
		    } else
		    {
		        align_disp(disp,offs-2,offs+2,head+1);
		    }
		}
		
		if(!offs) offs = (radius > cham->Rmid)?  4 : 16;
		
		if (radius > prev_r) --offs;
		else if (radius < prev_r) ++offs;
		
		if (radius == cham->Rmid) offs = 10;
		
		*disp[offs] << "=========";
		align_disp(disp,offs-1,offs+1,head);
		*disp[offs-1] << " " << std::setw(6) << std::setprecision(1) 
		              << std::setiosflags(std::ios::fixed) 
			      << pack->gas_layer_radius[1];
                *disp[offs+1] << " " << std::setw(6) << std::setprecision(1) 
		              << std::setiosflags(std::ios::fixed) 
			      << pack->gas_layer_radius[0];
                head = (disp[offs])->COUNT;
		
		if(pack0) pack0 = pack0->next_in_z[0];
		if(pack1) pack1 = pack1->next_in_z[1];
	    }
	    
	    *disp[10] << " ---- Rmid=" << std::setw(6) << std::setprecision(1)
		      << std::setiosflags(std::ios::fixed) << cham->Rmid << " ";
            align_disp(disp,10,10,head);


//          ============= Close and output the stream <------------------------
            myprin('=',79);
	    printf("\n");
	    cham_info(cham,stat);
	    
	    for(int i=0;i<23;++i)
            {
#if (__GNUC__) && (__GNUC__ > 2)   // put your gcc 3.2 specific code here
                std::cout << (disp[i])->str() << std::endl;
#else                              // put your gcc 2.95 specific code here
                *disp[i] << std::ends;
		printf("%s\n",display[i]);
#endif
	        delete disp[i];
            }
	    
	}
    }
}

void
RPCGeometry::dump_pack_check(__osstream **disp, RPC_pack *pack,int proj)
{
    if (!pack) return;
    
    int width = (pack->half_dim[1] > 50.)? 10 : 4;
    float y_center = pack->pack_coo[0]*sin(-pack->rotation_angle) +
	                 pack->pack_coo[1]*cos(-pack->rotation_angle);
    if (proj == 1)
    {
        if (y_center - pack->half_dim[0] + 5. < 0. && !(fabsf(y_center) < 24.)
	                                           && !s_testbeam)
	{
	    dump_pack_roof  (disp[11],6,width);
	    dump_pack_bound (disp[10],0,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[9],6,width,0,0);
	    dump_pack_bound (disp[8],6,width,0,0);
	    dump_pack_bound (disp[7],6,width,0,0);
	}
	
	if (fabsf(y_center) < 24. || s_testbeam)
	{
	    dump_pack_roof  (disp[10],0,width);
	    dump_pack_bound (disp[9],6,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[8],6,width,0,0);
	    dump_pack_bound (disp[7],6,width,0,0);
	}
	
	if(y_center - pack->half_dim[0] - 5. > 0.)
	{
	    dump_pack_roof  (disp[8],6,width);
	    dump_pack_bound (disp[7],6,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[6],6,width,0,0);
	    
	    *disp[5] << std::setw(5) << std::setprecision(0);
	    *disp[5] << std::setiosflags(std::ios::fixed) << pack->half_dim[0];
	    dump_pack_bound (disp[5],1,width,pack->nstrip_f,1);
	} else
	{
	    *disp[6] << std::setw(5) << std::setprecision(0);
	    *disp[6] << std::setiosflags(std::ios::fixed) << pack->half_dim[0];
	    dump_pack_bound (disp[6],1,width,pack->nstrip_f,1);
	    dump_pack_bound (disp[5],6,width,0,0);
	}
	
	dump_pack_bound (disp[4],6,width,0,0);
	dump_pack_roof  (disp[3],6,width);
			
	*disp[2] << "      |";
        for(int i=0;i<width/4;++i) *disp[2]<<" ";
        *disp[2] << std::setw(4) << std::setprecision(0);
	*disp[2] << std::setiosflags(std::ios::fixed) << pack->half_dim[1]*2;
	
	*disp[1] << "      |";
      
        *disp[0] << "   " << std::setw(7) << std::setprecision(0)
                 << std::setiosflags(std::ios::fixed)
	         << pack->pack_coo[2]-pack->half_dim[1];   
    }
    
    
    if (proj == 0)
    { 
        if (y_center + pack->half_dim[0] - 5. > 0. && !(fabsf(y_center) < 24.)
	                                           && !s_testbeam )
	{
	    dump_pack_roof  (disp[9],6,width);
	    dump_pack_bound (disp[10],0,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[11],6,width,0,0);
	    dump_pack_bound (disp[12],6,width,0,0);
	    dump_pack_bound (disp[13],6,width,0,0);
	}
	
	if (fabsf(y_center) < 24. || s_testbeam)
	{
	    dump_pack_bound (disp[11],6,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[12],6,width,0,0);
	    dump_pack_bound (disp[13],6,width,0,0);
	}
	
	if(y_center + pack->half_dim[0] + 5. < 0.)
	{
	    dump_pack_roof  (disp[12],6,width);
	    dump_pack_bound (disp[13],6,width,pack->nstrip_z,0);
	    dump_pack_bound (disp[14],6,width,0,0);
	    
	    *disp[15] << std::setw(5) << std::setprecision(0);
	    *disp[15] << std::setiosflags(std::ios::fixed) << pack->half_dim[0];
	    dump_pack_bound (disp[15],1,width,pack->nstrip_f,1);
	} else
	{
	    *disp[14] << std::setw(5) << std::setprecision(0);
	    *disp[14] << std::setiosflags(std::ios::fixed) << pack->half_dim[0];
	    dump_pack_bound (disp[14],1,width,pack->nstrip_f,1);
	    dump_pack_bound (disp[15],6,width,0,0);
	}
	
	dump_pack_bound (disp[16],6,width,0,0);
	dump_pack_roof  (disp[17],6,width);
	
        *disp[18] << "      |";
        for(int i=0;i<width/4;++i) *disp[18]<<" ";
        *disp[18] << std::setw(4) << std::setprecision(0);
	*disp[18] << std::setiosflags(std::ios::fixed) << pack->half_dim[1]*2;
	
	*disp[19] << "      |";
	
        *disp[20] << "   " << std::setw(7) << std::setprecision(0)
                  << std::setiosflags(std::ios::fixed)
	          << pack->pack_coo[2]-pack->half_dim[1];
    }
}

void
RPCGeometry::dump_pack_roof (__osstream *disp,int offset,int width)
{
    for(int i=0;i<offset;++i) *disp << " ";
    *disp << "+"; 
    for(int i=0;i<width;++i) *disp << "-"; 
    *disp << "+";
}

void
RPCGeometry::dump_pack_bound (__osstream *disp,
                              int offset, 
			      int width, 
			      int strips,
			      int xy)
{
    for(int i=0;i<offset;++i) *disp << " ";
    *disp << "|";
    if (strips)
    {
        if(xy)
	{
            *disp << std::setw(2) << strips;
	    for(int i=0;i<width-2;++i) *disp << " "; 
	    *disp << "|";
        } else
	{
	    for(int i=0;i<width/2-1;++i) *disp << " ";
	    *disp << std::setw(2) << strips;
	    for(int i=0;i<width/2-1;++i) *disp << " ";
	    *disp << "|";
	}
    } else
    {
        for(int i=0;i<width;++i) *disp << " "; 
	*disp << "|";
    }
}


void
RPCGeometry::dump_logic_sectors(void)
{
    __osstream *disp[23];
    for(int stat=First;stat<=Extended;++stat)
    {
        for(int logic_sector = 62;logic_sector >= 0; logic_sector -= 2)
        {
	    int upper_part = logic_sector;
	    int lower_part = (logic_sector%32)? logic_sector-1:logic_sector+31;
	
	    RPC_pack *next1 = LogicSector[upper_part][stat][0];
	    RPC_pack *next0 = LogicSector[lower_part][stat][0];
	    RPC_pack *pack1 = 0;
	    RPC_pack *pack0 = 0;
#if (__GNUC__) && (__GNUC__ > 2)   // put your gcc 3.2 specific code here
            for(int i=0;i<23;++i) disp[i] = new __osstream();
#else                              // put your gcc 2.95 specific code here
            for(int i=0;i<23;++i) disp[i] = new __osstream(display[i],80);
#endif	   
	    
	    int offs  = (lower_part < 32)? 21 : 20;
	    int head = -1;
	    	    
   	    do
	    {

	        pack1 = next1;
		pack0 = next0;
	    
	        int width = 4;
	
//          =============== Loop for dumping the RPC packs in the X-Y view <---
	   
	        if (pack1)
                {
		    float y_center = pack1->pack_coo[0]*
		                sin(-pack1->rotation_angle) +
	                        pack1->pack_coo[1]*cos(-pack1->rotation_angle);
		                        
		    if (y_center - pack1->half_dim[0] + 5. < 0. && 
	                             !(fabsf(y_center) < 24.) && !s_testbeam)
	            {
	                dump_pack_roof  (disp[11],0,width);
	                dump_pack_bound (disp[10],0,width,pack1->nstrip_z,0);
	                dump_pack_bound (disp[9],0,width,0,0);
	                dump_pack_bound (disp[8],0,width,0,0);
	                dump_pack_bound (disp[7],0,width,0,0);
	            }
	
	            if (fabsf(y_center) < 24. || s_testbeam)
	            {
	                dump_pack_roof  (disp[10],0,width);
	                dump_pack_bound (disp[9],0,width,pack1->nstrip_z,0);
	                dump_pack_bound (disp[8],0,width,0,0);
	                dump_pack_bound (disp[7],0,width,0,0);
	            }
	
	            if(y_center - pack1->half_dim[0] - 5. > 0.)
	            {
	                dump_pack_roof  (disp[8],0,width);
	                dump_pack_bound (disp[7],0,width,pack1->nstrip_z,0);
	                dump_pack_bound (disp[6],0,width,0,0);
	                dump_pack_bound (disp[5],0,width,pack1->nstrip_f,1);
	            } else
	            {
	                dump_pack_bound (disp[6],0,width,pack1->nstrip_f,1);
	                dump_pack_bound (disp[5],0,width,0,0);
	            }
	
//	            dump_pack_bound (disp[4],0,width,0,0);
		    dump_pack_id(disp[4],0,width,pack1->station,
		                                 pack1->lvl1_z_index);
	            dump_pack_roof  (disp[3],0,width);
		    
		    *disp[2] << " " << pack1->cham->name << pack1->cham->type;
                }
	   
	        if (pack0)
                {                    
		    float y_center = pack0->pack_coo[0]*
		                sin(-pack0->rotation_angle) +
	                        pack0->pack_coo[1]*cos(-pack0->rotation_angle);
		                        
                    if (y_center + pack0->half_dim[0] - 5. > 0. && 
		                     !(fabsf(y_center) < 24.) && !s_testbeam)
	            {
	                dump_pack_roof  (disp[9],0,width);
	                dump_pack_bound (disp[10],0,width,pack0->nstrip_z,0);
	                dump_pack_bound (disp[11],0,width,0,0);
	                dump_pack_bound (disp[12],0,width,0,0);
	                dump_pack_bound (disp[13],0,width,0,0);
	            }
	
                    if (fabsf(y_center) < 24. || s_testbeam)
	            {
	                dump_pack_bound (disp[11],0,width,pack0->nstrip_z,0);
	                dump_pack_bound (disp[12],0,width,0,0);
	                dump_pack_bound (disp[13],0,width,0,0);
	            }
	
	            if(y_center + pack0->half_dim[0] + 5. < 0.)
	            {
	                dump_pack_roof  (disp[12],0,width);
	                dump_pack_bound (disp[13],0,width,pack0->nstrip_z,0);
	                dump_pack_bound (disp[14],0,width,0,0);
	                dump_pack_bound (disp[15],0,width,pack0->nstrip_f,1);
	            } else
	            {
	                dump_pack_bound (disp[14],0,width,pack0->nstrip_f,1);
	                dump_pack_bound (disp[15],0,width,0,0);
	            }
		    
//		    dump_pack_bound (disp[16],0,width,0,0);
		    dump_pack_id(disp[16],0,width,pack0->station,
		                                  pack0->lvl1_z_index);
                    dump_pack_roof  (disp[17],0,width);
		    
		    *disp[18] << " " << pack0->cham->name << pack0->cham->type;
		    
		    align_disp(disp,0,18,find_front(disp));  
		
//                  ==================== Dump the RPC radial view<-------------

                    if(!s_testbeam)
		    {
                    RPC_pack * prev = pack0->prev_in_z[0];
                      
		    float radius = give_radius(pack0);
		    float prev_r = (!prev)?  0 : give_radius(prev);
		         				
		    if (prev && LogicSector[lower_part][stat][0] != pack0)
		    {   
			int next_offs = give_radial_offset(radius,prev_r,offs);
			
			if(pack0->cham != prev->cham)
			{
			    *disp[offs] << "]";
			    align_disp(disp,19,22,(disp[17])->COUNT - 6);
			    if(same_cham(pack0->next_in_z[0],pack0))
                                *disp[next_offs] << " [====";
			    else			       
			        *disp[next_offs] << "[====";
			    
			} else
			{
			    align_disp(disp,19,22,head);
			    if (pack0->pack_coo[2] - pack0->half_dim[1] < 
		                prev->pack_coo[2] + prev->half_dim[1]) 
                            {
			        *disp[next_offs] << "====";
			    
		            } else
		            {
			        *disp[next_offs] << "  ====";
		            }
			}
			
			offs = next_offs;
		    
		    } else
		    {
		        if(same_cham(pack0->next_in_z[0],pack0))
                            *disp[offs] << " [====";
			else			       
			    *disp[offs] << "[====";
		    }
		    
		    head = (disp[offs])->COUNT;
		    
		    }
                
		}
	   	   
		if (LogicSector[lower_part][stat][1] == pack0 && pack0)
		{
		    *disp[offs] << "]";
		}
		
		   
		if (pack1 && pack1->next_in_z[1]) next1 = pack1->next_in_z[1];
		if (pack0 && pack0->next_in_z[0]) next0 = pack0->next_in_z[0];
		     
            } while (pack1 && pack1 != LogicSector[upper_part][stat][1] &&
	             pack0 && pack0 != LogicSector[lower_part][stat][1]);

//          ============= Close and outup the stream <-------------------------
	    	    
	      myprin('=',21);
	      std::string sign = (lower_part < 32)? "negative" : "positive";
	      printf(" %s half barrel, Station = %1d ",sign.c_str(),stat+1);
	      myprin('=',22);
	      printf("\n");
	      
	      
	      myprin(' ',30);
	      printf("-------------------> Z         Logic Sector = %2d\n",
	              upper_part%32);


	      for(int i=0;i<23;++i)
              {
#if (__GNUC__) && (__GNUC__ > 2)   // put your gcc 3.2 specific code here
                  std::cout << (disp[i])->str() << std::endl;
#else                              // put your gcc 2.95 specific code here
                  *disp[i] << std::ends;
		  printf("%s\n",display[i]);
#endif
	          delete disp[i];
              }	      

	      
	      myprin(' ',61);
	      printf("Logic Sector = %2d\n",lower_part%32);

	}
    }   
}


void
RPCGeometry::dump_pack_id (__osstream *disp,
                              int offset, 
			      int width, 
			      int station,
			      int z_index)
{
    boost::io::ios_all_saver ias(*disp);
    for(int i=0;i<offset;++i) *disp << " ";
    *disp << "|";
    for(int i=0;i<(width-4)/2;++i) *disp << " ";
    *disp << std::setw(1) << station + 1 << "*" << std::setw(2) 
          << std::setfill('0')<<z_index;
    for(int i=0;i<(width-4)/2;++i) *disp << " ";
    *disp << "|";
    ias.restore();
}


int
RPCGeometry::give_radial_offset(float current_rad, float previous_rad, int offs)
{
    float delta = fabs(current_rad - previous_rad);
    if (delta <= 1.) return offs;
    if (current_rad > previous_rad) return(--offs);
    else  return(++offs);
}
//=============================================================================
//*********************** Structure lists utility *****************************
//=============================================================================

void 
RPCGeometry::rreset()
{
    Removed = 0;
}


template<class entry> entry*
RPCGeometry::remove(entry* first, entry* item)
{
    if(!item) return first;
    
    entry* tmp = first;
    
    if(tmp == item)
    {
        entry* newfirst = first->next;
	item->next = 0;
	++Removed;
	return newfirst;
    }
    
    entry* prv = tmp;
    tmp = prv->next;
    
    while(tmp)
    {
        if(tmp == item)
	{
	    prv->next = tmp->next;
	    item->next = 0;
	    ++Removed;
	    return first;
	}
	prv = tmp;
        tmp = prv->next;
    }
    
    printf("RPCGeometry, remove: error in removing pack!\n");
    exit(1);
}


template<class entry> entry*
RPCGeometry::give_last(entry* first)
{
    if(!first) return 0;
    
    entry* last = first;
    
    while(last->next)
    {
        last = last->next;
    }
    
    return last;
}


void
RPCGeometry::set_first_packs(MultiChamber* cham)
{
    for (int stat=First;stat<=Extended;++stat)
    {
        float Zmin = 99999.;
        RPC_pack* pack = cham->RPC_packs;
        while(pack)
        {
            if(s_Zsign*pack->pack_coo[2]<=Zmin && pack->station==stat)
            {
                Zmin = s_Zsign*pack->pack_coo[2];
                if(pack->f_proj[0]) cham->firstRPC_z[0][stat] = pack;
                if(pack->f_proj[1]) cham->firstRPC_z[1][stat] = pack;   
            }
	    pack = pack->next;
        }
	if(cham->firstRPC_z[0][stat] == cham->firstRPC_z[1][stat])
	{
	    cham->RPC_packs =remove(cham->RPC_packs,cham->firstRPC_z[0][stat]);
	} else
	{
	    cham->RPC_packs =remove(cham->RPC_packs,cham->firstRPC_z[0][stat]);
	    cham->RPC_packs =remove(cham->RPC_packs,cham->firstRPC_z[1][stat]);
	}
	
	if (DebuGeo) first_packs_info(cham->firstRPC_z[0][stat],
	                              cham->firstRPC_z[1][stat],stat);
    }	   
}


RPC_pack*
RPCGeometry::find_next_pack(MultiChamber* cham, RPC_pack* pack,int sect)
{
    RPC_pack* it = cham->RPC_packs;
    RPC_pack* found = 0; 
    
//    if(!pack->f_proj[sect]) return found;
    
    float dist = 99999;
    while (it)
    {
        float tmp_dist=(s_Zsign*(it->pack_coo[2]) - s_Zsign*(pack->pack_coo[2]));
        if(tmp_dist < dist                                         && 
	   s_Zsign*(it->pack_coo[2]) > s_Zsign*(pack->pack_coo[2]) &&
	   it->station == pack->station                            && 
	   it->f_proj[sect])
	{
	    dist = tmp_dist;
	    found = it;
	}
        it = it->next;
    }
    return found;
}


void
RPCGeometry::connect(RPC_pack* prev, RPC_pack* next)
{
    if(!prev||!next) return;
    
    if(prev->f_proj[0] && next->f_proj[0])
    {
        prev->next_in_z[0] = next;
        next->prev_in_z[0] = prev;
    }
    
    if(prev->f_proj[1] && next->f_proj[1])
    {
        prev->next_in_z[1] = next;
        next->prev_in_z[1] = prev;
    }
    
//    if(next->f_proj[0]) prev->next_in_z[0] = next;
//    if(next->f_proj[1]) prev->next_in_z[1] = next;
    
//    if(prev->f_proj[0]) next->prev_in_z[0] = prev;
//    if(prev->f_proj[1]) next->prev_in_z[1] = prev;
}


MultiChamber*
RPCGeometry::find_first_chamber(MultiChamber *cham, int stat, int proj)
{
    float Zmin = 99999.;
    MultiChamber *first = 0;
    
    while(cham)
    {
//        int i = 0;
//	printf("Step=%d\n",++i);
//	cham_info (cham,stat);
        float z = s_Zsign*((cham->firstRPC_z[proj][stat])->pack_coo[2]);
        if(z <= Zmin)
        {
            Zmin = z;
	    first = cham;
        }
        cham = cham->next;
    }
    return first;
    
}


RPC_pack*
RPCGeometry::give_last_pack(MultiChamber *cham, int stat, int proj)
{
    RPC_pack *last = cham->firstRPC_z[proj][stat];
    while(last->next_in_z[proj]) last = last->next_in_z[proj];
    return last;
}


float
RPCGeometry::give_radius(RPC_pack *pack)
{
    float radius = pack->pack_coo[0]*cos(-pack->rotation_angle) -
                   pack->pack_coo[1]*sin(-pack->rotation_angle);
    return radius;
}


int
RPCGeometry::same_cham(RPC_pack *one, RPC_pack *two)
{
    if(!one || !two) return 0;
    if(one->cham == two->cham) return 1;
    else return 0;
}
//=============================================================================
//********************** Ordering of geometry structures **********************
//=============================================================================

void
RPCGeometry::setupmap()
{    
//  == Setting the pack z_linker and sector+station code into a multichamber<--
    for (int Jtype=1;Jtype<=MaxJtype;++Jtype)
    {
        for (int Jff=1;Jff<=Mff;++Jff)
	{
	    for (int Jzz=-Mzz;Jzz<=Mzz;++Jzz)
	    {
	        MultiChamber *cham = 
		                 T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
				  		
	        if (cham)
		{
		    if (DebuGeo) list_of_packs(cham);
		    
		    rreset();
		    set_first_packs(cham);
                    for (int stat=First;stat<=Extended;++stat)
                    {
                       if(cham->firstRPC_z[0][stat]==cham->firstRPC_z[1][stat])
                       {
                        build_InterChamS_links(cham,cham->firstRPC_z[0][stat]);
                       }else
		       {
			build_InterChamS_links(cham,cham->firstRPC_z[0][stat]);
			build_InterChamS_links(cham,cham->firstRPC_z[1][stat]);
		       }
                    }
		    
		    if (Removed != cham->nRPC_packs)
		    {
		        std::cout << "Jtype=" << Jtype << ",  Jff=" << Jff
			          << ",  Jzz=" << Jzz << std::endl;
		        printf("RPCGeometry error: number of RPC packs = %d, ",
			cham->nRPC_packs);
			printf("removed packs = %d\n",Removed);
			exit(1);
		    }
		    
		    check_map(cham);      // Check the single chamber structure
		    
		    if(DumpMap) dump_chamber(cham);    // Dunp chamber on stdio
		}
            }
        }
    }
}


void
RPCGeometry::build_InterChamS_links(MultiChamber* cham, RPC_pack* pack)
{
    if(!pack) return;

    RPC_pack* pack1 = find_next_pack(cham,pack,0);
    RPC_pack* pack2 = find_next_pack(cham,pack,1);
    
    if(!pack1&&!pack2) return;

    if(pack1 == pack2)
    {
        connect(pack,pack1);
	if(pack->f_proj[1]) cham->RPC_packs = remove(cham->RPC_packs,pack1);

	build_InterChamS_links(cham,pack1);
    } else
    {
        connect(pack,pack1);
	connect(pack,pack2);
	cham->RPC_packs = remove(cham->RPC_packs,pack1);
	cham->RPC_packs = remove(cham->RPC_packs,pack2);

	build_InterChamS_links(cham,pack1);
        build_InterChamS_links(cham,pack2);
    }
}


void
RPCGeometry::check_map(MultiChamber *cham)
{
    int count = 0;
    
    for (int stat=First;stat<=Extended;++stat)
    {	
	int step[2] = {0,0};
	
	for(int i=0;i<2;++i)
	{
	    RPC_pack* pack = cham->firstRPC_z[i][stat];
	    
	    if(pack)
	    {
	        if(pack->prev_in_z[0] || pack->prev_in_z[1])
	           check_error(cham,"link for previous z pack are not empty!");
	    
	        while (RPC_pack* next = pack->next_in_z[i])
	        {
	            if(pack->station!=next->station) 
		        check_error(cham,"station inconsistence!");
		    
	            if(!pack->f_proj[i]) 
		     check_error(cham,"previous(->next) logic inconsistence!");
		    if(!next->f_proj[i])
		     check_error(cham,"next(->previous) logic inconsistence!");
		    
		    if(next->prev_in_z[i] != pack)
		        check_error(cham,"next->previous link inconsistence!");
		 
	            pack = next;
		    ++step[i];
	        }
	    }
	}
		
	if(step[0]!=step[1]) check_error(cham,"z index mismatch!");
	
	RPC_pack* pack0 = cham->firstRPC_z[0][stat];
	RPC_pack* pack1 = cham->firstRPC_z[1][stat];
	
	if (pack0&&pack1)
	{
	    if(pack0==pack1) ++count;
	    else count += 2;
	
	    while(step[0]--)
	    {
	        pack0 = pack0->next_in_z[0];
	        pack1 = pack1->next_in_z[1];
	        if(pack0==pack1) ++count;
	        else count += 2; 
	    }
        } else
	{
	    if(pack0)
	    {
	        count += step[0] +1;
	    } else if (pack1)
	    {
	        count += step[1] +1;
	    }
	} 
	
    }
    
    if(cham->nRPC_packs != count) check_error(cham,"RPC packs lost!");
}


void
RPCGeometry::setuplogic()
{    
    for(int sect=0;sect<16;++sect)
    {
        int Jff = sect/2 + 1;
        
	for(int proj=0;proj<2;++proj)
	{
	    for(int stat=First;stat<=Extended;++stat)
	    {
	        MultiChamber* cham_sector[3];
		
		cham_sector[0] = 0;
		cham_sector[1] = 0;
		cham_sector[2] = 0;
		
	        MultiChamber* last = 0;
	    
	        for (int Jtype=1;Jtype<=MaxJtype;++Jtype)
	        {
                    for (int Jzz=-Mzz;Jzz<=Mzz;++Jzz)
	            {
		        int z_index;
		        if(Jzz<0)        
			{
			    z_index = 0;
			    last = give_last(cham_sector[z_index]);
			} else if (Jzz==0)
			{
			    z_index = 1;
			    last = give_last(cham_sector[z_index]);
                        } else   //if (Jzz>0)
			{
			    z_index = 2;
			    last = give_last(cham_sector[z_index]);
                        }
		    		    
		        MultiChamber *cham =
		                T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
		    
		        if(cham                         && 
		           cham->firstRPC_z[proj][stat] && 
		           sect == cham->physics_sector)
		        {
		            if(cham_sector[z_index])
                            {
		                last->next = cham; 
                            } else
                            {
			        cham_sector[z_index] = cham;
                            }
			    last = cham;
			}
		    }		    
                }
		
		build_TransChamS_links(cham_sector,sect,stat,proj);
		build_RPC_coding(sect,stat,proj);
            }
        }
    }
    
    if(!s_testbeam) check_logic();
    //else shift_logic();
    if(DumpLog) dump_logic_sectors();
}


void
RPCGeometry::build_TransChamS_links(MultiChamber **chambers,
                                    int sect,int stat,int proj)
{
    if(!chambers[0] && !s_testbeam )
    {
      if (stat == Extended) return;
      printf("RPCGeometry, build_TransChamS_links: no chambers for Jzz < 0\n");
      exit(1);
    }
    
    if(chambers[1] && (chambers[1])->next)
    {
       printf("sect = %d\n",sect);
       printf("RPCGeometry, build_TransChamS_links: more than 1 chamber");
       printf(" whit Jzz = 0\n");
       MultiChamber *cham = chambers[1];
       do {cham_info(cham,10);} while((cham = cham->next)); 
       exit(1);
    }
    
    if(!chambers[2] && !s_testbeam )
    {
      if (stat == Extended) return;
      printf("RPCGeometry, build_TransChamS_links: no chambers for Jzz > 0\n");
      exit(1);
    }
    
        
    int neg_sector = sect*2 + proj - 1;//logic sector for negative half barrel
    if (neg_sector < 0) neg_sector =31;//Put the 1-st "half" Large at sector 31
    int z_index = 0;
	    
    MultiChamber *found = find_first_chamber(chambers[z_index],stat,proj);
    MultiChamber *last  = found;
    RPC_pack     *prev  = 0;
    RPC_pack     *next  = 0;
    
    LogicSector[neg_sector][stat][0] = (found)? found->firstRPC_z[proj][stat]:0;    
    chambers[z_index] = remove(chambers[z_index],found);
    
    while((found = find_first_chamber(chambers[z_index],stat,proj)))
    {
        prev = give_last_pack(last,stat,proj);
	next = found->firstRPC_z[proj][stat];
	prev->next_in_z[proj] = next;
	next->prev_in_z[proj] = prev;
        chambers[z_index] = remove(chambers[z_index],found);
	last = found;
    }
    
    LogicSector[neg_sector][stat][1] = (LogicSector[neg_sector][stat][0])? 
                                       give_last_pack(last,stat,proj) : 0;
    
    if(chambers[1])
    {
        RPC_pack *first = (chambers[1])->firstRPC_z[proj][stat];
	RPC_pack *last  = give_last_pack(chambers[1],stat,proj);
	if(first != last)
	{
	    printf("RPCGeometry, build_TransChamS_links: Jzz = 0 packs ");
	    printf("inconsistence\n");
	    exit(1);
	}
	if(LogicSector[neg_sector][stat][1]) (LogicSector[neg_sector][stat][1])->next_in_z[proj] = first;
        first->prev_in_z[proj] = LogicSector[neg_sector][stat][1];
	LogicSector[neg_sector][stat][1] = last;
	(chambers[1])->next = chambers[2];
    }
    
    z_index = (chambers[1])? 1 : 2;
    int pos_sector = neg_sector + 32; // logic sector for positive half barrel
    
    found = find_first_chamber(chambers[z_index],stat,proj);
    last  = found;
    
    LogicSector[pos_sector][stat][0] = (found)? found->firstRPC_z[proj][stat]:0;
    chambers[z_index] = remove(chambers[z_index],found);
    
    while((found = find_first_chamber(chambers[z_index],stat,proj)))
    {
        prev = give_last_pack(last,stat,proj);
	next = found->firstRPC_z[proj][stat];
	prev->next_in_z[proj] = next;
	next->prev_in_z[proj] = prev;
        chambers[z_index] = remove(chambers[z_index],found);
	last = found;
    }
    
    LogicSector[pos_sector][stat][1] = (LogicSector[pos_sector][stat][0])?
                                        give_last_pack(last,stat,proj) : 0;
    
    if((next = LogicSector[pos_sector][stat][0]) !=
       (prev = LogicSector[neg_sector][stat][1]))
    {
       if(prev) prev->next_in_z[proj] = next;
       if(next) next->prev_in_z[proj] = prev;
    }
}


void
RPCGeometry::build_RPC_coding(int sect,int stat,int proj)
{
    int sect_neg = sect*2 + proj - 1; // Logic sector for negative half barrel
    if (sect_neg < 0) sect_neg = 31;  // Put the 1-st "half" Large at sector 31
    int sect_pos = sect_neg + 32;     // Logic sector for positive half barrel
	
    RPC_pack *negative_half = LogicSector[sect_neg][stat][1];
    RPC_pack *positive_half = LogicSector[sect_pos][stat][0];

    bool bothExist = (negative_half && positive_half)? true : false;
	    
    int start     = (negative_half == positive_half)? 0 : 1;
    int neg_index = start;
    
    while(negative_half)
    {
        int code = neg_index++;
	
	if(negative_half->lvl1_z_index && negative_half->lvl1_z_index != code)
	{
	    printf("RPCGeometry,  build_RPC_coding: wrong RPC code for ");
	    printf("sector logic = %2d, stat = %1d\n",sect_neg,stat);
	    printf("    old code = %d,  new code = %d\n",
	                                     negative_half->lvl1_z_index,code);
            exit(1);
	}
	
	negative_half->lvl1_z_index = code;
        negative_half = negative_half->prev_in_z[proj];
    }
    
    int pos_index = start;
    
    while(positive_half)
    {
        int code = pos_index++;
        
	if(positive_half->lvl1_z_index && positive_half->lvl1_z_index != code)
	{
	    printf("RPCGeometry,  build_RPC_coding: wrong RPC code for ");
	    printf("sector logic = %2d, stat = %1d\n",sect_pos,stat);
	    printf("    old code = %d,  new code = %d\n",
	                                     positive_half->lvl1_z_index,code);
            exit(1);
	}
	
	positive_half->lvl1_z_index = code;
        positive_half = positive_half->next_in_z[proj];
    }
    
    if((pos_index != neg_index) && bothExist)
    {
        printf("RPCGeometry,  build_RPC_coding: different index for the two ");
	printf("half barrel\n");
	printf("    logic sector = %2d,   station = %1d\n",sect_neg,stat);
	printf("    negative index = %2d,  positive index = %2d\n",neg_index,
	                                                           pos_index);
	//exit(1);
    }
}


void
RPCGeometry::check_logic(void)
{
    for(int logic_sector=0;logic_sector<64;++logic_sector)
    {
        for(int RPC_station=First;RPC_station<=Third;++RPC_station)
	{
	    for(int i=0;i<2;++i)
	    {
	        if(!LogicSector[logic_sector][RPC_station][i])
		{
		    std::string pos = (i)? "end position" : "start position";
		    printf("RPCGeometry, check_logic: no link for %s of ",
		                                                 pos.c_str());
		    printf("logic sector %2d, station = %1d\n",logic_sector,
		                                               RPC_station+1);
                    exit(1);
		}
	    }
	}
    }
}

void
RPCGeometry::shift_logic(void)
{
    int start_logic = 0;
    
    for(int logic_sector=0;logic_sector<64;++logic_sector)
    {
        if(LogicSector[logic_sector][0][0])
	{
            start_logic = logic_sector;
	    break;
	}

    }
    
    int sector = 0;
    
    for(int logic_sector=start_logic;logic_sector<64;++logic_sector)
    {
        for(int RPC_station=First;RPC_station<=Extended;++RPC_station)
	{
	    for(int i=0;i<2;++i)
	    {
	        LogicSector[sector][RPC_station][i] = 
		LogicSector[logic_sector][RPC_station][i];
		LogicSector[logic_sector][RPC_station][i] = 0;
	    }
	}
	++sector;
    }  
}

//=============================================================================
//***************** Dumping the geometry and the logic maps *******************
//=============================================================================

bool
RPCGeometry::RPC_packs_not_equal(RPC_pack* pack1, RPC_pack* pack2) const
{
    if(!pack1 || !pack2) return true;
    if((pack1->lvl1_z_index == pack2->lvl1_z_index) &&
       (pack1->station      == pack2->station     ) &&
       (pack1->nstrip_z     == pack2->nstrip_z    ) &&    
       (pack1->stripsize_z  == pack2->stripsize_z ) &&
       (pack1->nstrip_f     == pack2->nstrip_f    ) &&    
       (pack1->stripsize_f  == pack2->stripsize_f ) )  return false;
    return true;
}


bool
RPCGeometry::Logic_sectors_are_equal(int sector1, int sector2) const
{
    int proj1 = (sector1+1)%2;
    int proj2 = (sector2+1)%2;

    for (int RPC_station=First;RPC_station<=Extended;++RPC_station)
    {
       RPC_pack* pack1 = (sector1>=32)? LogicSector[sector1][RPC_station][0] :
                                        LogicSector[sector1][RPC_station][1];
       RPC_pack* pack2 = (sector2>=32)? LogicSector[sector2][RPC_station][0] :
                                        LogicSector[sector2][RPC_station][1];

       while(pack1 && pack2)
       {
          if(RPC_packs_not_equal(pack1,pack2)) return false;
	  pack1=(sector1>=32)? pack1->next_in_z[proj1]:pack1->prev_in_z[proj1];
	  pack2=(sector2>=32)? pack2->next_in_z[proj2]:pack2->prev_in_z[proj2];
       }
				   
    }
    
    return true;
}


void
RPCGeometry::write_geodata(std::ofstream& file) const
{
    int type = 0;
    int sector_map[64];
    int sector_typ[64];
    for (int i=0;i<64;++i) {sector_map[i] = 0; sector_typ[i] = 0;}
    
    for (int sec=0;sec<64;++sec)
    {
        if (!sector_map[sec])
	{
	    sector_map[sec] = ++type;
	    sector_typ[type-1] = sec;
	    for(int i=sec;i<64;++i)
	    {
	        if(Logic_sectors_are_equal(sec,i)) sector_map[i] = type;
	    }
	}
    }
    
    file << std::endl << "# Start Sector type enumeration" << std::endl;
    file << std::endl << "SECTOR TYPES (  0 - 15 ): ";
    for (int i=0;i<16;++i) file << " " << std::setw(2) << sector_map[i];
    file << std::endl << "SECTOR TYPES ( 16 - 31 ): ";
    for (int i=16;i<32;++i) file << " " << std::setw(2) << sector_map[i];
    file << std::endl << std::endl << "SECTOR TYPES ( 32 - 47 ): ";
    for (int i=32;i<48;++i) file << " " << std::setw(2) << sector_map[i];
    file << std::endl << "SECTOR TYPES ( 48 - 63 ): ";
    for (int i=48;i<64;++i) file << " " << std::setw(2) << sector_map[i];
    file << std::endl << std::endl;
    
    int dumped_type = 0;
    while(dumped_type != type)
    {
        int sec = sector_typ[dumped_type];
	++dumped_type;
	file << std::endl << std::endl << "########################################";
	file << "#######################################" << std::endl;
	file << "#################################### TYPE " << dumped_type;
	file << " ###################################" << std::endl;
	file << "########################################";
	file << "#######################################" << std::endl;
	for (int RPC_station=First;RPC_station<=Extended;++RPC_station)
	{
	    if(!LogicSector[sec][RPC_station][0]) break;
	    
	    file << std::endl << "RPC GEOM  " << std::setw(2) << dumped_type; 
	    file << " : station " << std::setw(1) << RPC_station + 1;
	    int ncham = 0;
	    if (sec > 32) 
	    {
	        ncham = ((LogicSector[sec][RPC_station][0])->lvl1_z_index)? 
		         (LogicSector[sec][RPC_station][1])->lvl1_z_index : 
		         (LogicSector[sec][RPC_station][1])->lvl1_z_index + 1; 
	    } else
	    {
	        ncham = ((LogicSector[sec][RPC_station][1])->lvl1_z_index)? 
		         (LogicSector[sec][RPC_station][0])->lvl1_z_index : 
		         (LogicSector[sec][RPC_station][0])->lvl1_z_index + 1;
	    }
	    file << " made of " << std::setw(2) << ncham << " chamber. ";
	    file << "Strips in connectors: eta ";
	    int eta_conn = (RPC_station==Second)? 8 : 4;
	    file << std::setw(1) << eta_conn << " phi 8" << std::endl;
	    
	    file << "{" << std::endl;
	    RPC_pack* pack = (sec>=32)? LogicSector[sec][RPC_station][0] :
                                        LogicSector[sec][RPC_station][1];
            int proj = (sec+1)%2;
            
	    while(pack)
	    {
	        file << " cham " << std::setw(2) << pack->lvl1_z_index;
		file << " type " << pack->cham->name << "-";
		file << "    eta_side  " << std::setw(2) << pack->nstrip_z;
		file << " " << std::setw(2) << pack->nstrip_z/eta_conn << "  2";
		file << "    phi_side  " << std::setw(2) << pack->nstrip_f;
		file << " " << std::setw(2) << pack->nstrip_f/8 << "  2";
		file << "  #MDT chamber " << abs(pack->cham->amdb_z_index)
		     << std::endl;
		
	        pack=(sec>=32)? pack->next_in_z[proj]:pack->prev_in_z[proj];
	    }
	    
	    file << "}" << std::endl;
	}
    }
    
   // exit(1);
}


//=============================================================================
//********************* facilities for the data decoding **********************
//=============================================================================
const encodeProj*
RPCGeometry::find_proj(int Jobj) const
{
    encodeProj* tmp = projMap;
    while(tmp)
    {
        if (tmp->Jobj == Jobj) return tmp;
	tmp = tmp->next;
    }

    return 0;
}

void 
RPCGeometry::add_proj(encodeProj* item)
{
    if (!projMap) 
    {
        projMap = item;
	return;
    }
    
    encodeProj* tmp = projMap;
    while(tmp)
    {
    if(tmp->Jobj==item->Jobj && tmp->proj==item->proj) {
        delete item;
    	return;
    }    
	if(tmp->Jobj==item->Jobj && tmp->proj!=item->proj)
	{
	    std::cout << "RPCGeometry ERROR in projMap:" 
	              << " try to insert Jobj=" << item->Jobj
		      << " with proj=" << item->proj << " in" << std::endl;
            dump_projMap();
	    exit(1);
	}
        if (tmp->next == 0) 
	{
	    tmp->next = item;
	    return;
	}
	tmp = tmp->next;
    }
}

/*
void
RPCGeometry::check_projMap()
{
    encodeProj* tmp = projMap;
    while(tmp)
    {
	encodeProj* match = tmp->next;
	while(match)
        {
	    if(tmp->Jobj == match->Jobj)
	    {
	        std::cout << "RPCGeometry ERROR in projMap:" << std::endl;
		dump_projMap();
	        exit(1);
	    }
            match = match->next;
	}
	tmp = tmp->next;
    }
}
*/
void
RPCGeometry::dump_projMap()
{
    encodeProj* tmp = projMap;
    while(tmp)
    {
        std::cout << "RPCGeometry projMap:  Jobj=" << tmp->Jobj
	          << ",  proj=" << tmp->proj << std::endl;
	tmp = tmp->next;
    }
}

//=============================================================================
//***************** Interface between geometry and LVL1 world *****************
//=============================================================================

#ifndef LVL1_STANDALONE
bool
RPCGeometry::give_strip_index(unsigned long int code,int& Jtype, 
                              int& StationEta, int& StationPhi, int& DoubletR, 
                              int& DoubletZ,   int& DoubletP,   int& GasGap, 
                              int& MeasuresPhi,int& Strip) const
{
    AMDC;

    int Jff;
    int Jzz;
    int Job;
    int Jspli;
    int Jsl;
    int Jsz;
    int Jstri;

    RPCdecoder decode(code);

    if (decode)
    {
        int z_index      = decode.rpc_z_index();
        int lvl1_station = decode.lvl1_station() - 1;
        int logic_sector = decode.logic_sector();
        int proj         = (logic_sector+1)%2;
    
        RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
        while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];

        if (pack == 0) return false;
	
        Jff   = (int)((pack->cham->physics_sector)/2) + 1;
        Jzz   = pack->cham->amdb_z_index;
        Job   = pack->Jobj;
	Jspli = (pack->Readout_f==1)? 1 : proj + 1;
	Jsl   = decode.rpc_layer() + 1;
        Jsz   = decode.strip_type();
        Jstri = decode.strip_number() + 1;
        
	if( Jsz==1 ) {
	    if(Jsz>pack->nstrip_f) return false;   
	} else {
	    if(Jsz>pack->nstrip_z) return false;   
	}
	
	std::string StationName = pack->cham->name;
        Jtype = amdc->GetJtyp(StationName);

        amdc->GetRPCindexFromAMDC(StationName,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
                                  StationEta,StationPhi,DoubletR,DoubletZ,
                                  DoubletP,GasGap,MeasuresPhi,Strip);
				
    }
    else return false;
    
    return true;
}


unsigned long int
RPCGeometry::give_strip_code( std::string StationName, int StationEta,
			      int StationPhi, int DoubletR, int DoubletZ,
                              int DoubletP, int GasGap, int MeasuresPhi,
			      int Strip) const
{
    AMDC;

    int Jtype = amdc->GetJtyp(StationName);
    int Jff;
    int Jzz;
    int Job;
    int Jspli;
    int Jsl;
    int Jsz;
    int Jstri;
/*
    std::cout << "StationName=" << StationName << ",  StationEta=" << StationEta
              << ",  StationPhi=" << StationPhi << std::endl
	      << "DoubletR=" << DoubletR << ",  DoubletZ=" << DoubletZ
	      << ",  DoubletP=" << DoubletP << std::endl
	      << "GasGap=" << GasGap << ",  MeasurePhi=" << MeasuresPhi
	      << ",  Strip=" << Strip << std::endl;
*/	      
    amdc->GetAMDCindexFromRPC(StationName,StationEta,StationPhi,DoubletR,
                              DoubletZ,DoubletP,GasGap,MeasuresPhi,Strip,
                              Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri);

    if ( Jtype==4 && Jff!=6 && Jff!=7 && Job>=s_JobMinForEncodeProj &&
        (Jzz==2 || Jzz==4 || Jzz==-2 || Jzz==-4) )
    {
        const encodeProj* tmp = find_proj(Job);
	if (!tmp) 
	{
	    std::cout << "RPCGeometry ERROR: can't find proj index" 
	              << std::endl;
	    exit(1);
	}
	else Jspli = tmp->proj;
	//std::cout << "Jobj=" << Job << "  and Jspli=" << Jspli << std::endl;
    }
    
    return this->give_strip_code(Jtype,Jff,Jzz,Job,Jsl,Jsz,Jstri,Jspli);
}
#endif

unsigned long int
RPCGeometry::give_strip_code(int Jtype,int Jff,int Jzz,int Jobj,int Jlay,
                             int Jsty,int Jstr,int proj) const			     
{

//    printf("Jtype=%d, Jff=%d, Jzz=%d. Jobj=%d. Jlay=%d. Jsty=%d, Jstr=%d,proj=%d\n",
//    Jtype,Jff,Jzz,Jobj,Jlay,Jsty,Jstr,proj);
    MultiChamber *cham = T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz];
/*    
    std::cout << "Jtype="   << Jtype << ",  Jff=" << Jff << ",  Jzz=" << Jzz 
         << std::endl
         << ",  Jobj=" << Jobj  << ",  Jlay=" << Jlay  << std::endl
	 << ",  Jsty=" << Jsty  << ",  Jstr=" << Jstr << ",  proj=" << proj 
	 << std::endl;
*/    
    RPC_pack *pack = 0;
    
    for(int stat=First;stat<=Extended;++stat)
    {
        RPC_pack *tmp = cham->firstRPC_z[proj-1][stat];
	        
	while(tmp && tmp->cham == cham)
	{
//	    printf("Jobj = %d\n",tmp->Jobj);
	    if(tmp->Jobj == Jobj)
	    {
	        if(pack) 
		{
		    printf("RPCGeometry,  give_strip_code: ");
		    printf("more than 1 pack found!\n");
		    return 0;
		} else
		{
		    pack = tmp;
		}
	    }
	    tmp = tmp->next_in_z[proj-1];
	}
    }
    
    if(!pack)
    {
        proj++;
	for(int stat=First;stat<=Extended;++stat)
        {
            RPC_pack *tmp = cham->firstRPC_z[proj-1][stat];
            while(tmp && tmp->cham == cham)
	    {
//	        printf("Jobj = %d\n",tmp->Jobj);
	        if(tmp->Jobj == Jobj)
	        {
	            if(pack) 
		    {
		        printf("RPCGeometry,  give_strip_code: ");
		        printf("more than 1 pack found!\n");
		        return 0;
		    } else
		    {
		        pack = tmp;
		    }
	        }
	        tmp = tmp->next_in_z[proj-1];
	    }
        }
    }
    
    int station = pack->station;
    
    int z_index = pack->lvl1_z_index;
    
    int logic_sector = cham->physics_sector*2 + proj - 2;
    if (logic_sector < 0 ) logic_sector = 31;
        
    int nstrip = (Jsty-1)? pack->nstrip_z : pack->nstrip_f;
    
    if (Jstr > nstrip)
    {
        printf("RPCGeometry,  give_strip_code: ");
        printf("inconsistence on strip number!\n");
	printf("    RPC strips = %2d,    digitized strip number = %2d\n",
	       nstrip,Jstr);
        return 0;
    }
    
//    float Z = *(pack->z_proj + Jstr - 1);
//    if(Z > 0.) logic_sector += 32;
    if (Jzz >= 0) logic_sector += 32;
    
//    printf ("logic_sector = %d, station=%d\n",logic_sector,station);
    
    RPC_pack *check = LogicSector[logic_sector][station][0];
    
    while (check)
    {
        if(check->lvl1_z_index == z_index) break;
	check = check->next_in_z[proj-1];
    }
    
    if (check != pack)
    {
        printf("RPCGeometry,  give_strip_code: ");
        printf("RPC pack inconsistence in Sector Logic map!\n");
	exit(1);
        return 0;
    }
    
    RPCdecoder decode(Jsty,logic_sector,station+1,Jlay-1,z_index,Jstr-1);    
    //unsigned int code = Jsty         * 100000000 +
    //                    logic_sector * 1000000   +
    //	            PoolRDOInput = [ "rfio:/castor/cern.ch/user/d/drebuzzi/preprod9.0.2/p.03-mu+eta0.4-0.9.root" ]    station      * 100000    +
    //	                (Jlay - 1)   * 10000     +
    //	                z_index      * 100       +
    //	                Jstr -  1;
    
    return decode.code();
}


bool
RPCGeometry::give_strip_coordinates(unsigned long int code, float *xyz) const
{
    RPCdecoder decode(code);
    if (!decode) return false;
        
    int strip_number = decode.strip_number();
    int z_index      = decode.rpc_z_index();
    int rpc_layer    = decode.rpc_layer();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int Jsty         = decode.strip_type();
    int proj         = (logic_sector+1)%2;
    
    
    //std::cout << "strip_number = " << strip_number << std::endl;
    //std::cout << "z_index = " << z_index << std::endl;
    //std::cout << "rpc_layer = " << rpc_layer << std::endl;
    //std::cout << "lvl1_station = " << lvl1_station << std::endl;
    //std::cout << "logic_sector = " << logic_sector << std::endl; 
    //std::cout << "Jsty = " << Jsty << std::endl; 
    //std::cout << "proj = " << proj << std::endl; 
    
    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
   
    if (pack == 0) return false;
   
    if(Jsty == 1)
    {
        if(strip_number > pack->nstrip_f) return false;
        xyz[0] = *(pack->f_proj[proj] + strip_number*4 + rpc_layer*2);
	xyz[1] = *(pack->f_proj[proj] + strip_number*4 + rpc_layer*2 + 1);
        xyz[2] = pack->pack_coo[2];
    } else
    { 
        if(strip_number > pack->nstrip_z) return false;  
        xyz[0] = pack->gas_layer_XY[proj][rpc_layer][0];
	xyz[1] = pack->gas_layer_XY[proj][rpc_layer][1];
	xyz[2] = *(pack->z_proj + strip_number);
	
	//float DeltaZ = pack->pack_ref_coo[2][rpc_layer][0] - xyz[2];
	//xyz[0] = pack->pack_ref_coo[0][rpc_layer][0] - 
	//         DeltaZ*pack->pack_ref_deltas[0][rpc_layer];
	//xyz[1] = pack->pack_ref_coo[1][rpc_layer][0] - 
	//         DeltaZ*pack->pack_ref_deltas[1][rpc_layer]; 
    }
    
    return true;
}


bool
RPCGeometry::give_strip_radius(unsigned long int code,float& radius) const
{
    RPCdecoder decode(code);
    if(!decode) return false;

    int z_index      = decode.rpc_z_index();
    int rpc_layer    = decode.rpc_layer();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;

    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
    
    if (pack==0) return false;
    
    radius = pack->gas_layer_radius[rpc_layer];
    return true;
}

bool
RPCGeometry::give_eta_strip_size(unsigned int code,float& size) const
{    
    RPCdecoder decode(code);
    if(!decode) return false;

    int z_index      = decode.rpc_z_index();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;

    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
    
    if (pack==0) return false;
    
    size = pack->stripsize_z;
    return true;
}

bool
RPCGeometry::give_phi_strip_size(unsigned int code,float& size) const
{
    RPCdecoder decode(code);
    if(!decode) return false;

    int z_index      = decode.rpc_z_index();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;
    
    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
    
    if(pack==0) return false;
    
    size = pack->stripsize_f;
    return true;
}

bool
RPCGeometry::existCode(unsigned int code) const
{
    RPCdecoder decode(code);
    if(!decode) return false;
    
    int z_index      = decode.rpc_z_index();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;
        
    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
    
    if(pack)
    {
        int strip_number = decode.strip_number();
        int Jsty = decode.strip_type();
	 
	if( Jsty==1 ) {
	    if(strip_number<pack->nstrip_f) return true;   
	} else {
	    if(strip_number<pack->nstrip_z) return true;   
	}
    }
    
    return false;
}



bool 
RPCGeometry::give_station_radius(unsigned int code,float& radius) const
{    
    RPCdecoder decode(code);
    if(!decode) return false;    
    
    int z_index      = decode.rpc_z_index();
    int rpc_layer    = decode.rpc_layer();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;
    
    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];
    
    if(pack==0) return false;
    
    radius = pack->gas_layer_radius[rpc_layer];
    
    return true;
}


bool
RPCGeometry::give_station_phi(unsigned int code,float& phi) const
{
    RPCdecoder decode(code);
    if(!decode) return false;    
    
    int z_index      = decode.rpc_z_index();
    int lvl1_station = decode.lvl1_station() - 1;
    int logic_sector = decode.logic_sector();
    int proj         = (logic_sector+1)%2;
    
    RPC_pack *pack = LogicSector[logic_sector][lvl1_station][0];
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];

    if (pack==0) return false;

    phi = pack->rotation_angle;
    
    // account for the new PHI coordinate system
    //float PI = 3.1415926535899;
    //if (phi >= PI) phi = phi -2*PI;
    
    return true;
}

bool
RPCGeometry::give_rpc_boundaries(int logic_sector,int station,int z_index,
                                 float& zmin, float& zmax) const
{
    int proj         = (logic_sector+1)%2;
    
    RPC_pack* pack = LogicSector[logic_sector][station-1][0];    
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];

    if(pack==0) return false;

    zmin = fabsf(pack->pack_coo[2] - pack->half_dim[1]);
    zmax = fabsf(pack->pack_coo[2] + pack->half_dim[1]);
    
    if (zmin > zmax)
    {
        float tmp = zmax;
	zmax = zmin;
	zmin = tmp;
    }

    return true;
}

bool
RPCGeometry::give_rpc_XYdim(int logic_sector,int station,int z_index,
                                 float& XYdim) const
{
    int proj         = (logic_sector+1)%2;
    
    RPC_pack* pack = LogicSector[logic_sector][station-1][0];    
    while(pack && pack->lvl1_z_index != z_index) pack = pack->next_in_z[proj];

    if(pack==0) return false;

    XYdim = pack->half_dim[0];

    return true;
}

bool
RPCGeometry::isFull() const
{
    return !s_testbeam;
}

//=============================================================================
//*********************** Access to the geometry data *************************
//=============================================================================


//#ifndef LVL1_STANDALONE

void
RPCGeometry::buildgeo() 
{
  //amdbgeo();
  //amdbtoc();   // fill the AMDB bank
    
   // printf("Proseguo con la geometria!\n");
    
   // Amdbsimrec*  amdb = new (Amdbsimrec);
   // Amdbsimrec*  amdb = s_amdb;
   
    AMDC;
    
    Mtype = amdc->Mtyp();
    Mff   = amdc->Mphi();
    Mzz   = amdc->Mzz();
    
    Mtype = amdc->StationTypeEnd();
    Mff   = amdc->PositionPhiEnd();
    Mzz   = amdc->PositionZEnd();
    
    
//  ===== INITIALIZE THE GEOMETRY MAP<-----------------------------------------
    for (int Jtype=0;Jtype<MaxJtype;++Jtype)
    {
        for (int J=0;J<Mff*(2*Mzz+1);++J)
	{
	    T1muonGeo[Jtype][J] = 0;
	}
    }
    
//  =========== START LOOPING ON MULTI-CHAMBERS<-------------------------------
    for (int Jtype=1;Jtype<=MaxJtype;++Jtype)
    {
	std::string name = amdc->GetStationType(Jtype);
	int MJzz = amdc->PositionZEnd();
	
			  	
	int IsValid;
	int MJgeo = amdc->StationGeomEnd();
	double DimLocX,DimLocY,DimLocZ,CenterLocX,CenterLocY,CenterLocZ;
	
	for(int Jgeo = amdc->StationGeomBegin();Jgeo < MJgeo;++Jgeo) {

	  int Jcut = 0;
	  amdc->GetStationDimensions(name,Jgeo,Jcut,IsValid,
	                             DimLocX,DimLocY,DimLocZ,
		                     CenterLocX,CenterLocY,CenterLocZ );
	
	  if (IsValid > 0){
	      
	    std::string GeomTechnoName;
	    int GeomIsValid,GeomTechnoIndex;
            int GeomSplitX,GeomSplitY,GeomShapeIndex;
            double GeomDx,GeomDy,GeomDz,GeomWs,GeomWl;
            double GeomLe,GeomEx,GeomD1,GeomD2,GeomD3;
		 
	    int NJobj = amdc->GetNumberOfStationObjects(name,Jgeo);
	    
	    
//          ================ COMPUTING THE OFFSET OF THE R-MIDDLE 	     
	    enum tech {SPA,MDT,RPC};
            int exist_tech[3] = {0,0,0};
	    
            for(int Jobj=1;Jobj<=NJobj;Jobj++) {
						  
              int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
              std::string tech_name = amdc->TechnologyName(Itec);

              if (tech_name=="SPA") exist_tech[SPA] = Jobj;
              if (tech_name=="CHV") exist_tech[SPA] = Jobj;
	      if (tech_name=="MDT") exist_tech[MDT] = Jobj;
              if (tech_name=="RPC") exist_tech[RPC] = Jobj;
            }
	    
	    int    JobjMid  = 0;
	    int    IstaMid  = 0;
	    int    ItecMid  = 0;
	    double widthSpa = 0;
	    
	    
	    if (exist_tech[SPA]) {
	      JobjMid = exist_tech[SPA];
	      IstaMid = amdc->ISTA(Jtype,Jgeo,JobjMid);
	      ItecMid = amdc->ITEC(Jtype,Jgeo,JobjMid);
	      
	      std::string name = amdc->TechnologyName(ItecMid);
	      
	      widthSpa = (name == "CHV")? (amdc->STAPP(ItecMid,IstaMid))/2. :
	                                  (amdc->STAEE(ItecMid,IstaMid))/2.;
	    } else if (exist_tech[MDT]) {
	      JobjMid  = exist_tech[MDT];
	      IstaMid = amdc->ISTA(Jtype,Jgeo,JobjMid);
	      ItecMid = amdc->ITEC(Jtype,Jgeo,JobjMid);
	      widthSpa = (amdc->STAEE(ItecMid,IstaMid))/2.;
	    } else if (exist_tech[RPC]) {
	      JobjMid  = exist_tech[RPC];
	      IstaMid = amdc->ISTA(Jtype,Jgeo,JobjMid);
	      ItecMid = amdc->ITEC(Jtype,Jgeo,JobjMid);
	      widthSpa = (amdc->STATT(ItecMid,IstaMid,9))/2.;
	    }
	    
	    
	    amdc->GetStationObjectParam( name,Jgeo,JobjMid,GeomIsValid,
		                         GeomTechnoName,GeomTechnoIndex, 
		                         GeomSplitX,GeomSplitY,
					 GeomShapeIndex,GeomDx,GeomDy,
					 GeomDz,GeomWs,GeomWl,GeomLe,  
					 GeomEx,GeomD1,GeomD2,GeomD3 );
	    
	    double RmidOffset = GeomDz + widthSpa;
		    		    
	    
            if (!RmidOffset) {
              printf("RPCGeometry: error in computing RmidOffset!\n");
              exit(1);
            }
	    

            // Start looping over all stations
	    for (int Jff=amdc->PositionPhiBegin();Jff<=Mff;++Jff) {   
	      for (int Jzz=amdc->PositionZBegin();Jzz<=MJzz;++Jzz) {
	      
	        int PosiIsValid,PosiJgeo,PosiJcut,PosiIsBarrel;
                double PosiPhi,PosiZ,PosiR,PosiS;
                double PosiAlfa, PosiBeta,PosiGamma;
			     
                amdc->GetStationPositionParam(name,Jff,Jzz,PosiIsValid,
		                              PosiJgeo,PosiJcut,PosiIsBarrel,
		                              PosiPhi,PosiZ,PosiR,PosiS,
				              PosiAlfa,PosiBeta,PosiGamma);
						
                if( PosiIsValid > 0 && PosiJgeo == Jgeo ) {

                  //if(!m_full && Jff == 1) m_full = true;
		  		
		  MultiChamber*  cham = 0;
		  RPC_pack* last_pack = 0; 
		  
		  double Rmid = nor(PosiR + RmidOffset);
                  		    
                  for(int Jobj=1;Jobj<=NJobj;Jobj++) {
		
	          amdc->GetStationObjectParam( name,Jgeo,Jobj,GeomIsValid,
		                               GeomTechnoName,GeomTechnoIndex, 
		                               GeomSplitX,GeomSplitY,
				               GeomShapeIndex,GeomDx,GeomDy,
					       GeomDz,GeomWs,GeomWl,GeomLe,  
					       GeomEx,GeomD1,GeomD2,GeomD3 );
						  
                  int Ista = amdc->ISTA(Jtype,Jgeo,Jobj);
	          int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);	      
	          std::string tech_name = amdc->TechnologyName(Itec);
	      		  
		    if(tech_name == "RPC")
		    {

///////////////////////////////////////////////////////////////////////////////
//                          ==================================== CHAM INIT <---
			    if (!cham)
			    {
			        cham = new (MultiChamber);
				cham->Rmid = Rmid;
				cham->name = name;
				cham->type = Jgeo;
				cham->amdb_z_index = Jzz;
				cham->physics_sector =(name.c_str()[2] == 'L')?
                                                      (Jff-1)*2 : (Jff-1)*2+1;
				cham->nRPC_packs = 0;
				cham->RPC_packs  = 0;
				cham->next = 0;
				for(int i=0;i<2;++i)for(int j=First;j<=Extended;++j)
				    cham->firstRPC_z[i][j] = 0;
				T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz] =
				cham;
			    }
			    
			    RPC_pack* pack = new (RPC_pack);
			    pack->nextall = allpacks;
			    allpacks = pack;
			    pack->Jobj        = Jobj;
			    pack->Readout_f   = 0;
			    pack->cham        = cham;
			    pack->next = 0;
			    pack->z_proj = 0;
			    pack->lvl1_z_index = 0;
			    for(int i=0;i<2;++i)
			    {
			        pack->f_proj[i] = 0;
				pack->next_in_z[i] = 0;
				pack->prev_in_z[i] = 0;
				for(int j=0;j<2;++j) for(int k=0;k<2;++k)
				    pack->gas_layer_XY[i][j][k] = 0.;
			    }
			    
//                          ======================== STRIP SIZE AND NUMBER <---
			    pack->stripsize_f = nor(amdc->STATT(Itec,Ista,10));
			    pack->stripsize_z = nor(amdc->STATT(Itec,Ista,11));
			    
			    float Readout_f = amdc->STAOO(Itec,Ista,2);
                            // Change to account Anna's whishes!
			    // float Readout_z = amdc->STAOO(Itec,Ista,3);
			    float Readout_z = 1.;
			    float Length    = nor(GeomLe);
			    float Width     = nor(GeomWs);
			    float Dead      = nor(GeomD1);
			    
			    pack->Readout_f = (int) Readout_f;
			    
			    pack->nstrip_z=(int)((Length/Readout_z - Dead)/
			                          pack->stripsize_z);
                            pack->nstrip_f=(int)((Width/Readout_f - Dead)/
			                          pack->stripsize_f);
			    
//                          ========================== CHAMBER POSITION <------
			    pack->half_dim[0] = Width/2.;
                            pack->half_dim[1] = Length/2.;
                            pack->half_dim[2] = nor(amdc->STATT(Itec,Ista,9))/2.;
                            
			    
			    float shift_rad = nor(GeomDz);
			    float shift_z   = nor(GeomDy);
			    float shift_s   = nor(GeomDx);
			    float orto_rad  = nor(PosiS);
			    bool  isOrtoRad = (orto_rad>15.) ? true : false;
			    float x = nor(PosiR)+ shift_rad + pack->half_dim[2];
			    float y = shift_s + orto_rad;
			    float z = nor(PosiZ)+ shift_z + pack->half_dim[1];
			    float phi = PosiPhi;
			    
			    
			    float tmp[3] = {x,y,z};
			    			
                            rotate(nor(PosiZ),nor(PosiR),orto_rad,
			           PosiAlfa,PosiBeta,PosiGamma,tmp);
						   		    
                            pack->rotation_angle = phi;
			    
			    x = tmp[0];
			    y = tmp[1];
			    z = tmp[2];
			    
			    pack->pack_coo[0] = x*cos(phi) - y*sin(phi);
                            pack->pack_coo[1] = x*sin(phi) + y*cos(phi);
                            pack->pack_coo[2] = z;						
						
						
						
                            if (name.c_str()[1] == 'O')
			    {						
                                pack->station = Third;
				if (name.c_str()[2] == 'F' ||
				    name.c_str()[2] == 'G'   )
				{
				    pack->station = (x<Rmid)? Third : Extended;   
				}
                            } else
			    {
			        pack->station = (x<Rmid)? First : Second;
				if( !exist_tech[SPA] && !exist_tech[MDT])
				pack->station = Second;
				//std::cout << "special pack=" << pack->station
				//     << std::endl;
			    }
			    
			    
//                          ========================== RPC INTERNAL STRUCTURE<-
			    float rpc_thickness= nor(amdc->STATT(Itec,Ista,4));
			    float low_honeycomb= nor(amdc->STATT(Itec,Ista,7));
			    float cen_honeycomb= nor(amdc->STATT(Itec,Ista,5));
			    float gas_volume   = nor(amdc->STATT(Itec,Ista,13));
			    float bakelite     = nor(amdc->STATT(Itec,Ista,14));
			    float gas_gap_pet  = nor(amdc->STAOO(Itec,Ista,7));
			    float gas_gap_air  = nor(amdc->STAOO(Itec,Ista,8));
			    float str_thickness= nor(amdc->STATT(Itec,Ista,15));
			    float str_support  = nor(amdc->STATT(Itec,Ista,16));
			    float str_pet_upper= nor(amdc->STAOO(Itec,Ista,9));
			    float str_pet_lower= nor(amdc->STAOO(Itec,Ista,10));
			    
//			   printf("rpc_thickness     = %f\n",rpc_thickness);
//			   printf("lower_honeycomb   = %f\n",low_honeycomb);
//			   printf("central_honeycomb = %f\n",cen_honeycomb);
//			   printf("gas_volume        = %f\n",gas_volume);
//			   printf("bakelite          = %f\n",bakelite);
//			   printf("gas_gap_pet       = %f\n",gas_gap_pet);
//			   printf("gas_gap_air       = %f\n",gas_gap_air);
//			   printf("strip_thickness   = %f\n",str_thickness);
//			   printf("strip_support     = %f\n",str_support);
//			   printf("strip_pet_upper   = %f\n",str_pet_upper);
//			   printf("strip_pet_lower   = %f\n",str_pet_lower);

                            float gas_center  = str_thickness + 
				 	        str_support*2 + 
					        str_pet_upper + 
					        str_pet_lower +
					        gas_gap_pet   +
					        gas_gap_air   +
					        bakelite      +
					        gas_volume/2. ;
						 
			    float mid_layer_1 = low_honeycomb + 
			                        gas_center; 
					       
                            float mid_layer_2 = low_honeycomb +
			                        rpc_thickness +
					        cen_honeycomb +
					        gas_center;
			    
			    float radius_lay1 = nor(PosiR) + shift_rad +
						mid_layer_1;
			    float radius_lay2 = nor(PosiR) + shift_rad +
			                        mid_layer_2;
						
                            if(amdc->ISHAPE(Jtype,Jgeo,Jobj) == -1)
			    {
			        radius_lay1 -= low_honeycomb;
				radius_lay2 -= low_honeycomb;
			    }
			    
			    pack->gas_layer_radius[0] = radius_lay1;
			    pack->gas_layer_radius[1] = radius_lay2;
			    
			    
			    // Sertup coordinate systems for radius computation
			    float Ref1[3];
			    Ref1[0] = radius_lay1;
			    Ref1[1] = shift_s + orto_rad;
			    Ref1[2] = nor(PosiZ) + shift_z + pack->half_dim[1];
			    
			    float Ref2[3];
			    Ref2[0] = radius_lay1;
			    Ref2[1] = shift_s + orto_rad;
			    Ref2[2] = nor(PosiZ) + shift_z;
			    
			    rotate(nor(PosiZ),nor(PosiR),orto_rad,
			           PosiAlfa,PosiBeta,PosiGamma,Ref1);
			    
			    rotate(nor(PosiZ),nor(PosiR),orto_rad,
			           PosiAlfa,PosiBeta,PosiGamma,Ref2);
			    
			    for (int nco=0;nco<3;++nco)
			    {
			        pack->pack_ref_coo[nco][0][0] = Ref2[nco];
			        pack->pack_ref_coo[nco][0][1] = Ref1[nco];
			    }
			    
			    float DeltaX = pack->pack_ref_coo[0][0][0] -
			                   pack->pack_ref_coo[0][0][1];
			    float DeltaY = pack->pack_ref_coo[1][0][0] -
			                   pack->pack_ref_coo[1][0][1];
			    float DeltaZ = pack->pack_ref_coo[2][0][0] -
			                   pack->pack_ref_coo[2][0][1];
			    
			    pack->pack_ref_deltas[0][0] = DeltaX/DeltaZ;
			    pack->pack_ref_deltas[1][0] = DeltaY/DeltaZ;
			    
			    
			    
			    Ref1[0] = radius_lay2;
			    Ref1[1] = shift_s + orto_rad;
			    Ref1[2] = nor(PosiZ) + shift_z + pack->half_dim[1];
			    
			    Ref2[0] = radius_lay2;
			    Ref2[1] = shift_s + orto_rad;
			    Ref2[2] = nor(PosiZ) + shift_z;
			    
			    rotate(nor(PosiZ),nor(PosiR),orto_rad,
			           PosiAlfa,PosiBeta,PosiGamma,Ref1);
			    
			    rotate(nor(PosiZ),nor(PosiR),orto_rad,
			           PosiAlfa,PosiBeta,PosiGamma,Ref2);
                            
			    for (int nco=0;nco<3;++nco)
			    {
			        pack->pack_ref_coo[nco][1][0] = Ref2[nco];
			        pack->pack_ref_coo[nco][1][1] = Ref1[nco];
			    }
			    
			    DeltaX = pack->pack_ref_coo[0][1][0] -
			             pack->pack_ref_coo[0][1][1];
			    DeltaY = pack->pack_ref_coo[1][1][0] -
			             pack->pack_ref_coo[1][1][1];
			    DeltaZ = pack->pack_ref_coo[2][1][0] -
			             pack->pack_ref_coo[2][1][1];
			    
			    pack->pack_ref_deltas[0][1] = DeltaX/DeltaZ;
			    pack->pack_ref_deltas[1][1] = DeltaY/DeltaZ;
			    
			    float Zp = nor(PosiZ);
			    float Rp = nor(PosiR);
			    float Sp = orto_rad;
			    float Aa = PosiAlfa;
			    float Ba = PosiBeta;
			    float Ga = PosiGamma;

			    if((int)Readout_f == 2)
			    {
			        for(int i=0;i<2;++i) for(int j=0;j<2;++j)
				{
				    float sy = (i)? pack->half_dim[0]/2. :
				                    -pack->half_dim[0]/2.;
				    float sx = pack->gas_layer_radius[j];
				    float z  = nor(PosiZ)+ shift_z + 
					       pack->half_dim[1];
					       
				    float Ce[3] = {sx,sy,z};
				    rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
				    float X = Ce[0]*cos(phi) - Ce[1]*sin(phi);
				    float Y = Ce[0]*sin(phi) + Ce[1]*cos(phi);
                                    pack->gas_layer_XY[i][j][0] = X;
                                    pack->gas_layer_XY[i][j][1] = Y;
				}
			    } else
			    {
			        //std::cout << "Jtype=" << Jtype << ", Jff="
				//          << Jff << ", Jzz=" << Jzz << ", Jobj="
				//	  << Jobj << ",  shift_s=" << shift_s
				//	  << std::endl;
                                if (Jtype!=4 || Jff==6 || Jff==7 || //Jobj<=16 ||
				    !(Jzz==-2 || Jzz==-4 || Jzz==2 || Jzz==4))
				{
				    std::cout << "RPCGeometry WARNING:"
				              << " ambiguities in data decoding"
					      << std::endl;
				}
				if(Jobj <= s_JobMinForEncodeProj)
				    s_JobMinForEncodeProj = Jobj; 
				encodeProj* tmp = new (encodeProj);
				tmp->Jobj = Jobj;
				tmp->proj = (shift_s<=0)? 1 : 2;
				tmp->next = 0;
				add_proj(tmp);
				
				//warning: do not trust orto_rad !!!!!!
			        int i = (shift_s<0||(orto_rad<0 && isOrtoRad))? 0:1;
				for(int j=0;j<2;++j)
				{
				    float sy = shift_s + orto_rad;
				    float sx = pack->gas_layer_radius[j];
				    float z  = nor(PosiZ)+ shift_z + 
					       pack->half_dim[1];
				    
				    
				    float Ce[3] = {sx,sy,z};
				    rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
				    float X = Ce[0]*cos(phi) - Ce[1]*sin(phi);
				    float Y = Ce[0]*sin(phi) + Ce[1]*cos(phi);
                                    pack->gas_layer_XY[i][j][0] = X;
                                    pack->gas_layer_XY[i][j][1] = Y;
				}
			    }
//                          ========================== SETUP OF THE Z STRIPS<--
                            float *str_z = new float [pack->nstrip_z];
			    
			    float sign_z  = (Jzz)? (Jzz)/abs(Jzz) : +1;
			    
			    float start_position = (sign_z > 0)?
			                           nor(PosiZ) + shift_z:
			                           nor(PosiZ) + shift_z+ Length;
	   
                            float offset = (pack->half_dim[1]*2 - 
			          pack->stripsize_z * pack->nstrip_z)/2.;
                            float start_z = start_position + (offset +
                                  pack->stripsize_z/2.)*sign_z; 

			    //float start_z = start_position + Dead*sign_z + 
			    //	    sign_z*pack->stripsize_z/2.;

                            for (int i=0;i<pack->nstrip_z;++i)
			    {
			      //  float x = pack->gas_layer_radius[0];
			      // float y = shift_s + orto_rad;
			        float z = start_z + i*pack->stripsize_z*sign_z;
				float Ce[3] = {pack->gas_layer_radius[0],0,z};
				rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
				
				double xyz1[3];
				double xyz2[3];
				
				this->get_xyz(name, Jff, Jzz, Jobj, 1, 1, 
				                     2, i+1, xyz1, xyz2);
				
				double ZETA = nor ( (xyz1[2]+xyz2[2])/2. );
				double Mx   = nor ( (xyz1[0]+xyz2[0])/2. );
				double My   = nor ( (xyz1[1]+xyz2[1])/2. );
				
				// returned by the coordinate methods
				Mx = pack->gas_layer_XY[1][0][0];
				My = pack->gas_layer_XY[1][0][1];
				
				if(s_useOldGeoAccess) {
				    *(str_z+i) = Ce[2];
				} else {
				    *(str_z+i) = ZETA;		     
				}
				#ifndef LVL1_STANDALONE
				if (s_hist_alignCorrOnZ) 
	                            s_hist_alignCorrOnZ->Fill(Ce[2]-ZETA);
                                if (s_hist_alignCorrOnMeanX)
	                            s_hist_alignCorrOnMeanX->Fill(Ce[0]-Mx);
				if (s_hist_alignCorrOnMeanY)
	                            s_hist_alignCorrOnMeanY->Fill(Ce[1]-My);
	                        
				#else
				std::cout << "old Z=" << Ce[2]
				          << ",  X1=" << xyz1[0]
					  << ",  Y1=" << xyz1[1]
					  << ",  Z1=" << xyz1[2]
					  << " ;  X2=" << xyz2[0]
					  << ",  Y2=" << xyz2[1]
					  << ",  Z2=" << xyz2[2]
					  << " : Z=" << nor((xyz1[2]+xyz2[2])/2.)
					  << std::endl;
				
				std::cout << "old X=" << Ce[0]
				          << ",  new X=" << Mx
					  << std::endl;

                                std::cout << "old Y=" << Ce[1]
				          << ",  new Y=" << My
					  << std::endl;
				#endif
			    }
			    
			    pack->z_proj = str_z;

//                          ========================== SETUP OF THE F STRIPS<--
                            //-06-05-2005:
			    //modified for reversing the order of Phi strip
			    //numbering.
			    
			    if ((int)Readout_f == 2) 
			    {
			        float* str_f1 = new float [4*pack->nstrip_f];
				float* str_f2 = new float [4*pack->nstrip_f];
                      
                                float offset = (pack->half_dim[0] - 
			          pack->stripsize_f * pack->nstrip_f)/2.;
                              
			        //-06-05-2005 
			        //float start_y = pack->half_dim[0] - offset  + 
 			        //                  shift_s + orto_rad -
                                //                  pack->stripsize_f/2.;
				float start_y = offset + shift_s + orto_rad +
                                                pack->stripsize_f/2.;	

				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            {
				        //-06-05-2005
				        //float sy = start_y-i*pack->stripsize_f;
				        float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float z  = nor(PosiZ)+ shift_z + 
					           pack->half_dim[1];
						   
					float Ce[3] = {sx,sy,z};
					rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
					float X = Ce[0]*cos(phi) - Ce[1]*sin(phi);
					float Y = Ce[0]*sin(phi) + Ce[1]*cos(phi);
					
					double xyz1[3];
				        double xyz2[3];
				
				        this->get_xyz(name, Jff, Jzz, Jobj, 2,
					              r+1, 1, i+1, xyz1, xyz2);
				        
					double Xx = nor ((xyz1[0]+xyz2[0])/2.);
				        double Yy = nor ((xyz1[1]+xyz2[1])/2.);
					
					if (s_useOldGeoAccess) {
					    *(str_f1+i*4+r*2)   = X;
				            *(str_f1+i*4+r*2+1) = Y;
					} else {
					    *(str_f1+i*4+r*2)   = Xx;
				            *(str_f1+i*4+r*2+1) = Yy;
					}
					#ifndef LVL1_STANDALONE
					double Mz = nor((xyz1[2]+xyz2[2])/2.);
				        if (s_hist_alignCorrOnX) 
	                                    s_hist_alignCorrOnX->Fill(X-Xx);
                                        if (s_hist_alignCorrOnY)
	                                    s_hist_alignCorrOnY->Fill(Y-Yy);
				        if (s_hist_alignCorrOnMeanZ)
	                                    s_hist_alignCorrOnMeanZ->Fill(pack->pack_coo[2]-Mz);
				        #else
				        std::cout << "old:  X=" << X
				                  << ", Y=" << Y
					          << ",   new: X=" << Xx
					          << ", Y=" << Yy
					          << std::endl;
			                #endif
				    }
				
				
                                //-06-05-2005
				//start_y = - offset + shift_s + orto_rad -
				//            pack->stripsize_f/2.;
					    
			        start_y = - pack->half_dim[0] + offset 
				          + shift_s + orto_rad +
				            pack->stripsize_f/2.;				
				
				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            {
				        //-06-05-2005
				        //float sy = start_y-i*pack->stripsize_f;
				        float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float z  = nor(PosiZ)+ shift_z + 
					           pack->half_dim[1];
					
					float Ce[3] = {sx,sy,z};
					rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
					float X = Ce[0]*cos(phi) - Ce[1]*sin(phi);
					float Y = Ce[0]*sin(phi) + Ce[1]*cos(phi);
					
					double xyz1[3];
				        double xyz2[3];
				
				        this->get_xyz(name, Jff, Jzz, Jobj, 1,
					              r+1, 1, i+1, xyz1, xyz2);
                                        
					double Xx = nor ( (xyz1[0]+xyz2[0])/2. );
				        double Yy = nor ( (xyz1[1]+xyz2[1])/2. );
					
					if (s_useOldGeoAccess) {
					    *(str_f2+i*4+r*2)   = X;
				            *(str_f2+i*4+r*2+1) = Y;
					} else {
					    *(str_f2+i*4+r*2)   = Xx;
				            *(str_f2+i*4+r*2+1) = Yy;
					}
					#ifndef LVL1_STANDALONE
					double Mz = nor((xyz1[2]+xyz2[2])/2.);
				        if (s_hist_alignCorrOnX) 
	                                    s_hist_alignCorrOnX->Fill(X-Xx);
                                        if (s_hist_alignCorrOnY)
	                                    s_hist_alignCorrOnY->Fill(Y-Yy);
				        if (s_hist_alignCorrOnMeanZ)
	                                    s_hist_alignCorrOnMeanZ->Fill(pack->pack_coo[2]-Mz);
				        #else
				        std::cout << "old:  X=" << X
				                  << ", Y=" << Y
					          << ",   new: X=" << Xx
					          << ", Y=" << Yy
					          << std::endl;
			                #endif
				    }

				pack->f_proj[1] = str_f1;
				pack->f_proj[0] = str_f2;
			    
                            } else
			    {
			        float* str_f1 = new float [4*pack->nstrip_f];
			        
				//-06-05-2005
			        //float start_y = pack->half_dim[0] - Dead +
				//                shift_s + orto_rad -
				//		pack->stripsize_f/2.;
				float start_y = -pack->half_dim[0] + Dead +
				                shift_s + orto_rad +
						pack->stripsize_f/2.;
				
				if(shift_s!=0&&(orto_rad!=0 && isOrtoRad)) 
				{
				    printf("RPCGeometry: inconsistence!\n");
				    printf("    shift_s=%f,  orto_rad=%f\n",
				           shift_s,orto_rad);
				    exit(1);
				}
				
				int Jspli = 1;
				
				if(shift_s<0||(orto_rad<0 && isOrtoRad)) 
				{
				    Jspli = 1;
				} else
				{
				    Jspli = 2;
				}
				
				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            {
				        //-06-05-2005
				        //float sy = start_y-i*pack->stripsize_f;
				        float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float z  = nor(PosiZ)+ shift_z + 
					           pack->half_dim[1];
					
					float Ce[3] = {sx,sy,z};
					rotate(Zp,Rp,Sp,Aa,Ba,Ga,Ce);
					float X = Ce[0]*cos(phi) - Ce[1]*sin(phi);
					float Y = Ce[0]*sin(phi) + Ce[1]*cos(phi);
					
					double xyz1[3];
				        double xyz2[3];
				
				        this->get_xyz(name, Jff, Jzz, Jobj, Jspli,
					              r+1, 1, i+1, xyz1, xyz2);
				        
					double Xx = nor ( (xyz1[0]+xyz2[0])/2. );
				        double Yy = nor ( (xyz1[1]+xyz2[1])/2. );
					
					if(s_useOldGeoAccess) {
					    *(str_f1+i*4+r*2)   = X;
				            *(str_f1+i*4+r*2+1) = Y;
					} else {
					    *(str_f1+i*4+r*2)   = Xx;
				            *(str_f1+i*4+r*2+1) = Yy;
					}
					#ifndef LVL1_STANDALONE
					double Mz = nor((xyz1[2]+xyz2[2])/2.);
				        if (s_hist_alignCorrOnX) 
	                                    s_hist_alignCorrOnX->Fill(X-Xx);
                                        if (s_hist_alignCorrOnY)
	                                    s_hist_alignCorrOnY->Fill(Y-Yy);
				        if (s_hist_alignCorrOnMeanZ)
	                                   
                                        s_hist_alignCorrOnMeanZ->Fill(pack->pack_coo[2]-Mz);
				        #else
				        std::cout << "old:  X=" << X
				                  << ", Y=" << Y
					          << ",   new: X=" << Xx
					          << ", Y=" << Yy
					          << std::endl;
			                #endif
				    }
				/*
				if(shift_s!=0&&(orto_rad!=0 && isOrtoRad)) 
				{
				    printf("RPCGeometry: inconsistence!\n");
				    printf("    shift_s=%f,  orto_rad=%f\n",
				           shift_s,orto_rad);
				    exit(1);
				}
				*/
				if(shift_s<0||(orto_rad<0 && isOrtoRad)) 
				{
				    pack->f_proj[1] = 0;
				    pack->f_proj[0] = str_f1;
				} else
				{
				    pack->f_proj[1] = str_f1;
				    pack->f_proj[0] = 0;
				}
				
				
			    }
			    
			    
			    //tmp[0] = pack->gas_layer_radius[0];
			    //tmp[1] = shift_s + orto_rad;
			    //tmp[2] = nor(PosiZ)+ shift_z + pack->half_dim[1]; 
			    //rotate(Zp,Rp,Sp,Aa,Ba,Ga,tmp);
			    //pack->gas_layer_radius[0] = tmp[0];
			    
                            //tmp[0] = pack->gas_layer_radius[1];
			    //tmp[1] = shift_s + orto_rad;
			    //tmp[2] = nor(PosiZ)+ shift_z + pack->half_dim[1]; 
			    //rotate(Zp,Rp,Sp,Aa,Ba,Ga,tmp);
			    //pack->gas_layer_radius[1] = tmp[0];
			    
			    
			    ++(cham->nRPC_packs);
			    if (!cham->RPC_packs)
			    {
			        cham->RPC_packs = pack;
			    } else
			    {
			        last_pack->next = pack;
			    }
			    last_pack = pack;

               /*
                printf("--------------------------------------------------\n");
                printf("MultiChamber type %s: Jtype=%d, Jff=%d, Jzz=%d\n",
			           name.c_str(),Jtype,Jff,Jzz);
		printf("MultiChamber position: Z=%9.2f,  R=%9.2f,  PHI=%9.2f\n",

		                   amdc->PosZ(Jtype,Jff,Jzz),
				   amdc->PosR(Jtype,Jff,Jzz),
				   amdc->PosPhi(Jtype,Jff,Jzz));
		printf("MultiChamber position: Rmid=%9.2f\n",cham->Rmid);
          printf("Rpc object: Jobj=%d, type=%d, strip_z=%9.2f, strip_f=%9.2f\n",
	                         Jobj,Ista,pack->stripsize_z,pack->stripsize_f);
                printf("RPC station: trigger station = %d,  Readout_z = %f",
		                 pack->station,Readout_z);
		printf(",  Readout_f = %f\n",Readout_f);
		printf("RPC station: nstrip_z = %d,  nstrip__f = %d\n",
		                 pack->nstrip_z,pack->nstrip_f);
		printf("RPC station: chamber center=> x=%f, y=%f, z=%f\n",
		        pack->pack_coo[0],pack->pack_coo[1],pack->pack_coo[2]);
                printf("RPC shift position: dx=%9.2f,  dy=%9.2f, dz=%9.2f\n",
			           amdc->Geodx(Jtype,Jgeo,Jobj),
				   amdc->Geody(Jtype,Jgeo,Jobj),
				   amdc->Geodz(Jtype,Jgeo,Jobj));
                printf("RPC chamber dimension: width=%9.2f,  length=%9.2f\n",
			           amdc->GeoWs(Jtype,Jgeo,Jobj),
				   amdc->GeoLe(Jtype,Jgeo,Jobj));
                printf("RPC dead spaces: D1=%9.2f,  D2=%9.2f, D3=%9.2f\n",
			           amdc->GeoD1(Jtype,Jgeo,Jobj),
				   amdc->GeoD2(Jtype,Jgeo,Jobj),
				   amdc->GeoD3(Jtype,Jgeo,Jobj));
                printf("RPC radius: radius1=%9.2f,  radius2=%9.2f\n",
			           pack->gas_layer_radius[0],
				   pack->gas_layer_radius[1]);
                printf("Pippo\n"); */			
			
	            }
	      
	          }      
                }  // end loop over stations objects

              }
	    }  // end loop over all stations
	    
	    
          }
	}
	
	
/*	
	//printf ("Name=%s, Mzz=%d \n",name.c_str(),MJzz);
        for (int Jff=1;Jff<=Mff;++Jff)
	{
	    for (int Jzz=-MJzz;Jzz<=MJzz;++Jzz)
	    {	        
	        if (int Jgeo = amdc->IGEO(Jtype,Jff,Jzz))
		{
		    MultiChamber*  cham = 0;
		    RPC_pack* last_pack = 0;
		    int MJobj = amdc->NOBJ(Jtype,Jgeo);
		    //int MJobj = amdc->GetNumberOfStationObjects(name,Jgeo);

//                  ================ COMPUTING THE R-MIDDLE OF THE MULTICHAMBER
		    float Rmid = 0;
		    
		    enum tech {SPA,MDT,RPC};
		    int exist_tech[3] = {0,0,0};
		    
		    for (int Jobj=1;Jobj<=MJobj;++Jobj)
		    {
		        int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			std::string tech_name = amdc->TechnologyName(Itec);
			if (tech_name=="SPA") exist_tech[SPA] = Jobj;
			if (tech_name=="MDT") exist_tech[MDT] = Jobj;
			if (tech_name=="RPC") exist_tech[RPC] = Jobj;
                    }
		    
		    if (exist_tech[SPA])
		    {
		        int Ista = amdc->ISTA(Jtype,Jgeo,exist_tech[SPA]);
			int Itec = amdc->ITEC(Jtype,Jgeo,exist_tech[SPA]);
			Rmid = amdc->PosR(Jtype,Jff,Jzz)/1.                +
			       amdc->Geodz(Jtype,Jgeo,exist_tech[SPA])/1.  +
			       amdc->STAEE(Itec,Ista)/2.;
		    } else if (exist_tech[MDT])
		    {
		        int Ista = amdc->ISTA(Jtype,Jgeo,exist_tech[MDT]);
			int Itec = amdc->ITEC(Jtype,Jgeo,exist_tech[MDT]);
			Rmid = amdc->PosR(Jtype,Jff,Jzz)/1.                +
			       amdc->Geodz(Jtype,Jgeo,exist_tech[MDT])/1.  +
			       amdc->STAEE(Itec,Ista)/2.;
		    } else if (exist_tech[RPC])
		    {
		        int Ista = amdc->ISTA(Jtype,Jgeo,exist_tech[RPC]);
			int Itec = amdc->ITEC(Jtype,Jgeo,exist_tech[RPC]);
			Rmid = amdc->PosR(Jtype,Jff,Jzz)/1.                +
			       amdc->Geodz(Jtype,Jgeo,exist_tech[RPC])/1.  +
			       amdc->STATT(Itec,Ista,9)/2.;
		    }
		    		    
		    if (!Rmid) 
		    {
		        printf("RPCGeometry: error in computing Rmid!\n");
			exit(1);
		    }
		    
//                  ===================== COMPUTING CHAMBER STRUCTURE <--------
		    for (int Jobj=1;Jobj<=MJobj;++Jobj)
		    {
		        int Itec = amdc->ITEC(Jtype,Jgeo,Jobj);
			if (amdc->TechnologyName(Itec)=="RPC")
			{
			    int Ista = amdc->ISTA(Jtype,Jgeo,Jobj);
			    
//                          ==================================== CHAM INIT <---
			    if (!cham)
			    {
			        cham = new (MultiChamber);
				cham->Rmid = Rmid;
				cham->name = name;
				cham->type = Jgeo;
				cham->amdb_z_index = Jzz;
				cham->physics_sector =(name.c_str()[2] == 'L')?
                                                      (Jff-1)*2 : (Jff-1)*2+1;
				cham->nRPC_packs = 0;
				cham->RPC_packs  = 0;
				cham->next = 0;
				for(int i=0;i<2;++i)for(int j=First;j<=Third;++j)
				    cham->firstRPC_z[i][j] = 0;
				T1muonGeo[Jtype-1][(Jff-1)*(2*Mzz+1)+Jzz+Mzz] =
				cham;
			    }
			    
			    RPC_pack* pack = new (RPC_pack);
			    pack->Jobj        = Jobj;
			    pack->Readout_f   = 0;
			    pack->cham        = cham;
			    pack->next = 0;
			    pack->z_proj = 0;
			    pack->lvl1_z_index = 0;
			    for(int i=0;i<2;++i)
			    {
			        pack->f_proj[i] = 0;
				pack->next_in_z[i] = 0;
				pack->prev_in_z[i] = 0;
				for(int j=0;j<2;++j) for(int k=0;k<2;++k)
				    pack->gas_layer_XY[i][j][k] = 0.;
			    }
			    
//                          ======================== STRIP SIZE AND NUMBER <---
			    pack->stripsize_f = amdc->STATT(Itec,Ista,10)/1.;
			    pack->stripsize_z = amdc->STATT(Itec,Ista,11)/1.;
			    
			    float Readout_f = amdc->STAOO(Itec,Ista,2);
                            // Change to account Anna's whishes!
			    // float Readout_z = amdc->STAOO(Itec,Ista,3);
			    float Readout_z = 1.;
			    float Length    = amdc->GeoLe(Jtype,Jgeo,Jobj)/1.;
			    float Width     = amdc->GeoWs(Jtype,Jgeo,Jobj)/1.;
			    float Dead      = amdc->GeoD1(Jtype,Jgeo,Jobj)/1.;
			    
			    pack->Readout_f = (int) Readout_f;
			    
			    pack->nstrip_z=(int)((Length/Readout_z - Dead)/
			                          pack->stripsize_z);
                            pack->nstrip_f=(int)((Width/Readout_f - Dead)/
			                          pack->stripsize_f);
			    
//                          ========================== CHAMBER POSITION <------
			    pack->half_dim[0] = Width/2.;
                            pack->half_dim[1] = Length/2.;
                            pack->half_dim[2] = amdc->STATT(Itec,Ista,9)/2.;
                            
			    
			    float shift_rad = amdc->Geodz(Jtype,Jgeo,Jobj)/1.;
			    float shift_z   = amdc->Geody(Jtype,Jgeo,Jobj)/1.;
			    float shift_s   = amdc->Geodx(Jtype,Jgeo,Jobj)/1.;
			    float orto_rad  = amdc->PosS(Jtype,Jff,Jzz)/1.;
			    float x = amdc->PosR(Jtype,Jff,Jzz)/1.+ shift_rad+
			              pack->half_dim[2];
			    float y = shift_s + orto_rad;
			    float phi = amdc->PosPhi(Jtype,Jff,Jzz);
			    			   		    
                            pack->rotation_angle = phi;
			    
			    pack->pack_coo[0] = x*cos(phi) - y*sin(phi);
                            pack->pack_coo[1] = x*sin(phi) + y*cos(phi);
                            pack->pack_coo[2] = amdc->PosZ (Jtype,Jff,Jzz)/1.+
			                        shift_z + pack->half_dim[1];
						
                            if (name.c_str()[1] == 'O')
			    {						
                                pack->station = Third;
                            } else
			    {
			        pack->station = (x<Rmid)? First : Second;
			    }
			    
			    
//                          ========================== RPC INTERNAL STRUCTURE<-
			    float rpc_thickness= amdc->STATT(Itec,Ista,4)/1.;
			    float low_honeycomb= amdc->STATT(Itec,Ista,7)/1.;
			    float cen_honeycomb= amdc->STATT(Itec,Ista,5)/1.;
			    float gas_volume   = amdc->STATT(Itec,Ista,13)/1.;
			    float bakelite     = amdc->STATT(Itec,Ista,14)/1.;
			    float gas_gap_pet  = amdc->STAOO(Itec,Ista,7)/1.;
			    float gas_gap_air  = amdc->STAOO(Itec,Ista,8)/1.;
			    float str_thickness= amdc->STATT(Itec,Ista,15)/1.;
			    float str_support  = amdc->STATT(Itec,Ista,16)/1.;
			    float str_pet_upper= amdc->STAOO(Itec,Ista,9)/1.;
			    float str_pet_lower= amdc->STAOO(Itec,Ista,10)/1.;
			    
//			   printf("rpc_thickness     = %f\n",rpc_thickness);
//			   printf("lower_honeycomb   = %f\n",low_honeycomb);
//			   printf("central_honeycomb = %f\n",cen_honeycomb);
//			   printf("gas_volume        = %f\n",gas_volume);
//			   printf("bakelite          = %f\n",bakelite);
//			   printf("gas_gap_pet       = %f\n",gas_gap_pet);
//			   printf("gas_gap_air       = %f\n",gas_gap_air);
//			   printf("strip_thickness   = %f\n",str_thickness);
//			   printf("strip_support     = %f\n",str_support);
//			   printf("strip_pet_upper   = %f\n",str_pet_upper);
//			   printf("strip_pet_lower   = %f\n",str_pet_lower);

                            float gas_center  = str_thickness + 
				 	        str_support*2 + 
					        str_pet_upper + 
					        str_pet_lower +
					        gas_gap_pet   +
					        gas_gap_air   +
					        bakelite      +
					        gas_volume/2. ;
						 
			    float mid_layer_1 = low_honeycomb + 
			                        gas_center; 
					       
                            float mid_layer_2 = low_honeycomb +
			                        rpc_thickness +
					        cen_honeycomb +
					        gas_center;

                            float radius_lay1 = amdc->PosR(Jtype,Jff,Jzz)/1. +
			                        shift_rad +
						mid_layer_1;
			    float radius_lay2 = amdc->PosR(Jtype,Jff,Jzz)/1. +
			                        shift_rad +
			                        mid_layer_2;
						
                            if(amdc->ISHAPE(Jtype,Jgeo,Jobj) == -1)
			    {
			        radius_lay1 -= low_honeycomb;
				radius_lay2 -= low_honeycomb;
			    }
			    
			    pack->gas_layer_radius[0] = radius_lay1;
			    pack->gas_layer_radius[1] = radius_lay2;
			    
			    if((int)Readout_f == 2)
			    {
			        for(int i=0;i<2;++i) for(int j=0;j<2;++j)
				{
				    float sy = (i)? pack->half_dim[0]/2. :
				                    -pack->half_dim[0]/2.;
				    float sx = pack->gas_layer_radius[j];
				    
				    float X = sx*cos(phi) - sy*sin(phi);
				    float Y = sx*sin(phi) + sy*cos(phi);
                                    pack->gas_layer_XY[i][j][0] = X;
                                    pack->gas_layer_XY[i][j][1] = Y;
				}
			    } else
			    {
			        int i = (shift_s<0||orto_rad<0)? 0:1;
				for(int j=0;j<2;++j)
				{
				    float sy = shift_s + orto_rad;
				    float sx = pack->gas_layer_radius[j];
				    
				    float X = sx*cos(phi) - sy*sin(phi);
				    float Y = sx*sin(phi) + sy*cos(phi);
                                    pack->gas_layer_XY[i][j][0] = X;
                                    pack->gas_layer_XY[i][j][1] = Y;
				}
			    }
//                          ========================== SETUP OF THE Z STRIPS<--
                            float *str_z = new float [pack->nstrip_z];
			    
			    float sign_z  = (Jzz)? (Jzz)/abs(Jzz) : +1;
			    
			    float start_position = (sign_z > 0)?
			       amdc->PosZ(Jtype,Jff,Jzz)/10. + shift_z:
			       amdc->PosZ(Jtype,Jff,Jzz)/10. + shift_z+ Length;
	   
                            float offset = (pack->half_dim[1]*2 - 
			          pack->stripsize_z * pack->nstrip_z)/2.;
                            float start_z = start_position + (offset +
                                  pack->stripsize_z/2.)*sign_z; 

			    //float start_z = start_position + Dead*sign_z + 
			    //	    sign_z*pack->stripsize_z/2.;

                            for (int i=0;i<pack->nstrip_z;++i)
			    {
				*(str_z+i) = start_z +
				                 i*pack->stripsize_z*sign_z;
			    }
			    
			    pack->z_proj = str_z;

//                          ========================== SETUP OF THE F STRIPS<--
                            //-10-12-2003:
			    //modified for reversing the order of Phi strip
			    //numbering.
			    
			    if ((int)Readout_f == 2) 
			    {
			        float* str_f1 = new float [4*pack->nstrip_f];
				float* str_f2 = new float [4*pack->nstrip_f];
                      
                                float offset = (pack->half_dim[0] - 
			          pack->stripsize_f * pack->nstrip_f)/2.;
				  
			      //-10-12-2003 
			      //float start_y = pack->half_dim[0] - offset  + 
 			      //                  shift_s + orto_rad -
                              //                  pack->stripsize_f/2.;
				float start_y = offset + shift_s + orto_rad +
                                                pack->stripsize_f/2.;		
			      //float start_y = pack->half_dim[0] - Dead  + 
			      //                  shift_s + orto_rad -
			      //                  pack->stripsize_f/2.;

				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            {
				      //-10-12-2003
				      //float sy = start_y-i*pack->stripsize_f;
					float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float X = sx*cos(phi) - sy*sin(phi);
					float Y = sx*sin(phi) + sy*cos(phi);
			                *(str_f1+i*4+r*2)   = X;
				        *(str_f1+i*4+r*2+1) = Y;
			            }
				
				//-10-12-2003
				//start_y = - offset + shift_s + orto_rad -
				//            pack->stripsize_f/2.;
					    
			        start_y = - pack->half_dim[0] + offset 
				          + shift_s + orto_rad +
				            pack->stripsize_f/2.;				
				//start_y = - Dead + shift_s + orto_rad -
				//            pack->stripsize_f/2.;
				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            {
				      //-10-12-2003
				      //float sy = start_y-i*pack->stripsize_f;
				        float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float X = sx*cos(phi) - sy*sin(phi);
					float Y = sx*sin(phi) + sy*cos(phi);
			                *(str_f2+i*4+r*2)   = X;
				        *(str_f2+i*4+r*2+1) = Y;
			            }
				
				pack->f_proj[1] = str_f1;
				pack->f_proj[0] = str_f2;
			    
                            } else
			    {
			        float* str_f1 = new float [4*pack->nstrip_f];
			        
				//-10-12-2003
			        //float start_y = pack->half_dim[0] - Dead +
				//                shift_s + orto_rad -
				//		pack->stripsize_f/2.;
				float start_y = -pack->half_dim[0] + Dead +
				                shift_s + orto_rad +
						pack->stripsize_f/2.;
								
				for (int i=0;i<pack->nstrip_f;++i)
				    for(int r=0;r<2;++r)
			            { 
				      //-10-12-2003
				      //float sy = start_y-i*pack->stripsize_f;
				        float sy = start_y+i*pack->stripsize_f;
				        float sx = pack->gas_layer_radius[r];
					float X = sx*cos(phi) - sy*sin(phi);
					float Y = sx*sin(phi) + sy*cos(phi);
			                *(str_f1+i*4+r*2)   = X;
				        *(str_f1+i*4+r*2+1) = Y;
			            }
				
				if(shift_s!=0&&orto_rad!=0) 
				{
				    printf("RPCGeometry: inconsistence!\n");
				    printf("    shift_s=%f,  orto_rad=%f\n",
				           shift_s,orto_rad);
				    exit(1);
				}
				
				if(shift_s<0||orto_rad<0) 
				{
				    pack->f_proj[1] = 0;
				    pack->f_proj[0] = str_f1;
				} else
				{
				    pack->f_proj[1] = str_f1;
				    pack->f_proj[0] = 0;
				}
			    }

			    ++(cham->nRPC_packs);
			    if (!cham->RPC_packs)
			    {
			        cham->RPC_packs = pack;
			    } else
			    {
			        last_pack->next = pack;
			    }
			    last_pack = pack;

               
                printf("--------------------------------------------------\n");
                printf("MultiChamber type %s: Jtype=%d, Jff=%d, Jzz=%d\n",
			           name.c_str(),Jtype,Jff,Jzz);
		printf("MultiChamber position: Z=%9.2f,  R=%9.2f,  PHI=%9.2f\n",

		                   amdc->PosZ(Jtype,Jff,Jzz),
				   amdc->PosR(Jtype,Jff,Jzz),
				   amdc->PosPhi(Jtype,Jff,Jzz));
		printf("MultiChamber position: Rmid=%9.2f\n",cham->Rmid);
          printf("Rpc object: Jobj=%d, type=%d, strip_z=%9.2f, strip_f=%9.2f\n",
	                         Jobj,Ista,pack->stripsize_z,pack->stripsize_f);
                printf("RPC station: trigger station = %d,  Readout_z = %f",
		                 pack->station,Readout_z);
		printf(",  Readout_f = %f\n",Readout_f);
		printf("RPC station: nstrip_z = %d,  nstrip__f = %d\n",
		                 pack->nstrip_z,pack->nstrip_f);
		printf("RPC station: chamber center=> x=%f, y=%f, z=%f\n",
		        pack->pack_coo[0],pack->pack_coo[1],pack->pack_coo[2]);
                printf("RPC shift position: dx=%9.2f,  dy=%9.2f, dz=%9.2f\n",
			           amdc->Geodx(Jtype,Jgeo,Jobj),
				   amdc->Geody(Jtype,Jgeo,Jobj),
				   amdc->Geodz(Jtype,Jgeo,Jobj));
                printf("RPC chamber dimension: width=%9.2f,  length=%9.2f\n",
			           amdc->GeoWs(Jtype,Jgeo,Jobj),
				   amdc->GeoLe(Jtype,Jgeo,Jobj));
                printf("RPC dead spaces: D1=%9.2f,  D2=%9.2f, D3=%9.2f\n",
			           amdc->GeoD1(Jtype,Jgeo,Jobj),
				   amdc->GeoD2(Jtype,Jgeo,Jobj),
				   amdc->GeoD3(Jtype,Jgeo,Jobj));
                printf("RPC radius: radius1=%9.2f,  radius2=%9.2f\n",
			           pack->gas_layer_radius[0],
				   pack->gas_layer_radius[1]);
                printf("Pippo\n"); 
			}
		    }
		    
		    
		}
	    }
	}
*/	
	
    }
}

//#else

void
RPCGeometry::rotate(float ZposCha, float RposCha, float OrtoRadp,
                    float Alfa, float Beta, float Gamma,
		    float xyz[3]) const
{
    
    Alfa  = -Alfa*s_pi/180;
    Beta  = -Beta*s_pi/180;
    Gamma = -Gamma*s_pi/180;
    
    float t = xyz[0] - RposCha;
    float s = xyz[1] - OrtoRadp;
    float z = xyz[2] - ZposCha;
    
    //implement rotation around t axis
    s =  s*cos(Alfa) + z*sin(Alfa); 
    z = -s*sin(Alfa) + z*cos(Alfa); 
    
    //implement rotation around z axis
    t =  t*cos(Beta) + s*sin(Beta); 
    s = -t*sin(Beta) + s*cos(Beta); 
    
    //implement rotation around s axis
    z =  z*cos(Gamma) + t*sin(Gamma); 
    t = -z*sin(Gamma) + t*cos(Gamma); 
    
    xyz[0] = t + RposCha;
    xyz[1] = s + OrtoRadp;
    xyz[2] = z + ZposCha; 
    
}

void
RPCGeometry::readgeo() const
{
  /*
    ifstream t2mugeo;
    Header header;
    
    const Lvl1Configuration* config = Lvl1Configuration::instance();
    
    t2mugeo.open(config->geometry_file().c_str(),ios::in | ios::binary);
    
    if(!t2mugeo) {
      cout << "Cannot open file t1mugeo in the working directory" << endl;
      exit (1);
    }
        
    while (t2mugeo.peek()!=EOF) {           // start looping on all geom data.
      t2mugeo.read((unsigned char *) &header, sizeof header);
      t2mugeo.read((unsigned char *) &incha, sizeof incha);
      t2mubgeo(&header.JJ,&header.JFF,&header.JZZ);
    }

    t2mugeo.close();                       // close the geometry data file.
  */
}

//#endif

void
RPCGeometry::get_xyz(std::string StationName, int Jff, int Jzz, int Job,
                     int Jspli, int Jsl, int Jsz, int Jstri,
		     double* xyz1, double* xyz2) const
{
    int StationEta;
    int StationPhi;
    int DoubletR;
    int DoubletZ;
    int DoubletP;
    int GasGap;
    int MeasuresPhi;
    int Strip;
    
    AMDC;
    
    amdc->GetRPCindexFromAMDC(StationName,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
                                  StationEta,StationPhi,DoubletR,DoubletZ,
                                  DoubletP,GasGap,MeasuresPhi,Strip);

    double XYZ1[3];
    double XYZ2[3];
    double XYZ3[3];
    double XYZ4[3];
    double SZT1[3];
    double SZT2[3];
    double SZT3[3];
    double SZT4[3];
    				  
    amdc->GetStripPositionFromRPCindex( StationName, StationEta, StationPhi,
                DoubletR, DoubletZ, DoubletP, GasGap, MeasuresPhi, Strip,
                XYZ1,  XYZ2,  XYZ3,  XYZ4,  SZT1,  SZT2,  SZT3,  SZT4 );

    xyz1[0] = (XYZ1[0] + XYZ2[0]) / 2.;
    xyz1[1] = (XYZ1[1] + XYZ2[1]) / 2.;
    xyz1[2] = (XYZ1[2] + XYZ2[2]) / 2.;

    xyz2[0] = (XYZ3[0] + XYZ4[0]) / 2.;
    xyz2[1] = (XYZ3[1] + XYZ4[1]) / 2.;
    xyz2[2] = (XYZ3[2] + XYZ4[2]) / 2.;
}
