/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRIPCLUSTEROFFLINEDATA_H
#define STRIPCLUSTEROFFLINEDATA_H

// Identifier includes
#include "Identifier/Identifier.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonDigitContainer/sTgcDigit.h"
// local includes
#include "TrigT1NSWSimTools/StripClusterData.h"
#include <string.h>

// namespace for the NSW LVL1 related classes

namespace NSWL1 {


  /**
   *
   *   @short class modeling the strip hit fragment for the NSW L1 offline simulation
   *
   * This holds the hit offline identifier with the corresponding helper plus the following:
   * Trigger BCID, strip charge, band Id, phi Id. Additional data can be added providing the
   * relative access methods are also added in the abstract class.
   *
   *  @author Alessandro Di Mattia <dimattia@cern.ch>
   *
   *
   */

  class StripClusterOfflineData: public StripClusterData {

  private:
    int m_bandId;
    int m_phiId;
    int m_isSmall;
    int m_moduleId;
    int m_sectorId;
    int m_wedge;
    int m_layer;
    int m_size;
    float m_charge;
    float m_globX;
    float m_globY;
    float m_globZ;

  public:

    StripClusterOfflineData(int bandId,
                int phiId,
			    int isSmall,
			    int moduleId,
			    int sectorId,
			    int wedge,
			    int layer,
			    int size,
			    float charge,
			    float globX,
			    float globY,
			    float globZ);
    ~StripClusterOfflineData();
        
   //! methods for retrieving the bare data 
    /*
    S.I :  are you sure are these all for retrieving the data ? Someone seems to copy paste without spending even a single second to figure things out 
    and the bad news is, it all around this codebase
    */                                        
    int set_bandId(int bandId);
    int set_phiId(int phiId);
    bool set_isSmall(bool set_isSmall);
    int set_moduleId(int moduleId);
    int set_sectorId(int sectorId);
    int set_wedge(int wedge);
    int set_layer(int layer);
    int set_size(int size);
    float set_charge(float charge);
    float set_globX(float globX);
    float set_globY(float globY);
    float set_globZ(float globZ);
    


    //! methods for retrieving the bare data //S.I yes see my comment above
    int bandId()  const {return m_bandId;}; //!< get the band id // wow so explanatory
    int phiId() const {return m_phiId;}
    //! helper  methods decoding the hit fragment position within the NSW detector
    int isSmall() const {return m_isSmall;};
    int moduleId() const {return m_moduleId;};
    int sectorId() const {return m_sectorId;};
    int wedge() const {return m_wedge;};
    int layer() const {return m_layer;};
    int size() const {return m_size;};
    float charge() const {return m_charge;};
    float globX() const {return m_globX;};
    float globY() const {return m_globY;};
    float globZ() const {return m_globZ;};
     
  };  // end of StripClusterOfflineData class

} // namespace NSWL1

#endif
