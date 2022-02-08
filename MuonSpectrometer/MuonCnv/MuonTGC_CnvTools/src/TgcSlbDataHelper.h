/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCSLBDATAHELPER_H
#define MUONTGC_CNVTOOLS_TGCSLBDATAHELPER_H

#include "AthenaBaseComps/AthMessaging.h"
#include <vector>
#include <inttypes.h>

class MsgStream;
class TgcRawData;

namespace Muon 
{
  class TgcSlbData;

  /** @class TgcSlbDataHelper
   * This is the SLB data helper class which 
   * stores bit map, 
   * reconstructs to hits and coincidences, and
   * converts to celladdr and data. 
   * 
   * @author Susumu Oda <Susumu.Oda@cern.ch>
   * @author Hisaya Kurashige  
   * 
   * This class was developed by Tadashi Maeno.  
   */

  class TgcSlbDataHelper
    : public AthMessaging
    {
    public:
      /** SLB Module Type in SLB Header */
      enum {
	SLB_WD =0,
	SLB_SD,
	SLB_WT,
	SLB_ST,
	SLB_EIFI,
	SLB_NONE
      };

      /** Constructor */
      TgcSlbDataHelper(void);
      /** Destructor */
      virtual ~TgcSlbDataHelper(void);

      TgcSlbDataHelper (const TgcSlbDataHelper&) = delete;
      TgcSlbDataHelper& operator= (const TgcSlbDataHelper&) = delete;
  
      /** Convert to Hits. 
       *  subDetectorID and ROD ID are dummy. */
      void convertToHits(uint16_t subDetectorId,
			 uint16_t rodId,
			 const TgcSlbData * slb,
			 std::vector<TgcRawData *> &vChannel) const;

      /** Convert to coincidences. 
       *  subDetectorID and ROD ID are dummy. */
      void convertToCoincidences(uint16_t subDetectorId,
				 uint16_t rodId,
				 const TgcSlbData * slb,
				 std::vector<TgcRawData *> &vChannel) const;

      /** Set SLB Type based on sswId and moduleType. */ 
      bool setType( uint16_t subDetectorId,
		    uint16_t rodId,
		    TgcSlbData * slb, 
		    int moduleType)  const; 

      /** Adjacent or not */ 
      bool isAdjacent(int ibit) const;

      /** Set sector for HpT/SL based on slbId. */
      uint16_t getSector(bool forward, 
			 uint16_t subDetectorId,
			 uint16_t rodId,
			 const TgcSlbData * slb ) const; 

    protected:
      /** Number of Trigger block in an SLB */
      enum {
	N_LPT_D  = 2,
	N_LPT_TW = 3,
	N_LPT_TS = 8,
	N_LPT_I = 8,
	N_HPT_F  = 6,
	N_HPT_E  = 13,  // additional 2 for duplicate strip hits   
	N_SL = 2
      };

      /** Decode low-pT coincidence (tracklet) information 
       *  for doublet from bit array. */
      void getLPTforDoublet(const bool * bitArray,
			    bool hit[],
			    int delta[],
			    int pos[]) const;
  
      /** Decode low-pT coincidence (tracklet) information 
       *  for triplet wire from bit array. */
      void getLPTforTripletWire(const bool * bitArray,
				bool hit[],
				int pos[]) const;
  
      /** Decode low-pT coincidence (tracklet) information 
       *  for triplet strip from bit array. */
      void getLPTforTripletStrip(const bool * bitArray,
				 bool hit[],
				 int pos[]) const;

      /** Decode low-pT coincidence (tracklet) information 
       *  for inner from bit array. */
      void getLPTforInner(const bool * bitArray,
			  bool hit[]) const;

      /** Decode Sector Logic coincidence information 
       *  for forward region from bit array. */
      void getSL_F(const bool * bitArray,
		   bool& cand3plus,
		   bool hit[],
		   bool muplus[],
		   uint16_t pt[],
		   bool overlap[],
		   uint16_t roi[]) const;

      /** Decode Sector Logic coincidence information 
       *  for endcap region from bit array. */
      void getSL_E(const bool * bitArray,
		   bool& cand3plus,
		   bool hit[],
		   bool muplus[],
		   uint16_t pt[],
		   bool overlap[],
		   uint16_t roi[]) const;
 
      /** Decode high-pT coincidence information 
       *  for forward region from bit array. */
      void getHPT_F(const bool * bitArray,
		    bool hit[],
		    bool strip[],
		    uint16_t chip[],
		    uint16_t index[],
		    bool hipt[],
		    uint16_t hitId[],
		    uint16_t sub[],
		    int16_t delta[]) const;

      /** Decode high-pT coincidence information 
       *  for endcap region from bit array. */
      void getHPT_E(const bool * bitArray,
		    bool hit[],
		    bool strip[],
		    uint16_t chip[],
		    uint16_t index[],
		    bool hipt[],
		    uint16_t hitId[],
		    uint16_t sub[],
		    int16_t delta[]) const;

      /** Check if it is valid high-pT coincidence 
       *  or not. */ 
      bool isValid_HPT(bool     hit,
		       bool     strip,
		       uint16_t chip,
		       uint16_t index,
		       bool     hipt,
		       uint16_t hitId,
		       uint16_t pos,
		       int16_t  delta) const;

    private:
      /** Get a value in a range of a bitArray */
      uint16_t getVal(const bool* bitArray, std::size_t start, std::size_t width) const;

    };

} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGCSLBDATAHELPER_H
