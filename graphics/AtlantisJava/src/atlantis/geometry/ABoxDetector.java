package atlantis.geometry;

import atlantis.utils.AMath;
import atlantis.parameters.*;
import atlantis.graphics.ACoord;

/**
 * in ATLAS boxes are RPC and MDT in the barrel
 */

public class ABoxDetector extends ADetector {

  double zMin;
  double zMax;
  double rMin;
  double rMax;
  double phi;
  double excl;
  double size;
  String stationName;
  int stationEta;
  int stationPhi;
  int sector;

  public ABoxDetector(String name, double zMin, double zMax, double rMin,
      double rMax, double phi, double excl, double size, int stationEta, int stationPhi) {
    super(name, " ", name.substring(0, 3));
    this.zMin=zMin;
    this.zMax=zMax;
    this.rMin=rMin;
    this.rMax=rMax;
    this.phi=phi;
    this.excl=excl;
    this.size=size;
    this.stationName=name.substring(4, 7);
    this.stationEta=stationEta;
    this.stationPhi=stationPhi;
    this.sector=((int)((phi+Math.PI/16)/(Math.PI/8)))%16;
  }

  protected ACoord getFZUser() {
    double cosPhi=Math.cos(phi);
    double sinPhi=Math.sin(phi);
    double d=size/2.;
    double r=rMin;
    double x=r*cosPhi-excl*sinPhi;
    double y=r*sinPhi+excl*cosPhi;
    double dsinPhi=d*sinPhi;
    double dcosPhi=d*cosPhi;
    double phi1=Math.toDegrees(Math.atan2(y-dcosPhi, x+dsinPhi));
    double phi2=Math.toDegrees(Math.atan2(y+dcosPhi, x-dsinPhi));

    if(phi1<0.) phi1+=360.;
    if(phi2<0.) phi2+=360.;
    if(phi1-phi2>180.)  phi1-=360.;
    if(phi2-phi1>180.)  phi2-=360.;

    return new ACoord(AMath.xBox((zMin+zMax)/2.,(zMax-zMin)/2.),
                      AMath.yBox((phi1+phi2)/2.,(phi1-phi2)/2.));
  }

  protected ACoord getRZUser() {
     double phiMid=Math.toRadians(parameterStore.get("RZ", "Phi").getD());
     double phiDiff=Math.abs(phi-phiMid);
     double sign=-1.;
     if(phiDiff<Math.PI/2.||phiDiff>3*Math.PI/2.)
       sign=+1.;
     return getXZRZUser(sign);
   }

   protected ACoord getXZUser() {
    AParameter p=parameterStore.get("XZ", "Phi");

    // display muon data when phi is in the middle of a sector
    if(p.getD() % 22.5 < 1e-2) {
      int sect=(int)Math.round((p.getD() % 360.) / 22.5);
        if(sector==sect) {
          return getXZRZUser(1.);
        } else if(sector==sect-8||sector==sect+8) {
          return getXZRZUser(-1.);
        }
      }
    return ACoord.NO_DATA;
  }

  protected ACoord getXZRZUser(double sign) {
     return new ACoord(
        AMath.xBox( (zMin + zMax) / 2., (zMax - zMin) / 2.),
        AMath.yBox( sign*(rMin + rMax) / 2., sign*(rMax - rMin) / 2.));
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
      double d=size/2.;
      double r=rMin;
      double x=r*cosPhi-excl*sinPhi;
      double y=r*sinPhi+excl*cosPhi;
      double dx=d*sinPhi;
      double dy=d*cosPhi;

      hv[0][0]=x+dx;
      hv[1][0]=y-dy;
      hv[0][1]=x-dx;
      hv[1][1]=y+dy;
      r=rMax;
      x=r*cosPhi-excl*sinPhi;
      y=r*sinPhi+excl*cosPhi;

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

  protected boolean equalsYX(ADetector o) {
    if (o instanceof ABoxDetector) {
      ABoxDetector that = (ABoxDetector) o;
      return this.rMin == that.rMin &&
          this.rMax == that.rMax &&
          this.phi == that.phi &&
          this.size == that.size &&
          this.excl == that.excl;
    } else
      return false;
  }

  protected boolean equalsRZ(ADetector o) {
    if (o instanceof ABoxDetector) {
      ABoxDetector that = (ABoxDetector) o;
    return this.rMin == that.rMin &&
        this.rMax == that.rMax &&
        this.zMin == that.zMin &&
        this.zMax == that.zMax;
  } else
    return false;
  }
  
  public double getZMin() { return zMin; }
  public double getZMax() { return zMax; }
  public double getRMin() { return rMin; }
  public double getRMax() { return rMax; }
  public double getPhi() { return phi; }
  public double getSize() { return size; }
  
}

