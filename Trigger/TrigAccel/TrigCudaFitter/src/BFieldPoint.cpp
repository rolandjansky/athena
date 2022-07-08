#include "BFieldPoint.h"

BFieldPoint::BFieldPoint(double x, double y, double z, float* B)
{
	m_coordinates[0]=x;m_coordinates[1]=y;m_coordinates[2]=z;
	for(int i=0;i<3;i++) m_field[i]=B[i];
}

BFieldPoint::~BFieldPoint(void)
{
}
