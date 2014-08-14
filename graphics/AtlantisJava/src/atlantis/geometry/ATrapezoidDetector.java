package atlantis.geometry;


import atlantis.utils.AMath;
import atlantis.parameters.*;
import atlantis.graphics.ACoord;

/**
 * In ATLAS Trapezoids are TGC and MDT in the endcap.
 */

public class ATrapezoidDetector extends ADetector {

  double zMin;
  double zMax;
  double rMin;
  double rMax;
  double phi;
  double sizeS;
  double sizeL;
  double alpha;
  int stationEta;
  int stationPhi;
  int sector;

  public ATrapezoidDetector(String name, double zMin, double zMax, double rMin,
                     double rMax, double phi, double sizeS, double sizeL,
                     double alpha, int stationEta, int stationPhi) {
    super(name, "  ", name.substring(0, 3));
    this.zMin = zMin;
    this.zMax = zMax;
    this.rMin = rMin;
    this.rMax = rMax;
    this.phi = phi;
    this.sizeS = sizeS;
    this.sizeL = sizeL;
    this.alpha = alpha;
    this.stationEta = stationEta;
    this.stationPhi = stationPhi;
    this.sector = ( (int) ( (phi + Math.PI / 16) / (Math.PI / 8))) % 16;
  }

  protected boolean equalsYX(ADetector o) {
    if (o instanceof ATrapezoidDetector) {
      ATrapezoidDetector that = (ATrapezoidDetector) o;
      return this.rMin == that.rMin &&
          this.rMax == that.rMax &&
          this.phi == that.phi &&
          this.sizeS == that.sizeS &&
          this.sizeL == that.sizeL;
    }    else
      return false;
  }

  protected boolean equalsRZ(ADetector o) {
    if (o instanceof ATrapezoidDetector) {
      ATrapezoidDetector that = (ATrapezoidDetector) o;
      return this.rMin == that.rMin &&
          this.rMax == that.rMax &&
          this.zMin == that.zMin &&
          this.zMax == that.zMax &&
          this.alpha == that.alpha;
   }
    else
      return false;
  }

  protected ACoord getRZUser() {
    double phiMid = Math.toRadians(parameterStore.get("RZ", "Phi").getD());
    double phiDiff = Math.abs(phi - phiMid);
    double sign = -1.;
    if (phiDiff < Math.PI / 2. || phiDiff > 3 * Math.PI / 2.)
      sign = +1.;
    return getXZRZUser(sign);
  }

  protected ACoord getXZUser() {
    AParameter p=parameterStore.get("XZ", "Phi");

    // display muon data when phi is in the middle of a sector
    if(p.getD() % 22.5 < 1e-2) {
      int sect=(int)Math.round((p.getD() % 360.) / 22.5);
      if (sector == sect) {
        return getXZRZUser(1.);
      }
      else if (sector == sect - 8 || sector == sect + 8) {
        return getXZRZUser( -1.);
      }
    }
    return ACoord.NO_DATA;
  }

  protected ACoord getXZRZUser(double sign) {
    if (alpha == 0.) {
      return new ACoord(
          AMath.xBox( (zMin + zMax) / 2., (zMax - zMin) / 2.),
          AMath.yBox(sign * (rMin + rMax) / 2., sign * (rMax - rMin) / 2.));
    }
    else {
      double[][] hv = new double[2][];
      double zMid = (zMin + zMax) / 2.;
      double rMid = (rMin + rMax) / 2.;
      double dRho = (rMax - rMin) / 2;
      double dZ = (zMax - zMin) / 2;
      double sinAlpha = Math.sin(Math.toRadians(alpha));
      double cosAlpha = Math.cos(Math.toRadians(alpha));

      if (zMid * rMid > 0.)
        sinAlpha *= -1.;

      hv[0] = new double[] {
          zMid - sinAlpha * dRho - cosAlpha * dZ,
          zMid - sinAlpha * dRho + cosAlpha * dZ,
          zMid + sinAlpha * dRho + cosAlpha * dZ,
          zMid + sinAlpha * dRho - cosAlpha * dZ};
      hv[1] = new double[] {
          sign * (rMid + cosAlpha * dRho - sinAlpha * dZ),
          sign * (rMid + cosAlpha * dRho + sinAlpha * dZ),
          sign * (rMid - cosAlpha * dRho + sinAlpha * dZ),
          sign * (rMid - cosAlpha * dRho - sinAlpha * dZ)};
      return new ACoord(hv);
    }
  }

  protected ACoord getFZUser() {
    double deltaPhi = Math.max(Math.atan2(sizeL / 2., rMax),
                               Math.atan2(sizeS / 2., rMin));
    
    if (alpha == 0.) {
      return new ACoord(
          AMath.xBox( (zMin + zMax) / 2., (zMax - zMin) / 2.),
          AMath.yBox( Math.toDegrees(phi), Math.toDegrees(deltaPhi) ));
    }
    else {
      double[][] hv = new double[2][];
      double zMid = (zMin + zMax) / 2.;
      double rMid = (rMin + rMax) / 2.;
      double dRho = (rMax - rMin) / 2;
      double dZ = (zMax - zMin) / 2;
      double sinAlpha = Math.sin(Math.toRadians(alpha));
      double cosAlpha = Math.cos(Math.toRadians(alpha));

      if (zMid * rMid > 0.)
        sinAlpha *= -1.;

      hv[0] = new double[] {
          zMid - sinAlpha * dRho - cosAlpha * dZ,
          zMid - sinAlpha * dRho - cosAlpha * dZ,
          zMid + sinAlpha * dRho + cosAlpha * dZ,
          zMid + sinAlpha * dRho + cosAlpha * dZ};
      hv[1] = new double[] {
          Math.toDegrees(phi - deltaPhi),
          Math.toDegrees(phi + deltaPhi),
          Math.toDegrees(phi + deltaPhi),
          Math.toDegrees(phi - deltaPhi)};
      return new ACoord(hv);
    }
  }

  protected ACoord getYXUser() {
    return getYXUser(0);
  }

  protected ACoord getFRUser() {
    return convertYXToFR(getYXUser(1));
  }

  private ACoord getYXUser(int flag) {
    int[] split= {12, 1, 12, 1};
    int numPoints=4;
    boolean splitIt=parameterStore.get("YX", "FishEye").getStatus()||parameterStore.get("YX", "Clock").getStatus()
        ||flag==1;

    if(splitIt)
      numPoints=26;
    double[] temp=new double[4];
    double[][] hv=new double[2][numPoints];
      double cosPhi=Math.cos(phi);
      double sinPhi=Math.sin(phi);
      double d=sizeS/2.;
      double r=rMin;
      double x=r*cosPhi;
      double y=r*sinPhi;
      double dx=d*sinPhi;
      double dy=d*cosPhi;

      hv[0][0]=x+dx;
      hv[1][0]=y-dy;
      hv[0][1]=x-dx;
      hv[1][1]=y+dy;
      r=rMax;
      x=r*cosPhi;
      y=r*sinPhi;
      d=sizeL/2.;
      dx=d*sinPhi;
      dy=d*cosPhi;

      hv[0][3]=x+dx;
      hv[1][3]=y-dy;
      hv[0][2]=x-dx;
      hv[1][2]=y+dy;
      if(splitIt)
        for(int j=0; j<2; ++j) {
          for(int k=0; k<4; ++k)
            temp[k]=hv[j][k];
          AMath.splitArrayIntoPieces(temp, hv[j], split);
        }
    return new ACoord(hv);
  }
}

