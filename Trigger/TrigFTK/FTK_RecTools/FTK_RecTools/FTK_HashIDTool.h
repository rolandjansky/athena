/*
  Copyright (C) 2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTK_HASHIDTOOL_H
#define FTK_HASHIDTOOL_H
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include <vector>
#include <string>
#include <map>
#include "CLHEP/Vector/ThreeVector.h"
#include "FTK_RecToolInterfaces/IFTK_HashIDTool.h"
#include "TrigFTK_RawData/FTK_RawTrackContainer.h"
#include "GeoPrimitives/GeoPrimitives.h"

using Amg::Vector3D;


class PixelID;
class AtlasDetectorID;
namespace InDetDD {
  class PixelDetectorManager;
}

class FTK_HashIDTool : public AthAlgTool, virtual public IFTK_HashIDTool {
  
  
 public:
  FTK_HashIDTool ( const std::string&, const std::string&, const IInterface* );
  virtual ~FTK_HashIDTool ();
  virtual StatusCode initialize();
  
  unsigned int getHash(unsigned int tower, unsigned int sector,  unsigned int plane);

  unsigned int getHashFromAuxSector(unsigned int tower, unsigned int sector,  unsigned int plane);
  
  bool findHash(unsigned int hash, bool isSCT, unsigned int& tower, unsigned int& sector, unsigned int& plane);
  
  FTK_RawTrackContainer* processTracks(const FTK_RawTrackContainer& inputTracks, const bool reverseIBL);
  
 private:
  
  
  const Amg::Vector3D getPixelClusterPosition(const FTK_RawPixelCluster& raw_pixel_cluster, const bool reverseIBL);

  inline float get_d0(Amg::Vector3D& s0, Amg::Vector3D& s1, Amg::Vector3D& s2) {
    float phi0 = s0.phi();
    float phi1 = s1.phi();
    float phi2 = s2.phi();
    float r0 = s0.perp();
    float r1 = s1.perp();
    float r2 = s2.perp();
    
    return r0*r1*r2*(phi0*r1 - phi0*r2 - phi1*r0 + phi1*r2 + phi2*r0 - phi2*r1)/(r0*r0*r1 - r0*r0*r2 - r0*r1*r1 + r0*r2*r2 + r1*r1*r2 - r1*r2*r2);
  }
  
  //Parametrise triplet as quadratic a + t*x + (alpha/2)*x**2
  inline float get_alpha(Amg::Vector3D& s0, Amg::Vector3D& s1, Amg::Vector3D& s2) {
    float dx1 = s1.x()-s0.x();
    float dy1 = s1.y()-s0.y();
    float dx2 = s2.x()-s0.x();
    float dy2 = s2.y()-s0.y();
    
    //Transform into line with s1 and s2 on x-axis, s1 at (0,0)
    float x1prime = sqrt(dx1*dx1+dy1*dy1);
    float x2prime = (dx2*dx1 + dy2*dy1)/x1prime;
    float y2prime = (dy2*dx1 - dx2*dy1)/x1prime;
    return 2*y2prime/(x2prime*(x2prime-x1prime));
  }
  
  //IDSCAN Hit Filter style pT calculation
  inline float get_invpT(Amg::Vector3D& s0, Amg::Vector3D& s1, Amg::Vector3D& s2) {
    //pT = 0.3*B*R
    float a = get_alpha(s0, s1, s2);
    if (a==0.0) {
      return 1e-9;
    }
    return -a/0.6;
  }
  
  //IDSCAN Hit Filter style phi calculation
  inline float get_phi(Amg::Vector3D& s0, Amg::Vector3D& s1, Amg::Vector3D& s2) {
    float x0 = s0.x();
    float x1 = s1.x();
    float y0 = s0.y();
    float y1 = s1.y();
    
    //Transform into coordinate system rotated so that y0'=y1'=0, at angle phi
    float phi = std::atan2(y1-y0,x1-x0);       
    float L = std::fabs(x0*cos(phi) - y0*sin(phi));//Distance to 0th spacepoint from origin
    //y = (alpha/2)*x**2 + t*x to describe parabola
    float alpha = get_alpha(s0,s1,s2);
    //Distance between 0th and 1st spaceppoint
    float x1prime = std::sqrt((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0));
    float t   = -0.5*alpha*x1prime;
    //t' = phi0' in small angle approximation
    float phi0prime = t - alpha*L;
    
    float phi_prime = phi + phi0prime;
    if(phi_prime<-M_PI) phi_prime+=2*M_PI;
    if(phi_prime>M_PI)  phi_prime-=2*M_PI;
    return phi_prime;
  }
  
  inline float get_z0(Amg::Vector3D& s1, Amg::Vector3D& s2) {
    
    return ((s2.perp()*s1.z()-s2.z()*s1.perp())/(s2.perp() - s1.perp()));
    
  }
  inline float get_cotTheta(Amg::Vector3D& s1, Amg::Vector3D& s2) {
    
    return ((s2.z() - s1.z())/ (s2.perp() - s1.perp()));
  }
  
 private:
  
  typedef std::vector<std::vector<int>> ftk_sectormap;
  
  int readModuleIds(unsigned int itower, ftk_sectormap& hashID);
  
  size_t m_max_tower;
  
  std::vector<ftk_sectormap*> m_moduleFromSector;
  std::vector<std::vector<int>> m_sector8toSector12;
  
  std::string m_ConstantsDir;
  std::string m_PatternsVersion;
  
  const PixelID* m_pixelId;
  const AtlasDetectorID* m_id_helper;
  const InDetDD::PixelDetectorManager* m_pixelManager;

  bool m_maxD0;
};

#endif // FTK_HASHIDTOOL_H
