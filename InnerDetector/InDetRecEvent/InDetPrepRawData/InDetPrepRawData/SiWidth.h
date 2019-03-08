/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiWidth.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETPREPRAWDATA_SIWIDTH_H
#define INDETPREPRAWDATA_SIWIDTH_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"

class MsgStream;

/** standard namespace for the Inner Detector */
namespace InDet {

  /** @class SiWidth

      @author Veronique.Boisvert
    */
  class SiWidth {

   ///////////////////////////////////////////////////////////////////
   // Public methods:
   ///////////////////////////////////////////////////////////////////
  public:

   // Implicit constructor:
   SiWidth();

   // Copy constructor:
   SiWidth(const SiWidth& position);

   // Constructor with parameters: <col, row> in units of RDOs (so should be int), 
   //                              <phiR width in mm, Z width in mm>
   SiWidth(const Amg::Vector2D& colrow, const  Amg::Vector2D& phiRZ);
   
   // online constructor: only the col, row in units of RDOs
   SiWidth(const Amg::Vector2D& colrow);

   // Destructor:
   virtual ~SiWidth();

   // Assignment operator:
   SiWidth &operator=(const SiWidth& width);

   ///////////////////////////////////////////////////////////////////
   // Const methods:
   ///////////////////////////////////////////////////////////////////

   // return col row:
   const Amg::Vector2D& colRow() const;

   // return phiRZ in mm:
   const Amg::Vector2D& widthPhiRZ() const;
   
   // return PhiR
   double phiR() const;

   // return z
   double z() const;


   ///////////////////////////////////////////////////////////////////
   // Non-const methods:
   ///////////////////////////////////////////////////////////////////

   void setColumn(const double col);

   void setRow(const double row);

   void setColRow(const Amg::Vector2D& colRow);

   void setPhirWidth(const double phirWidth);

   void setZWidth(const double zWidth);

   void setPhirzWidth(const Amg::Vector2D& phirzWidth);


  // addition

  /** dump information about the PRD object. */
  virtual MsgStream&    dump( MsgStream&    stream) const;

  /** dump information about the PRD object. */
  virtual std::ostream& dump( std::ostream& stream) const;


  //scaling

   ///////////////////////////////////////////////////////////////////
   // Private data:
   ///////////////////////////////////////////////////////////////////
 private:
   // Need to force proper alignment; otherwise cling gets it wrong.
   alignas(16) Amg::Vector2D m_colrow;//<col, row>
   alignas(16) Amg::Vector2D m_phirzWidth;
 };

 MsgStream&    operator << (MsgStream& stream,    const SiWidth& prd);
 std::ostream& operator << (std::ostream& stream, const SiWidth& prd);


  ///////////////////////////////////////////////////////////////////
  // Inline methods:
  ///////////////////////////////////////////////////////////////////
  inline const Amg::Vector2D& SiWidth::colRow() const
  {
  return (m_colrow);
  }


  inline const Amg::Vector2D& SiWidth::widthPhiRZ() const
  {
    return m_phirzWidth;        
  }

  inline double SiWidth::phiR() const
  {
    return m_phirzWidth[Trk::locPhiR]; 
  }

  inline double SiWidth::z() const
  {
    return m_phirzWidth[Trk::locZ];    
  }
  
  inline void SiWidth::setColumn(const double col)
  {
   m_colrow[0] = col;
  }

  inline void SiWidth::setRow(const double row)
  {
   m_colrow[1] = row;
   }

  inline void SiWidth::setColRow(const Amg::Vector2D& colRow)
  {
   m_colrow=colRow;
  }

  inline void SiWidth::setPhirWidth(const double phir)
  {
    m_phirzWidth[Trk::locPhiR] = phir;
  }
  inline void SiWidth::setZWidth(const double zwidth)
  {
   m_phirzWidth[Trk::locZ] = zwidth;
  }
  inline void SiWidth::setPhirzWidth(const Amg::Vector2D& phirzwidth)
  {
    m_phirzWidth = phirzwidth;
  }


  ///////////////////////////////////////////////////////////////////
  // Binary operators:
  ///////////////////////////////////////////////////////////////////

} // end of namespace

#endif // INDETPREPRAWDATA_SIWIDTH_H
