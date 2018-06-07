// emacs: this is -*- c++ -*-
//
//   @file    TrigRoiDescriptor.h        
//
//             RoiDescriptor class - includes constructors in preparation
//             for enforcement of correct roi sizes       
//  
//   @author sutt@cern.ch
//   Copyright (C) 2013 M.Sutton (sutt@cern.ch)    
//
//   $Id: TrigRoiDescriptor.h, v0.1   Fri 08 Jun 2013 23:52:09 CEST sutt $


/// nope - should be used for standalone also, perhaps need to 
///        protect the class def bits
/// #ifndef XAOD_ANALYSIS
/// #if !defined(TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H)  && !defined(XAOD_STANDALONE)

#ifndef TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H
#define TRIGSTEERINGEVENT_TRIGROIDESCRIPTOR_H

#include <stdint.h>
#include <map>
#include <ostream>

// can we leave this in for standalone running?
// #ifndef XAOD_STANDALONE
// #include "CLIDSvc/CLASS_DEF.h"
// #include "GaudiKernel/MsgStream.h"

#include "xAODCore/CLASS_DEF.h"

// #endif

#include "RoiDescriptor/RoiDescriptor.h"



/**
 * @brief Describes the Region of Ineterest geometry
 *  It has basically 9 parameters
 *
 * -# zed : z position of RoI
 * -# zedPlus  : the most forward zed position of the RoI at the beamline
 * -# zedMinus : the most backward zed position of the RoI at the beamline
 * -# phi : phi position of RoI
 * -# phiPlus  : phi at the most anticlockwise position of the RoI
 * -# phiMinus : phi at the most clockwise position of the RoI
 * -# eta : pseudo-rapidity of RoI at zed
 * -# etaPlus  : pseudo-rapidity at zedPlus
 * -# etaMinus : pseudo-rapidity at zedMinus
 * \warning An attempt to cnstruct the objects of this calss with phi0 out of allowed range reasults in throwing exception
 */

class TrigRoiDescriptor : public RoiDescriptor {

public:

  /**
   * @brief default constructor
   * @param fullscan      if fullscan is true, this RoI will span the entire detector
   */
  TrigRoiDescriptor(bool fullscan=false);

  /**
   * @brief constructor
   * @param eta      eta of RoI
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phi      phi of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   * @param zed      zed of RoI
   * @param zedMinus zed at rear  of RoI
   * @param zedPlus  zed at front of RoI
   */
  TrigRoiDescriptor(double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);
  
  /**
   * @brief constructor
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta      eta of RoI
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phi      phi of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   * @param zed      zed of RoI
   * @param zedMinus zed at rear  of RoI
   * @param zedPlus  zed at front of RoI
   */
  TrigRoiDescriptor(unsigned int l1id, unsigned int id, 
		    double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);

  /**
   * @brief constructor
   * @param roiWord LVL1 trigger decision word
   * @param l1id is event number asigned by LVL1 trigger
   * @param id RoI number within event (0,1,.....)
   * @param eta      eta of RoI
   * @param etaMinus eta at rear  of RoI
   * @param etaPlus  eta at front of RoI
   * @param phi      phi of RoI
   * @param phiMinus minimum phi of RoI
   * @param phiPlus  maximum phi of RoI
   * @param zed      zed of RoI
   * @param zedMinus zed at rear  of RoI
   * @param zedPlus  zed at front of RoI
   */
  TrigRoiDescriptor(unsigned int roiWord, unsigned int l1id, unsigned int id, 
		    double eta, double etaMinus, double etaPlus, 
		    double phi, double phiMinus, double phiPlus, 
		    double zed=0, double zedMinus=-zedWidthDefault, double zedPlus=zedWidthDefault);


  TrigRoiDescriptor( const IRoiDescriptor& roi );

  TrigRoiDescriptor& operator=( const IRoiDescriptor& roi );

  // Destructor
  ~TrigRoiDescriptor();

  unsigned int roiId() const { return m_roiId; }
  unsigned int l1Id() const { return m_l1Id; }
  unsigned int roiWord() const { return m_roiWord; }

  operator std::string() const;

protected:

  unsigned int m_l1Id;          //!< lvl1 event number
  unsigned int m_roiId;         //!< RoI number
  unsigned int m_roiWord;       //!< lvl1 RoI word from which this RoI was initially constructed

  std::vector<uint32_t> m_serialized;

};


std::string str( const TrigRoiDescriptor& d );                           //<! printing helper
std::ostream& operator<<( std::ostream& m, const TrigRoiDescriptor& d ); //<! printing helper (wraps above)


// can this be left in or should it be removed?
// #ifndef XAOD_STANDALONE
CLASS_DEF(TrigRoiDescriptor, 6455, 1)
// #endif

/// why is this included here? can't packages which need both 
/// include it explicitly? 
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#endif // TRIGROIDESCRIPTOR_H


// #endif //XAOD_ANALYSIS
