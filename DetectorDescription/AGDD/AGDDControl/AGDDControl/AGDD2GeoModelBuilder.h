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

class AGDDDetectorStore;
class AGDDVolumeStore;
class AGDDSectionStore;
class AliasStore;
class AGDDMaterialStore;


#include <string>

class AGDD2GeoModelBuilder:public AGDDBuilder {
public:
	// constructor
	AGDD2GeoModelBuilder(AGDDDetectorStore& ds,
                             AGDDVolumeStore& vs,
                             AGDDSectionStore& ss,
                             AliasStore& as,
                             AGDDMaterialStore& ms);
	~AGDD2GeoModelBuilder()=default;
	// elements & materials 
	GeoElement* CreateElement(const std::string&);
	const GeoMaterial* CreateMaterial(const std::string&);
	virtual void CreateElements() override;
	virtual void CreateMaterial() override;
		
	virtual void CreateBox(AGDDBox*) const override;
	virtual void CreateTrd(AGDDTrd*) const override;
	virtual void CreateTubs(AGDDTubs*) const override;
	virtual void CreateElcyl(AGDDElcyl*) const override;
	virtual void CreateCons(AGDDCons*) const override;
	virtual void CreatePcon(AGDDPcon*) const override;
	virtual void CreatePgon(AGDDPgon*) const override;
	virtual void CreateGvxy(AGDDGvxy*) const override;
	virtual void CreateSnake(AGDDSnake*) const override;
	virtual void CreateUnion(AGDDUnion*) const override;
	virtual void CreateIntersection(AGDDIntersection*) const override;
	virtual void CreateSubtraction(AGDDSubtraction*) const override;
	
	virtual void CreateBolt(AGDDBolt*) const override;
	virtual void CreateIbeam(AGDDIbeam*) const override;
	virtual void CreateUbeam(AGDDUbeam*) const override;
	
	virtual void CreateVolume(AGDDVolume*) override;
	virtual void CreateComposition(AGDDComposition*) override;

	virtual void SetMotherVolume(GeoPhysVol *v) {m_mother=v;}
	virtual void BuildAllVolumes();
	virtual void BuildFromSection(const std::string&) override;
	virtual void BuildFromVolume(const std::string&) override;

        virtual const StoredMaterialManager& GetMaterialManager() const override;

private:
	GeoPhysVol *m_mother;
        AGDDDetectorStore& m_ds;
        AGDDVolumeStore& m_vs;
        AGDDSectionStore& m_ss;
        AliasStore& m_as;
        AGDDMaterialStore& m_ms;

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
