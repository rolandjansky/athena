package atlantis.geometry;

import atlantis.parameters.AParameter;
import atlantis.parameters.APar;
import atlantis.globals.AGlobals;
import atlantis.graphics.ACoord;

/**
 * Base class of all individual detectors
 */

public abstract class ADetector {

  String name;
  String projection;
  String color;
  AParameter fillColorParameter;
  
  protected static APar parameterStore = APar.instance();
  protected static AGlobals globals = AGlobals.instance();

  final static protected double[][] NO_DETECTORS=new double[2][0];

  ADetector(String name, String projection, String color) {
    this.name=name;
    this.projection=projection;
    this.color=color;
    fillColorParameter=APar.instance().get("Det", color+"Fill");
  }

  protected ACoord getYXUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getRZUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getYZUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getXZUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getFRUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getFZUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getVPUser() {
    return ACoord.NO_DATA;
  }

  protected ACoord getUser(String projection) {
    if (projection.equals("YX"))
      return getYXUser();
    else if (projection.equals("RZ"))
      return getRZUser();
    else if (projection.equals("XZ"))
      return getXZUser();
    else if (projection.equals("YZ"))
      return getYZUser();
    else if (projection.equals("FZ"))
      return getFZUser();
    else if (projection.equals("FR"))
      return getFRUser();
    else if (projection.equals("VP"))
      return getVPUser();
    else
      return ACoord.NO_DATA;
  }

  public String getName() {
    return name;
  }

  public int getFillColor() {
    return fillColorParameter.getI();
  }

  protected boolean getDraw() {
    return fillColorParameter.getStatus();
  }

  protected boolean equalsYX(ADetector that) {
    return false;
  }

  protected boolean equalsRZ(ADetector that) {
    return false;
  }

  protected ACoord convertYXToFR(ACoord coord) {
    double phiLast=0.;

    for (int i=0; i<coord.hv[0].length; i++) {
        for (int j=0; j<coord.hv[0][i].length; j++) {
            double xx=coord.hv[0][i][j];
            double yy=coord.hv[1][i][j];
            double rho=Math.sqrt(xx*xx+yy*yy);
            double phi=Math.toDegrees(Math.atan2(yy, xx));

            if(phi<0.)
              phi+=360.;
            if(j>0) {
              if(phi-phiLast>180.) phi-=360.;
              if(phi-phiLast<-180.) phi+=360.;
            }
            coord.hv[0][i][j]=rho;
            coord.hv[1][i][j]=phi;
            phiLast=phi;
        }
    }

   return coord;
  }

    protected double[][][] generateAnnulusTris(int numPhi, double phi0,
            double rMin, double rMax) {
        
        //rMin *= 1.02;
        
        int numTris = 2 * (numPhi);
        double[][][] hv = new double[2][numTris][3];

        double deltaPhi = 2. * Math.PI / numPhi;

        for (int i = 0; i < numPhi; i++) {
            hv[0][i][0] = rMax * Math.cos(phi0 + i * deltaPhi);
            hv[1][i][0] = rMax * Math.sin(phi0 + i * deltaPhi);

            hv[0][i][1] = rMax * Math.cos(phi0 + (i + 1) * deltaPhi);
            hv[1][i][1] = rMax * Math.sin(phi0 + (i + 1) * deltaPhi);

            float midi = ((float) i) + 0.5f;
            hv[0][i][2] = rMin * Math.cos(phi0 + midi * deltaPhi);
            hv[1][i][2] = rMin * Math.sin(phi0 + midi * deltaPhi);

            int offi = i + (numPhi);
            hv[0][offi][0] = rMin * Math.cos(phi0 + midi * deltaPhi);
            hv[1][offi][0] = rMin * Math.sin(phi0 + midi * deltaPhi);

            hv[0][offi][1] = rMax * Math.cos(phi0 + (i + 1) * deltaPhi);
            hv[1][offi][1] = rMax * Math.sin(phi0 + (i + 1) * deltaPhi);

            hv[0][offi][2] = rMin * Math.cos(phi0 + (midi + 1) * deltaPhi);
            hv[1][offi][2] = rMin * Math.sin(phi0 + (midi + 1) * deltaPhi);
        }

        return hv;
    }
}
