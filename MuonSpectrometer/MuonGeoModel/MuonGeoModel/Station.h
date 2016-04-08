/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Station_H
#define Station_H

#include <string>
#include <vector>
#include <map>
#include "MuonGeoModel/Component.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/Position.h"
#include "MuonGeoModel/AlignPos.h"
#include "CLHEP/Geometry/Transform3D.h"

/*
   This class holds an std::map of Position, AlignPos and an std::vector of 
   Comonents* and Cutouts*
   It is used only to build the geometry - can be deleted as soon as the job 
   is done (in MYSQL).
   It is responsible for releasing the memory allocated by Components and Cutouts
*/


class IMessageSvc;

namespace MuonGM {


typedef std::map<int, Position, std::less<int> > PositionMap;
typedef std::multimap<int, AlignPos, std::less<int> > AlignPosMap;
typedef std::map<int, Position, std::less<int> >::const_iterator PositionIterator;
typedef std::multimap<int, AlignPos, std::less<int> >::const_iterator AlignPosIterator;


class Station {
public:
	Station();
	~Station();
	Station(const Station& s);
	Station(std::string name);
	void SetComponent(Component* c);
	void SetCutout(Cutout* c);
	Component* GetComponent(int i) const;
	Cutout* GetCutout(int i) const;
	int GetNrOfComponents() const;
	int GetNrOfCutouts() const;
	void SetPosition(Position p);
	void SetAlignPos(const AlignPos& p);
//	Position GetPosition(int i) const;
	PositionIterator begin() const;
	PositionIterator end() const;
	AlignPosIterator abegin() const;
	AlignPosIterator aend() const;
	PositionIterator FindPosition(int iz, int iphi) const;
	AlignPosIterator FindAlignPos(int iz, int iphi) const;
	int CountAlignPos(int iz, int iphi) const;
	AlignPosIterator getFirstAlignPosInRange(int iz, int iphi, AlignPosIterator& lastAlignPosInRange) const;
        int Npositions() const;
    //HepGeom::Transform3D  getTransform(const Position & p) const;
    //HepGeom::Transform3D getTransform(const AlignPos & ap) const;

    //{ the use for atlas setup is:
    // in MuonDetectorFactory
    // GeoAlignableTransform *xf = new GeoAlignableTransform( station->getNominalTransform( (*pit).second) );
    // apit = station->FindAlignPos(zi,fi);
    // Delta = station->getDeltaTransform( ap, (*pit).second );
    // xf->setDelta( Delta );
        HepGeom::Transform3D native_to_tsz_frame( const Position & p ) const;
        HepGeom::Transform3D tsz_to_native_frame( const Position & p ) const;
        HepGeom::Transform3D tsz_to_global_frame( const Position & p ) const;
        HepGeom::Transform3D global_to_tsz_frame( const Position & p ) const;
        HepGeom::Transform3D getNominalTransform( const Position & p ) const;
        HepGeom::Transform3D getDeltaTransform_tszFrame( const AlignPos & ap ) const;
        HepGeom::Transform3D getDeltaTransform( const AlignPos & ap, const Position & p ) const;
        HepGeom::Transform3D getAlignedTransform( const AlignPos & ap, const Position & p ) const;
    //HepGeom::Transform3D getAmdbOrgTrans(const Position & p) const;
    //}

    //	int NrOfPositionedStations();
	friend std::ostream& operator<<(std::ostream& os, const Station& s);
	std::string GetName() const;
	double GetThickness() const;
	double GetExtraBottomThickness() const;
	double GetExtraTopThickness() const;
	double GetLength() const;
	double GetWidth1() const;
	double GetWidth2() const;
    double getAmdbOrigine_along_length() const;
    double getAmdbOrigine_along_thickness() const;
    bool hasMdts() const {return m_hasMdts;} 
    void setHasMdts(bool x) {m_hasMdts = x;} 
    
    double mdtHalfPitch() const;

private:
    double getYMin() const;

	mutable double m_thickness;
	mutable double m_length;
        mutable double m_amdbOrigine_along_length;
        mutable double m_amdbOrigine_along_thickness;
	mutable double m_width1;
	mutable double m_width2;
	mutable double m_mdthalfpitch;
	std::string m_name;
        bool m_hasMdts;
	std::vector<Component *> m_components;
	std::vector<Cutout *> m_cutouts;
	PositionMap m_positions;
	AlignPosMap m_alignpositions;
	IMessageSvc*    m_msgSvc;
	Station & operator=(const Station &right);
};
} // namespace MuonGM

#endif
