/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ******************************************************************************
// -----------------------------------------
// ******************************************************************************

//<doc><file> $Id: TgcReadoutParams.h,v 1.2 2009-02-24 16:47:20 dwright Exp $
//<version>   $Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_TGCREADOUTPARAMS_H
#define MUONGEOMODEL_TGCREADOUTPARAMS_H

// ******************************************************************************
// class TgcReadoutParams
// ******************************************************************************
//
// Description
// -----------
// Provides access to the TGC readout parameters.  Created from one of the
// "sources": TGC_ZebraDetDescrSource, TGC_NovaDetDescrSource, etc.  One
// object is instantiated per chamber type and is pointed to by the
// TGC_DetDescriptor object corresponding to that type.
//
// Inheritance
// -----------
// None.
//
// Dependencies
// ------------
// None.
//
// Author
// ------
// Steven Goldfarb <Steven.Goldfarb@cern.ch>
//
// ******************************************************************************

#include <string>

class MsgStream;
class IMessageSvc;

namespace MuonGM {

class TgcReadoutParams
{
public:

   TgcReadoutParams(std::string name, int iCh, int Version, float WireSp, const float NCHRNG,
                    const float* NWGS, const float* IWGS1, const float* IWGS2, const float* IWGS3,
                    const float* ROFFST, const float* NSPS, const float* POFFST, IMessageSvc* msgSvc);

   // Another constructor for the layout Q
   TgcReadoutParams(std::string name, int iCh, int Version, float WireSp, const int NCHRNG,
                    const float* NWGS, const float* IWGS1, const float* IWGS2, const float* IWGS3,
                    float PDIST, const float* SLARGE, const float* SSHORT,
                    const float* ROFFST, const float* NSPS, const float* POFFST, IMessageSvc* msgSvc);

   ~TgcReadoutParams();

   inline const std::string GetName() const;
   int chamberType() const;
   int readoutVersion() const;
   int nPhiChambers() const;
   int nGaps() const;

   // Access to wire gang parameters

   float wirePitch() const;
   float gangThickness() const;
   int nGangs(int gasGap) const;
   int totalWires(int gasGap) const;
   int nWires(int gasGap, int gang) const;
   int gangOffset(int gasGap) const;

   // Access to strip parameters

   float stripThickness() const;
   int nStrips(int gasGap) const;
   float stripOffset(int gasGap) const;
   float physicalDistanceFromBase() const;
   float stripPositionOnLargeBase(int strip) const;
   float stripPositionOnShortBase(int strip) const;

private:

   // Must be built with the parameters
   TgcReadoutParams();

   // Copy constructor and equivalence operator
   TgcReadoutParams(const TgcReadoutParams& other);
   TgcReadoutParams &operator= (const TgcReadoutParams& other);

   // Readout array sizes
   enum TgcReadoutArraySizes
    {
        MaxNGaps   =   3,
        MaxNGangs  = 180,
        MaxNStrips =  33
    };

   // Data members
   std::string m_chamberName;
   int m_chamberType;
   int m_readoutVersion;
   float m_wirePitch;
   int m_nPhiChambers;

   int m_nGangs[MaxNGaps];
   int m_nWires[MaxNGaps][MaxNGangs];
   int m_gangOffset[MaxNGaps];
   int m_nStrips[MaxNGaps];
   float m_stripOffset[MaxNGaps];
   int m_totalWires[MaxNGaps]; 

   // strip postion on the bases for the first layer in +Z
   float m_physicalDistanceFromBase;
   float m_stripPositionOnLargeBase[MaxNStrips];
   float m_stripPositionOnShortBase[MaxNStrips];

   // Hard-coded data
   static float s_gangThickness;
   static float s_stripThickness;

   // message stream pointer and method to access it 
   MsgStream* m_MsgStream;
   inline MsgStream& reLog() const;

};

MsgStream& TgcReadoutParams::reLog() const {return *m_MsgStream;} 

const std::string TgcReadoutParams::GetName() const
{return m_chamberName;}

} // namespace MuonGM

#endif // MUONGEOMODEL_TGCREADOUTPARAMS_H
