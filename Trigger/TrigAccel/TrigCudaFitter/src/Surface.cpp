#include "TrkPlanarSurface.h"
#include "Surface.h"
#include <math.h>
#include <iostream>

Surface::Surface(float par[6])
{
	for(int i=0;i<3;i++)
	{
		m_center[i]=par[i];
		m_normal[i]=par[i+3];
		for(int j=0;j<3;j++) m_matrix[i][j]=0.0;
	}
}

Surface::~Surface(void)
{
}

void Surface::addRow(int i, float* row)
{
	for(int j=0;j<3;j++) m_matrix[i][j]=row[j];
}

TrkPlanarSurface* Surface::createDkfSurface(void) const
{
	return (new TrkPlanarSurface(m_center,m_normal,m_matrix,0.025f));
}

long int Surface::generateKey() const {
	const double pi = 3.1415926f;
	
	double r = sqrt(m_center[0]*m_center[0]+m_center[1]*m_center[1]+m_center[2]*m_center[2]);
	long int key=int(10.0*r)*1000;
	double phi = 180.0f*atan2(m_center[1],m_center[0])/pi;
	if(phi<0) phi+=360.0f;
	key += int(phi);
	if(m_center[2]<0.0) key=-key;
	//std::cout<<"x="<<m_center[0]<<" y="<<m_center[1]<<" z="<<m_center[2]<<" key="<<key<<std::endl;
	return key;
}

std::ostream& operator << (std::ostream& o, const Surface* pS) {
	o<<pS->m_center[0]<<" "<<pS->m_center[1]<<" "<<pS->m_center[2]<<std::endl;
	o<<pS->m_normal[0]<<" "<<pS->m_normal[1]<<" "<<pS->m_normal[2]<<std::endl;
	for(int i = 0; i<3; i++) {
		o<<pS->m_matrix[i][0]<<" "<<pS->m_matrix[i][1]<<" "<<pS->m_matrix[i][2]<<std::endl;
	}
	return o;
}
