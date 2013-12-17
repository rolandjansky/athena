/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKRECORD_P1_H
#define TRACKRECORD_P1_H
#include <string>

class TrackRecord_p1 {
public:
	TrackRecord_p1()  {};
        int PDG_code() const {return m_PDG_code;}
        float energy() const {return m_energy;}
        float momentumX() const {return m_momentumX;}
        float momentumY() const {return m_momentumY;}
        float momentumZ() const {return m_momentumZ;}
        float positionX() const {return m_positionX;}
        float positionY() const {return m_positionY;}
        float positionZ() const {return m_positionZ;}
        float time() const {return m_time;}
        int barCode() const {return m_barCode;}
        std::string volName() const {return m_volName;}
        friend class TrackRecordCnv_p1;

private:
	int m_PDG_code;
	float m_energy;
        float m_momentumX, m_momentumY, m_momentumZ;
        float m_positionX, m_positionY, m_positionZ;
      	float m_time;
	int m_barCode;
	std::string m_volName;
};

#endif
