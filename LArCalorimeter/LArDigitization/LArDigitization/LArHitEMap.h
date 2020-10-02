//Dear emcas this is -*-c++-*--
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_LARHITEMAP_H
#define LARDIGITIZATION_LARHITEMAP_H
//
#include <utility>
#include <vector>

#include "LArDigitization/LArHitList.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArRawEvent/LArDigit.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaKernel/CLASS_DEF.h"


class McEventCollection;


class LArHitEMap
{

public: 
 typedef std::vector<const LArDigit*> DIGMAP;

 enum { EMBARREL_INDEX  = 0
       ,EMENDCAP_INDEX  = 1
       ,HADENDCAP_INDEX = 2
       ,FORWARD_INDEX   = 3
       ,NUMDET=4
     };

private:
  std::vector<LArHitList> m_emap; 
  DIGMAP m_digmap;
  const LArOnOffIdMapping* m_cabling;
  const CaloCell_ID* m_calocell_id;
  const CaloDetDescrManager* m_cddMgr;

public:
  LArHitEMap() = delete;
  LArHitEMap(const LArOnOffIdMapping* cabling, const CaloCell_ID* cellid, const CaloDetDescrManager* cddMgr, bool digit=false);
  ~LArHitEMap(void);
  bool AddEnergy(const IdentifierHash index, const float energy, const float time);
  bool AddEnergy(const Identifier cellid, const float energy, const float time);
  bool BuildWindows(const McEventCollection* mcCollptr,
                    float deta, float dphi, float ptmin);
  int GetNbCells(void) const;
  inline const LArHitList& GetCell(const unsigned int index) const {return m_emap[index];} ;
  inline const std::vector<std::pair<float,float> >& GetTimeE(const IdentifierHash index) const { return  m_emap[index].getData();}


  bool AddDigit(const LArDigit* digit);
  inline const LArDigit* GetDigit(unsigned int index) const {
    if (index<m_digmap.size()) return m_digmap[index];
    else return 0; 
  }

};

CLASS_DEF( LArHitEMap , 103846231 , 1 )
#endif
