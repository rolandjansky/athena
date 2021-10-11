/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECEVENT_LARCELL_H
#define LARRECEVENT_LARCELL_H

/**
   @class LArCell
   @brief Data object for LAr calorimeter readout cell

   LArCell does not add any information to its parent CaloCell. It is  
   kept mainly for historical reasons, but also if it turns out to be  
   necessary to add data members pertaining only to LArCell and not to  TileCell. 

   @author Hong Ma
   @author Srini Rajagopalan

*/

/*
CREATED:  Dec. 15, 1999

Updated: May 10, 2000    (SR, HM)
         Migrated to Athena Framework from PASO

Updated: Nov. 03, 2000   (HM)
         Follow new LArCell Contructor (include time, quality)

Updated: Dec. 02, 2000    (M. Wielers, P. Loch)
         Extended to work for FCAL

Updated: Jan. 2, 2001    (SR)
         Adhere to QC standards (based on comments from S. Albrand)

Updated: Jan. 2, 2001    (Efrank)
         Add Navigation code.

Updated:  April 2002  (H. Ma) 
	moved to LArCalorimeter/LArRecEvent 

Updated:  June 2003  (H. Ma) 
	moved to CaloCell with CaloDDE* 

********************************************************************/


#include "CaloEvent/CaloCell.h"
#include "CaloIdentifier/CaloGain.h"

class LArCell: public CaloCell
// Note: this class also inherits from Navigable, via CaloCell
{

public:

  
  /** @brief default constructor (should not be used, only there for persistency reason) */    
  LArCell();
  
  /** @brief Legacy constructor */
  LArCell(const CaloDetDescrElement* caloDDE, 
	  double energy,
	  double time,
	  double quality,
	  CaloGain::CaloGain gain);

  /** @brief Main constructor */
  LArCell(const CaloDetDescrElement* caloDDE, 
          double energy, 
          double time,
          uint16_t quality,
          uint16_t provenance,
          CaloGain::CaloGain gain);

  
  /** @brief  Legacy constructor with ID */
  LArCell(const CaloDetDescrElement* caloDDE, 
	  const Identifier& id,
	  double energy,
	  double time,
	  double quality,
	  CaloGain::CaloGain gain);

  /** @brief   constructor with ID */
  LArCell(const CaloDetDescrElement* caloDDE, 
          const Identifier& id,
          double energy, 
          double time,
          uint16_t quality,
          uint16_t provenance,
          CaloGain::CaloGain gain);

  /** @brief default destructor */
  virtual ~LArCell ();
  
  /** @brief For initializing a cell we get from a @c DataPool. */
  void assign (const CaloDetDescrElement* caloDDE, 
               const Identifier& id,
               double energy, 
               double time,
               uint16_t quality,
               uint16_t provenance,
               CaloGain::CaloGain gain);

  /** @brief get deltaR (using 4mom helper) with respect to another cell */
  double deltaR	(const LArCell* other) const ; 
  
  /** @brief print all relevant info about this cell */
  void   print	(void) const; 

  /** @brief clone this cell into another one*/
  virtual std::unique_ptr<CaloCell> clone() const;
  
  /** @brief Add energy to this cell. 
      This is typically used in case of duplicate cells 
      where only one LArCell Object is created and 	
      all duplicate cells are summed. */	 
  void add_energy     (double energy);

  /** @brief set Energy to this cell with inline method. 
      This is requested by HLT to build LArCell fast.
  */	 
  void setEnergyFast(float e) ; 

  /** @brief set Quality to this cell with inline method. 
      This is requested by HLT to build LArCell fast.
  */	 
  void setQualityFast(float q) ; 

  /** @brief set Time to this cell with inline method. 
      This is requested by HLT to build LArCell fast.
  */	 
  void setTimeFast(float t) ; 

  /** @brief  Check from provenance bit is cell is dead */
  virtual bool badcell() const;



private:

};


// inline methods:

inline LArCell::LArCell() : CaloCell()
{}

inline LArCell::LArCell(const CaloDetDescrElement* caloDDE, 
			double energy,
			double time,
			double quality,
			CaloGain::CaloGain gain)
    :
    CaloCell(caloDDE,energy,time,quality,gain)
{}

inline LArCell::LArCell(const CaloDetDescrElement* caloDDE, 
			const Identifier& id, 
			double energy,
			double time,
			double quality,
			CaloGain::CaloGain gain)
    :
    CaloCell(caloDDE,id,energy,time,quality,gain)
{}

inline LArCell::LArCell(const CaloDetDescrElement* caloDDE, 
                        double energy, 
                        double time,
                        uint16_t quality,
                        uint16_t provenance,
                        CaloGain::CaloGain gain)
    :
    CaloCell(caloDDE,energy,time,quality,provenance,gain)
{}

inline LArCell::LArCell(const CaloDetDescrElement* caloDDE, 
                        const Identifier& id, 
                        double energy, 
                        double time,
                        uint16_t quality,
                        uint16_t provenance,
                        CaloGain::CaloGain gain)
    :
    CaloCell(caloDDE,id,energy,time,quality,provenance,gain)
{}



inline void LArCell::assign(const CaloDetDescrElement* caloDDE, 
                            const Identifier& id, 
                            double energy, 
                            double time,
                            uint16_t quality,
                            uint16_t provenance,
                            CaloGain::CaloGain gain)
{
  CaloCell::assign(caloDDE,id,energy,time,quality,provenance,gain);
}


inline void LArCell::add_energy(double energy) 	 
{ 	 
m_energy += energy; 	 
}


inline void LArCell::setEnergyFast(float e) 	 
{ 	 
  m_energy = e ;  
}


inline void LArCell::setQualityFast(float q) 	 
{ 	 
  m_quality = (int)(q) ;  
}


inline void LArCell::setTimeFast(float t) 	 
{ 	 
  m_time = t ;  
}

#endif


