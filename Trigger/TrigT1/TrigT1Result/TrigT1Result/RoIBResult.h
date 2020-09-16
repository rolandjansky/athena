/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1RESULT_ROIBRESULT_H
#define TRIGT1RESULT_ROIBRESULT_H

// STL include(s):
#include <vector>

// Local include(s):
#include "TrigT1Result/MuCTPIResult.h"
#include "TrigT1Result/CTPResult.h"
#include "TrigT1Result/EMTauResult.h"
#include "TrigT1Result/JetEnergyResult.h"
#include "TrigT1Result/L1TopoResult.h"

// Forward declaration(s):
class MsgStream;

namespace ROIB {

  /**
   * 
   *   @short Class holding the LVL1 RoIB result build by the RoIBuilder
   *
   *          RoIBResult holds the complete LVL1 information send by the
   *          subsystems to the RoIBuilder.
   *
   *          RoIBResult is the L1 RDO structure.
   *
   *     @see ROIB::Header
   *     @see ROIB::Trailer
   *     @see ROIB::CTPResult
   *     @see ROIB::MuCTPIResult
   *     @see ROIB::EMTauResult
   *     @see ROIB::JetEnergyResult
   *     @see ROIB::L1TopoResult
   *
   *  @author Thomas Schoerner-Sadenius <thomas.schoerner@cern.ch>
   *
   * @version \$Id: RoIBResult.h,v 1.8 2008-02-01 17:07:23 efeld Exp $
   *
   */

  class RoIBResult {

  public:
    //! default constructor: empty object 
    RoIBResult() = default;

    //! Full move constructor, with all RODs provided 
    RoIBResult( MuCTPIResult&&,
                CTPResult&&,
                std::vector< JetEnergyResult >&&,
                std::vector< EMTauResult >&& );
    //! Constructor without Muon rod 
    RoIBResult( CTPResult&&,
                std::vector< EMTauResult >&&,
                std::vector< JetEnergyResult >&& );
    //! Constructor for only CTP and egamma rods 
    RoIBResult( CTPResult&&, std::vector< EMTauResult >&& );
    //! Constructor for only CTP rod 
    RoIBResult( CTPResult&& );

    //! Gets the MuCTPI part of the L1 RDO 
    const MuCTPIResult& muCTPIResult() const;
    //! Gets the CTP part of the L1 RDO 
    const CTPResult& cTPResult() const;
    //! Gets the jet/energy part of the L1 RDO 
    const std::vector< JetEnergyResult >& jetEnergyResult() const;
    //! Gets the egamma part of the L1 RDO 
    const std::vector< EMTauResult >& eMTauResult() const;
    //! Gets the L1Topo part of the L1 RDO 
    const std::vector< L1TopoResult >& l1TopoResult() const;
    //! Sets the L1Topo part of the L1 RDO 
    void l1TopoResult(std::vector< L1TopoResult >&&) noexcept;
    
    //! dump raw object content to string
    /** method used for read/write testing and T/P separation */
    const std::string dump() const;

    //! print object content in a human readable format to string
    const std::string print(const bool longFormat = false) const;

    //! print object content to default message stream 
    void dumpData() const;
    //! print object content to given message stream 
    void dumpData(MsgStream&) const;

    /* definitions used for overflow checking  */
    enum {EM1_DOFL=1, EM2_DOFL=2, EM3_DOFL=4, EM4_DOFL=8, 
	  J1_DOFL=16, J2_DOFL=32, 
	  MU_DOFL=64, MU_SUP1=128, MU_SUP2=256, MU_SOFL=512};
    
    constexpr static unsigned int DOFL_bitMask = 16;
    constexpr static unsigned int SUP1_bitMask = 1 << 16;
    constexpr static unsigned int SUP2_bitMask = 1 << 17;
    
    constexpr static unsigned int emOverflow  = EM1_DOFL | EM2_DOFL | EM3_DOFL | EM4_DOFL;
    constexpr static unsigned int jOverflow   = J1_DOFL | J2_DOFL;
    constexpr static unsigned int anyOverflow = emOverflow | jOverflow | MU_DOFL;
    constexpr static unsigned int emLink[4] = {EM1_DOFL, EM2_DOFL, EM3_DOFL, EM4_DOFL};
    constexpr static unsigned int jLink[2] = {J1_DOFL, J2_DOFL};
  
  
    /* true if overlfow on any link (note that the SUP1, SUP2 and SOFL bits are not checked) */
    bool CheckOverflow() const  { return (GetOverflowMask(true,true,true) & anyOverflow) != 0;}

    /* true if overlfow on any of the EMTau links */
    bool CheckEMOverflow() const { return (GetOverflowMask(false,true,false) & emOverflow) != 0;}

    /* true if overlfow on either of the JetEnergy links */
    bool CheckJOverflow() const { return (GetOverflowMask(false,false,true) & jOverflow) != 0;}

    /* true if overlfow on either of the MuCTPI link */
    bool CheckMuOverflow() const { return (GetOverflowMask(true,false,false) & MU_DOFL) != 0;}
    
    /* true if a low threshhold muon from any sector was suppressed */
    bool CheckMu1Suppression() const {return (GetOverflowMask(true,false,false) & MU_SUP1) != 0;}
    
    /* true if a high threshhold muon from any sector was suppressed */
    bool CheckMu2Suppression() const {return (GetOverflowMask(true,false,false) & MU_SUP2) != 0;}
    
    /* true for sorter overflow (> 10 candidates on same threshold) */
    bool CheckMuSorterOverflow() const {return (GetOverflowMask(true,false,false) & MU_DOFL) != 0;}
    
    /* Check for a specific EMTau link -- there are 4 EMTau links, numbered from 0 through 3 */
    bool CheckEMLinkOverflow(unsigned int link) const
    { if(link > 3) return false; return (GetOverflowMask(false,true,false) & emLink[link]) != 0;}
    
    /* Check for a specific JetEnergy link -- there are two JetEnergy links (0, 1) */
    bool CheckJLinkOverflow(unsigned int link) const
    { if(link > 1) return false; return (GetOverflowMask(false,false,true) & jLink[link]) != 0;}
    
  private:
    /* The members - namely the muctpi, ctp, jetenegry
    and egamma parts of the L1 RDO */
    MuCTPIResult m_RoIBResultMuCTPI;                         //!< result from muon system
    CTPResult m_RoIBResultCTP;                               //!< result from CTP
    std::vector< JetEnergyResult > m_RoIBResultJetEnergy;    //!< result from calo system: JetEnergy
    std::vector< EMTauResult > m_RoIBResultEMTau;            //!< result from calo system: EmTauResult
    std::vector< L1TopoResult> m_RoIBResultL1Topo;           //!< result from L1Topo

    unsigned int GetOverflowMask(bool setMu, bool setEM, bool setJE) const;

  }; // class RoIBResult

} // namespace ROIB


/* The RoIBResult class_def part, defining a CLID of 6000 */
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ROIB::RoIBResult , 6000 , 0 )

#endif // TRIGT1RESULT_ROIBRESULT_H
