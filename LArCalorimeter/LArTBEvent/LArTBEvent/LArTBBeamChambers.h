/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARTBBEAMCHAMBERS_H
#define LARTBBEAMCHAMBERS_H


/** @class LArTBBeamChambers LArTBBeamChambers.h "LArTBEvent/LArTBBeamChambers.h"

    @author A. Abdesselam and P. Ghez
*/

#include <vector>
#include <string>

class LArTBBeamChambers {
  
 public:
  /** Constructor for H8 */
  LArTBBeamChambers( int flag, int TDC_up, int TDC_right, int TDC_left, 
		     int TDC_down, float x, float y, float res_x, float res_y,
		     float clusterSize_x, float clusterSize_y );
  
  /** Constructor for H6(DWPC without drift time information) */
  LArTBBeamChambers( int flag, float x, float y, float res_x, float res_y,
		     float clusterSize_x, float clusterSize_y);
  
  /** Destructor */
  virtual ~LArTBBeamChambers();
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /**  Get reconstruction flag 
   *   \li \c 0th bit for reconstruction for X (1=SUCCESS) 
   *   \li \c 1th bit for reconstruction for Y (1=SUCCESS) */
  inline int getFlag()      const{ return m_flag;}
  
  /** Get drift time for the upper side(Y) */
  inline int getTDC_up()    const{ return m_TDC_up;}

  /** Get drift time for the right side(X) */
  inline int getTDC_right() const{ return m_TDC_right;}

  /** Get drift time for the left side(X) */
  inline int getTCD_left() const{ return m_TDC_left;}

  /** Get drift time for the down side(Y) */
  inline int getTDC_down() const{ return m_TDC_down;}

  /** Get cluster X position from reconstruction */
  inline float getX() const{ return m_x;}

  /** Get cluster Y position from reconstruction */
  inline float getY() const{ return m_y;}

  /** Get residual along X-direction \f$ = x_{rec} - x_{raw} \f$. */
  inline float getResiduX() const{ return m_residu_x;}

  /** Get residual along Y-direction \f$ = y_{rec} - y_{raw} \f$. */
  inline float getResiduY() const{ return m_residu_y;}
  
  /** Get cluster size for x-direction */
  inline float getClusterSizeX() const{ return m_clusterSize_x;}

  /** Get cluster size for y-direction */
  inline float getClusterSizeY() const{ return m_clusterSize_y;}

  /** Get the collection of the center wire number of a cluster */
  inline const std::vector<int> & getCwireno()
    const { return m_cwireno; }

  /** Get the collection of the number of wire of a cluster */
  inline const std::vector<int> & getNwires()
    const { return m_nwires; }

  /** Get the collection of the xy flag of a cluster */
  inline const std::vector<short> & getWireXY()
    const { return m_wire_xy; }
  
  /* ------------------------------------------------------------  */
  /* ------------------------ S E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  
  /** Set the collection of the center wire number */
  inline void setCwireno(std::vector<int> cwireno) {
    m_cwireno = cwireno;
  }    

  /** Set the collection of the number of wires in a cluster */
  inline void setNwires(std::vector<int> nwires) {
    m_nwires  = nwires;
  }    

  /** Set the collection of xy flag X=1, Y=0 */
  inline void setWireXY(std::vector<short> wire_xy) {
    m_wire_xy  = wire_xy;
  }    
  
  /** Convertion operator to a std::string */
  virtual operator std::string() const;
  
 private:
  LArTBBeamChambers();
  int m_flag;                /**< Quality flag for both X and Y */
  int m_TDC_up;              /**< Raw TDC */
  int m_TDC_right;           /**< Raw TDC */
  int m_TDC_left;            /**< Raw TDC */
  int m_TDC_down;            /**< Raw TDC */
  float m_x;                 /**< X position in the chamber (cm)*/
  float m_y;                 /**< Y position in the chamber (cm)*/
  float m_residu_x;          /**< X position in the chamber (cm)*/
  float m_residu_y;          /**< Y position in the chamber (cm)*/
  float m_clusterSize_x;     /**< half with of cluster (x)  (cm)*/
  float m_clusterSize_y;     /**< half with of cluster (y)  (cm)*/

  std::vector<int> m_cwireno;   /**< center wire number */
  std::vector<int> m_nwires;    /**< the number of wires a cluster */
  std::vector<short> m_wire_xy; /**< xy flag for a cluster(1=x, 0=y) */
};

#endif // LARTBBEAMCHAMBERS_H
