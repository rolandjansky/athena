/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <stdexcept>
#include <algorithm> // For upper_bound
#include <iterator>  // for std::distance()
#include <cmath>
#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"
#include "Identifier/Identifier.h"
#include "TrkSurfaces/AnnulusBounds.h"
#include "TrkSurfaces/AnnulusBoundsPC.h"

using namespace std;



namespace InDetDD {
StripStereoAnnulusDesign::StripStereoAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                               const SiDetectorDesign::Axis &thicknessDirection,
                               const double &thickness,
                               const int &readoutSide,
                               const InDetDD::CarrierType &carrier,
                               const int &nRows,
                               const std::vector<int> &nStrips,
                               const std::vector<double> &pitch,
                               const std::vector<double> &stripStartRadius,
                               const std::vector<double> &stripEndRadius,
                               const double &stereoAngle,
                               const double &centreR,
                               const double &waferCentreR,
                               const bool &usePC) :
    SCT_ModuleSideDesign(thickness, false, false, true, 1, 0, 0, 0, false, carrier,
                         readoutSide, stripDirection, thicknessDirection),
  m_nRows(nRows),
  m_nStrips(nStrips),
  m_pitch(pitch),
  m_stripStartRadius(stripStartRadius),
  m_stripEndRadius(stripEndRadius),
  m_stereo(stereoAngle),
  m_cos_minus_stereo(std::cos(-m_stereo)),
  m_sin_minus_stereo(std::sin(-m_stereo)),
  m_cos_plus_stereo(m_cos_minus_stereo),
  m_sin_plus_stereo(-m_sin_minus_stereo),
  m_R(centreR),
  m_waferCentreR(waferCentreR),//if not specified in constructor, wafer centre assumed to simply be element centre 
  m_lengthBF(2. * waferCentreR * std::sin(stereoAngle*0.5)), // Eq. 5 p. 7
  m_usePC(usePC)
{
    if (nRows < 0) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with negative number of rows");
    }

    if (pitch.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant pitch values for no. of rows");
    }

    if (stripStartRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant strip start-positions for no. of rows");
    }

    if (stripEndRadius.size() != (unsigned) nRows) {
        throw std::runtime_error(
                  "ERROR: StripStereoAnnulusDesign called with insufficiant strip end-positions for no. of rows");
    }

    int startStrip = 0;
    for (int r = 0; r < nRows; ++r) {
        m_firstStrip.push_back(startStrip);
        startStrip += m_nStrips[r];
    }
    int totalStrips = startStrip;
    m_firstStrip.push_back(totalStrips); // Add one more as total number of strips, so you can safely use row + 1 in a loop

    m_scheme.setCells(totalStrips);
    m_scheme.setDiodes(totalStrips);

    double phi = m_nStrips[0] * m_pitch[0];

    if (m_usePC) {
        // Maths for calculating PC bounds is based off Trk::AnnulusBoundsPC::fromCartesian() and detailed at https://hep.ph.liv.ac.uk/~jsmith/dropbox/AnnulusBoundsPC_Constructor_Maths.pdf. This will later form some kind of INT Note or report or similar internal documentation,
        Amg::Vector2D origin(m_waferCentreR * (1.0 - std::cos(m_stereo)) , m_waferCentreR*std::sin(-m_stereo));
        m_bounds = std::make_unique<Trk::AnnulusBoundsPC>(Trk::AnnulusBoundsPC(m_stripStartRadius[0], m_stripEndRadius.back(),phi*-0.5,phi*0.5,origin,0));
    } else {
        m_bounds = std::make_unique<Trk::AnnulusBounds>(Trk::AnnulusBounds(m_stripStartRadius[0], m_stripEndRadius.back(), m_waferCentreR, phi, m_stereo));
    }
}

StripStereoAnnulusDesign::StripStereoAnnulusDesign(const SiDetectorDesign::Axis &stripDirection,
                               const SiDetectorDesign::Axis &thicknessDirection,
                               const double &thickness,
                               const int &readoutSide,
                               const InDetDD::CarrierType &carrier,
                               const int &nRows,
                               const std::vector<int> &nStrips,
                               const std::vector<double> &pitch,
                               const std::vector<double> &stripStartRadius,
                               const std::vector<double> &stripEndRadius,
                               const double &stereoAngle,
                               const double &centreR,
                               const bool &usePC):
    StripStereoAnnulusDesign(stripDirection,thicknessDirection,thickness,readoutSide,carrier,nRows,nStrips,
                             pitch,stripStartRadius,stripEndRadius,stereoAngle,centreR,centreR,usePC){
//assuming here that centreR==waferCentreR
}

SiLocalPosition StripStereoAnnulusDesign::beamToStrip(const SiLocalPosition &pos) const {
  if (m_usePC) return beamToStripPC(pos);

  const double x_beam = pos.xEta();
  const double y_beam = pos.xPhi();

  //    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
  //
  //    x_strip       cos(-m_stereo)  -sin(-m_stereo)  x-R     R
  //   (       )  =  [                               ](   ) + ( )
  //    y_strip       sin(-m_stereo)   cos(-m_stereo)   y      0
  const double x_strip = m_cos_minus_stereo * (x_beam - m_waferCentreR) - m_sin_minus_stereo * y_beam + m_waferCentreR;
  const double y_strip = m_sin_minus_stereo * (x_beam - m_waferCentreR) + m_cos_minus_stereo * y_beam;
  
  return {x_strip, y_strip};
}

SiLocalPosition StripStereoAnnulusDesign::beamToStripPC(const SiLocalPosition &pos) const {
  const double phi_beam = pos.xPhi();
  const double rad_beam = pos.xEta(); 

  // Convert to cart and use cartesian transform
  const double x_beam = rad_beam * std::cos(phi_beam);
  const double y_beam = rad_beam * std::sin(phi_beam);

  //    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
  //
  //    x_strip       cos(-m_stereo)  -sin(-m_stereo)  x-R     R
  //   (       )  =  [                               ](   ) + ( )
  //    y_strip       sin(-m_stereo)   cos(-m_stereo)   y      0
  const double x_strip = m_cos_minus_stereo * (x_beam - m_waferCentreR) - m_sin_minus_stereo * y_beam + m_waferCentreR;
  const double y_strip = m_sin_minus_stereo * (x_beam - m_waferCentreR) + m_cos_minus_stereo * y_beam;

  const double phi_strip = std::atan2(y_strip, x_strip);
  const double rad_strip = std::hypot(x_strip, y_strip);

  return {rad_strip, phi_strip};
}

/**
 * @brief Version of StripStereoAnnulusDesign::beamToStripPC transform based exclusively in a polar system
 * 
 * Not currently used. Requires debugging and profiling
 * 
 * @param pos 
 * @return SiLocalPosition 
 */
SiLocalPosition StripStereoAnnulusDesign::beamToStripPCpolar(const SiLocalPosition &pos) const {
  const double phi_beam = pos.xPhi();
  const double rad_beam = pos.xEta(); 

  // Exclusively-polar transform:
  // Trig which can be precalculated in the future
  const double m_stereo_2 = m_stereo*0.5;
  const double m_cos_plus_stereo_2 = std::cos(m_stereo_2);
  const double m_sin_plus_stereo_2 = std::sin(m_stereo_2);

  // calculation for strip-frame radius uses cosine law - see ATL-COM-ITK-2021-048
  const double rad_conv = 2.0*m_waferCentreR*std::abs(m_sin_plus_stereo_2);
  const double rad_strip = std::sqrt( std::pow(rad_beam,2.0) + std::pow(rad_conv,2.0) + 2.0*rad_beam*rad_conv*m_cos_plus_stereo_2);
  
  // calculation for strip-frame angle uses sine law - see ATL-COM-ITK-2021-048
  const double phi_strip = M_PI_2 - m_stereo_2 - std::asin((rad_beam*std::sin(M_PI + phi_beam - m_stereo_2))/rad_strip);

  return {rad_strip, phi_strip};
}

SiLocalPosition StripStereoAnnulusDesign::stripToBeam(const SiLocalPosition &pos) const {
  if (m_usePC) return stripToBeamPC(pos);

  const double x_strip = pos.xEta();
  const double y_strip = pos.xPhi();

  //    Transform to beam frame (eq. 36 in ver G, combined with eq. 2 since we are in strip frame)
  //
  //    x_beam       cos(m_stereo)  -sin(m_stereo)  x-R     R
  //   (       )  =  [                               ](   ) + ( )
  //    y_beam       sin(m_stereo)   cos(m_stereo)   y      0
  const double x_beam = m_cos_plus_stereo * (x_strip - m_waferCentreR) - m_sin_plus_stereo * y_strip + m_waferCentreR;
  const double y_beam = m_sin_plus_stereo * (x_strip - m_waferCentreR) + m_cos_plus_stereo * y_strip;
  
  return {x_beam, y_beam};
}

SiLocalPosition StripStereoAnnulusDesign::stripToBeamPC(const SiLocalPosition &pos) const {
  const double phi_strip = pos.xPhi();
  const double rad_strip = pos.xEta(); 

  // Convert to cart and use previous transform
  const double x_strip = rad_strip * std::cos(phi_strip);
  const double y_strip = rad_strip * std::sin(phi_strip);

  //    Transform to beam frame (eq. 36 in ver G, combined with eq. 2 since we are in strip frame)
  //
  //    x_beam        cos(m_stereo)  -sin(m_stereo)  x-R     R
  //   (       )  =  [                             ](   ) + ( )
  //    y_beam        sin(m_stereo)   cos(m_stereo)   y      0
  const double x_beam = m_cos_plus_stereo * (x_strip - m_waferCentreR) - m_sin_plus_stereo * y_strip + m_waferCentreR;
  const double y_beam = m_sin_plus_stereo * (x_strip - m_waferCentreR) + m_cos_plus_stereo * y_strip;

  const double phi_beam = std::atan2(y_beam, x_beam);
  const double rad_beam = std::hypot(x_beam, y_beam);

  return {rad_beam, phi_beam};
}

/**
 * @brief Version of StripStereoAnnulusDesign::stripToBeamPC transform based exclusively in a polar system
 * 
 * Not currently used. Requires debugging and profiling
 * 
 * @param pos 
 * @return SiLocalPosition 
 */
SiLocalPosition StripStereoAnnulusDesign::stripToBeamPCpolar(const SiLocalPosition &pos) const {
  const double phi_strip = pos.xPhi();
  const double rad_strip = pos.xEta(); 

  // Exclusively-polar transform:
  // Trig which can be precalculated in the future
  const double m_stereo_2 = m_stereo*0.5;
  const double m_sin_plus_stereo_2 = std::sin(m_stereo_2);

  // calculation for beam-frame radius uses cosine law - see ATL-COM-ITK-2021-048
  const double rad_conv = 2.0*m_waferCentreR*std::abs(m_sin_plus_stereo_2);
  const double rad_beam = std::sqrt( std::pow(rad_strip,2.0) + std::pow(rad_conv,2.0) + 2.0*rad_strip*rad_conv*std::cos(M_PI_2 - m_stereo_2 - phi_strip));
  
  // calculation for beam-frame angle uses sine law - see ATL-COM-ITK-2021-048
  const double phi_beam = m_stereo_2 - M_PI_2 + std::asin((rad_strip*std::sin(M_PI_2 - m_stereo_2 - phi_strip))/rad_beam);

  return {rad_beam, phi_beam};
}

HepGeom::Point3D<double> StripStereoAnnulusDesign::sensorCenter() const {
  return HepGeom::Point3D<double>(m_R, 0., 0.);
}

double StripStereoAnnulusDesign::sinStripAngleReco(double phiCoord, double etaCoord) const {
//
//    Transform to strip frame SF (eq. 36 in ver G, combined with eq. 2 since we are in beam frame)
//
    SiLocalPosition pos_strip = beamToStrip({etaCoord, phiCoord});
    
    double phi_strip = (m_usePC) ? pos_strip.xPhi() : std::atan2(pos_strip.xPhi(), pos_strip.xEta());

    // The minus sign below is because this routine is called by tracking software, which swaps x and y, then measures angles from y 
    // to x
    return -sin(phi_strip + m_stereo);
}

void StripStereoAnnulusDesign::getStripRow(SiCellId cellId, int *strip2D, int *rowNum) const {
    int strip1D = cellId.phiIndex();
    *rowNum = row(strip1D);
    *strip2D = strip1D - m_firstStrip[*rowNum];
    return;
}

int  StripStereoAnnulusDesign::strip1Dim(int strip, int row) const {

    return m_firstStrip[row] + strip;
}

void StripStereoAnnulusDesign::neighboursOfCell(const SiCellId &cellId, std::vector<SiCellId> &neighbours) const {


    neighbours.clear();

    if (!cellId.isValid()) {
        return;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripM = strip - 1;
    int stripP = strip + 1;

    if (stripM >= m_firstStrip[row]) {
        neighbours.push_back(stripM);
    }
    if (stripP < m_firstStrip[row] + m_nStrips[row]) {
        neighbours.push_back(stripP);
    }

    return;
}

/**
 * @brief Get a reference to the module bounds object. 
 * 
 * @return const Trk::SurfaceBounds& The module bounds.
 */
const Trk::SurfaceBounds &StripStereoAnnulusDesign::bounds() const {
    return *(m_bounds.get()); // Equivalent but more explicit than *m_bounds - 
                              // gets the normal pointer from the unique then dereferences it.
}

SiCellId StripStereoAnnulusDesign::cellIdOfPosition(SiLocalPosition const &pos) const {
//
//    Find the row
//
    double rad_beam = (m_usePC) ? pos.xEta() : pos.r();
    if (rad_beam < m_stripStartRadius[0] || rad_beam >= m_stripEndRadius.back()) { // Check r is in the module
      REPORT_MESSAGE( MSG::DEBUG ) << "Invalid SiLocalPosition (radius outside module), returning invalid SiCellId (r="<<rad_beam<<", inside="<<m_stripStartRadius[0]<<", outside = "<<m_stripEndRadius.back()<<")\n";
      return SiCellId(); // return an invalid id
    }

    int row = 0;
    if(m_nRows>1){ //only do this if we have multiple rows...
      vector<double>::const_iterator endPtr = upper_bound(m_stripStartRadius.begin(), m_stripStartRadius.end(), rad_beam);
      row = distance(m_stripStartRadius.begin(), endPtr) - 1;
      // Following should never happen, check is done on r above
      if (row < 0 || row >= m_nRows) {
        REPORT_MESSAGE( MSG::DEBUG ) << "Invalid SiLocalPosition, returning invalid SiCellId: bad row = " << row << " for r = " << rad_beam << " \n";
        return SiCellId(); // return an invalid id
      }
    }
//
//    Find the strip
//
    SiLocalPosition pos_strip = beamToStrip(pos);
    double phi_strip = (m_usePC) ? pos_strip.xPhi() : std::atan2(pos_strip.xPhi(), pos_strip.xEta()); 
    int strip = floor(phi_strip / m_pitch[row]) + m_nStrips[row] / 2.0;
    if (strip < 0 || strip >= m_nStrips[row]) { // Outside
      REPORT_MESSAGE( MSG::DEBUG ) << "Invalid SiLocalPosition (conversion to strip frame gave invalid strip), returning invalid SiCellId";
      REPORT_MESSAGE( MSG::DEBUG ) << "Strip was "<<strip<<", max strip was "<<m_nStrips[row]<<", phiPrime="<<phi_strip<<"\n";
      return SiCellId(); // return an invalid id
    }

    int strip1D = strip1Dim(strip, row);
    return SiCellId(strip1D, 0);
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfCell(SiCellId const &cellId) const {
    int strip, row;
    getStripRow(cellId, &strip, &row);
    // this is the radius in the module / radial system
    double r = (m_stripEndRadius[row] + m_stripStartRadius[row]) / 2.;

    // get phi of strip in the strip system
    double phi_strip = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * sin(m_stereo/2. + phi_strip);
    double c = m_lengthBF * m_lengthBF - r * r;
    // this is the radius in the strip system
    double rad_strip = (-b + sqrt(b * b - 4. * c))/2.;
    
    if (m_usePC) return stripToBeam({rad_strip,phi_strip}); 
    else {
      //else use cart
      double x_strip = rad_strip * cos(phi_strip);
      double y_strip = rad_strip * sin(phi_strip);
      return stripToBeam({x_strip, y_strip});
    }
}

/**
 * @brief Explicit polar version, no longer required. To be depreceated
 * @deprecated 
 * 
 * @param cellId 
 * @return SiLocalPosition 
 */
SiLocalPosition StripStereoAnnulusDesign::localPositionOfCellPC(SiCellId const &cellId) const {
  
    int strip, row;
    getStripRow(cellId, &strip, &row);
    // this is the radius in the module / radial system
    double r = (m_stripEndRadius[row] + m_stripStartRadius[row]) / 2.;

    // get phi of strip in the strip system
    double phiPrime = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * sin(m_stereo/2. + phiPrime);
    double c = m_lengthBF * m_lengthBF - r * r;
    // this is the radius in the strip system
    double rPrime = (-b + sqrt(b * b - 4. * c))/2.;

    // flip this, since coordinate system is defined the other way round
    double phi = -1*phiPrime;

    // xEta => r, xPhi = phi
    return SiLocalPosition(rPrime, phi); 
}

SiLocalPosition StripStereoAnnulusDesign::stripPosAtR(int strip, int row, double r) const {

    double phi_strip = (strip - m_nStrips[row] / 2. + 0.5) * m_pitch[row];

    double b = -2. * m_lengthBF * sin(m_stereo/2. + phi_strip);
    double c = m_lengthBF * m_lengthBF - r * r;
    double rad_strip = (-b + sqrt(b * b - 4. * c))/2.;

    if (m_usePC) return stripToBeam({rad_strip,phi_strip});

    //else use cart
    double x_strip = rad_strip * cos(phi_strip);
    double y_strip = rad_strip * sin(phi_strip);
    return stripToBeam({x_strip, y_strip});
}

SiLocalPosition StripStereoAnnulusDesign::localPositionOfCluster(SiCellId const &cellId, int clusterSize) const {
//
//    Return the average centre-position of the first and last strips.
//

    SiLocalPosition startPos = localPositionOfCell(cellId); // Should automatically detect PC Usage

    if (clusterSize <= 1) {
        return startPos;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripEnd = strip + clusterSize - 1;
    SiCellId endId = strip1Dim(stripEnd, row);
    SiLocalPosition endPos = localPositionOfCell(endId); // Should automatically detect PC Usage

    return (startPos + endPos) / 2.0;
}

/**
 * @brief This is for debugging only. Call to explicitly get PC clusters.
 * 
 * @deprecated method StripStereoAnnulusDesign::localPositionOfCluster() should detect the use of polar co-ordinates
 * automatically and return the cluster in the appropriate co-ordinate system. This is handled in the call to the 
 * localPositionOfCell() function. Therefore, the only use of this method is to explicitly get a polar cluster when the 
 * module was created with a cartesian co-ordinate system. After the integration of polar co-ordinates is complete, this
 * method will be removed.
 * 
 * @param cellId 
 * @param clusterSize 
 * @return SiLocalPosition 
 */
SiLocalPosition StripStereoAnnulusDesign::localPositionOfClusterPC(SiCellId const &cellId, int clusterSize) const {
    SiLocalPosition startPos = localPositionOfCellPC(cellId);

    if (clusterSize <= 1) {
        return startPos;
    }

    int strip, row;
    getStripRow(cellId, &strip, &row);
    int stripEnd = strip + clusterSize - 1;
    SiCellId endId = strip1Dim(stripEnd, row);
    SiLocalPosition endPos = localPositionOfCellPC(endId);

    return (startPos + endPos) / 2.0;
}

/// Give end points of the strip that covers the given position
std::pair<SiLocalPosition, SiLocalPosition> StripStereoAnnulusDesign::endsOfStrip(SiLocalPosition const &pos) const {

    SiCellId cellId = cellIdOfPosition(pos);

    int strip, row;
    getStripRow(cellId, &strip, &row);

    SiLocalPosition innerEnd = stripPosAtR(strip, row, m_stripStartRadius[row]);
    SiLocalPosition outerEnd = stripPosAtR(strip, row, m_stripEndRadius[row]);

    return pair<SiLocalPosition, SiLocalPosition>(innerEnd, outerEnd);
}

bool StripStereoAnnulusDesign::inActiveArea(SiLocalPosition const &pos, bool /*checkBondGap*/) const {

    SiCellId id = cellIdOfPosition(pos);
    bool inside = id.isValid();

    return inside;
}

// Used in surfaceChargesGenerator
double StripStereoAnnulusDesign::scaledDistanceToNearestDiode(SiLocalPosition const &pos) const {
  //    Get phiPrime of pos   
  SiLocalPosition pos_stripframe = beamToStrip(pos);
  double pos_phi_stripframe = (m_usePC) ? pos_stripframe.xPhi() : std::atan2(pos_stripframe.xPhi(),pos_stripframe.xEta());

//
//    Get phiPrime of strip
//
  SiCellId cellId = cellIdOfPosition(pos);
  SiLocalPosition stripPos_beamframe = localPositionOfCell(cellId); // This can be polar or cart
  SiLocalPosition stripPos_stripframe = beamToStrip(stripPos_beamframe); // This should automatically work with polar or cart
  double strip_phi_stripframe = (m_usePC) ? stripPos_stripframe.xPhi() : std::atan2(stripPos_stripframe.xPhi(),stripPos_stripframe.xEta());

  int strip, row;
  getStripRow(cellId, &strip, &row);
  return std::abs(pos_phi_stripframe - strip_phi_stripframe) / m_pitch[row];
}

/// Return strip width, centre, length etc. Hard to find if this is used or not.
SiDiodesParameters StripStereoAnnulusDesign::parameters(SiCellId const & /*cellId*/) const {
    throw std::runtime_error("Call to StripStereoAnnulusDesign::parameters; not yet implemented");
}

// Used in VP1 graphics. DEPRECATED.
SiLocalPosition StripStereoAnnulusDesign::positionFromStrip(SiCellId const &cellId) const {
    return localPositionOfCell(cellId);
}

// DEPRECATED but pure virtual in base class; which row?? - assume row 0.
SiLocalPosition StripStereoAnnulusDesign::positionFromStrip(const int stripNumber) const {
    return localPositionOfCell(SiCellId(stripNumber, 0));
}

/// DEPRECATED: only used in a stupid example (2014)
///Check if cell is in range. Returns the original cellId if it is in range, otherwise it
// returns an invalid id.
SiCellId StripStereoAnnulusDesign::cellIdInRange(const SiCellId &cellId) const {

    if (!cellId.isValid()) {
        return SiCellId(); // Invalid
    }
    int row = cellId.etaIndex();
    int strip = cellId.phiIndex();
    if (strip < 0 || row < 0 || row >= m_nRows || strip >= m_nStrips[row]) {
        return SiCellId(); // Invalid
    }
    return cellId;
}

double StripStereoAnnulusDesign::length() const {
// Return the total length of all strips, i.e. the active area length.
    return m_stripEndRadius.back() - m_stripStartRadius[0];
}

double StripStereoAnnulusDesign::width() const {
// Return approximate width between the two central rows
  float middleRow = m_stripStartRadius.size() * 0.5 - 1;
  if (middleRow < 0) {
    //single-row version
    return 2. * tan((m_pitch[0] * m_nStrips[0])*0.5) * ((m_stripStartRadius[0] + m_stripEndRadius[0])*0.5);
  }
  else return 2. * tan((m_pitch[middleRow] * m_nStrips[middleRow]) * 0.5) * m_stripEndRadius[middleRow];
}

double StripStereoAnnulusDesign::minWidth() const {
    return 2. * tan((m_pitch[0] * m_nStrips[0]) * 0.5 ) * m_stripStartRadius[0];
}

double StripStereoAnnulusDesign::maxWidth() const {
    return 2. * tan((m_pitch.back() * m_nStrips.back()) * 0.5) * m_stripEndRadius.back();
}

double StripStereoAnnulusDesign::etaPitch() const {
// Return average strip length
    return length() / m_stripStartRadius.size();
}

HepGeom::Vector3D<double> StripStereoAnnulusDesign::phiMeasureSegment(const SiLocalPosition & /*position*/)
const {
    throw std::runtime_error("Call to phiMeasureSegment, DEPRECATED, not implemented.");
}

void StripStereoAnnulusDesign::distanceToDetectorEdge(SiLocalPosition const & pos, double & etaDist, double & phiDist) const {

// For eta, we use the Strip frame. This is centred at the beamline, x along eta, y along phi, z along depth
// Happens to coincide with SiLocalPosition; no transform needed.
  double rInner = m_stripStartRadius[0];
  double rOuter = m_stripEndRadius[m_nRows - 1];
  double rad_beam = (m_usePC) ? pos.xEta() : std::hypot(pos.xEta(), pos.xPhi());

  if (rad_beam < rInner)
    etaDist = rad_beam - rInner;
  else if (rad_beam > rOuter)
    etaDist = rOuter - rad_beam;
  else
    etaDist = std::min(rOuter - rad_beam, rad_beam - rInner);
 
// For phi, we use the Strip frame. Transform to Strip-frame:
  SiLocalPosition pos_strip = beamToStrip(pos);
  
  double rad_strip = (m_usePC) ? pos_strip.xEta() : std::hypot(pos_strip.xEta(), pos_strip.xPhi());
  double phi_strip = (m_usePC) ? pos_strip.xPhi() : std::atan2(pos_strip.xPhi(), pos_strip.xEta());
  
  double phiAngleMax = m_pitch[0] * m_nStrips[0] / 2.0;
  double phiAngleMin = -phiAngleMax;

  if (phi_strip < phiAngleMin)
    phiDist = rad_strip * (phi_strip - phiAngleMin);
  else if (phi_strip > phiAngleMax)
    phiDist = rad_strip * (phiAngleMax - phi_strip);
  else
    phiDist = rad_strip * std::min(phiAngleMax - phi_strip, phi_strip - phiAngleMin);

  return;
}

DetectorShape StripStereoAnnulusDesign::shape() const
{
  if (m_usePC) return InDetDD::PolarAnnulus; 
  else return InDetDD::Annulus;
}

const HepGeom::Transform3D StripStereoAnnulusDesign::SiHitToGeoModel() const {
   return HepGeom::RotateY3D(90.*CLHEP::deg) ;
}

double StripStereoAnnulusDesign::stripLength(const InDetDD::SiCellId &cellId) const
{
  SiLocalPosition lpoc = localPositionOfCell(cellId);
  std::pair<SiLocalPosition, SiLocalPosition> end = endsOfStrip(lpoc);
  double dx = end.second.xEta() - end.first.xEta();
  double dy = end.second.xPhi() - end.first.xPhi();
  return sqrt(dx * dx + dy * dy);
}


} // namespace InDetDD

