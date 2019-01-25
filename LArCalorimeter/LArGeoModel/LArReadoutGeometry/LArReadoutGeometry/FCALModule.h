/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARREADOUTGEOMETRY_FCALMODULE_H
#define LARREADOUTGEOMETRY_FCALMODULE_H
#include <vector>
#include "Identifier/Identifier.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "GeoModelKernel/GeoVDetectorElement.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"

class FCALDetectorManager;

/**
 *  @class FCALModule
 *
 *      @brief Description of an FCAL Module
 *
 *	This class combines a description of the cell
 *	granularity with a physical volume to create a
 *	description of an FCAL module positioned within ATLAS.
 *	It provides access to the FCAL Tiles.
 */



class FCALModule : public GeoVDetectorElement  
{
  
  public:
  typedef std::vector<FCALTile>::const_iterator ConstIterator;
  typedef enum {FCAL1=1, FCAL2=2, FCAL3=3} Module;
  typedef enum {NEG=0,   POS=1} Endcap;
  

  public:
    
      
  /**
   * @brief Constructor
   */
  FCALModule (const GeoVFullPhysVol *physVol
	      , Module module
	      , Endcap endcap
	      , double projectivityDisplacement = 4*Gaudi::Units::cm);
    
  /**
   * @brief Desctructor
   */
  ~FCALModule();
      
  /**
   * @brief Gets the manager
   */
  const FCALDetectorManager* getManager () const;
      
  /**
   * @brief Iteration over FCAL Tiles.
   */
  FCALModule::ConstIterator beginTiles () const;
      
  /**
   * @brief Iteration over FCAL Tiles
   */
  FCALModule::ConstIterator endTiles () const;
      
  /**
   * @brief Returns a tile by indices i and j
   */
  const FCALTile* getTile (int i, int j) const;
      
  /**
   * @brief Returns the side (O=Negative, 1=Positive)
   */
  FCALModule::Endcap getEndcapIndex () const;
      
  /**
   * @brief Returns the Module (1,2, or 3)
   */
  FCALModule::Module getModuleIndex () const;
      
  /**
   * @brief Gets Tile Full Width in X.
   */
  double getFullWidthX (const FCALTile& tile) const;
      
  /**
   * @brief Gets Tile Full Width in Y.
   */
  double getFullWidthY (const FCALTile& tile) const;
      
  /**
   * @brief Gets Tile (full) Depth.
   */
  double getFullDepthZ (const FCALTile& ) const;
      
  /**
   * @brief Returns the absolute transform of this element.
   */
  const GeoTrf::Transform3D &  getAbsoluteTransform () const;
      
  /**
   * @brief Returns the absolute transform of this element.
   */
  const GeoTrf::Transform3D &  getDefAbsoluteTransform () const;

  /**
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getAbsoluteTransformAmg () const;

  /**
   * @brief Returns the absolute transform of this element.
   */
  const Amg::Transform3D  getDefAbsoluteTransformAmg () const;


  double getProjectivityDisplacement() const;
      
  private:
    
      FCALModule(const FCALModule &right);
      FCALModule & operator=(const FCALModule &right);
    
      
      /**
       * @brief Sets the manager.  Used by the manager.  Initialized when managed.
       */
      void setManager (FCALDetectorManager* fcalManager);

      /**
       * @brief A List of Tiles.
       */
      std::vector<FCALTile> m_tileList;

      /**
       * @brief Module number:  1, 2, or 3.
       */
      Module m_Mod;
      
      /**
       * @brief Endcap. 0 is Negative, 1 is Positive
       */
      Endcap m_EC;
      
      /**
       * @brief Point to the manager
       */
      FCALDetectorManager* m_manager;
      
      /**
       * @brief A vector that holds the list of tile sizes.
       */
      mutable std::vector<double> m_TileSizeX;
      
      /**
       * @brief A vector that holds the list of tile sizes.
       */
      mutable std::vector<double> m_TileSizeY;
      
      /**
       * @brief Holds Delta Z,  Full width of a cell
       */
      double m_dz;
      

      double m_projectivityDisplacement;
      
      friend class FCALDetectorManager;
      
};




inline const FCALDetectorManager* FCALModule::getManager () const
{
  return m_manager;
}

inline double FCALModule::getProjectivityDisplacement() const
{
  return m_projectivityDisplacement;
}


#endif
