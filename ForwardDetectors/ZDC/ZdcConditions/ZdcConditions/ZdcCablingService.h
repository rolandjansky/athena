/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCCABLINGSERVICE_H
#define ZDCCABLINGSERVICE_H

// This class provides conversion between logical and hardware ID
// Implemented as a singleton

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "ZdcIdentifier/ZdcID_Exception.h"
#include "ZdcIdentifier/ZdcID.h"
#include "ZdcIdentifier/ZdcHardwareID.h"
#include <vector>

class ZdcCablingService
{
  //friend class ZdcInfoLoader;
  //friend class ZdcDetectorTool;
  //friend class ZdcDetectorFactory;
  friend class ZdcCablingSvc;
  //friend class ZdcROD_Decoder;

public:

    /** get pointer to service instance*/
    static ZdcCablingService * getInstance() ;

    /** delete service instance */
    static void deleteInstance() ;

    // Conversion between ZdcID and ZdcHWID

    /*
    Identifier          h2s_side_id             ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    Identifier          h2s_module_id           ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    Identifier          h2s_type_id             ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    Identifier          h2s_gain_id             ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    Identifier          h2s_delay_id            ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    */

    //Identifier          h2s_channel_id          ( const HWIdentifier & id ) const throw (ZdcID_Exception);
    Identifier          h2s_channel_id          ( int crate, int channel ) const;

    //HWIdentifier        s2h_ppm_hwid              ( const Identifier & id ) const;
    //HWIdentifier        s2h_channel_hwid          ( const Identifier & id ) const;// throw (ZdcID_Exception);

    int          hwid2side               ( int crate, int channel ) const;
    int          hwid2module               ( int crate, int channel ) const;
    int          hwid2type               ( int crate, int channel ) const;
    int          hwid2channel               ( int crate, int channel ) const;
    int          hwid2gain               ( int crate, int channel ) const;
    int          hwid2delay               ( int crate, int channel ) const;
    int          hwid2hv               ( int crate, int channel ) const;

    //int          swid2ppm                ( int side, int module, int type ) const;
    //int          swid2channel            ( int side, int module, int type, int channel, int gain, int delay) const;


 protected:

    /** Default constructor protected */
    ZdcCablingService() ;

    /** Destructor protected */
    virtual ~ZdcCablingService() ;


 private:

    bool        m_dbFilled;
    int         m_side_db[4][64];
    int         m_module_db[4][64];
    int         m_type_db[4][64];
    int         m_gain_db[4][64];
    int         m_delay_db[4][64];
    int         m_channel_db[4][64];
    int         m_hv_db[4][64];
    int         m_ppm_db[16];
    int         m_crate_db[8];
    int         m_crate_index[4];
    int         m_ncrate;
    //int         m_crate_lookup[2][3][2];
    //int         m_channel_lookup[2][3][2][16][2][2];
    //int         crate_index_lookup(int crate);


    const ZdcID*      m_zdcID;
    const ZdcHardwareID*    m_zdcHWID;


    enum ZdcCablingType { TestBeam = -1,
			  Sim    = 0,
			  FirstData = 1};

    ZdcCablingType m_cablingType;

 public:

    static ZdcCablingService * s_zdcCablingService ;

    void setCablingType (ZdcCablingType type);

    int m_connected[16][16];
    void setConnected(int ppm, int channel);
    void fillConnectionTables();
    void fillDB();

    void setZdcID   (const ZdcID* zdcID)     {m_zdcID   = zdcID;}
    void setZdcHWID (const ZdcHardwareID* zdcHWID) {m_zdcHWID = zdcHWID;}

    inline const ZdcID*       getZdcID()         const { return m_zdcID; }
    inline const ZdcHardwareID*     getZdcHWID()       const { return m_zdcHWID; }
    inline int                 getCablingType()    const { return m_cablingType; }
    inline int crate_index_lookup(int& crate) const
      {
	int ic=0;
	while (1)
	  {
	    if (m_crate_index[ic]==crate) return ic;
	    ic++;
	    if (ic==m_ncrate) return -1;
	  }
	return -1;
      }

};

#endif //ZDCCABLINGSERVICE_H

