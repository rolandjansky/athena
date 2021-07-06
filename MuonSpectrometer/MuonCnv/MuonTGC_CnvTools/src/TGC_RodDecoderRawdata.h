/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TGC_RodDecoderRawdata.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTGC_CNVTOOLS_TGC_RODDECODERRAWDATA_H
#define MUONTGC_CNVTOOLS_TGC_RODDECODERRAWDATA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonTGC_CnvTools/ITGC_RodDecoder.h"

class TgcRdo;

namespace Muon
{ 
  class TgcRODReadOut;


  /** @class TGC_RodDecoderRawdata
   *  A tool to decode a TGC ROB fragment written in the raw data format into TGC RDO.
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *  @author Hisaya Kurashige 
   *
   *  This class was originally developed by Tadashi Maeno based on RpcROD_Decoder 
   *  written by Ketevi A. Assamagan. Hisaya Kurashige removed TGC ROD Encoder from 
   *  this and updated decodeRodToRdo in January 2008. Takashi Kubota migrated to 
   *  MuonTGC_CnvTools package in July 2008. The previous class name was TgcROD_Decoder. 
   */
  
class TGC_RodDecoderRawdata : public extends<AthAlgTool, ITGC_RodDecoder>
    {
    public:
      /** Default constructor */
      TGC_RodDecoderRawdata(const std::string& t, const std::string& n, const IInterface* p);
      /** Default destructor */
      virtual ~TGC_RodDecoderRawdata();
  
      /** Standard AlgTool method */
      virtual StatusCode initialize() override;
      /** Standard AlgTool method */
      virtual StatusCode finalize() override;
      /** Convert ROBFragment to RDO */
      virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, TgcRdoContainer& rdoIdc) const override;

    private:
      /** Retrieve header of ROBFragment */
      TgcRdo* getCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag, TgcRdoContainer& rdoIdc) const;
      /** Convert data contents of ROBFragment to RDO 
       *  The same byteStream2Rdo method in TGC_RodDecoderReadout is used for checking conversion validation */
      void byteStream2Rdo(OFFLINE_FRAGMENTS_NAMESPACE::PointerType bs, TgcRdo& rdo, uint32_t source_id) const;
      /** Show status words */
      void showStatusWords(const uint32_t source_id, const uint16_t rdoId, const int idHash, 
			   const uint32_t nstatus, const uint32_t* status) const;
      /** Switch for reading IDs in SLB Header or ROD Header */
      bool m_readSlbHeaderId;
      /** Switch for checking rawdata format with readout format */
      bool m_checkRawData;
      /** RawData format converter */
      TgcRODReadOut* m_tgcRODReadOut;
      /** Flag for showStatusWords */
      bool m_showStatusWords;
    };

} // end of namespace 

#endif // MUONTGC_CNVTOOLS_TGC_RODDECODERRAWDATA_H
