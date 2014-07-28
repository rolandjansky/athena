/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/RoadsDefStd.h"
#include "TrigmuFast/Datatypes.h"
#include "TrigmuFast/MuRoads.h"

#include "MDTcabling/MDTGeometry.h"
#include "MuonCablingTools/OutputDef.h"

#include <iomanip>

using namespace std;

const string RoadsDefStd::s_type = "Roads";

RoadsDefStd::RoadsDefStd(MuMessageInterface& msg,
                   const TriggerData& trigger_data, const TrackFit& track,
		   RoadRegions& roads, const MDTGeometry* mdtgeo, 
		   const MuRoads* muRoads)
    : m_msg(&msg), m_trig_data(trigger_data), m_track(track), m_roads (roads),
      m_mdtgeo(mdtgeo), m_muRoads(muRoads)
{
}

RoadsDefStd::~RoadsDefStd(void)
{
}

void
RoadsDefStd::printout(void) const
{
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    __osstream display;
#else                                // put your gcc 2.95 specific code here
    char buffer[5000];
    for (int i=0;i<5000;++i) buffer[i] = '\0';
    __osstream display(buffer,5000);
#endif

    std::string test = m_msg->testString();

    display << test << name() << " started!" << endl;

    display.setf(ios::fixed);
    
    if(m_trig_data.type==1) display << test << "  Low Pt trigger: " << endl;
    else display << test << "  Hi Pt trigger: " << endl;

    if(m_trig_data.isEndcap)
    {
        display << test << "    Innermost: R(min,max)=("
                << setw(9) << setprecision(2) << m_roads.radMin[0][0] << ","
	        << setw(9) << setprecision(2) << m_roads.radMax[0][0] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][0] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][0] << ")" 
		<< endl;
        display << test << "    Middle   : R(min,max)=("
                << setw(9) << setprecision(2) << m_roads.radMin[0][1] << ","
	        << setw(9) << setprecision(2) << m_roads.radMax[0][1] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][1] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][1] << ")" 
		<< endl;
        display << test << "    Outermost: R(min,max)=("
                << setw(9) << setprecision(2) << m_roads.radMin[0][2] << ","
	        << setw(9) << setprecision(2) << m_roads.radMax[0][2] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][2] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][2] << ")" 
		<< endl;
    }
    else
    {
        display << test << "    Innermost: Z(min,max)=("
                << setw(9) << setprecision(2) << m_roads.zetaMin[0][0] << ","
	        << setw(9) << setprecision(2) << m_roads.zetaMax[0][0] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][0] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][0] << ")" 
		<< endl;
        display << test << "    Middle   : Z(min,max)=("
                << setw(9) << setprecision(2) << m_roads.zetaMin[0][1] << ","
	        << setw(9) << setprecision(2) << m_roads.zetaMax[0][1] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][1] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][1] << ")" 
		<< endl;
        display << test << "    Outermost: Z(min,max)=("
                << setw(9) << setprecision(2) << m_roads.zetaMin[0][2] << ","
	        << setw(9) << setprecision(2) << m_roads.zetaMax[0][2] << ")"
	        << "  Eta(min,max)=("
	        << setw(6) << setprecision(3) << m_roads.etaMin[0][2] << ","
	        << setw(6) << setprecision(3) << m_roads.etaMax[0][2] << ")" 
		<< endl;
    }
#if (__GNUC__) && (__GNUC__ > 2)     // put your gcc 3.2 specific code here
    //nothing to do
#else                                // put your gcc 2.95 specific code here
    //close the stream
    display << ends;
#endif

    m_msg->displayInfo(display.str());
}

bool
RoadsDefStd::find_barrel_road_dim(int position,int type,float aw, float bw,
             float Rmin,float Rmax,float *Zmin,float *Zmax)
{
    float ia,iaq,dz,z,points[4];
    int i;
  
    *Zmin =  9999.;
    *Zmax = -9999.;
  
    if(aw) {
        ia  = 1/aw;
        iaq = ia*ia;
    } else {
        ia  = 0;
        iaq = 0;
    }
  
    float max_road = (m_muRoads->max_roads())[type][position];

    if(!max_road) return false;
    
    dz = max_road*sqrt(1.+iaq);
    z  = (ia)? (Rmin-bw)*ia : bw;
    points[0] = z - dz;
    points[1] = z + dz;
    z  = (ia)? (Rmax-bw)*ia : bw;
    points[2] = z - dz;
    points[3] = z + dz;
  
    for(i=0;i<4;i++) {
        if(*Zmin>=points[i]) *Zmin = points[i];
        if(*Zmax<=points[i]) *Zmax = points[i];
    }
    return true;
}


bool 
RoadsDefStd::find_endcap_road_dim(float road,float aw, float bw, float Zmin,
                                  float Zmax,float *Rmin,float *Rmax)
{
    float r,points[4];
    int i;
  
    *Rmin = 999999.;
    *Rmax = 0.;
    
    if(Zmin>=99999. && Zmax<=-99999.)
    {
        *Rmin = 0.;
	*Rmax = 0.;
	return true;
    }
    
    r = Zmin*aw + bw;
    points[0] = r - road;
    points[1] = r + road;
    r = Zmax*aw + bw;
    points[2] = r - road;
    points[3] = r + road;
    
    for(i=0;i<4;i++) {
        if(*Rmin>=points[i]) *Rmin = points[i];
        if(*Rmax<=points[i]) *Rmax = points[i];
    }
    
    if(*Rmin<0.) *Rmin = 0.;
    if(*Rmax<0.) *Rmax = 0.;
    
    return true;
}

void 
RoadsDefStd::find_eta_min_max(float Zmin, float Rmin, float Zmax, float Rmax,
                              float& etaMin, float& etaMax)
{   
    if(Zmin>=99999. && Zmax<=-99999.)
    {
        etaMin = 0.;
	etaMax = 0.;
	return;
    }
    
    float theta = 0.;
    float signZmin = (Zmin!=0) ? Zmin/fabs(Zmin) : 0;
    float signZmax = (Zmax!=0) ? Zmax/fabs(Zmax) : 0;
    float signEta  = (signZmin)? signZmin : signZmax; 
     
    if(Rmin > 0.)
    {
        theta  = (fabs(Zmin)>0.1)? atan(Rmin/fabsf(Zmin)): M_PI/2.;
        etaMin = (Zmin>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.));
    } else etaMin = 7.0*signEta;
    
    if(Rmax > 0.)
    {
        theta  = (fabs(Zmax)>0.1)? atan(Rmax/fabsf(Zmax)): M_PI/2.;
        etaMax = (Zmax>0.)?  -log(tan(theta/2.)) : log(tan(theta/2.)); 
    } else etaMax = 7.0*signEta;
    
    if (etaMin > etaMax)
    {
        float tmp = etaMax;
	etaMax = etaMin;
	etaMin = tmp;
    }
}

double 
RoadsDefStd::compute_endcap_phi(int station, int tr)
{
    if (station==1 || !m_track.isPhidir) return m_track.phi;
    
    double MiddleZ = fabs(m_roads.zetaMin[tr][1] + m_roads.zetaMax[tr][1])/2.;
    double MiddleR = MiddleZ*m_roads.aw2[tr] + m_roads.bw2[tr];
    
    double computed_phi = 0.;
    double dz = 0.;
    
    if (station==2)
    {
        double OuterZ  = fabs(m_roads.zetaMin[tr][2]+m_roads.zetaMax[tr][2])/2.;
        double OuterR  = OuterZ*m_roads.aw3[tr] + m_roads.bw3[tr];
        double DrOuter = fabs(OuterR-MiddleR);
	dz = sqrt((OuterZ-MiddleZ)*(OuterZ-MiddleZ) + DrOuter*DrOuter);
	dz = (OuterZ-MiddleZ);
    } 
    
    if (station==0)
    {
        if(m_trig_data.tgcInn[2] != 0.) return m_trig_data.tgcInn[1];
        double InnerZ  = fabs(m_roads.zetaMin[tr][0]+m_roads.zetaMax[tr][0])/2.;
        double InnerR  = InnerZ*m_roads.aw1[tr] + m_roads.bw1[tr];
        double DrInner = fabs(InnerR-MiddleR);
	dz = -sqrt((InnerZ-MiddleZ)*(InnerZ-MiddleZ) + DrInner*DrInner);
	dz = -fabsf(InnerZ-MiddleZ);
    }
        
    computed_phi = (dz)* m_track.aphi + m_track.phi;
    while (computed_phi > M_PI)  computed_phi -= 2*M_PI;
    while (computed_phi <-M_PI)  computed_phi += 2*M_PI;
    
    if (station==0)
    {
        (const_cast<TrackFit&>(m_track)).extrInnerPhi = computed_phi;
    }
    
    return computed_phi;
}

double 
RoadsDefStd::compute_barrel_phi(int station, int tr)
{
    if (station==1 || !m_track.isPhidir) return m_track.phi;
    
    double MiddleR = fabs(m_roads.radMin[tr][1] + m_roads.radMax[tr][1])/2.;
    double MiddleZ = MiddleR*m_roads.aw2[tr] + m_roads.bw2[tr];
    
    double computed_phi = 0.;
    double dz = 0.;
    
    if (station==2)
    {
        double mm = (m_roads.aw3[tr]!=0)? 1./m_roads.aw3[tr] : 0.;
        double OuterR  = fabs(m_roads.radMin[tr][2]+m_roads.radMax[tr][2])/2.;
        double OuterZ  = (mm)? (OuterR-m_roads.bw3[tr])/mm : m_roads.bw3[tr];
        double DzOuter = fabs(OuterZ-MiddleZ);
	dz = sqrt((OuterR-MiddleR)*(OuterR-MiddleR) + DzOuter*DzOuter);
	dz = (OuterR-MiddleR);
    } 
    
    if (station==0)
    {
        double mm = (m_roads.aw1[tr]!=0)? 1./m_roads.aw1[tr] : 0.;
        double InnerR  = fabs(m_roads.radMin[tr][0]+m_roads.radMax[tr][0])/2.;
        double InnerZ  = (mm)? (InnerR-m_roads.bw1[tr])/mm  : m_roads.bw1[tr];
        double DzInner = fabs(InnerZ-MiddleZ);
	dz = -sqrt((InnerR-MiddleR)*(InnerR-MiddleR) + DzInner*DzInner);
	dz = - fabsf(InnerR-MiddleR);
    }
        
    computed_phi = (dz)* m_track.aphi + m_track.phi;
    while (computed_phi > M_PI)  computed_phi -= 2*M_PI;
    while (computed_phi <-M_PI)  computed_phi += 2*M_PI;
    
    return computed_phi;
}
