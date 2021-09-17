/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDD2GeoModelBuilder_H
#define AGDD2GeoModelBuilder_H

#include "GeoModelKernel/GeoPhysVol.h"
#include "AGDDKernel/AGDDBuilder.h"

class GeoMaterial;
class GeoElement;
class AGDDComposition;
class AGDDBox;
class AGDDTubs;
class AGDDElcyl;
class AGDDTrd;
class AGDDCons;
class AGDDPcon;
class AGDDPgon;
class AGDDGvxy;
class AGDDSnake;
class AGDDUnion;
class AGDDIntersection;
class AGDDSubtraction;
class AGDDVolume;
class AGDDBolt;
class AGDDIbeam;
class AGDDUbeam;


#include <string>

class AGDD2GeoModelBuilder:public AGDDBuilder {
public:
	// constructor
	AGDD2GeoModelBuilder();
	~AGDD2GeoModelBuilder()=default;
	// elements & materials 
	GeoElement* CreateElement(const std::string&) const;
	const GeoMaterial* CreateMaterial(const std::string&) const;
	void CreateElements() const override;
	void CreateMaterial() const override;
		
	void CreateBox(AGDDBox*) const override;
	void CreateTrd(AGDDTrd*) const override;
	void CreateTubs(AGDDTubs*) const override;
	void CreateElcyl(AGDDElcyl*) const override;
	void CreateCons(AGDDCons*) const override;
	void CreatePcon(AGDDPcon*) const override;
	void CreatePgon(AGDDPgon*) const override;
	void CreateGvxy(AGDDGvxy*) const override;
	void CreateSnake(AGDDSnake*) const override;
	void CreateUnion(AGDDUnion*) const override;
	void CreateIntersection(AGDDIntersection*) const override;
	void CreateSubtraction(AGDDSubtraction*) const override;
	
	void CreateBolt(AGDDBolt*) const override;
	void CreateIbeam(AGDDIbeam*) const override;
	void CreateUbeam(AGDDUbeam*) const override;
	
	void CreateVolume(AGDDVolume*) const override;
	void CreateComposition(AGDDComposition*) const override;

	void SetMotherVolume(GeoPhysVol *v) {m_mother=v;}
	void BuildAllVolumes() const;
	void BuildFromSection(const std::string&) const override;
	void BuildFromVolume(const std::string&) const override;

private:
	GeoPhysVol *m_mother;
	const GeoMaterial* GetMMMaterial(const std::string&) const;

    /** phi method (cf. EventPrimitives/AmgMatrixBasePlugin.h) */
    inline double phi(const GeoTrf::Vector3D &vec) const {
        if (vec.rows() < 2) return 0.;
        return std::atan2(vec[1],vec[0]);
    }
    /** theta method (cf. EventPrimitives/AmgMatrixBasePlugin.h) */
    inline double theta(const GeoTrf::Vector3D &vec) const {
        if (vec.rows() < 3) return 0.;
        return std::atan2(std::hypot(vec[0],vec[1]),vec[2]);
    }
};

#endif
