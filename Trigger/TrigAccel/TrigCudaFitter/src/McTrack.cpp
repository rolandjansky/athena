#include "McBremPoint.h"
#include "McTrack.h"

McTrack::McTrack(float p[6])
{
  m_bremPoints.clear();
  for(int i=0;i<5;i++)
    m_params[i]=p[i];
  m_totalEloss=p[5];
}

McTrack::~McTrack(void)
{
  m_bremPoints.clear();
}

void McTrack::addBremPoint(float x, float y, float z, float loss)
{
  m_bremPoints.push_back(McBremPoint(x,y,z,loss));
}
