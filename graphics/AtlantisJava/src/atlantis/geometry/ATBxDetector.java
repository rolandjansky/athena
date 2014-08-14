package atlantis.geometry;

import atlantis.graphics.ACoord;
import atlantis.utils.AMath;

/**
 * Special t shape Box used for MDT chambers near the feet.
 * These are only different in th FZ projection
 */

public class ATBxDetector extends ABoxDetector {

  double zis;
  double zos;
  double ws;
  double or;

  public ATBxDetector(String name, double zMin, double zMax, double rMin,
      double rMax, double phi, double excl, double size, double zis, double zos,
      double ws, double or, int etaIndex, int phiIndex) {
    super(name, zMin, zMax, rMin, rMax, phi, excl, size, etaIndex, phiIndex);
    this.zis=zis;
    this.zos=zos;
    this.ws=ws;
    this.or=or;
  }

  protected ACoord getFZUser() {
    double[][] hv=new double[2][8];
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
    double phiLower=Math.min(phi1, phi2);
    double phiUpper=Math.max(phi1, phi2);
    double phiMid=phiLower+ws/size*(phiUpper-phiLower);
    hv[0][0]=zMin;
    hv[0][1]=zMin;
    hv[0][2]=zis;
    hv[0][3]=zis;
    hv[0][4]=zos;
    hv[0][5]=zos;
    hv[0][6]=zMax;
    hv[0][7]=zMax;
    if(or>90.) {
      phiMid=phiLower+ws/size*(phiUpper-phiLower);
      hv[1][0]=phiUpper;
      hv[1][1]=phiMid;
      hv[1][2]=phiMid;
      hv[1][3]=phiLower;
      hv[1][4]=phiLower;
      hv[1][5]=phiMid;
      hv[1][6]=phiMid;
      hv[1][7]=phiUpper;
    } else {
      phiMid=phiUpper-ws/size*(phiUpper-phiLower);
      hv[1][0]=phiLower;
      hv[1][1]=phiMid;
      hv[1][2]=phiMid;
      hv[1][3]=phiUpper;
      hv[1][4]=phiUpper;
      hv[1][5]=phiMid;
      hv[1][6]=phiMid;
      hv[1][7]=phiLower;
    }

    return new ACoord(hv);
  }

}
