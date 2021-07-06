/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCluster.cxx
//   Implementation file for class PixelCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "InDetPrepRawData/PixelCluster.h"
// forward includes
#include "GaudiKernel/MsgStream.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include <ostream>
#include <sstream>

// Set maximum charge for calibrated clusters
// Needed as protection to avoid FPE in transient-to-persistent
// converters. There, to save disk space, charge is stored as an int
// up to 0xFFFFFF. Higher charges have no meaning anyhow (may only come from 
// big clusters with no position information). 
#define MAXCHARGE (float(0xFFFFFF))

namespace InDet{


    PixelCluster::PixelCluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX& locErrMat,
                const float omegax,
                const float omegay,
                bool split,
                float splitProb1,
                float splitProb2
              ) :
                SiCluster(RDOId, locpos, rdoList, width, detEl, locErrMat) //call base class constructor
    {
      m_omegax = omegax;
      m_omegay = omegay;
      m_totList.clear();
      m_totalToT=0;
      m_chargeList.clear();
      m_totalCharge=0;
      m_fake = false;
      m_ambiguous = false;
      m_lvl1 = 0;
      m_tooBigToBeSplit = false;

      packSplitInformation(split,splitProb1,splitProb2);
        
    }
    
      
    PixelCluster::PixelCluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const int lvl1a,
                const std::vector<int>& totList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX& locErrMat,
                const float omegax,
                const float omegay,
                bool split,
                float splitProb1,
                float splitProb2
              ) :
      SiCluster(RDOId, locpos, rdoList, width, detEl, locErrMat), //call base class constructor
      m_omegax (omegax),
      m_omegay (omegay),
      m_totList (totList),
      m_totalToT (0),
      m_chargeList(),
      m_totalCharge (0),
      m_fake (false),
      m_ambiguous (false),
      m_lvl1 (lvl1a),
      m_tooBigToBeSplit (false)
    {
      int n = m_totList.size();
      for (int i=0; i<n; i++) m_totalToT+=int(totList[i]);
      packSplitInformation(split,splitProb1,splitProb2);
    
    }
    
    PixelCluster::PixelCluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
    	          const int lvl1a,
    	          const std::vector<int>& totList,
    	          const std::vector<float>& chargeList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX& locErrMat,
                const float omegax,
                const float omegay,
                bool split,
                float splitProb1,
                float splitProb2
              ) :
      SiCluster(RDOId, locpos, rdoList, width, detEl, locErrMat), //call base class constructor
      m_omegax (omegax),
      m_omegay (omegay),
      m_totList (totList),
      m_totalToT (0),
      m_chargeList (chargeList),
      m_totalCharge (0),
      m_fake (false),
      m_ambiguous (false),
      m_lvl1 (lvl1a),
      m_tooBigToBeSplit (false)
    {
      int n = m_totList.size();
      for (int i=0; i<n; i++) m_totalToT+=int(totList[i]);
      n = m_chargeList.size();
      for (int i=0; i<n; i++) m_totalCharge+=chargeList[i];
      if ( m_totalCharge>MAXCHARGE ) m_totalCharge=MAXCHARGE;
      packSplitInformation(split,splitProb1,splitProb2);
    }
 
    PixelCluster::PixelCluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const Amg::Vector3D& globpos,
                const std::vector<Identifier>& rdoList,
    	          const int lvl1a,
    	          const std::vector<int>& totList,
    	          const std::vector<float>& chargeList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX& locErrMat,
                const float omegax,
                const float omegay,
                bool split,
                float splitProb1,
                float splitProb2
              ) :
      SiCluster(RDOId, locpos, globpos, rdoList, width, detEl, locErrMat), //call base class constructor
      m_omegax (omegax),
      m_omegay (omegay),
      m_totList (totList),
      m_totalToT (0),
      m_chargeList (chargeList),
      m_totalCharge (0),
      m_fake (false),
      m_ambiguous (false),
      m_lvl1 (lvl1a),
      m_tooBigToBeSplit (false)
    {
      int n = m_totList.size();
      for (int i=0; i<n; i++) m_totalToT+=int(totList[i]);
      n = m_chargeList.size();
      for (int i=0; i<n; i++) m_totalCharge+=chargeList[i];
      if ( m_totalCharge>MAXCHARGE ) m_totalCharge=MAXCHARGE;
      packSplitInformation(split,splitProb1,splitProb2);
    }
    
    PixelCluster::PixelCluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                std::vector<Identifier>&& rdoList,
                const int lvl1a,
                int totalToT,
                const std::vector<float>& chargeList,
                float totalCharge,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                Amg::MatrixX&& locErrMat,
                const float omegax,
                const float omegay,
                int splitInfoRaw
              ) :
      SiCluster(RDOId, locpos,
                rdoList,
                width, detEl, locErrMat), //call base class constructor
      m_omegax (omegax),
      m_omegay (omegay),
      m_totList(),
      m_totalToT (totalToT),
      m_chargeList (chargeList),
      m_totalCharge (totalCharge),
      m_fake (false),
      m_ambiguous (false),
      m_lvl1 (lvl1a),
      m_splitInfo (splitInfoRaw),
      m_tooBigToBeSplit (false)
    {
    }
    
    // Default constructor:
    PixelCluster::PixelCluster():
      SiCluster(),
      m_omegax (-1),
      m_omegay (-1),
      m_totalToT (0),
      m_totalCharge (0),
      m_fake (false),
      m_ambiguous (false),
      m_lvl1 (0),
      m_splitInfo (0),
      m_tooBigToBeSplit (false)
    {
    }
    
    bool 
    PixelCluster::type(Trk::PrepRawDataType type) const{
      return (type == Trk::PrepRawDataType::PixelCluster or type == Trk::PrepRawDataType::SiCluster);
    }

    

    MsgStream& PixelCluster::dump( MsgStream&    stream) const
    {
        std::ostringstream out;
        dump(out);
        stream<<out.str();
        return stream;
    }
    
    std::ostream& PixelCluster::dump( std::ostream&    stream) const
    {
        const std::string lf{"\n"};
        stream << "PixelCluster object"<<lf;
        stream << "omegax: " << m_omegax << "	omegay: " << m_omegay << lf;
        stream << "total ToT: " << m_totalToT << ", calibrated charge =" << m_totalCharge << lf;
        stream << "split information: " << (isSplit() ? "split" : "not split" );
        stream << ", probabilities " << splitProbability1() << ", " << splitProbability2() << lf;
        stream << "Base class (SiCluster):" << lf;
        this->SiCluster::dump(stream);
    
        return stream;
    }
    
    
    MsgStream&    operator << (MsgStream& stream,    const PixelCluster& prd)
    {
        return prd.dump(stream);
    }
    
    std::ostream& operator << (std::ostream& stream, const PixelCluster& prd)
    {
        return prd.dump(stream);
    }


    void PixelCluster::setToTList (std::vector<int>&& totList)
    {
      m_totList = std::move (totList);
      m_totalToT = 0;
      for (int f : m_totList) {
        m_totalToT += f;
      }
    }


} // end of namespace InDet
