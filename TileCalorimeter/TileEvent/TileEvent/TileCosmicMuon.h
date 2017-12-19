/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEEVENT_TILECOSMICMUON
#define TILEEVENT_TILECOSMICMUON
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Information on TileCosmicAlgs/TileMuonFitter results
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>

/**
* @class TileCosmicMuon
* @author Jose Maneira (maneira@lip.pt)
* @brief Class containing detailed results from TileMuonFitter
*
* TileCosmicMuon contains the results of the TileMuonFitter (see TileCosmicAlgs
* package) algorithm, which is basically a straight line fit to the TileCal cell
* positions, useful for cosmic muons analysis (but also to beam halo muons). 
* It contains information about the fit (a fit quality parameter and the number 
* of cells used), the geometrical track parameters (position at the selected plane,
* direction angles) and derived quantities (time at the selected plane, path 
* length in TileCal and energy along track). The selected plane is at y=0 for 
* beamType=cosmics and z=0 for singlebeam and collisions. 
* A list of cells close to the track is also kept.
*/


class TileCosmicMuon
{
 public:
 
  /** Constructor */
  TileCosmicMuon() : m_time((double)0.),
    m_positionX((double)0.),m_positionY((double)0.),m_positionZ((double)0.),
    m_directionPhi((double)0.),m_directionTheta((double)0.),
    m_fitQuality((double)0.),m_fitNCells((int)0) {}
  
  ~TileCosmicMuon() {};
  std::string whoami   (void) const { return "TileCosmicMuon"; }
  operator std::string() const;


  
  /** Setters. See class members description. */
  inline void SetTime(double time)                                {m_time  		 = time;}
  inline void SetPositionX(double posx)                           {m_positionX 	 = posx;}
  inline void SetPositionY(double posy)                           {m_positionY 	 = posy;}
  inline void SetPositionZ(double posz)                           {m_positionZ 	 = posz;}
  inline void SetDirectionPhi(double phi)                         {m_directionPhi     = phi;}
  inline void SetDirectionTheta(double theta)                     {m_directionTheta   = theta;}
  inline void SetFitQuality(double quality)                       {m_fitQuality	      = quality;}
  inline void SetFitNCells(int ncells)                            {m_fitNCells 	      = ncells;}

  inline void SetPathTop(const std::vector<double>& path)
  {m_pathTop          = path;}
  inline void SetPathBottom(const std::vector<double>& path)
  {m_pathBottom       = path;}
  inline void SetEnergyTop(const std::vector<double>& energy)
  {m_energyTop        = energy;}
  inline void SetEnergyBottom(const std::vector<double>& energy)
  {m_energyBottom     = energy;}
  inline void SetTrackCellHash(const std::vector<IdentifierHash>& cells)
  {m_trackCellHash    = cells;}
  inline void SetSegmentPath(const std::vector<double>& path)
  {m_segmentPath      = path;}
  inline void SetSegmentPartition(const std::vector<int>& partition)
  {m_segmentPartition = partition;}
  inline void SetSegmentModule(const std::vector<int>& module)
  {m_segmentModule    = module;}
  inline void SetSegmentSampling(const std::vector<int>& sampling)
  {m_segmentSampling  = sampling;}

  inline void SetPathTop(std::vector<double>&& path)
  {m_pathTop          = std::move(path);}
  inline void SetPathBottom(std::vector<double>&& path)
  {m_pathBottom       = std::move(path);}
  inline void SetEnergyTop(std::vector<double>&& energy)
  {m_energyTop        = std::move(energy);}
  inline void SetEnergyBottom(std::vector<double>&& energy)
  {m_energyBottom     = std::move(energy);}
  inline void SetTrackCellHash(std::vector<IdentifierHash>&& cells)
  {m_trackCellHash    = std::move(cells);}
  inline void SetSegmentPath(std::vector<double>&& path)
  {m_segmentPath      = std::move(path);}
  inline void SetSegmentPartition(std::vector<int>&& partition)
  {m_segmentPartition = std::move(partition);}
  inline void SetSegmentModule(std::vector<int>&& module)
  {m_segmentModule    = std::move(module);}
  inline void SetSegmentSampling(std::vector<int>&& sampling)
  {m_segmentSampling  = std::move(sampling);}

 
  /** Getters. See class members description. */
  double		 GetTime()  			      const {return m_time;}				  
  double		 GetPositionX() 		      const {return m_positionX;}			  
  double		 GetPositionY() 		      const {return m_positionY;}			  
  double		 GetPositionZ() 		      const {return m_positionZ;}			  
  double		 GetDirectionPhi()  	      const {return m_directionPhi;}		  
  double		 GetDirectionTheta()	      const {return m_directionTheta;}  	  
  double		 GetFitQuality()		      const {return m_fitQuality;}  		  
  int            GetFitNCells() 		      const {return m_fitNCells;}			  
  int            GetTrackNCells()		      const {return m_trackCellHash.size();}  
  int            GetNSamples()  		      const {return m_pathTop.size();}  	  
  int            GetNSegments()  		      const {return m_segmentPath.size();}  	  
  double		 GetPathTop(int ind)	      const {return m_pathTop[ind];}		  
  double		 GetPathBottom(int ind)	      const {return m_pathBottom[ind];} 	  
  double		 GetEnergyTop(int ind) 	      const {return m_energyTop[ind];}  	  
  double		 GetEnergyBottom(int ind)     const {return m_energyBottom[ind];}	  
  IdentifierHash GetTrackCellHash(int ind)    const {return m_trackCellHash[ind];}    
  double		 GetSegmentPath(int ind)      const {return m_segmentPath[ind];}	  
  int            GetSegmentPartition(int ind) const {return m_segmentPartition[ind];}   
  int            GetSegmentModule(int ind)    const {return m_segmentModule[ind];}   
  int            GetSegmentSampling(int ind)  const {return m_segmentSampling[ind];}   

  /** Sum up components of m_pathTop and m_pathBottom: full path in Tile */
  double GetFullPath() const;
  /** Sum up components of m_energyTop and m_energyBottom: full energy in track in Tile */
  double GetFullEnergy() const;

 private:
    
  double m_time;//!< Time of track at selected plane (y=0 for cosmics z=0 for beam)
  double m_positionX; //!< X coordinate of point in track at selected plane (y=0 for cosmics z=0 for beam)
  double m_positionY; //!< Y coordinate of point in track at selected plane (y=0 for cosmics z=0 for beam)
  double m_positionZ; //!< Z coordinate of point in track at selected plane (y=0 for cosmics z=0 for beam)
  double m_directionPhi; //!< Phi angle of track direction
  double m_directionTheta; //!< Theta angle of track direction
  double m_fitQuality; //!< Fit parameter: 0= no fit; (Hough) 1=fit ok; (Minuit) >0 chi-square 
  int    m_fitNCells; //!< Number of cells used in fit
  
  /** Vector with length of track within Tile on top modules [0]:sampling A; [1]: BC; [2]: D */
  std::vector<double> m_pathTop; 
  /** Vector with length of track within Tile on bottom modules [0]:sampling A; [1]: BC; [2]: D */
  std::vector<double> m_pathBottom; 
  /** Vector with sum energy of cells close to track on top modules [0]:sampling A; [1]: BC; [2]: D */
  std::vector<double> m_energyTop; 
  /** Vector with sum energy of cells close to track on bottom modules [0]:sampling A; [1]: BC; [2]: D */
  std::vector<double> m_energyBottom; 
 
  /** Vector with list of Identifier Hash of cells close to track.*/
  std::vector<IdentifierHash> m_trackCellHash; 
  
  /** Vector with length of track within Tile on a given segment */
  std::vector<double> m_segmentPath; 
  /** Vector with segment partition*/
  std::vector<int> m_segmentPartition; 
  /** Vector with segment module*/
  std::vector<int> m_segmentModule; 
  /** Vector with segment sampling */
  std::vector<int> m_segmentSampling; 
  
  
};

CLASS_DEF( TileCosmicMuon , 2950 , 0 )
#endif
