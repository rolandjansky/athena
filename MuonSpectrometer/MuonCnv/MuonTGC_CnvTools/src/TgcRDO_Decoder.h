/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CNVTOOLS_TGCRDO_DECODER_H
#define MUONTGC_CNVTOOLS_TGCRDO_DECODER_H

#include "MuonTGC_CnvTools/ITGC_RDO_Decoder.h"

#include "AthenaBaseComps/AthAlgTool.h"

#include "CxxUtils/checker_macros.h"

class TgcDigit;
class TgcRawData;
class Identifier;
class ITGCcablingSvc;

namespace Muon 
{
  
  /** @class TgcRDO_Decoder 
   *  This class provides conversion from TGC RDO data to 
   *  TGC Digits and offline ID. 
   *
   *  @author Susumu Oda <Susumu.Oda@cern.ch>
   *
   *  This class was developed by Tadashi Maeno based on 
   *  CscRDO_Decoder written by Ketevi A. Assamagan. 
   */

  class TgcRDO_Decoder : virtual public ITGC_RDO_Decoder, public AthAlgTool 
    {
    public:
      
      /** Constructor */
      TgcRDO_Decoder(const std::string& type, const std::string& name, 
		     const IInterface* parent);
      /** Destructor */
      ~TgcRDO_Decoder() {}

      /** Initializer */
      virtual StatusCode initialize();
      /** Finalizer */
      virtual StatusCode finalize();

      /** Set a flag for application of patch.
       *  Need to describe when patch is required. */
      void applyPatch(bool patch);
      /** Get TGC Digit from TGC RDO */
      TgcDigit * getDigit(const TgcRawData * rawData, bool orFlag) const;
      /** Get offline ID and bcTag from TGC RDO */
      Identifier getOfflineData(const TgcRawData * rawData, bool orFlag, uint16_t& bctag) const;

    private:
      StatusCode getCabling();

      const ITGCcablingSvc *m_cabling;
      bool m_applyPatch;

    };

} // end of namespace

#endif // MUONTGC_CNVTOOLS_TGCRDO_DECODER_H
