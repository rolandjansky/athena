// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_MUCTPIL1TOPOCANDIDATE_H
#define TRIGT1INTERFACES_MUCTPIL1TOPOCANDIDATE_H

#include <string>

namespace LVL1 {

   /**
    *  @short MuCTPI input class to the L1Topo simulation
    *
    *         This class is used to hold one LVL1 muon candidate as sent
    *         from the MuCTPI simulation to the L1Topo simulation.
    *         The setter functions perform the conversion of number encodeing
    *         to eta/phi etc
    *
    * $author Thorsten Wengler 
    *
    */
   class MuCTPIL1TopoCandidate {

   public:
     //constructor 
     MuCTPIL1TopoCandidate();
     
     
     ~MuCTPIL1TopoCandidate() = default;
     
     // set candidate data
     void setCandidateData(std::string  sectorName, 
			   unsigned int roiID,     
			   unsigned int bcid,      
			   unsigned int ptThresholdID,  
			   unsigned int ptL1TopoCode,   
			   unsigned int ptValue,     
			   float eta,       
			   float phi,       
			   unsigned int etacode,   
			   unsigned int phicode,   
			   float etamin,    
			   float etamax,    
			   float phimin,    
			   float phimax,
			   unsigned int mioctID,
			   int ieta,
			   int iphi);

     // set the RoI word (done at a differnt stage of the simulation, hence the separate function
     void setRoiWord( unsigned int roi);

     //setter to set second candidate in Mioct pt to 3 (i.e. 11) in case of > 2 candidates (OverFlow)
     void setptL1TopoCodeToOvFl() { m_ptL1TopoCode=3; }
     /**
        @brief whether there was an overflow

        In the current encoding, the pt bits set to '11' are used to indicate >2 candidates in the Mioct
        https://twiki.cern.ch/twiki/bin/view/Atlas/LevelOneCentralTriggerUpgradePhase0Mioct#Close_to_final_format
      */
     bool moreThan2CandidatesOverflow() const { return m_ptL1TopoCode==3; }

     // getter functions
     std::string  getSectorName() const { return m_sectorName; }
     unsigned int getRoiID() const { return  m_roiID; }
     unsigned int getbcid() const { return m_bcid; }  
     unsigned int getptThresholdID() const { return m_ptThresholdID; }
     unsigned int getptL1TopoCode() const { return m_ptL1TopoCode; }  
     unsigned int getptValue() const { return m_ptValue; }  
     float geteta() const { return m_eta; }  
     float getphi() const { return m_phi; }  
     unsigned int getetacode() const { return m_etacode; }  
     unsigned int getphicode() const { return m_phicode; }  
     float getetamin() const { return m_etamin; }  
     float getetamax() const { return m_etamax; }  
     float getphimin() const { return m_phimin; }  
     float getphimax() const { return m_phimax; }  
     unsigned int getRoiWord() const { return m_roiWord; }  
     unsigned int getMioctID() const { return m_mioctID; }  
     int getieta() const { return m_ieta; }  
     int getiphi() const { return m_iphi; }  

     // formatted print for debugging 
     void print() const ;
     
   private:
     // properties of the candidate 
     std::string  m_sectorName;     // name of the muon sector in string format(e.g. B30, with: B=Barrel, 
                                    // EA=Endcap A-side, EC=Endcap C side, FA=Forward A-side, FC=Forward C-side)
     unsigned int m_roiID;          // ID of region of Interest for this candidate
     unsigned int m_bcid;           // BCID of this candidate
     unsigned int m_ptThresholdID;  // pt threshold passed (1 to 6)
     unsigned int m_ptL1TopoCode;   // pt threshold in 2 bit L1Topo encoding  
     unsigned int m_ptValue;        // Value (GeV) of pt threshold passed 
     float m_eta;            // eta value for this candidate
     float m_phi;            // phi value for this candidate
     unsigned int m_etacode;        // eta encoded as in the output numbering scheme of MuCTPI to L1Topo
     unsigned int m_phicode;        // phi encoded as in the output numbering scheme of MuCTPI to L1Topo
     float m_etamin;         // eta min of this eta/phi box encoded
     float m_etamax;         // eta max of this eta/phi box encoded
     float m_phimin;         // phi min of this eta/phi box encoded
     float m_phimax;         // phi max of this eta/phi box encoded
     unsigned int m_roiWord; // 32 bit Roi Word as sent from MuCTPI to RoIB
     unsigned int m_mioctID; // ID of the Octant board (MIOCT) that sent this candidate
     int m_ieta;             // ieta code for L1Topo
     int m_iphi;             // iphi code for L1Topo

   }; // class MuCTPIL1Topo

} // namespace LVL1

#endif // TRIGT1INTERFACES_MUCTPIL1TOPOCANDIDATE_H
